#include "smartarr/defines.h"

#define _ARRAY_OMP_ENABLE
#define _ARRAY_DEBUG
#include "smartarr/basic_type_array.h"

#include "third/greatest.h"

TEST test_matrix_matrix_mul(void)
{
    constexpr size_t ca = 2;
    constexpr size_t ra = 4;
    constexpr size_t cb = 3;
    constexpr size_t rb = 2;

    ATTR_SMART_ARRAY_ALIGNED double a[ra*ca] = {1,2,3,4,5,6,7,8};
    ATTR_SMART_ARRAY_ALIGNED double b[rb*cb] = {1,2,3,4,5,6};
    ATTR_SMART_ARRAY_ALIGNED double c[ra*cb] = {};

    f64_array_matrix_matrix_multiply(
        ra*ca, ca, a,
        rb*cb, cb, b,
        ra*cb, cb, c
    );

    ASSERT_EQ(
        a[matrix_index(0,0,ca)]*b[matrix_index(0,1,cb)] + a[matrix_index(0,1,ca)]*b[matrix_index(1,1,cb)],
        c[matrix_index(0,1,cb)]);
    ASSERT_EQ(
        a[matrix_index(2,0,ca)]*b[matrix_index(0,2,cb)] + a[matrix_index(2,1,ca)]*b[matrix_index(1,2,cb)],
        c[matrix_index(2,2,cb)]);

    PASS();
}

TEST test_smart_matrix_matrix_mul(void)
{
    constexpr size_t ca = 2;
    constexpr size_t ra = 4;
    constexpr size_t cb = 3;
    constexpr size_t rb = 2;

    static f64_smart_array_t a = {ra*ca, ca, {1,2,3,4,5,6,7,8}};
    static f64_smart_array_t b = {rb*cb, cb, {1,2,3,4,5,6}};
    static f64_smart_array_t c = {ra*cb, cb, {[0 ... 11]=0}};

    f64_matrix_matrix_multiply(&a, &b, &c);

    #define _EL(a,r,c) f64_matrix_get_at(&a,r,c)
    ASSERT_EQ(
        _EL(a,0,0)*_EL(b,0,1) + _EL(a,0,1)*_EL(b,1,1),
        _EL(c,0,1));
    ASSERT_EQ(
        _EL(a,2,0)*_EL(b,0,2) + _EL(a,2,1)*_EL(b,1,2),
        _EL(c,2,2));
    #undef _EL

    PASS();
}

TEST test_omp_matrix_matrix_mul(void)
{
    constexpr size_t ca = 2;
    constexpr size_t ra = 4;
    constexpr size_t cb = 3;
    constexpr size_t rb = 2;

    static f64_smart_array_t a = {ra*ca, ca, {1,2,3,4,5,6,7,8}};
    static f64_smart_array_t b = {rb*cb, cb, {1,2,3,4,5,6}};
    static f64_smart_array_t c = {ra*cb, cb, {[0 ... 11]=0}};

    f64_omp_matrix_matrix_multiply(&a, &b, &c);

    #define _EL(a,r,c) f64_matrix_get_at(&a,r,c)
    ASSERT_EQ(
        _EL(a,0,0)*_EL(b,0,1) + _EL(a,0,1)*_EL(b,1,1),
        _EL(c,0,1));
    ASSERT_EQ(
        _EL(a,2,0)*_EL(b,0,2) + _EL(a,2,1)*_EL(b,1,2),
        _EL(c,2,2));
    #undef _EL

    PASS();
}

SUITE(matrix_operations) {
    RUN_TEST(test_matrix_matrix_mul);
    RUN_TEST(test_smart_matrix_matrix_mul);
    RUN_TEST(test_omp_matrix_matrix_mul);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(matrix_operations);

    GREATEST_MAIN_END();
}
