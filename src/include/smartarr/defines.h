#pragma once

#include <stdlib.h>

#define STDC17 201710L

#if __STDC_VERSION__ <= STDC17
    #pragma error "C23 is required!"
#endif

#define FN_ATTR_CONST               __attribute__ ((const))
#define FN_ATTR_PURE                __attribute__ ((pure))
#define FN_ATTR_WARN_UNUSED_RESULT  __attribute__ ((warn_unused_result))
#define FN_ATTR_RETURNS_NONNULL     __attribute__ ((returns_nonnull))


/* Macro to define GCC extention "transparent union".
 *
 * Example:
 * ```
 * typedef transparent_union(Num64) {unsigned long i; double f;} Num64;
 * void transunion_print(Num64 n) {printf("%lx %f\n", n.i, n.f);}
 * transunion_print(1.0); // output: 3ff0000000000000 1.000000
 * transunion_print(0x4000000000000000UL); // output: 4000000000000000 2.000000
 * ```
 */
#define transparent_union(name) union __attribute__((__transparent_union__)) name

/* C23 enum with specific underlying type.
 *
 * Example:
 * ```
 * typedef_enum(Rgb, unsigned char) {
 *   RED, GREEN=256 error, BLUE=-1 error
 * } Rgb;
 * ```
 */
#define typedef_enum(name, underlying_type) \
    typedef enum name : underlying_type

/* Macro to define variable with auto cleanup function
 * that is called when the variable lifetime is over.
 *
 * Example:
 * ```
 * void cleanup1(float** p) {printf("cleanup1 ptr=%p\n", *p);}
 * auto_cleanup(cleanup1, float*) a = nullptr;
 * ```
 */
#define auto_cleanup(fn, T) __attribute__((cleanup(fn))) T

/* To be used as `__attribute__((cleanup(cleanup_free)))`
 *
 */
static inline void cleanup_free(void* p) {
    free(*(void**)p);
}

/* Automatically free heap allocated memory.
 *
 * Example:
 * ```
 * auto_free array_int_t* heap_a = smart_array_heap_new(int, 100, malloc);
 * ```
 */
#define auto_free __attribute__((cleanup(cleanup_free)))


/* Count number of trailing zero bits in an integer.
 *
 */
#define ctz(x) _Generic( (x), \
                   unsigned int: __builtin_ctz, \
                   unsigned long: __builtin_ctzl, \
                   default: __builtin_ctz \
                )(x)

/* For loop over each set bit in an integer.
 *
 * Example:
 * ```
 *   __auto_type b = 0b1011000u;
 *   for_each_bit(b, pos) {
 *       printf("pos=%u bits=%b\n", pos, b);
 *   }
 * ```
 * pos=3 bits=1011000
 * pos=4 bits=1010000
 * pos=6 bits=1000000
 */
#define for_each_bit(bits, pos) \
    for (unsigned int pos = ctz(bits); bits; \
        bits &= bits - 1, pos = ctz(bits))

/* IF-ELSE statement that returns a value. 
 *
 * A compound statement enclosed in parentheses may appear as an expression in GNU C.
 *
 * Example:
 * ```
 * int x = 15;
 * enum Size {SMALL, MEDIUM, BIG};
 * enum Size sz = return_if (x < 10) {
 *     SMALL;
 * }
 * return_else_if (x < 20) {
 *     MEDIUM;
 * }
 * return_else {
 *     BIG;
 * }));
 * assert(sz == MEDIUM);
 * ```
 */
#define return_if(cond) (cond) ? (
#define return_else ):(
#define return_else_if(cond) ):( (cond) ? (

/* Macro to define Optional value type.
 *
 * Example:
 * ```
 * typedef optional_type(int) optional_int_t;
 * optional_int_t test_optional(int i) {
 *   return (i > 0)? (optional_int_t){.present=true, .value=i} :
 *                   (optional_int_t){.present=false};
 * }
 *
 * __auto_type a = test_optional(10);
 * assert(a.present && a.value == 10);
 * a = test_optional(-10);
 * assert(!a.present);
 * ```
 */
#define optional_type(T) struct { T value; bool present; }

typedef optional_type(int) optional_int_t;
typedef optional_type(unsigned int) optional_uint_t;

/* Macro to define value type with an error.
 *
 * Example:
 * ```
 * typedef error_value_type(float, int) err_float_t;
 * err_float_t test_err_value(int i) {
 *   return (i > 0)? (err_float_t){.value=(float)i, .iserr=false} :
 *                   (err_float_t){.error=7, .iserr=true};
 * }
 *
 * __auto_type a = test_err_value(10);
 * assert(!a.iserr && a.value == 10.0);
 * a = test_err_value(-10);
 * assert(a.iserr && a.error==7);
 * ```
 */
#define error_value_type(T, E) struct { union {T value; E error;}; bool iserr; }

/* Find parent struct pointer by its child pointer.
 *
 * Example:
 * ```
 * struct A {
 *     struct B {} b;
 *     struct C {} c;
 *     struct D {
 *         struct E {} e;
 *     } d;
 * };
 * 
 * int test_offsetof(void)
 * {
 *     struct A a;
 * 
 *     struct B* b = &a.b;
 *     struct C* c = &a.c;
 *     struct D* d = &a.d;
 *     struct E* e = &d->e;
 * 
 *     assert(container_of(b, struct A, b) == &a);
 *     assert(container_of(c, struct A, c) == &a);
 *     assert(container_of(d, struct A, d) == &a);
 *     assert(container_of(e, struct D, e) ==  d);
 * 
 *     return 0;
 * }
 * ```
 */
#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

/* Return number of elements in fixed length array.
 *
 * Example:
 * ```
 * int a[10] = {};
 * assert(fixlen_array_len(a) == 10);
 * static_assert(fixlen_array_len(a) == 10);
 * float b[] = {1, 2.0, 3.};
 * assert(fixlen_array_len(b) == 3);
 * static_assert(fixlen_array_len(b) == 3);
 * ```
 */
#define fixlen_array_len(a) ((sizeof(a)) / (sizeof(a[0])))


