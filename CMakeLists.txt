cmake_minimum_required(VERSION 3.10)
project(PolynomialApp LANGUAGES C)

# Build type default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building with AddressSanitizer (Linux/macOS only)")

    if(NOT WIN32) # ASan not supported on MSVC
        add_compile_options(
            -g
            -O0
            -fsanitize=address
            -fno-omit-frame-pointer
            -Wall
            -Wextra
            -Wpedantic
        )
        add_link_options(-fsanitize=address)
    endif()
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- Core library (no main) ----
add_library(polynomial_core
    src/polynomial_properties.c
    src/polynomial_arithmetic.c
    src/polynomial_compute.c
    src/polynomial_create.c
    src/polynomial_evaluate.c
    src/polynomial_to_string.c
    src/int_array_list.c
    src/extended_value.c
    src/interval.c
    src/interval_array_list.c
    src/sturm_sequence.c
    src/point.c
    src/point_array_list.c
    src/root.c
    src/root_array_list.c
    src/string_utils.c
    src/plot.c
    src/bmp.c
)

# ---- Platform-specific library setup ----
if(WIN32)
    # Windows → PDCurses
    # Assume you have PDCurses source in external/pdcurses or installed path
    target_include_directories(polynomial_core PUBLIC
        ${CMAKE_SOURCE_DIR}/external/pdcurses/include
    )

    target_link_libraries(polynomial_core PUBLIC
        ${CMAKE_SOURCE_DIR}/external/pdcurses/lib/pdcurses.lib
)
else()
    # Linux/macOS → ncurses
    find_package(Curses REQUIRED)
    target_link_libraries(polynomial_core PUBLIC m ncurses)
endif()

target_include_directories(polynomial_core PUBLIC include ${CURSES_INCLUDE_DIR})

# ---- Main application ----
add_executable(polynomial_app src/main.c)
target_link_libraries(polynomial_app polynomial_core)

# ---- Unit tests ----
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()

    if(NOT WIN32)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(CMOCKA REQUIRED cmocka)
    else()
        message(WARNING "CMocka tests not configured on Windows")
    endif()

    set(TEST_SOURCES
        tests/test_create_polynomial.c
        tests/test_polynomial_arithmetic.c
        tests/test_sturm_sequence.c
        tests/test_polynomial_evaluation.c
        tests/test_polynomial_derivative.c
        tests/test_roots.c
        tests/test_positive_negative_intervals.c
        tests/test_monotonicity_and_extrema.c
        tests/test_inflection_and_concavity.c
    )

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} ${test_src})
        if(NOT WIN32)
            target_include_directories(${test_name} PRIVATE ${CMOCKA_INCLUDE_DIRS})
            target_link_libraries(${test_name} polynomial_core ${CMOCKA_LIBRARIES})
        else()
            target_link_libraries(${test_name} polynomial_core)
        endif()
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()
