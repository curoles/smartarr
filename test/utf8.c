#include <wchar.h>
#include <locale.h>

#include "smartarr/defines.h"
#include "smartarr/third/utf8.h"

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


SUITE(basic) {
    setlocale(LC_CTYPE, "en_US.UTF-8");
    RUN_TEST(test_printf);
    RUN_TEST(test_utf8_h);
    //RUN_TEST();
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(basic);

    GREATEST_MAIN_END();
}

