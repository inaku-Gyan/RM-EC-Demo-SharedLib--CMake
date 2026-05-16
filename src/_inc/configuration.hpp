#pragma once

/********************************************/
/*************    引入用户配置    ************/
/********************************************/

#ifndef ECX_USER_CONFIG_INCLUDED
  #define ECX_USER_CONFIG_INCLUDED 0
#endif

#ifndef ECX_INC_USER_CONFIG
  #define ECX_INC_USER_CONFIG "ecx_config.hpp"
#endif

#if ECX_USER_CONFIG_INCLUDED == 0
  // 让用户配置覆盖默认配置
  #if __has_include(ECX_INC_USER_CONFIG)
    #include ECX_INC_USER_CONFIG  // IWYU pragma: export
    #undef ECX_USER_CONFIG_INCLUDED
    #define ECX_USER_CONFIG_INCLUDED 1
  #endif
#endif  // ECX_USER_CONFIG_INCLUDED

/********************************************/
/***********    ECX 开发调试配置    **********/
/********************************************/

// 检查 ECX 的代码是否正确（用于开发 ECX 库本身）
#ifndef ECX_USE_DEV_DEBUG
  #define ECX_USE_DEV_DEBUG 0
#endif

#if ECX_USE_DEV_DEBUG && !defined(ECX_DEV_ASSERT)
  #include <cassert>
  #define ECX_DEV_ASSERT(expr) assert(expr)
#elif ECX_USE_DEV_DEBUG == 0
  #define ECX_DEV_ASSERT(expr) (static_cast<void>(0))
#endif

/********************************************/
/************    用户使用检查    *************/
/********************************************/

// 检查用户对 ECX 的使用是否正确
#ifndef ECX_USE_USAGE_CHECK
  #define ECX_USE_USAGE_CHECK 0
#endif

#if ECX_USE_USAGE_CHECK && !defined(ECX_USAGE_ASSERT)
  #include <cassert>
  #define ECX_USAGE_ASSERT(expr) assert(expr)
#elif ECX_USE_USAGE_CHECK == 0
  #define ECX_USAGE_ASSERT(expr) (static_cast<void>(0))
#endif

/********************************************/
/*******    D-Cache 配置和相关工具宏    ******/
/********************************************/

#ifdef ECX_INC_CMSIS_DEVICE_HEADER
  #include ECX_INC_CMSIS_DEVICE_HEADER
#endif

// Cortex-M7 有 D-Cache，而 Cortex-M4 没有。
#ifndef ECX_USE_DCACHE
  #if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT == 1
    #define ECX_USE_DCACHE 1
  #else
    #define ECX_USE_DCACHE 0
  #endif
#endif

// 对齐到 D-Cache 行大小，以避免伪共享（仅当启用 D-Cache 时有效）
// 伪共享（false sharing）指多个处理器核心频繁访问同一缓存行中的不同变量，导致性能下降。
// 通过将共享变量对齐到缓存行大小，可以避免这种情况。
#ifndef ECX_DCACHE_LINE_SIZE
  #if defined(__SCB_DCACHE_LINE_SIZE) && ECX_USE_DCACHE
    #define ECX_DCACHE_LINE_SIZE __SCB_DCACHE_LINE_SIZE
  #else
    #define ECX_DCACHE_LINE_SIZE 1
  #endif
#endif

#if ECX_USE_DCACHE
  // 确保类型 tp 至少对齐到 D-Cache 行大小
  #define ECX_ALIGNAS_DCACHE_LINE(...) \
      alignas(ECX_DCACHE_LINE_SIZE) alignas(__VA_ARGS__) __VA_ARGS__

  // 如果启用了 D-Cache，则 CPU 和 DMA 之间的数据交换需要进行 D-Cache 清除和无效化操作，
  // 否则数据一致性无法得到保证。
  #define ECX_DCACHE_CLEAN(addr, size) SCB_CleanDCache_by_Addr((addr), (size))
  #define ECX_DCACHE_INVALIDATE(addr, size) SCB_InvalidateDCache_by_Addr((addr), (size))
  #define ECX_DCACHE_CLEAN_INVALIDATE(addr, size) SCB_CleanInvalidateDCache_by_Addr((addr), (size))

#else

  #define ECX_ALIGNAS_DCACHE_LINE(...) alignas(__VA_ARGS__) __VA_ARGS__

  #define ECX_DCACHE_CLEAN(addr, size) (static_cast<void>(0))
  #define ECX_DCACHE_INVALIDATE(addr, size) (static_cast<void>(0))
  #define ECX_DCACHE_CLEAN_INVALIDATE(addr, size) (static_cast<void>(0))

#endif
