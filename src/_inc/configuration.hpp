#pragma once

#include "ecx_config_default.hpp"  // IWYU pragma: export

// 让用户配置覆盖默认配置
#if __has_include(<ecx_config.hpp>)
  #include <ecx_config.hpp>  // IWYU pragma: export
#endif

//// ECX 库内部使用的断言（仅在开发 ECX 库时启用） ////

#ifndef ECX_USE_DEV_DEBUG
  #define ECX_USE_DEV_DEBUG 0
#endif

#if ECX_USE_DEV_DEBUG && !defined(ECX_DEV_ASSERT)
  #include <cassert>
  #define ECX_DEV_ASSERT(expr) assert(expr)
#elif ECX_USE_DEV_DEBUG == 0
  #define ECX_DEV_ASSERT(expr) (static_cast<void>(0))
#endif

//// 用户使用 ECX 时的断言（检查用户对 ECX 的使用是否正确） ////

#ifndef ECX_USE_USAGE_ASSERT
  #define ECX_USE_USAGE_ASSERT 0
#endif

#if ECX_USE_USAGE_ASSERT && !defined(ECX_USAGE_ASSERT)
  #include <cassert>
  #define ECX_USAGE_ASSERT(expr) assert(expr)
#elif ECX_USE_USAGE_ASSERT == 0
  #define ECX_USAGE_ASSERT(expr) (static_cast<void>(0))
#endif
