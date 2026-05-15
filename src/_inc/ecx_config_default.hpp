#pragma once

#define ECX_USE_DEV_DEBUG 0     // 检查 ECX 的代码是否正确（用于开发 ECX 库本身）
#define ECX_USE_USAGE_ASSERT 0  // 检查用户对 ECX 的使用是否正确

// #define ECX_INC_CMSIS_DEVICE_HEADER "stm32f4xx.h"
#define ECX_INC_HAL "stm32f4xx_hal.h"
#define ECX_INC_CMSIS_RTOS2 "cmsis_os2.h"
