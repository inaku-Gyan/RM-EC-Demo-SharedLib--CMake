# 默认 config 下的基础测试
ecx_add_test_executable(
    NAME    test_SpscZeroCopyQueue_default_config
    SOURCES test_SpscZeroCopyQueue.cpp
    PREFIX  "SpscZeroCopyQueue::DefaultConfig::"
)

# General config 下的基础测试（启用 USAGE_ASSERT + DEV_DEBUG，含死亡测试）
ecx_add_test_executable(
    NAME    test_SpscZeroCopyQueue_general
    SOURCES test_SpscZeroCopyQueue.cpp
            test_SpscZeroCopyQueue_Death.cpp
    CONFIG  "common/ecx_config_general.hpp"
    PREFIX  "SpscZeroCopyQueue::"
)

# 禁用 usage 检查的 config
ecx_add_test_executable(
    NAME    test_SpscZeroCopyQueue_no_usage_check
    SOURCES test_SpscZeroCopyQueue.cpp
    CONFIG  "common/ecx_config_no_usage_assert.hpp"
    PREFIX  "SpscZeroCopyQueue::NoUsageCheck::"
)
