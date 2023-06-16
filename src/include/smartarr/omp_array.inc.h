#include <omp.h>

#define _OMP_ARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_array_, name))
#define _OMP_SARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_smart_array_, name))
#define _OMP_MATRIX_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_matrix_, name))

static inline
_ARRAY_RO(3, 2) _ARRAY_RO(4, 2) _ARRAY_WO(5, 2) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_OMP_ARRAY_FN(add)(
    unsigned int num_threads,
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

    omp_set_num_threads(num_threads);

    #pragma omp parallel for
    for (size_t i = 0; i < len; ++i) {
         c[i] = a[i] + b[i];
    }

    return c;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
size_t
_OMP_ARRAY_FN(find_max)(size_t len, const _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    size_t pos = 0;
    _ARRAY_TYPE max_val = a[0];

    #pragma omp parallel for reduction(max:max_val) 
    for (size_t i = 0; i < len; ++i) {
        bool max_val_lt_a = max_val < a[i]; 
        max_val = max_val_lt_a ? a[i] : max_val;
        pos     = max_val_lt_a ? i : pos;
    }

    return pos;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
size_t
_OMP_SARRAY_FN(find_max)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(find_max)(a->len, a->data);
}

static inline
_ARRAY_RO(4, 2) _ARRAY_RO(7, 5) _ARRAY_WO(10, 8) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_OMP_ARRAY_FN(matrix_matrix_multiply)(
    unsigned int num_threads UNUSED,
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

    if (num_threads) {
        omp_set_num_threads(num_threads);
    }


    // order (i,j,k) changed to (i,k,j), now for c and b we move along row and
    // therefore along cache line
    #pragma omp parallel for
    for (size_t ra = 0; ra < rows_a; ++ra) {
        _ARRAY_TYPE a_ra_k;
        for (size_t k = 0; k < cols_a; ++k) {
            a_ra_k = a[ra*cols_a + k];
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
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(2,3,4)))
_ARRAY_TYPE*
_OMP_MATRIX_FN(matrix_multiply)(unsigned int num_threads,
    _SMART_ARRAY_T* a, _SMART_ARRAY_T* b, _SMART_ARRAY_T* c)
{
    return _OMP_ARRAY_FN(matrix_matrix_multiply)(
        num_threads,
        a->len, a->num_cols, a->data,
        b->len, b->num_cols, b->data,
        c->len, c->num_cols, c->data);
}

#undef _OMP_ARRAY_FN
#undef _OMP_SARRAY_FN
#undef _OMP_MATRIX_FN
