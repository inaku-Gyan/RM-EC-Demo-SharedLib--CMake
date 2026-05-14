#pragma once

#include "ecx_config_default.hpp"  // IWYU pragma: export

// 让用户配置覆盖默认配置
#if __has_include(<ecx_config.hpp>)
  #include <ecx_config.hpp>  // IWYU pragma: export
#endif

#ifndef ECX_USE_DEBUG
  #define ECX_USE_DEBUG 0
#endif

#if ECX_USE_DEBUG && !defined(ECX_ASSERT)
  #include <cassert>
  #define ECX_ASSERT(expr) assert(expr)
#elif !defined(ECX_DEBUG)
  #define ECX_ASSERT(expr) (static_cast<void>(0))
#endif
