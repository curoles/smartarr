#include "smartarr/defines.h"

#define _ARRAY_TYPE int
#define _ARRAY_TYPE_NAME int
#include "smartarr/array.inc.h"

// see https://github.com/silentbicycle/greatest
#include "third/greatest.h"

int_smart_array_t global_a = {3, {1, 2, 3}};

TEST make_global(void)
{
    ASSERT_EQ(1, global_a.data[0]);
    ASSERT_EQ(2, global_a.data[1]);
    ASSERT_EQ(3, global_a.data[2]);

    PASS();
}

TEST make_static(void)
{
    static int_smart_array_t static_b = {3, {4, 5, 6}};

    ASSERT_EQ(4, static_b.data[0]);
    ASSERT_EQ(4, int_smart_array_get_at(&static_b, 0));
    ASSERT_EQ(5, static_b.data[1]);
    ASSERT_EQ(6, static_b.data[2]);

    static int_smart_array_t static_c = {3, {[0 ... 2]=7}};

    ASSERT_EQ(7, static_c.data[0]);
    ASSERT_EQ(7, static_c.data[1]);
    ASSERT_EQ(7, static_c.data[2]);

    //printf("sizeof(int_smart_array_t)=%lu\n", sizeof(int_smart_array_t));
    ASSERT_EQm("xxx_smart_array_t struct size must >= then the VLA array alignment\n",
        _SMART_ARRAY_ALIGN, sizeof(int_smart_array_t));

    PASS();
}

TEST make_on_stack(void)
{
    int_smart_array_t* a = smart_array_stack_new(int_smart_array_t, int, 100);

    ASSERT_NEQ(nullptr, a);

    ASSERT_EQ(100, a->len);

    int* d = a->data;

    //printf("stack allocated array data pointer %p\n", d);
    ASSERT_EQ(0, ((size_t)d & (_SMART_ARRAY_ALIGN - 1)));
    ARRAY_ASSERT_ALIGNED(d);

    for (unsigned int i = 0; i < a->len; ++i) {
        d[i]=i;
    }
    for (unsigned int i = 0; i < a->len; ++i) {
        ASSERT_EQ((int)i, d[i]);
    }

    PASS();
}

TEST make_on_heap(void)
{
    auto_free int_smart_array_t* a = int_smart_array_heap_new(100);

    ASSERT_NEQ(nullptr, a);

    ASSERT_EQ(100, a->len);

    int* d = a->data;

    //printf("heap allocated array data pointer %p\n", d);
    ASSERT_EQ(0, ((size_t)d & (_SMART_ARRAY_ALIGN - 1)));
    ARRAY_ASSERT_ALIGNED(d);

    for (unsigned int i = 0; i < a->len; ++i) {
        d[i]=i;
    }
    for (unsigned int i = 0; i < a->len; ++i) {
        ASSERT_EQ((int)i, d[i]);
    }

    PASS();
}

SUITE(make_arrays) {
    RUN_TEST(make_global);
    RUN_TEST(make_static);
    RUN_TEST(make_on_stack);
    RUN_TEST(make_on_heap);
}

TEST array_fill(void)
{
    ATTR_SMART_ARRAY_ALIGNED
    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto pos = int_array_find(10, a, 7);
    ASSERT_EQ(true, pos.present);
    ASSERT_EQ(7, pos.value);

    pos = int_array_find(10, a, 77);
    ASSERT_FALSE(pos.present);

    ASSERT_EQ(true, int_array_contains(10, a, 5));
    ASSERT_EQ(false, int_array_contains(10, a, 55));

    ASSERT_EQ(true, int_array_equal(10, a, a));

    ATTR_SMART_ARRAY_ALIGNED
    int b[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    ASSERT_EQ(false, int_array_equal(10, a, b));

    ATTR_SMART_ARRAY_ALIGNED
    int c[10] = {};

    int_array_copy(10, a, c);
    ASSERT_EQ(true, int_array_equal(10, a, c));

    int_array_memcopy(10, b, c);
    ASSERT_EQ(false, int_array_equal(10, a, c));
    ASSERT_EQ(true, int_array_equal(10, b, c));

    int_array_fill(10, c, 777);
    ASSERT_EQ(777, int_array_get_at(10, c, 3));

    int_array_set_at(10, c, 3, 888);
    ASSERT_EQ(888, int_array_get_at(10, c, 3));

    PASS();
}

TEST array_sort(void)
{
    auto_free int_smart_array_t* ee = int_smart_array_heap_new(100);
    ee->data[5] = 55;

    auto_free void* misal1 = malloc(10*64+1);
    auto_free void* misal2 = malloc(21*64+2);
    auto_free void* misal3 = malloc(23*64+3);

    auto_free int_smart_array_t* e = int_smart_array_heap_realloc(ee, 1000);

    //printf("heap re-allocated array data pointer %p\n", e->data);
    ASSERT_EQ(0, ((size_t)e->data & (_SMART_ARRAY_ALIGN - 1)));

    // realloction must preserve values
    ASSERT_EQ(55, e->data[5]);

    for (unsigned int i = 0; i < e->len; ++i) {
        e->data[i] = e->len - i;
    }

    int_smart_array_insertion_sort(e);
    for (unsigned int i = 1; i < e->len; ++i) {
        //printf("a[%u]:%d <= a[%u]:%d\n", i-1, e->data[i-1], i, e->data[i]);
        ASSERT_LT(e->data[i-1], e->data[i]);
    }

    for (unsigned int i = 0; i < e->len; ++i) {
        e->data[i] = e->len - i;
    }

    int_smart_array_bubble_sort(e);
    for (unsigned int i = 1; i < e->len; ++i) {
        //printf("a[%u]:%d <= a[%u]:%d\n", i-1, e->data[i-1], i, e->data[i]);
        ASSERT_LT(e->data[i-1], e->data[i]);
    }

    // so they not optimized out
    ASSERT_NEQ(nullptr, misal1);
    ASSERT_NEQ(nullptr, misal2);
    ASSERT_NEQ(nullptr, misal3);

    PASS();
}

SUITE(array_functions) {
    RUN_TEST(array_fill);
    RUN_TEST(array_sort);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(make_arrays);
    RUN_SUITE(array_functions);

    GREATEST_MAIN_END();
}

