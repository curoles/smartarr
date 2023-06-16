/**@file
 * @brief Functions on array.
 * @author Igor Lesik 2023
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "smartarr/defines.h"
#include "smartarr/cpu.h"


#define PPCAT_NX(a, b) a ## b
#define PPCAT(a, b) PPCAT_NX(a, b)

#define _ARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_array_, name))
#define _SARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_smart_array_, name))
#define _MATRIX_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_matrix_, name))

#define _ARRAY_RO(ref_index, size_index) __attribute__ ((access (read_only, ref_index, size_index)))
#define _ARRAY_WO(ref_index, size_index) __attribute__ ((access (write_only, ref_index, size_index)))
#define _ARRAY_RW(ref_index, size_index) __attribute__ ((access (read_write, ref_index, size_index)))

#ifndef _ARRAY_TYPE_EQ
#define _ARRAY_TYPE_EQ(a, b) ({(a) == (b);})
#endif

#ifndef _ARRAY_TYPE_LT
#define _ARRAY_TYPE_LT(a, b) ({(a) < (b);})
#endif

#define _SMART_ARRAY   PPCAT(_ARRAY_TYPE_NAME, _smart_array)
#define _SMART_ARRAY_T PPCAT(_ARRAY_TYPE_NAME, _smart_array_t)

#ifndef _SMART_ARRAY_ALIGN
#define _SMART_ARRAY_ALIGN SMARTARR_SIMD_VLEN
#endif

#define ATTR_SMART_ARRAY_ALIGNED __attribute__((aligned(_SMART_ARRAY_ALIGN)))

#ifdef _ARRAY_DEBUG
#define ARRAY_ASSERT_ALIGNED(ptr) assert(((size_t)ptr & (_SMART_ARRAY_ALIGN - 1)) == 0);
#else
#define ARRAY_ASSERT_ALIGNED(ptr)
#endif

/** Smart_array knows its own length.
 *
 * Example:
 * ```
 * static int_smart_array_t static_b = {3, 1, {4, 5, 6}};
 * assert(static_b.data[0] == 4);
 * assert(static_b.data[1] == 5);
 * assert(static_b.data[2] == 6);
 * static int_smart_array_t static_c = {3, 1, {[0 ... 2]=7}};
 *
 * auto_free int_smart_array_t* heap_a = int_smart_array_heap_new(100, aligned_alloc);
 * d = heap_a->data;
 * for (int i = 0; i < heap_a->len; ++i) {d[i]=i;}
 * for (int i = 0; i < heap_a->len; ++i) {assert(d[i] == i);}
 * ```
 */
typedef struct _SMART_ARRAY {
    size_t len;
    size_t num_cols;
    _ARRAY_TYPE data[] __attribute__((aligned(_SMART_ARRAY_ALIGN)));
} _SMART_ARRAY_T;

static inline
FN_ATTR_CONST
size_t
_SARRAY_FN(align_len)(size_t len)
{
    size_t len_bytes = len * sizeof(_ARRAY_TYPE);
    len_bytes = ((len_bytes + _SMART_ARRAY_ALIGN) / _SMART_ARRAY_ALIGN) * _SMART_ARRAY_ALIGN;

    return len_bytes / sizeof(_ARRAY_TYPE);
}

/* Allocate smart_array on stack.
 *
 * Example:
 * ```
 * int_smart_array_t* stack_a = smart_array_stack_new(int_smart_array_t, int, 100);
 * int* d = stack_a->data;
 * for (int i = 0; i < stack_a->len; ++i) {d[i]=i;}
 * for (int i = 0; i < stack_a->len; ++i) {assert(d[i] == i);}
 * ```
 */
#define smart_array_stack_new(ARRAY, T, alen) \
    ({ \
    typeof (alen) _ARRAY_len = (alen); \
    size_t len_bytes = _ARRAY_len * sizeof(T); \
    size_t align = sizeof(ARRAY); \
    len_bytes = ((len_bytes + align) / align) * align; \
    size_t aligned_len = len_bytes / sizeof(T); \
    ARRAY* ptr = (ARRAY*) \
        __builtin_alloca_with_align(sizeof(ARRAY) + aligned_len*sizeof(T), 8*_SMART_ARRAY_ALIGN); \
    ptr->len = _ARRAY_len; \
    ptr->num_cols = 1; \
    ptr;})

