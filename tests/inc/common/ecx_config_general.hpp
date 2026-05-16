#pragma once

#include "config_utils.hpp"

// 启用对用户调用约定的运行时检查
#define ECX_USE_USAGE_CHECK 1

// 覆盖默认的 assert() 实现：即便 NDEBUG 被定义（test-opt 预设）也强制
// 在条件不满足时调用 std::abort()，让 GoogleTest 的 EXPECT_DEATH 在
// 所有构建配置下都能稳定捕获子进程死亡。
#define ECX_USAGE_ASSERT(expr) ABORT_UNLESS(expr)

// ECX 调试断言
#define ECX_USE_DEV_DEBUG 1

#define ECX_DEV_ASSERT(expr) ABORT_UNLESS(expr)
