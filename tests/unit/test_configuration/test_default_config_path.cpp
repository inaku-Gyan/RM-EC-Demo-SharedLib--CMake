#include <gtest/gtest.h>

// 随便引入一个 ECX 库文件用于测试
#include <memory.hpp>

#include "_inc/configuration.hpp"

static_assert(SELF_DEFINE_MACRO == 1, "User config should be included");

namespace {

TEST(ConfigurationTest, DefaultConfigPath) {

    static_assert(ECX_USE_USER_CONFIG == 1, "Expected default configuration");

    static_assert(ECX_USER_CONFIG_INCLUDED == 1, "Expected default configuration");

    ASSERT_EQ(ECX_INC_USER_CONFIG, "ecx_config.hpp");

    /// D-Cache 配置 ///

    static_assert(ECX_USE_DCACHE == 1, "Expected default configuration");

    // 测试断言检查功能

    static_assert(ECX_USE_DEV_DEBUG == 0, "Expected default configuration");
    static_assert(ECX_USE_USAGE_CHECK == 1, "Expected default configuration");

    EXPECT_NO_FATAL_FAILURE(ECX_DEV_ASSERT(true));
    EXPECT_NO_FATAL_FAILURE(ECX_DEV_ASSERT(false));

    EXPECT_NO_FATAL_FAILURE(ECX_USAGE_ASSERT(true));

    EXPECT_DEATH(ECX_USAGE_ASSERT(false), ".*");
}

}  // namespace