/** Allocate smart_array on heap.
 *
 * Example:
 * ```
 * auto_free int_smart_array_t* heap_a = int_smart_array_heap_new(100);
 * ```
 */
static inline
FN_ATTR_WARN_UNUSED_RESULT
_SMART_ARRAY_T*
_SARRAY_FN(heap_new)(size_t len)
{
    size_t aligned_len = _SARRAY_FN(align_len)(len);
    _SMART_ARRAY_T* ptr = (_SMART_ARRAY_T*)
        aligned_alloc(_SMART_ARRAY_ALIGN, sizeof(_SMART_ARRAY_T) + aligned_len*sizeof(_ARRAY_TYPE));
    ptr->len = len;
    ptr->num_cols = 1;
    ARRAY_ASSERT_ALIGNED(ptr->data);
    return ptr;
}

static inline
FN_ATTR_WARN_UNUSED_RESULT
_SMART_ARRAY_T*
_SARRAY_FN(heap_realloc)(_SMART_ARRAY_T* self, size_t len)
{
    size_t aligned_len = _SARRAY_FN(align_len)(len);
    _SMART_ARRAY_T* ptr = (_SMART_ARRAY_T*)
        aligned_alloc(_SMART_ARRAY_ALIGN, sizeof(_SMART_ARRAY_T) + aligned_len*sizeof(_ARRAY_TYPE));
    ptr->len = len;
    ptr->num_cols = self->num_cols;
    __builtin_memcpy(ptr->data, self->data,
        ((self->len < ptr->len)? self->len : ptr->len) * sizeof(_ARRAY_TYPE));
    ARRAY_ASSERT_ALIGNED(ptr->data);
    return ptr;
}

