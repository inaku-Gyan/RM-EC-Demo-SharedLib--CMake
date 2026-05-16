# containers 模块的测试 exec 矩阵

# 默认 config 下的基础测试
ecx_add_test_executable(
    NAME    ecx_tests_containers
    SOURCES test_SpscZeroCopyQueue.cpp
    PREFIX  "containers::"
)

# 在 usage_assert config 下重跑基础测试，并加上死亡测试 / usage_assert 行为测试
ecx_add_test_executable(
    NAME    ecx_tests_containers_usage_assert
    SOURCES test_SpscZeroCopyQueue.cpp
            test_SpscZeroCopyQueue_Death.cpp
    CONFIG  "ecx_config_usage_assert.hpp"
    PREFIX  "containers::usage_assert::"
)
