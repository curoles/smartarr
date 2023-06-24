/**@file
 * @brief     Macros to implement simple trait object model.
 * @author    Igor Lesik 2023
 * @copyright Igor Lesik 2023
 *
 * Example of using trait macros:
 *
 * ```
 * // Define Shape trait with 2 methods.
 * TRAIT(Shape,
 *     float (*area)(Shape*),
 *     float (*perimeter)(Shape*)
 * )
 * 
 * // Rectangle implements trait Shape.
 * typedef TRAIT_IMPL(Rectangle, Shape)
 *     float width;
 *     float height;
 * } Rectangle;
 * 
 * // Area of rectangle.
 * static float rectangle_area(Shape* shape) {
 *     Rectangle* rec = (Rectangle*)shape;
 *     return rec->width * rec->height;
 * }
 * 
 * // Perimeter of rectangle.
 * static float rectangle_perimeter(Shape* shape) {
 *     Rectangle* rec = (Rectangle*)shape;
 *     return (rec->width + rec->height) * 2;
 * }
 * 
 * // Circle implements trait Shape.
 * typedef TRAIT_IMPL(Circle, Shape)
 *     float radius;
 * } Circle;
 * 
 * static float circle_area(Shape* shape) {
 *     Circle* circle = (Circle*)shape;
 *     return circle->radius * circle->radius * 2.0 * 3.14;
 * }
 * 
 * static float circle_perimeter(Shape* shape) {
 *     Circle* circle = (Circle*)shape;
 *     return circle->radius * 2.0 * 3.14;
 * }
 * 
 * TEST test_shapes(void)
 * {
 *     Rectangle rectangle = {
 *         .trait = {
 *             .method = {
 *                 .area = rectangle_area,
 *                 .perimeter = rectangle_perimeter
 *             }
 *         },
 *         .width = 2.0,
 *         .height = 3.0
 *     };
 * 
 *     Shape* shape = (Shape*)&rectangle;
 *     ASSERT_EQ(2.0*3.0, shape->method.area(shape));
 *     ASSERT_EQ((2.0 + 3.0)*2, shape->method.perimeter(shape));
 * 
 *     Circle circle = {
 *         .trait = {
 *             .method = {
 *                 .area = circle_area,
 *                 .perimeter = circle_perimeter
 *             }
 *         },
 *         .radius = 3.0
 *     };
 *     shape = (Shape*)&circle;
 *     ASSERT_EQ_FMT(3.0f*3.0f*2.0f*3.14f, shape->method.area(shape), "%f");
 *     ASSERT_EQ(3.0f*2.0f*3.14f, shape->method.perimeter(shape));
 * 
 *     PASS();
 * }
 * ```
 *
 * Example of a class implementing multiple traits:
 *
 * ```
 * TRAIT(Color,
 *     int (*get)(Color*)
 * )
 * 
 * // ColoredRectangle implements trait Shape and trait Color.
 * typedef struct ColoredRectangle
 * {
 *     Color color;
 *     Shape shape;
 * 
 *     float width;
 *     float height;
 *     int color_id;
 * } ColoredRectangle;
 * 
 * static float colored_rectangle_perimeter(Shape* shape) {
 *     ColoredRectangle* rec = container_of(shape, ColoredRectangle, shape);
 *     return (rec->width + rec->height) * 2;
 * }
 * 
 * static float colored_rectangle_area(Shape* shape) {
 *     ColoredRectangle* rec = container_of(shape, ColoredRectangle, shape);
 *     return (rec->width * rec->height);
 * }
 * 
 * static int colored_rectangle_get_color(Color* color) {
 *     ColoredRectangle* rec = container_of(color, ColoredRectangle, color);
 *     return rec->color_id;
 * }
 * 
 * TEST test_colored_shapes(void)
 * {
 *     ColoredRectangle rectangle = {
 *         .color = {
 *             .method = {
 *                 .get = colored_rectangle_get_color
 *             }
 *         },
 *         .shape = {
 *             .method = {
 *                 .area = colored_rectangle_area,
 *                 .perimeter = colored_rectangle_perimeter
 *             }
 *         },
 *         .width = 2.0,
 *         .height = 3.0,
 *         .color_id = 42
 *     };
 * 
 *     Shape* colored_shape = &rectangle.shape;
 *     ASSERT_EQ(2.0*3.0, colored_shape->method.area(colored_shape));
 *     ASSERT_EQ((2.0 + 3.0)*2, colored_shape->method.perimeter(colored_shape));
 * 
 *     Color* shape_color = &rectangle.color;
 *     ASSERT_EQ(42, shape_color->method.get(shape_color));
 * 
 *     PASS();
 * }
 * ```
 */

