#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "smartarr/defines.h"
#include "smartarr/bench.h"

#include "smartarr/basic_type_array.h"


static
void bench(const char* name,
    int64_t* (*sorter)(i64_smart_array_t*),
    unsigned int len, unsigned int times,
    i64_smart_array_t* a, i64_smart_array_t* pattern)
{
    printf("%16s: ", name);

    auto start_time = bench_start_timer();
    for (unsigned int n = 0; n < times; ++n)
    {
        i64_array_memcopy(len, pattern->data, a->data);
        sorter(a);
    }
    double tf = bench_stop_timer(&start_time);

    printf("%10.8f\n", tf);

    for (unsigned int i = 1; i < a->len; ++i) {
        //printf("a[%u]:%ld <= a[%u]:%ld\n", i-1, a->data[i-1], i, a->data[i]);
        assert(a->data[i-1] <= a->data[i]);
    }
}

static
void bench_insertion_sort(unsigned int len, unsigned int times,
    i64_smart_array_t* a, i64_smart_array_t* pattern)
{
    bench("Insertion sort", i64_smart_array_insertion_sort, len, times, a, pattern);
}

static
void bench_bubble_sort(unsigned int len, unsigned int times,
    i64_smart_array_t* a, i64_smart_array_t* pattern)
{
    bench("Bubble sort", i64_smart_array_bubble_sort, len, times, a, pattern);
}

static
int int64_compare(const void* pa, const void* pb)
{
    int64_t a = *((int64_t*)pa);
    int64_t b = *((int64_t*)pb);

    //return ( (a < b)? -1 : ( (a == b)? 0 : 1) );
    return a - b;
}

static
int64_t*
library_qsort(i64_smart_array_t* a)
{
    qsort(a->data, a->len, sizeof(int64_t), int64_compare);
    return a->data;
}

static
void bench_lib_qsort(unsigned int len, unsigned int times,
    i64_smart_array_t* a, i64_smart_array_t* pattern)
{
    bench("Library Q sort", library_qsort, len, times, a, pattern);
}

static
void
pattern_sorted(i64_smart_array_t* a)
{
    for (unsigned int i = 0; i < a->len; ++i) {
         a->data[i] = i;
    }
}

static
void
pattern_reverse_sorted(i64_smart_array_t* a)
{
    for (unsigned int i = 0; i < a->len; ++i) {
         a->data[i] = a->len - i;
    }
}

static
void
pattern_random(i64_smart_array_t* a)
{
    for (unsigned int i = 0; i < a->len; ++i) {
         a->data[i] = i;
    }

    // randomly shuffle
    for (unsigned int range = a->len; range > 1; --range) {
        unsigned int random_index = rand() % range;
        i64_array_swap_two_pointers(&a->data[random_index], &a->data[range - 1]);
    }
}

static
void benches(unsigned int len, unsigned int times)
{
    auto_free i64_smart_array_t* a = i64_smart_array_heap_new(len);
    auto_free i64_smart_array_t* pattern = i64_smart_array_heap_new(len);

    printf("Sorted pattern\n");
    pattern_sorted(pattern);
    bench_insertion_sort(len, times, a, pattern);
    bench_bubble_sort(len, times, a, pattern);
    bench_lib_qsort(len, times, a, pattern);

    printf("Reverse Sorted pattern\n");
    pattern_reverse_sorted(pattern);
    bench_insertion_sort(len, times, a, pattern);
    bench_bubble_sort(len, times, a, pattern);
    bench_lib_qsort(len, times, a, pattern);

    printf("Random pattern\n");
    pattern_random(pattern);
    bench_insertion_sort(len, times, a, pattern);
    bench_bubble_sort(len, times, a, pattern);
    bench_lib_qsort(len, times, a, pattern);
}

int main(void)
{
    constexpr unsigned int len = 1024*32;
    constexpr unsigned int times = 10;

    benches(len, times);

    return 0;
}

