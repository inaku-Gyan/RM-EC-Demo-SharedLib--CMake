#include <gtest/gtest.h>

// 随便引入一个 ECX 库文件用于测试
#include "ecx/_inc/configuration.hpp"

namespace {

TEST(ConfigurationTest, NoUserConfig) {

    // 没有 include 任何用户配置
    // 校验默认配置是否生效
    static_assert(ECX_USE_USER_CONFIG == 0, "Expected default configuration");

    static_assert(ECX_USER_CONFIG_INCLUDED == 0, "Expected default configuration");

    /// D-Cache 配置 ///

    static_assert(ECX_USE_DCACHE == 0, "Expected default configuration");

    // 测试断言检查功能

    EXPECT_NO_FATAL_FAILURE(ECX_DEV_ASSERT(false));

    EXPECT_NO_FATAL_FAILURE(ECX_USAGE_ASSERT(false));
}

}  // namespace
