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

file(GLOB_RECURSE TEST_FILES
    CONFIGURE_DEPENDS
    "test_*.cpp"
    "*/test_*.cpp"
)

add_executable(ecx_tests
    ${TEST_FILES}
)
target_link_libraries(ecx_tests PRIVATE
    ecx::ecx
    GTest::gtest_main
    GTest::gmock
)

gtest_discover_tests(ecx_tests)