#pragma once

#include "smartarr/defines.h"

#define _ADD_SEMICOL0()
#define _ADD_SEMICOL1(x) x;
#define _ADD_SEMICOL2(x, ...) x; _ADD_SEMICOL1(__VA_ARGS__)
#define _ADD_SEMICOL3(x, ...) x; _ADD_SEMICOL2(__VA_ARGS__)
#define _ADD_SEMICOL4(x, ...) x; _ADD_SEMICOL3(__VA_ARGS__)
#define _ADD_SEMICOL5(x, ...) x; _ADD_SEMICOL4(__VA_ARGS__)
#define _ADD_SEMICOL6(x, ...) x; _ADD_SEMICOL5(__VA_ARGS__)
#define _ADD_SEMICOL7(x, ...) x; _ADD_SEMICOL6(__VA_ARGS__)
#define _ADD_SEMICOL8(x, ...) x; _ADD_SEMICOL7(__VA_ARGS__)
#define _ADD_SEMICOL9(x, ...) x; _ADD_SEMICOL8(__VA_ARGS__)
#define _ADD_SEMICOL10(x, ...) x; _ADD_SEMICOL9(__VA_ARGS__)
#define _ADD_SEMICOL11(x, ...) x; _ADD_SEMICOL10(__VA_ARGS__)
#define _ADD_SEMICOL12(x, ...) x; _ADD_SEMICOL11(__VA_ARGS__)
#define _ADD_SEMICOL13(x, ...) x; _ADD_SEMICOL12(__VA_ARGS__)

// based on https://codecraft.co/2014/11/25/variadic-macros-tricks/
#define _ADD_SEMICOL_FOR_EACH(...) \
    _GET_NTH_ARG("ignored", ##__VA_ARGS__, \
    _ADD_SEMICOL13, _ADD_SEMICOL12, _ADD_SEMICOL11, \
    _ADD_SEMICOL10, _ADD_SEMICOL9, _ADD_SEMICOL8, \
    _ADD_SEMICOL7, _ADD_SEMICOL6, _ADD_SEMICOL5, \
    _ADD_SEMICOL4, _ADD_SEMICOL3, _ADD_SEMICOL2, \
    _ADD_SEMICOL1, _ADD_SEMICOL0)(__VA_ARGS__)


/** Trait is a struct with function pointers.
 *
 * Example:
 *
 * ```
 * // Define Shape trait with 2 methods.
 * TRAIT(Shape,
 *     float (*area)(Shape*),
 *     float (*perimeter)(Shape*)
 * )
 * ```
 *
 */
#define TRAIT(Name, ...) \
    typedef struct Name Name; \
    typedef struct Trait##Name { \
        _ADD_SEMICOL_FOR_EACH(__VA_ARGS__) \
    } Trait##Name; \
    struct Name {Trait##Name method;};

/** Aggregate trait struct iside implementation struct.
 *
 * Example:
 *
 * ```
 * typedef TRAIT_IMPL(Rectangle, Shape)
 *     float width;
 *     float height;
 * } Rectangle;
 * 
 * static float rectangle_area(Shape* shape) {
 *     Rectangle* rec = (Rectangle*)shape;
 *     return rec->width * rec->height;
 * }
 *
 * Rectangle rectangle = {
 *     .trait = {
 *         .method = {
 *             .area = rectangle_area,
 *             .perimeter = rectangle_perimeter
 *         }
 *     },
 *     .width = 2.0,
 *     .height = 3.0
 * };
 * 
 * Shape* shape = (Shape*)&rectangle;
 * ASSERT_EQ(2.0*3.0, shape->method.area(shape));
 *
 * ```
 */
#define TRAIT_IMPL(Name, Trait) \
    struct Name { Trait trait;


