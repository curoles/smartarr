// Author: Igor Lesik 2023

#include <stddef.h>
#include <stdlib.h>

#include "smartarr/defines.h"
#include "smartarr/cpu.h"


#define PPCAT_NX(a, b) a ## b
#define PPCAT(a, b) PPCAT_NX(a, b)

#define _ARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_array_, name))
#define _SARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_smart_array_, name))

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

/* Smart_array knows its own length.
 *
 * Example:
 * ```
 * static int_smart_array_t static_b = {3, {4, 5, 6}};
 * assert(static_b.data[0] == 4);
 * assert(static_b.data[1] == 5);
 * assert(static_b.data[2] == 6);
 * static int_smart_array_t static_c = {3, {[0 ... 2]=7}};
 *
 * auto_free int_smart_array_t* heap_a = int_smart_array_heap_new(100, aligned_alloc);
 * d = heap_a->data;
 * for (int i = 0; i < heap_a->len; ++i) {d[i]=i;}
 * for (int i = 0; i < heap_a->len; ++i) {assert(d[i] == i);}
 * ```
 */
typedef struct _SMART_ARRAY {
    unsigned int len;
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
    ptr;})

/* Allocate smart_array on heap.
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
    __builtin_memcpy(ptr->data, self->data,
        ((self->len < ptr->len)? self->len : ptr->len) * sizeof(_ARRAY_TYPE));
    ARRAY_ASSERT_ALIGNED(ptr->data);
    return ptr;
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_ARRAY_FN(get_at)(unsigned int len, const _ARRAY_TYPE a[len], unsigned int pos)
{
#ifdef _ASSERT_H
    assert(pos < len); // access with bounds checking
#endif
    return a[pos];
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
_ARRAY_TYPE
_SARRAY_FN(get_at)(const _SMART_ARRAY_T* a, unsigned int pos)
{
    return _ARRAY_FN(get_at)(a->len, a->data, pos);
}

static inline
_ARRAY_WO(2, 1)
void
_ARRAY_FN(set_at)(unsigned int len, _ARRAY_TYPE a[len], unsigned int pos, _ARRAY_TYPE val)
{
#ifdef _ASSERT_H
    assert(pos < len); // access with bounds checking
#endif
    a[pos] = val;
}

static inline
__attribute__((nonnull(1)))
void
_SARRAY_FN(set_at)(_SMART_ARRAY_T* a, unsigned int pos, _ARRAY_TYPE val)
{
    _ARRAY_FN(set_at)(a->len, a->data, pos, val);
}

static inline
_ARRAY_RO(2, 1) FN_ATTR_WARN_UNUSED_RESULT
optional_uint_t
_ARRAY_FN(find)(unsigned int len, const _ARRAY_TYPE a[len], _ARRAY_TYPE val_to_find)
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    optional_uint_t pos = {.present = false};
    for (unsigned int i = 0; i < len; ++i) {
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
_ARRAY_FN(contains)(unsigned int len, const _ARRAY_TYPE a[len], _ARRAY_TYPE val_to_find)
{
    auto pos = _ARRAY_FN(find)(len, a, val_to_find);

    return pos.present;
}

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) FN_ATTR_WARN_UNUSED_RESULT
bool
_ARRAY_FN(equal)(unsigned int len, const _ARRAY_TYPE a[len], const _ARRAY_TYPE b[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    ARRAY_ASSERT_ALIGNED(b);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);
    b = __builtin_assume_aligned(b, _SMART_ARRAY_ALIGN);

    bool equal = true;

    for (unsigned int i = 0; i < len; ++i) {
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
_ARRAY_FN(copy)(unsigned int len, const _ARRAY_TYPE src[len], _ARRAY_TYPE dst[len])
{
    ARRAY_ASSERT_ALIGNED(src);
    ARRAY_ASSERT_ALIGNED(dst);
    src = __builtin_assume_aligned(src, _SMART_ARRAY_ALIGN);
    dst = __builtin_assume_aligned(dst, _SMART_ARRAY_ALIGN);

    for (unsigned int i = 0; i < len; ++i) {
        dst[i] = src[i];
    }

    return dst;
}

static inline
_ARRAY_RO(2, 1) _ARRAY_WO(3, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(memcopy)(unsigned int len, const _ARRAY_TYPE src[len], _ARRAY_TYPE dst[len])
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
_ARRAY_FN(fill)(unsigned int len, _ARRAY_TYPE a[len], _ARRAY_TYPE val)
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    for (unsigned int i = 0; i < len; ++i) {
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
_ARRAY_FN(insertion_sort)(unsigned int len, _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    unsigned int j;
    _ARRAY_TYPE key;

    for (unsigned int i = 1; i < len; ++i) {
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
_ARRAY_FN(bubble_sort)(unsigned int len, _ARRAY_TYPE a[len])
{
    ARRAY_ASSERT_ALIGNED(a);
    a = __builtin_assume_aligned(a, _SMART_ARRAY_ALIGN);

    _ARRAY_TYPE tmp;
    bool swapped;
    for (unsigned int step = 0; step < len - 1; ++step) {
        swapped = false;
        for (unsigned int i = 0; i < len - step - 1; ++i) {
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
_ARRAY_RW(2, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_ARRAY_FN(random_sequence)(unsigned int len, _ARRAY_TYPE a[len])
{
    for (unsigned int i = 0; i < len; ++i) {
         a[i] = i;
    }

    // randomly shuffle
    for (unsigned int range = len; range > 1; --range) {
        unsigned int random_index = rand() % range;
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
    unsigned int len,
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

    //#pragma GCC unroll 8
    #pragma GCC ivdep
    for (unsigned int i = 0; i < len; ++i) {
         c[i] = a[i] + b[i];
    }
    return c;
}

#undef _SMART_ARRAY
#undef _SMART_ARRAY_T
#undef _ARRAY_TYPE_EQ
#undef _ARRAY_TYPE_LT
#undef _ARRAY_RO
#undef _ARRAY_WO
#undef _ARRAY_RW
#undef _ARRAY_FN
#undef _SARRAY_FN
#undef PPCAT_NX
#undef PPCAT

#undef _ARRAY_TYPE
#undef _ARRAY_TYPE_NAME
