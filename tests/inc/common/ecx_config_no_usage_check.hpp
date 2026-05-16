#pragma once

#include "config_utils.hpp"

// 禁用对用户调用约定的运行时检查
#define ECX_USE_USAGE_CHECK 0

// ECX 调试断言
#define ECX_USE_DEV_DEBUG 1

#define ECX_DEV_ASSERT(expr) ABORT_UNLESS(expr)
