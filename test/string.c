#include "smartarr/defines.h"
#include "smartarr/string.h"

#include "third/greatest.h"

TEST test_append(void)
{
    SMART_STRING(s, 100);

    smart_string_append(&s, "Ai Ai!");

    //puts(smart_string_get(&s));
    ASSERT_EQ(0, smart_string_compare(&s, "Ai Ai!"));
    ASSERT(smart_string_equal(&s, "Ai Ai!"));
    ASSERT_FALSE(smart_string_equal(&s, "Aii Ai!"));

    PASS();
}

TEST test_find_first_substring(void)
{
    SMART_STRING_FROM(s, "Hello you!");
    //puts(smart_string_get(&s));

    smart_string_append(&s, " And you!");
    //puts(smart_string_get(&s));
    ASSERT(smart_string_equal(&s, "Hello you! And you!"));

    ASSERT(smart_string_find_first_char(&s, '!'));
    ASSERT(smart_string_find_first_char(&s, '?') == nullptr);

    //puts(smart_string_find_first_substring(&s, "And"));
    ASSERT(smart_string_contains(&s, "And"));
    ASSERT_FALSE(smart_string_contains(&s, "OR"));

    PASS();
}

SUITE(operations) {
    RUN_TEST(test_append);
    RUN_TEST(test_find_first_substring);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(operations);

    GREATEST_MAIN_END();
}
