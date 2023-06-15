#include <stdio.h>
#include <omp.h>

//#define _ARRAY_DEBUG
#define _ARRAY_OMP_ENABLE
#include "smartarr/basic_type_array.h"

double bench_mx_mx_mul(
    size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols,
    unsigned int times_repeat)
{

    auto_free f64_smart_array_t* a = f64_matrix_new(a_rows, a_cols);
    auto_free f64_smart_array_t* b = f64_matrix_new(b_rows, b_cols);
    auto_free f64_smart_array_t* c = f64_matrix_new(a_rows, b_cols);
    f64_smart_array_random_sequence(a);
    f64_smart_array_random_sequence(b);

    f64_matrix_matrix_multiply(a, b, c);

    printf("matrix matrix mul     "); fflush(0);

    double start_time = omp_get_wtime();
    for (unsigned int i = 0; i < times_repeat; ++i) {
        f64_matrix_matrix_multiply(a, b, c);
    }
    double time = omp_get_wtime() - start_time;

    double mops = ((a_rows*a_cols*b_cols) * times_repeat) / (1000000.0 * time);

    printf("%10.8f    %f MOPS\n", time, mops);

    auto_free f64_smart_array_t* d = f64_matrix_new(a_rows, b_cols);
    f64_matrix_matrix_multiply(a, b, d);
    assert(f64_array_equal(a_rows*b_cols, c->data, d->data));

    return time;
}

double bench_omp_mx_mx_mul(
    size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols,
    unsigned int times_repeat)
{

    auto_free f64_smart_array_t* a = f64_matrix_new(a_rows, a_cols);
    auto_free f64_smart_array_t* b = f64_matrix_new(b_rows, b_cols);
    auto_free f64_smart_array_t* c = f64_matrix_new(a_rows, b_cols);
    f64_smart_array_random_sequence(a);
    f64_smart_array_random_sequence(b);

    f64_omp_matrix_matrix_multiply(0, a, b, c);

    printf("omp matrix matrix mul "); fflush(0);

    double start_time = omp_get_wtime();
    for (unsigned int i = 0; i < times_repeat; ++i) {
        f64_omp_matrix_matrix_multiply(0, a, b, c);
    }
    double time = omp_get_wtime() - start_time;

    double mops = ((a_rows*a_cols*b_cols) * times_repeat) / (1000000.0 * time);

    printf("%10.8f    %f MOPS\n", time, mops);

    auto_free f64_smart_array_t* d = f64_matrix_new(a_rows, b_cols);
    f64_matrix_matrix_multiply(a, b, d);
    assert(f64_array_equal(a_rows*b_cols, c->data, d->data));
    /*for (size_t i = 0; i < a_rows*b_cols; ++i) {
        if (c->data[i] != d->data[i]) {
            printf("%lu  %f vs %f\n", i, a->data[i], b->data[i]);
            assert(0);
            break;
        }
    }*/

    return time;
}

int main(void)
{
    size_t a_rows = 1024*4, a_cols = 1024*3;
    size_t b_rows = a_cols, b_cols = 1024*2;

    unsigned int times = 1;//000;

    bench_mx_mx_mul(a_rows, a_cols, b_rows, b_cols, times);
    bench_omp_mx_mx_mul(a_rows, a_cols, b_rows, b_cols, times);

    return 0;
}
