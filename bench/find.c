#include <stdio.h>

#include "smartarr/defines.h"
#include "smartarr/bench.h"

#include "smartarr/basic_type_array.h"

float
bench_find_in_array(f64_smart_array_t* a,
    unsigned int times)
{
    printf("%16s: ", "Find in array");

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
void benches(unsigned int len, unsigned int times)
{
    auto_free f64_smart_array_t* a = f64_smart_array_heap_new(len);

    for (unsigned int n = 0; n < len; ++n)
    {
        a->data[n] = n;
    }

    bench_find_in_array(a, times);
}

int main(void)
{
    constexpr unsigned int len = 1024*1024*2 + 3;
    constexpr unsigned int times = 1000;

    benches(len, times);

    return 0;
}

