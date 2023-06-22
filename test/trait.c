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

TRAIT(Shape,
    float (*area)(Shape*),
    float (*perimeter)(Shape*)
)

typedef TRAIT_IMPL(Rectangle, Shape)
    float width;
    float height;
} Rectangle;

static
float rectangle_area(Shape* shape) {
    Rectangle* rec = (Rectangle*)shape;
    return rec->width * rec->height;
}

TEST test_shapes(void)
{
    Rectangle rectangle = {
        .trait = {
            .method = {
                .area = rectangle_area,
                .perimeter = nullptr
            }
        },
        .width = 2.0,
        .height = 3.0
    };

    Shape* shape = (Shape*)&rectangle;
    ASSERT_EQ(2.0*3.0, shape->method.area(shape));
    //TODO perimeter

    //Circle circle;
    //Shape* shape = (Shape*)&circle;
    //ASSERT_EQ(2.0*3.0, shape->method.area(shape));
    //perimeter

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_TEST(test_trait_wo_methods);
    RUN_TEST(test_trait_one_method);
    RUN_TEST(test_shapes);

    GREATEST_MAIN_END();
}
