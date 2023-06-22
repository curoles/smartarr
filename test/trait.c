#include "smartarr/defines.h"
#include "smartarr/trait.h"

#include "third/greatest.h"

TRAIT(Class0)

TRAIT(Class1,
    int (*method1)(void)
)


TEST test_trait_wo_methods(void)
{
    Class0 class0;
    static_assert(sizeof(class0) == 0);
    ASSERT_EQ(0, sizeof(class0));

    PASS();
}

static
int answer_is_42(void) { return 42; }

TEST test_trait_one_method(void)
{
    Class1 class1 = {.method = {.method1 = answer_is_42}};
    ASSERT_EQ(42, class1.method.method1());

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_TEST(test_trait_wo_methods);
    RUN_TEST(test_trait_one_method);

    GREATEST_MAIN_END();
}
