include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(ecx_tests
    test_pid.cpp
    test_cobs.cpp
    test_crc.cpp
)
target_link_libraries(ecx_tests PRIVATE
    ecx::ecx
    GTest::gtest_main
    GTest::gmock
)

gtest_discover_tests(ecx_tests)
