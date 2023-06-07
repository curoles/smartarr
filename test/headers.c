#include "smartarr/defines.h"

// see https://github.com/silentbicycle/greatest
#include "third/greatest.h"

TEST dummy(void) {
#if 0
    int x = 1;
    /* Compare, with an automatic "1 != x" failure message */
    ASSERT_EQ(1, x);

    /* Compare, with a custom failure message */
    ASSERT_EQm("Yikes, x doesn't equal 1", 1, x);

    /* Compare, and if they differ, print both values,
     * formatted like `printf("Expected: %d\nGot: %d\n", 1, x);` */
    ASSERT_EQ_FMT(1, x, "%d");
#endif
    PASS(); // FAIL|SKIP
}

SUITE(headers) {
    RUN_TEST(dummy);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    //RUN_SUITE(headers);
    RUN_TEST(dummy);

    GREATEST_MAIN_END();
}

