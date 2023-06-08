#pragma once

#include <stddef.h>

#ifdef __x86_64
    #define SMARTARR_L1_DCACHE_CL_SIZE 64u
    #define SMARTARR_L1_DCACHE_SIZE_KB 32u

    #ifdef __AVX512__
        #define SMARTARR_SIMD_VLEN 64
    #elifdef __AVX2__
        #define SMARTARR_SIMD_VLEN 32
    #else
        #define SMARTARR_SIMD_VLEN 8
    #endif
#else
    #pragma warning "Unknown architecture"
    #define SMARTARR_L1_DCACHE_CL_SIZE 64u
    #define SMARTARR_L1_DCACHE_SIZE_KB 32u
    #define SMARTARR_SIMD_VLEN 8

#endif

constexpr size_t SMARTARR_L1_DCACHE_SIZE = SMARTARR_L1_DCACHE_SIZE_KB * 1024;
