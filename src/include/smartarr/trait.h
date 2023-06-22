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



#define TRAIT(Name, ...) \
    typedef struct Name Name; \
    typedef struct Trait##Name { \
        _ADD_SEMICOL_FOR_EACH(__VA_ARGS__) \
    } Trait##Name; \
    struct Name {Trait##Name method;};

#define TRAIT_IMPL(Name, Trait) \
    struct Name { Trait trait;


