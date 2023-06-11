#include "smartarr/defines.h"

#define _ARRAY_TYPE int32_t
#define _ARRAY_TYPE_NAME i32
#include "smartarr/array.inc.h"

#include "smartarr/string.h"
#include "smartarr/third/list.h"

// see https://github.com/silentbicycle/greatest
#include "third/greatest.h"

TEST array(void)
{
    static i32_smart_array_t a = {3, 1, {4, 5, 6}};

    ASSERT_EQ(4, a.data[0]);
    ASSERT_EQ(4, i32_smart_array_get_at(&a, 0));
    ASSERT_EQm("must be 5 at a[5]", 5, a.data[1]);
    ASSERT_EQ_FMT(6, a.data[2], "%d");

    PASS(); // FAIL|SKIP
}

TEST string(void)
{
    SMART_STRING(s, 100);
    
    smart_string_append(&s, "Ai Ai!");
    //puts(smart_string_get(&s));
    ASSERT_EQ_FMT(strlen("Ai Ai!"), s.len, "%lu");
    ASSERT_EQ_FMT(0, smart_string_compare(&s, "Ai Ai!"), "%d");
    ASSERT_EQ(true, smart_string_equal(&s, "Ai Ai!"));

    PASS(); // FAIL|SKIP
}

SUITE(headers) {
    RUN_TEST(array);
    RUN_TEST(string);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(headers);

    GREATEST_MAIN_END();
}

