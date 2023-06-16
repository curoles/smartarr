#include <stdio.h>

#include "smartarr/defines.h"
#include "smartarr/bench.h"

#define _ARRAY_OMP_ENABLE
#include "smartarr/basic_type_array.h"

float
bench_find_in_array(f64_smart_array_t* a,
    unsigned int times)
{
    printf("%32s: ", "Find in array");

    // warm up
    auto pos = f64_smart_array_find(a, (double)(a->len - 1));
    assert(pos.present && pos.value == (a->len - 1));

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        auto pos = f64_smart_array_find(a, (double)(a->len - 1));
        assert(pos.present && pos.value == (a->len - 1));
    }
    double time = bench_stop_timer(&start_time);

    printf("%10.8f\n", time);

    return time;
}

static
void benches_find_val(unsigned int len, unsigned int times)
{
    auto_free f64_smart_array_t* a = f64_smart_array_heap_new(len);

    for (unsigned int n = 0; n < len; ++n)
    {
        a->data[n] = n;
    }

    bench_find_in_array(a, times);
}

float
bench_find_max_in_array(f64_smart_array_t* a,
    unsigned int times)
{
    printf("%32s: ", "Find MAX in array");

    // warm up
    auto pos = f64_smart_array_find_max(a);
    assert(pos == (a->len - 1));

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        auto pos = f64_smart_array_find_max(a);
        assert(pos == (a->len - 1));
    }
    double time = bench_stop_timer(&start_time);

    printf("%10.8f\n", time);

    return time;
}

float
bench_omp_find_max_in_array(f64_smart_array_t* a,
    unsigned int times)
{
    printf("%32s: ", "OMP Find MAX in array");

    // warm up
    auto pos = f64_omp_smart_array_find_max(a);
    assert(pos == (a->len - 1));

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        auto pos = f64_omp_smart_array_find_max(a);
        assert(pos == (a->len - 1));
    }
    double time = bench_stop_timer(&start_time);

    printf("%10.8f\n", time);

    return time;
}

static
void benches_find_max(unsigned int len, unsigned int times)
{
    auto_free f64_smart_array_t* a = f64_smart_array_heap_new(len);

    for (unsigned int n = 0; n < len; ++n)
    {
        a->data[n] = (double)n;
    }

    bench_find_max_in_array(a, times);
    bench_omp_find_max_in_array(a, times);
}

int main(void)
{
    unsigned int len = 1024*1024*2 + 3;
    unsigned int times = 1000;

    benches_find_val(len, times);

    len = 1024*1024*32 + 3;
    times = 10;
    benches_find_max(len, times);

    return 0;
}

