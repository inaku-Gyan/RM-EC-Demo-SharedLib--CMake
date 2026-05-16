#include <gtest/gtest.h>

// 随便引入一个 ECX 库文件用于测试
#include <memory.hpp>

namespace {

TEST(ConfigurationTest, NoUserConfig) {

    // 没有 include 任何用户配置
    // 校验默认配置是否生效
    static_assert(ECX_USE_USER_CONFIG == 0, "Expected default configuration");

    static_assert(ECX_USER_CONFIG_INCLUDED == 0, "Expected default configuration");

    /// D-Cache 配置 ///

    static_assert(ECX_USE_DCACHE == 0, "Expected default configuration");
}

}  // namespace
