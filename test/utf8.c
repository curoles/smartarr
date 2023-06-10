#include <wchar.h>
#include <locale.h>
#include <stdio.h>

#include "smartarr/defines.h"
#include "smartarr/third/utf8.h"
#include "smartarr/utf8_string.h"

#include "third/greatest.h"

TEST test_printf(void)
{
    // can we print to stdout?
    printf("Celsius °C, Fahrenheit °F, can I print UTF8 string? %s\n", "¯\\_(ツ)_/¯");

    const char* s = "°C °F ¯\\_(ツ)_/¯";
    puts(s);

#if 0 // printitng to stderr works, but I donot want to mess stderr up in case some error is there
    /* Per the C standard, each FILE stream has associated with it an "orientation"
     * (either "byte" or "wide) which is set by the first operation performed on it,
     * and which can be inspected with the fwide function.
     */
    const wchar_t* us = L"wide °C °F ¯\\_(ツ)_/¯";
    fwprintf(stderr, L"%ls\n", us);

    wchar_t suits[5] = {
        0x2660, 0x2665, 0x2663, 0x2666, 0x000A
    };

    for (unsigned int i = 0; i < 5; ++i) {
        fwprintf(stderr, L"%lc", suits[i]);
    }
#endif

    PASS();
}

TEST test_utf8_h(void)
{
    const char* s = "°C °F ¯\\_(ツ)_/¯";
    printf("%s\nstrlen:%lu utf8len:%lu\n", s, strlen(s), utf8len(s));
    ASSERT_EQ(15, utf8len(s));
    ASSERT_EQ(utf8len(s) + 5 + 1, strlen(s)); // why extra +1?

    PASS();
}

TEST test_has_unicode(void)
{
    UTF8_STRING_FROM(s1, "ascii only");
    UTF8_STRING_FROM(s2, "°C °F ¯\\_(ツ)_/¯");

    ASSERT_FALSE(utf8_string_has_unicode(&s1));
    ASSERT(utf8_string_has_unicode(&s2));

    PASS();
}


SUITE(basic) {
    setlocale(LC_CTYPE, "en_US.UTF-8");
    RUN_TEST(test_printf);
    RUN_TEST(test_utf8_h);
    RUN_TEST(test_has_unicode);
}

TEST test_append(void)
{
    UTF8_STRING(s, 100);

    utf8_string_append(&s, "°¯\\_(ツ)_/¯°!");

    printf("size:%lu\n", utf8size_lazy("°¯\\_(ツ)_/¯°!"));
    char* p = "°¯\\_(ツ)_/¯°!";
    while (*p != '\0') {
        utf8_int32_t codepoint;
        char* next = utf8codepoint(p, &codepoint);
        //printf("cur:%p next:%p diff:%ld codepoint:%x\n", p, next, next-p, codepoint);
        p = next;
    }

    puts(utf8_string_get(&s));
    printf("len bytes: %lu nr_cdp: %lu\n", s.len, s.nr_cdp);

    ASSERT_EQ(0, utf8_string_compare(&s, "°¯\\_(ツ)_/¯°!"));
    ASSERT(utf8_string_equal(&s, "°¯\\_(ツ)_/¯°!"));
    ASSERT_FALSE(utf8_string_equal(&s, "°¯\\_(ツ)_/¯° °F"));

    PASS();
}

TEST test_append2(void)
{
    UTF8_STRING(s, 100);

    utf8_string_append(&s, "°¯\\_(ツ)_/¯°");

    //printf("size:%lu\n", utf8size_lazy("°¯\\_(ツ)_/¯°"));

    puts(utf8_string_get(&s));
    printf("len bytes: %lu nr_cdp: %lu\n", s.len, s.nr_cdp);

    ASSERT_EQ(0, utf8_string_compare(&s, "°¯\\_(ツ)_/¯°"));
    ASSERT(utf8_string_equal(&s, "°¯\\_(ツ)_/¯°"));
    ASSERT_FALSE(utf8_string_equal(&s, "!°¯\\_(ツ)_/¯°"));

    PASS();
}

/*TEST test_find_first_substring(void)
{
    UTF8_STRING_FROM(s, "Hello you!");
    //puts(utf8_string_get(&s));

    utf8_string_append(&s, " And you!");
    //puts(smart_string_get(&s));
    ASSERT(utf8_string_equal(&s, "Hello you! And you!"));

    ASSERT(utf8_string_find_first_char(&s, '!'));
    ASSERT(utf8_string_find_first_char(&s, '?') == nullptr);

    //puts(smart_string_find_first_substring(&s, "And"));
    ASSERT(utf8_string_contains(&s, "And"));
    ASSERT_FALSE(utf8_string_contains(&s, "OR"));

    PASS();
}*/

TEST UNUSED test_read_file(void)
{
    UTF8_STRING(s, 100);

    int res = utf8_string_read_from_file(&s, "../smartarr/test/utf8.c");
    if (res < 0) {
        FAIL();
    }

    //puts(utf8_string_get(&s));

    ASSERT_EQ(0, utf8valid(s.storage));

    printf("capacity: %lu len: %lu cdp: %lu\n", s.capacity, s.len, s.nr_cdp);
    ASSERT(utf8_string_has_unicode(&s));

    PASS();
}

SUITE(functions) {
    setlocale(LC_CTYPE, "en_US.UTF-8");
    RUN_TEST(test_append);
    RUN_TEST(test_append2);
    //RUN_TEST(test_read_file);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(basic);
    RUN_SUITE(functions);

    GREATEST_MAIN_END();
}

