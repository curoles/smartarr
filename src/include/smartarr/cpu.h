#pragma once

#include <stddef.h>

#ifdef __x86_64
    #define SMARTARR_L1_DCACHE_CL_SIZE 64u
    #define SMARTARR_L1_DCACHE_SIZE_KB 32u
    constexpr size_t SMARTARR_SIMD_VLEN =
        #ifdef __AVX512__
            (512/8);
        #elifdef __AVX2__
            (256/8);
        #else
            (64/8);
        #endif
#else
    #pragma error "Unknown architecture"
#endif

constexpr size_t SMARTARR_L1_DCACHE_SIZE = SMARTARR_L1_DCACHE_SIZE_KB * 1024;
