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

// Define Shape trait with 2 methods.
TRAIT(Shape,
    float (*area)(Shape*),
    float (*perimeter)(Shape*)
)

// Rectangle implements trait Shape.
typedef TRAIT_IMPL(Rectangle, Shape)
    float width;
    float height;
} Rectangle;

// Area of rectangle.
static
float rectangle_area(Shape* shape) {
    Rectangle* rec = (Rectangle*)shape;
    return rec->width * rec->height;
}

// Perimeter of rectangle.
static
float rectangle_perimeter(Shape* shape) {
    Rectangle* rec = (Rectangle*)shape;
    return (rec->width + rec->height) * 2;
}

// Circle implements trait Shape.
typedef TRAIT_IMPL(Circle, Shape)
    float radius;
} Circle;

static
float circle_area(Shape* shape) {
    Circle* circle = (Circle*)shape;
    return circle->radius * circle->radius * 2.0 * 3.14;
}

static
float circle_perimeter(Shape* shape) {
    Circle* circle = (Circle*)shape;
    return circle->radius * 2.0 * 3.14;
}

TEST test_shapes(void)
{
    Rectangle rectangle = {
        .trait = {
            .method = {
                .area = rectangle_area,
                .perimeter = rectangle_perimeter
            }
        },
        .width = 2.0,
        .height = 3.0
    };

    Shape* shape = (Shape*)&rectangle;
    ASSERT_EQ(2.0*3.0, shape->method.area(shape));
    ASSERT_EQ((2.0 + 3.0)*2, shape->method.perimeter(shape));

    Circle circle = {
        .trait = {
            .method = {
                .area = circle_area,
                .perimeter = circle_perimeter
            }
        },
        .radius = 3.0
    };
    shape = (Shape*)&circle;
    ASSERT_EQ_FMT(3.0f*3.0f*2.0f*3.14f, shape->method.area(shape), "%f");
    ASSERT_EQ(3.0f*2.0f*3.14f, shape->method.perimeter(shape));

    PASS();
}

TRAIT(Color,
    int (*get)(Color*)
)

// ColoredRectangle implements trait Shape and trait Color.
typedef struct ColoredRectangle
{
    Color color;
    Shape shape;

    float width;
    float height;
    int color_id;
} ColoredRectangle;

static
float colored_rectangle_perimeter(Shape* shape) {
    ColoredRectangle* rec = container_of(shape, ColoredRectangle, shape);
    return (rec->width + rec->height) * 2;
}

static
float colored_rectangle_area(Shape* shape) {
    ColoredRectangle* rec = container_of(shape, ColoredRectangle, shape);
    return (rec->width * rec->height);
}

static
int colored_rectangle_get_color(Color* color) {
    ColoredRectangle* rec = container_of(color, ColoredRectangle, color);
    return rec->color_id;
}

TEST test_colored_shapes(void)
{
    ColoredRectangle rectangle = {
        .color = {
            .method = {
                .get = colored_rectangle_get_color
            }
        },
        .shape = {
            .method = {
                .area = colored_rectangle_area,
                .perimeter = colored_rectangle_perimeter
            }
        },
        .width = 2.0,
        .height = 3.0,
        .color_id = 42
    };

    Shape* colored_shape = &rectangle.shape;
    ASSERT_EQ(2.0*3.0, colored_shape->method.area(colored_shape));
    ASSERT_EQ((2.0 + 3.0)*2, colored_shape->method.perimeter(colored_shape));

    Color* shape_color = &rectangle.color;
    ASSERT_EQ(42, shape_color->method.get(shape_color));

    PASS();
}


GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_TEST(test_trait_wo_methods);
    RUN_TEST(test_trait_one_method);
    RUN_TEST(test_shapes);
    RUN_TEST(test_colored_shapes);

    GREATEST_MAIN_END();
}
