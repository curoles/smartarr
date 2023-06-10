#include <omp.h>

#define _OMP_ARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_array_, name))
#define _OMP_SARRAY_FN(name) PPCAT(_ARRAY_TYPE_NAME, PPCAT(_omp_smart_array_, name))

static inline
_ARRAY_RO(2, 1) _ARRAY_RO(3, 1) _ARRAY_WO(4, 1) FN_ATTR_RETURNS_NONNULL
_ARRAY_TYPE*
_OMP_ARRAY_FN(add)(
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

    #pragma omp parallel for
    for (unsigned int i = 0; i < len; ++i) {
         c[i] = a[i] + b[i];
    }
    return c;
}

#undef _OMP_ARRAY_FN
#undef _OMP_SARRAY_FN
