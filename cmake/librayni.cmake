add_library(rayni
            src/lib/color.cpp
            src/lib/color.h
            src/lib/containers/fixed_size_stack.h
            src/lib/containers/listener_list.h
            src/lib/containers/variant.cpp
            src/lib/containers/variant.h
            src/lib/image.cpp
            src/lib/image.h
            src/lib/math/enum.h
            src/lib/math/hash.h
            src/lib/math/math.h
            src/lib/string/string.cpp
            src/lib/string/string.h
            src/lib/system/command.cpp
            src/lib/system/command.h
            src/lib/time/duration_formatter.cpp
            src/lib/time/duration_formatter.h
            src/lib/time/scoped_stopwatch.cpp
            src/lib/time/scoped_stopwatch.h
            src/lib/time/stopwatch.cpp
            src/lib/time/stopwatch.h)

target_include_directories(rayni
                           PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(librayni-unit_tests
               src/unit_tests/lib/color.cpp
               src/unit_tests/lib/containers/fixed_size_stack.cpp
               src/unit_tests/lib/containers/listener_list.cpp
               src/unit_tests/lib/containers/variant.cpp
               src/unit_tests/lib/expect_real_eq.h
               src/unit_tests/lib/image.cpp
               src/unit_tests/lib/math/enum.cpp
               src/unit_tests/lib/math/hash.cpp
               src/unit_tests/lib/math/math.cpp
               src/unit_tests/lib/string/string.cpp
               src/unit_tests/lib/system/command.cpp
               src/unit_tests/lib/time/duration_formatter.cpp
               src/unit_tests/lib/time/scoped_stopwatch.cpp
               src/unit_tests/lib/time/stopwatch.cpp)

target_include_directories(librayni-unit_tests
                           PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(librayni-unit_tests
                      rayni
                      ${GTEST_BOTH_LIBRARIES})

add_test(librayni-unit_tests librayni-unit_tests)