static inline
FN_ATTR_WARN_UNUSED_RESULT
_SMART_ARRAY_T*
_MATRIX_FN(new)(size_t rows, size_t cols)
{
    _SMART_ARRAY_T* ptr = _SARRAY_FN(heap_new)(rows * cols);
    ptr->num_cols = cols;
    return ptr;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_ARRAY_FN(get_at)(size_t len, const _ARRAY_TYPE a[len], size_t pos)
{
    assert(pos < len); // access with bounds checking

    return a[pos];
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_SARRAY_FN(get_at)(const _SMART_ARRAY_T* a, size_t pos)
{
    return _ARRAY_FN(get_at)(a->len, a->data, pos);
}

static inline
_ARRAY_WO(2, 1)
void
_ARRAY_FN(set_at)(size_t len, _ARRAY_TYPE a[len], size_t pos, _ARRAY_TYPE val)
{
#ifdef _ASSERT_H
    assert(pos < len); // access with bounds checking
#endif
    a[pos] = val;
}

static inline
__attribute__((nonnull(1)))
void
_SARRAY_FN(set_at)(_SMART_ARRAY_T* a, size_t pos, _ARRAY_TYPE val)
{
    _ARRAY_FN(set_at)(a->len, a->data, pos, val);
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_MATRIX_FN(get_at)(const _SMART_ARRAY_T* a, size_t row, size_t col)
{
    return a->data[matrix_index(row , col, a->num_cols)];
}

static inline
__attribute__((nonnull(1)))
void
_MATRIX_FN(set_at)(_SMART_ARRAY_T* a, size_t row, size_t col, _ARRAY_TYPE val)
{
    a->data[matrix_index(row , col, a->num_cols)] = val;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
optional_uint_t
_ARRAY_FN(find)(size_t len, const _ARRAY_TYPE a[len], _ARRAY_TYPE val_to_find)
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    optional_uint_t pos = {.present = false};


    constexpr size_t step_size = 32;
    size_t steps = len / step_size;
    size_t alen = steps * step_size;

    for (size_t i = 0; i < alen; i += step_size) {
        long unsigned int positions = 0;
        for (size_t j = 0; j < step_size; ++j) {
            positions |= (_ARRAY_TYPE_EQ(a[i+j], val_to_find)? 1:0) << j;
        }
        int found = __builtin_popcountl(positions);
        if (found) {
            pos.present = true;
            pos.value = i + __builtin_ffsl(positions) - 1;
            return pos;
        }
    }

    for (size_t i = alen; i < len; ++i) {
        if (_ARRAY_TYPE_EQ(a[i], val_to_find)) {
            pos.present = true;
            pos.value = i;
            break;
        }
    }

    return pos;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
optional_uint_t
_SARRAY_FN(find)(_SMART_ARRAY_T* a, _ARRAY_TYPE val)
{
    return _ARRAY_FN(find)(a->len, a->data, val);
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
bool
_ARRAY_FN(contains)(size_t len, const _ARRAY_TYPE a[len], _ARRAY_TYPE val_to_find)
{
    auto pos = _ARRAY_FN(find)(len, a, val_to_find);

    return pos.present;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
size_t
_ARRAY_FN(find_max)(size_t len, const _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    size_t pos = 0;
    _ARRAY_TYPE max_val = a[0];

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
_SARRAY_FN(find_max)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(find_max)(a->len, a->data);
}

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) FN_ATTR_WARN_UNUSED_RESULT
bool
_ARRAY_FN(equal)(size_t len, const _ARRAY_TYPE a[len], const _ARRAY_TYPE b[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    ARRAY_ASSERT_ALIGNED(b);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);
    b = __builtin_assume_aligned(b, _SMART_ARRAY_ALIGN);

    bool equal = true;

    for (size_t i = 0; i < len; ++i) {
        if (!_ARRAY_TYPE_EQ(a[i], b[i])) {
            equal = false;
            break;
        }
    }

    return equal;
}

static inline
_ARRAY_RO(2, 1) _ARRAY_WO(3, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(copy)(size_t len, const _ARRAY_TYPE src[len], _ARRAY_TYPE dst[len])
{
    ARRAY_ASSERT_ALIGNED(src);
    ARRAY_ASSERT_ALIGNED(dst);
    src = __builtin_assume_aligned(src, _SMART_ARRAY_ALIGN);
    dst = __builtin_assume_aligned(dst, _SMART_ARRAY_ALIGN);

    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }

    return dst;
}

static inline
_ARRAY_RO(2, 1) _ARRAY_WO(3, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(memcopy)(size_t len, const _ARRAY_TYPE src[len], _ARRAY_TYPE dst[len])
{
    ARRAY_ASSERT_ALIGNED(src);
    ARRAY_ASSERT_ALIGNED(dst);
    src = __builtin_assume_aligned(src, _SMART_ARRAY_ALIGN);
    dst = __builtin_assume_aligned(dst, _SMART_ARRAY_ALIGN);

    return (_ARRAY_TYPE*) __builtin_memcpy(dst, src, len * sizeof(dst[0]));
}

static inline
_ARRAY_WO(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(fill)(size_t len, _ARRAY_TYPE a[len], _ARRAY_TYPE val)
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    for (size_t i = 0; i < len; ++i) {
        a[i] = val;
    }

    return a;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(fill)(_SMART_ARRAY_T* a, _ARRAY_TYPE val)
{
    return _ARRAY_FN(fill)(a->len, a->data, val);
}

static inline
_ARRAY_RW(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(insertion_sort)(size_t len, _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    size_t j;
    _ARRAY_TYPE key;

    for (size_t i = 1; i < len; ++i) {
        key = a[i];
        j = i;
 
        /* Shift elements of a[0..i-1], that are GT key, to one position right */
        while (j > 0 && _ARRAY_TYPE_LT(key, a[j - 1])) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }

    return a;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(insertion_sort)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(insertion_sort)(a->len, a->data);
}

static inline
__attribute__((nonnull(1, 2)))
void
_ARRAY_FN(swap_two_pointers)(_ARRAY_TYPE* a, _ARRAY_TYPE* b)
{
    _ARRAY_TYPE tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline
_ARRAY_RW(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(bubble_sort)(size_t len, _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    _ARRAY_TYPE tmp;
    bool swapped;
    for (size_t step = 0; step < len - 1; ++step) {
        swapped = false;
        for (size_t i = 0; i < len - step - 1; ++i) {
            if (_ARRAY_TYPE_LT(a[i + 1], a[i]) ) {
                //_ARRAY_FN(swap_two_pointers)(&a[i], &a[i + 1]);
                tmp = a[i+1]; a[i+1] = a[i]; a[i] = tmp;
                swapped = true;
            }
        }
    
        // no swapping means the array is already sorted
        if (!swapped) {
            break;
        }
    }

    return a;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(bubble_sort)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(bubble_sort)(a->len, a->data);
}

static inline
FN_ATTR_WARN_UNUSED_RESULT
int
_ARRAY_FN(qsort_compare)(const void *restrict pa, const void *restrict pb)
{
    _ARRAY_TYPE a = *((_ARRAY_TYPE*)pa);
    _ARRAY_TYPE b = *((_ARRAY_TYPE*)pb);

    // (a-b) does not work for unsigned types
    return ( (a < b)? -1 : ( (a == b)? 0 : 1) );
}

static inline
_ARRAY_RW(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(qsort)(size_t len, _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    qsort(a, len, sizeof(_ARRAY_TYPE), _ARRAY_FN(qsort_compare));

    return a;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(qsort)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(qsort)(a->len, a->data);
}


static inline
_ARRAY_RW(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(random_sequence)(size_t len, _ARRAY_TYPE a[len])
{
    // make a sequence [0, 1, 2, 3, 4...]
    for (size_t i = 0; i < len; ++i) {
         a[i] = i;
    }

    // randomly shuffle elements of the original sequence
    for (size_t range = len; range > 1; --range) {
        size_t random_index = rand() % range;
        _ARRAY_FN(swap_two_pointers)(&a[random_index], &a[range - 1]);
    }

    return a;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(random_sequence)(_SMART_ARRAY_T* a)
{
    return _ARRAY_FN(random_sequence)(a->len, a->data);
}

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) _ARRAY_WO(4, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(add)(
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

    #pragma GCC ivdep
    for (size_t i = 0; i < len; ++i) {
         c[i] = a[i] + b[i];
    }
    return c;
}

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(add_destruct)(
    size_t len,
          _ARRAY_TYPE a[len],
    const _ARRAY_TYPE b[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    ARRAY_ASSERT_ALIGNED(b);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);
    b = __builtin_assume_aligned(b, _SMART_ARRAY_ALIGN);

    #pragma GCC ivdep
    for (size_t i = 0; i < len; ++i) {
         a[i] = a[i] + b[i];
    }
    return a;
}

static inline
FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_SARRAY_FN(add_destruct)(_SMART_ARRAY_T* a, _SMART_ARRAY_T* b)
{
    size_t len = (a->len < b->len)? a->len : b->len;
    return _ARRAY_FN(add_destruct)(len, a->data, b->data);
}

static inline
_ARRAY_RO(3, 1) _ARRAY_RO(6, 4) _ARRAY_WO(9, 7) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(matrix_matrix_multiply)(
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
    for (size_t ra = 0; ra < rows_a; ++ra) {
        for (size_t k = 0; k < cols_a; ++k) {
            for (size_t cb = 0; cb < cols_b; ++cb) {
                c[ra*cols_c + cb] +=
                    a[ra*cols_a + k] *
                    b[k*cols_b + cb];
            }
        }
    }

    return c;
}

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1,2,3)))
_ARRAY_TYPE*
_MATRIX_FN(matrix_multiply)(_SMART_ARRAY_T* a, _SMART_ARRAY_T* b, _SMART_ARRAY_T* c)
{
    return _ARRAY_FN(matrix_matrix_multiply)(
        a->len, a->num_cols, a->data,
        b->len, b->num_cols, b->data,
        c->len, c->num_cols, c->data);
}

#ifdef _ARRAY_OMP_ENABLE
#include "omp_array.inc.h"
#endif


#undef _SMART_ARRAY
#undef _SMART_ARRAY_T
#undef _ARRAY_TYPE_EQ
#undef _ARRAY_TYPE_LT
#undef _ARRAY_RO
#undef _ARRAY_WO
#undef _ARRAY_RW
#undef _ARRAY_FN
#undef _SARRAY_FN
#undef _MATRIX_FN
#undef PPCAT_NX
#undef PPCAT

#undef _ARRAY_TYPE
#undef _ARRAY_TYPE_NAME
