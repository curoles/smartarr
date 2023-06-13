# Find all .c files in `bench` directory and make an executable out of each file.
# Author: Igor Lesik 2023

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(BENCH_CC_FLAGS -static)
else()
    set(BENCH_CC_FLAGS -static)
endif()

find_package(OpenMP)
if(NOT OpenMP_C_FOUND)
    message(FATAL_ERROR "OpenMP not found")
endif()

set(benches
    add
    sort
    matrix_mul
)

set(add_cc_flags -fopenmp)
set(matrix_mul_cc_flags -fopenmp)

foreach(bench_name IN LISTS benches)

    set(bench_path ${CMAKE_CURRENT_SOURCE_DIR}/${bench_name}.c)

    if(DEFINED ${bench_name}_cc_flags)
        set(bench_cc_flags "${${bench_name}_cc_flags}")
    else()
        set(bench_cc_flags "")
    endif()

    add_executable(bench_${bench_name} ${bench_path})
    target_compile_options(bench_${bench_name} PRIVATE ${BENCH_CC_FLAGS} ${bench_cc_flags})
    #add_dependencies(${test_name} generate) target "generate" calls generating scripts

    if(bench_cc_flags)
        string(FIND ${bench_cc_flags} "-fopenmp" flag_fopenmp)
        if(NOT ${flag_fopenmp} EQUAL -1)
            target_link_libraries(bench_${bench_name} PUBLIC OpenMP::OpenMP_C)
        endif()
    endif()


endforeach()
