#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "smartarr/defines.h"
#include "smartarr/bench.h"

#define _ARRAY_OMP_ENABLE
#define _ARRAY_TYPE int64_t
#define _ARRAY_TYPE_NAME int64
#include "smartarr/array.inc.h"

#define _ARRAY_TYPE int64_t
#define _ARRAY_TYPE_NAME xint64
#undef _SMART_ARRAY_ALIGN
#define _SMART_ARRAY_ALIGN 8
#include "smartarr/array.inc.h"

int64_t* seeme1(size_t len,
    const int64_t *restrict a, const int64_t *restrict b,
    int64_t *restrict c)
{
    a = __builtin_assume_aligned(a, 64);
    b = __builtin_assume_aligned(b, 64);
    c = __builtin_assume_aligned(c, 64);
    int64_array_add(len, a, b, c);
    return c;
}

int64_t* seeme2(size_t len,
    const int64_t *restrict a, const int64_t *restrict b,
    int64_t *restrict c)
{
    a = __builtin_assume_aligned(a, 8);
    b = __builtin_assume_aligned(b, 8);
    c = __builtin_assume_aligned(c, 8);
    xint64_array_add(len, a, b, c);
    return c;
}

//static
double bench1(unsigned int len, unsigned int times)
{
    auto_free int64_smart_array_t* a = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* b = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* c = int64_smart_array_heap_new(len);
    int64_smart_array_random_sequence(a);
    int64_smart_array_random_sequence(b);
    int64_smart_array_fill(c, 0);

    printf("Alignment %2u: %16p ", SMARTARR_SIMD_VLEN, a->data); fflush(0);

    // warm up
    int64_array_add(len, a->data, b->data, c->data);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        int64_smart_array_fill(c, 0);
        int64_array_add(len, a->data, b->data, c->data);
        for (unsigned int i = 0; i < len; ++i) {
            assert(c->data[i] == (a->data[i] + b->data[i]));
        }
    }
    double tf = bench_stop_timer(&start_time);

    double mops = (len * times) / (1000000.0 * tf);

    printf("%10.8f    %f MOPS\n", tf, mops);

    return tf;
}

//static
double bench2(unsigned int len, unsigned int times)
{
    auto_free xint64_smart_array_t* a = xint64_smart_array_heap_new(len);
    auto_free xint64_smart_array_t* b = xint64_smart_array_heap_new(len);
    auto_free xint64_smart_array_t* c = xint64_smart_array_heap_new(len);
    xint64_smart_array_random_sequence(a);
    xint64_smart_array_random_sequence(b);
    xint64_smart_array_fill(c, 0);

    printf("Alignment  8: %16p ", a->data); fflush(0);

    // warm up
    xint64_array_add(len, a->data, b->data, c->data);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        xint64_smart_array_fill(c, 0);
        xint64_array_add(len, a->data, b->data, c->data);
        for (unsigned int i = 0; i < len; ++i) {
            assert(c->data[i] == (a->data[i] + b->data[i]));
        }
    }
    double tf = bench_stop_timer(&start_time);

    printf("%10.8f\n", tf);

    return tf;
}

double bench4(unsigned int len, unsigned int times)
{
    auto_free int64_smart_array_t* a = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* b = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* c = int64_smart_array_heap_new(len);
    int64_smart_array_random_sequence(a);
    int64_smart_array_random_sequence(b);
    int64_smart_array_fill(c, 0);

    printf("OMP for   %2u: %16p ", SMARTARR_SIMD_VLEN, a->data); fflush(0);

    int num_threads_old = omp_get_num_threads();
    omp_set_num_threads(2);

    // warm up
    int64_omp_array_add(len, a->data, b->data, c->data);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        int64_smart_array_fill(c, 0);
        int64_omp_array_add(len, a->data, b->data, c->data);
        for (unsigned int i = 0; i < len; ++i) {
            assert(c->data[i] == (a->data[i] + b->data[i]));
        }
    }
    double tf = bench_stop_timer(&start_time);

    double mops = (len * times) / (1000000.0 * tf);

    printf("%10.8f    %f MOPS\n", tf, mops);

    omp_set_num_threads(num_threads_old);

    return tf;
}

#include <omp.h>

double bench3(unsigned int len, unsigned int times)
{
    auto_free int64_smart_array_t* a = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* b = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* c = int64_smart_array_heap_new(len);
    int64_smart_array_random_sequence(a);
    int64_smart_array_random_sequence(b);
    int64_smart_array_fill(c, 0);

    printf("OpenMP      : %16p ", a->data); fflush(0);

    // warm up
    int64_array_add(len, a->data, b->data, c->data);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        #pragma omp parallel num_threads(2)
        {
            int tid = omp_get_thread_num();
            unsigned int chunk_size = len / 2;
            int64_array_fill(chunk_size, c->data + tid*chunk_size, 0);
            int64_array_add(
                chunk_size,
                a->data + tid*chunk_size,
                b->data + tid*chunk_size,
                c->data + tid*chunk_size
            );
            unsigned int limit = (tid + 1)*chunk_size;
            for (unsigned int i = tid*chunk_size; i < limit; ++i) {
                assert(c->data[i] == (a->data[i] + b->data[i]));
            }
        }
    }
    double tf = bench_stop_timer(&start_time);

    double mops = (len * times) / (1000000.0 * tf);

    printf("%10.8f    %f MOPS\n", tf, mops);

    return tf;
}

double bench5(unsigned int len, unsigned int times)
{
    auto_free int64_smart_array_t* a = int64_smart_array_heap_new(len);
    auto_free int64_smart_array_t* b = int64_smart_array_heap_new(len);
    int64_smart_array_random_sequence(a);
    int64_smart_array_random_sequence(b);

    printf("destruct  %2u: %16p ", SMARTARR_SIMD_VLEN, a->data); fflush(0);

    // warm up
    int64_smart_array_add_destruct(a, b);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        int64_smart_array_fill(a, 0);
        int64_smart_array_add_destruct(a, b);
        for (unsigned int i = 0; i < len; ++i) {
            assert(a->data[i] == b->data[i]);
        }
    }
    double tf = bench_stop_timer(&start_time);

    double mops = (len * times) / (1000000.0 * tf);

    printf("%10.8f    %f MOPS\n", tf, mops);

    return tf;
}

#include <math.h>

int main(void)
{
    // cpuid | grep -A 15 "data cache (1)" | grep 'size synth' => 32K|48K
    // 32K/sizeof(int64)=4K, 48K->6K
    // 3 arrays x 2K(8) = 6K(8)

    unsigned int len = 1024*2; // stay in L1 data cache
    unsigned int times = 1000*1000*10;

    double t1 = bench1(len, times);
    auto_free void* misalign_next = malloc(11*64+1);
    double t2 = bench2(len, times);

    printf("8 vs %u: %f = %f%%\n\n", SMARTARR_SIMD_VLEN, t2/t1, ((t2-t1)/t1)*100.0);

    double t3 = bench5(len, times);
    printf("non-distructive vs distructive: %f = %f%%\n\n", t1/t3, (fabs(t1-t3)/t3)*100.0);

    assert(misalign_next != nullptr);

    len = 1024*2*1024;
    times = 10;
    t1 = bench1(len, times);
    t2 = bench3(len, times);
    t3 = bench4(len, times);

    return 0;
}

