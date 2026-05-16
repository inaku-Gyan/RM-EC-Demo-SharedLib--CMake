#pragma once

#include <cstdlib>

// ecx_config_default.hpp 已无条件定义这些宏，覆盖前必须先 #undef。

// 启用对用户调用约定的运行时检查（acquire/commit 状态机）
#define ECX_USE_USAGE_ASSERT 1

// 覆盖默认的 assert() 实现：即便 NDEBUG 被定义（test-opt 预设）也强制
// 在条件不满足时调用 std::abort()，让 GoogleTest 的 EXPECT_DEATH 在
// 所有构建配置下都能稳定捕获子进程死亡。
#define ECX_USAGE_ASSERT(expr) \
    do {                       \
        if (!(expr)) {         \
            std::abort();      \
        }                      \
    } while (0)
