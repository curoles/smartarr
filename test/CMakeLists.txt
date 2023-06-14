# Find all .c files in test directory and make CTest out of each file.
# Author: Igor Lesik 2023

# To use memory checking tools from CMake:
include(CTest)
# Now we can run with valgrind: `ctest -T memcheck`
# see https://www.scivision.dev/cmake-valgrind/
# see https://subscription.packtpub.com/book/programming/9781801070058/12/ch12lvl1sec76/dynamic-analysis-with-valgrind

find_package(OpenMP)
if(NOT OpenMP_C_FOUND)
    message(FATAL_ERROR "OpenMP not found")
endif()

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(TEST_CC_FLAGS -static)
else()
    set(TEST_CC_FLAGS -static)
endif()

set(tests
    headers
    array
    string
    utf8
    list
    matrix
)

set(matrix_cc_flags -fopenmp)
#set(test8_cc_flags ${CMAKE_CURRENT_SOURCE_DIR}/test8.S)

foreach(test_name IN LISTS tests)

    set(test_path ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.c)

    if(DEFINED ${test_name}_cc_flags)
        set(test_cc_flags "${${test_name}_cc_flags}")
    else()
        set(test_cc_flags "")
    endif()

    add_executable(${test_name} ${test_path})
    target_compile_options(${test_name} PRIVATE ${TEST_CC_FLAGS} ${test_cc_flags})
    #add_dependencies(${test_name} generate) target "generate" calls generating scripts

    if(test_cc_flags)
        string(FIND ${test_cc_flags} "-fopenmp" flag_fopenmp)
        if(NOT ${flag_fopenmp} EQUAL -1)
            target_link_libraries(${test_name} PUBLIC OpenMP::OpenMP_C)
        endif()
    endif()

    add_test(NAME ${test_name} COMMAND ${test_name})

endforeach()
