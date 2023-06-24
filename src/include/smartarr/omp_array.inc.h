#include <omp.h>

#define _OMP_ARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_array_, name))
#define _OMP_SARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_smart_array_, name))
#define _OMP_MATRIX_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_matrix_, name))

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) _ARRAY_WO(4, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_OMP_ARRAY_FN(add)(
    size_t len,
    const _ARRAY_TYPE a[len],
    const _ARRAY_TYPE b[len],
          _ARRAY_TYPE c[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    ARRAY_ASSERT_ALIGNED(b);
    ARRAY_ASSERT_ALIGNED(c);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);
    b = __builtin_assume_aligned(b, _SMART_ARRAY_ALIGN);
    c = __builtin_assume_aligned(c, _SMART_ARRAY_ALIGN);

    #pragma omp parallel for if (len > 1024)
    for (size_t i = 0; i < len; ++i) {
         c[i] = a[i] + b[i];
    }

    return c;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_OMP_ARRAY_FN(reduce_add)(
    size_t len,
    const _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    _ARRAY_TYPE sum = 0;

    #pragma omp parallel for reduction (+:sum) if (len > 1024)
    for (size_t i = 0; i < len; ++i) {
         sum += a[i];
    }
    return sum;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
size_t
_OMP_SARRAY_FN(reduce_add)(_SMART_ARRAY_T* a)
{
    return _OMP_ARRAY_FN(reduce_add)(a->len, a->data);
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
size_t
_OMP_ARRAY_FN(find_max)(size_t len, const _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    typedef struct { _ARRAY_TYPE val; size_t pos; } val_pos_tuple_t;
    val_pos_tuple_t max_pos = {a[0], 0};

    #pragma omp declare reduction(get_max : val_pos_tuple_t :\
        omp_out = omp_out.val > omp_in.val ? omp_out : omp_in)\
        initializer (omp_priv=(omp_orig))

    #pragma omp parallel
    {
        #pragma omp for nowait reduction(get_max : max_pos)
        for (size_t i = 0; i < len; ++i) {
            if (a[i] > max_pos.val) { 
                max_pos.val = a[i];
                max_pos.pos = i;
            }
        }
    }
    return max_pos.pos;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
size_t
_OMP_SARRAY_FN(find_max)(_SMART_ARRAY_T* a)
{
    return _OMP_ARRAY_FN(find_max)(a->len, a->data);
}

static inline
_ARRAY_RO(3, 1) _ARRAY_RO(6, 4) _ARRAY_WO(9, 7) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_OMP_ARRAY_FN(matrix_matrix_multiply)(
    size_t len_a,
    size_t cols_a,
    const _ARRAY_TYPE a[len_a],
    size_t len_b,
    size_t cols_b,
    const _ARRAY_TYPE b[len_b],
    size_t len_c,
    size_t cols_c,
          _ARRAY_TYPE c[len_c])
{
    ARRAY_ASSERT_ALIGNED(a);
    ARRAY_ASSERT_ALIGNED(b);
    ARRAY_ASSERT_ALIGNED(c);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);
    b = __builtin_assume_aligned(b, _SMART_ARRAY_ALIGN);
    c = __builtin_assume_aligned(c, _SMART_ARRAY_ALIGN);

    const size_t rows_a        = len_a / cols_a;
    const size_t rows_b UNUSED = len_b / cols_b;
    const size_t rows_c UNUSED = len_c / cols_c;
    #ifdef _ARRAY_DEBUG
        assert((len_a % cols_a) == 0);
        assert((len_b % cols_b) == 0);
        assert((len_c % cols_c) == 0);
        assert(cols_a == rows_b);
        assert(rows_a == rows_c);
        assert(cols_b == cols_c);
    #endif

    __builtin_memset(c, 0, len_c*sizeof(_ARRAY_TYPE));

    // order (i,j,k) changed to (i,k,j), now for c and b we move along row and
    // therefore along cache line
    #pragma omp parallel for if (rows_a > 1024)
    for (size_t ra = 0; ra < rows_a; ++ra) {
        _ARRAY_TYPE a_ra_k;
        size_t ra_x_cols_a = ra*cols_a;
        for (size_t k = 0; k < cols_a; ++k) {
            a_ra_k = a[ra_x_cols_a + k];
            for (size_t cb = 0; cb < cols_b; ++cb) {
                c[ra*cols_c + cb] +=
                    a_ra_k *
                    b[k*cols_b + cb];
            }
        }
    }

    return c;
}

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1,2,3)))
_ARRAY_TYPE*
_OMP_MATRIX_FN(matrix_multiply)(
    _SMART_ARRAY_T* a, _SMART_ARRAY_T* b, _SMART_ARRAY_T* c)
{
    return _OMP_ARRAY_FN(matrix_matrix_multiply)(
        a->len, a->num_cols, a->data,
        b->len, b->num_cols, b->data,
        c->len, c->num_cols, c->data);
}

#undef _OMP_ARRAY_FN
#undef _OMP_SARRAY_FN
#undef _OMP_MATRIX_FN
