#pragma once

// 检查用户对 ECX 的使用是否正确
#ifndef ECX_USE_USAGE_CHECK
  #define ECX_USE_USAGE_CHECK 0
#endif

// 检查 ECX 的代码是否正确（用于开发 ECX 库本身）
#ifndef ECX_USE_DEV_DEBUG
  #define ECX_USE_DEV_DEBUG 0
#endif

#ifndef ECX_INC_CMSIS_DEVICE_HEADER
// #define ECX_INC_CMSIS_DEVICE_HEADER "stm32f4xx.h"
#endif

#ifndef ECX_INC_HAL
  #define ECX_INC_HAL "stm32f4xx_hal.h"
#endif

#ifndef ECX_INC_CMSIS_RTOS2
  #define ECX_INC_CMSIS_RTOS2 "cmsis_os2.h"
#endif
