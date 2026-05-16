# 仅用于「编译能否通过」的烟雾测试：用 arm-none-eabi-gcc 把 src 编成静态库。
# 不提供启动文件、linker script、libc，因此无法产出可执行；目标只是确认头文件在真实
# 嵌入式工具链下也能正常解析与代码生成。

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# CMake 在探测编译器时默认会尝试链接一个可执行；当前环境无 libc/启动文件会失败。
# 改为 STATIC_LIBRARY 跳过链接探测。
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 以 Cortex-M4 + 硬浮点作为代表（对应 STM32F4 系列）。
# 后续若需验证 STM32F7（Cortex-M7）或其他型号，可再增加 preset 覆盖。
set(_ecx_arch_flags "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS_INIT   "${_ecx_arch_flags} --specs=nano.specs")
set(CMAKE_CXX_FLAGS_INIT "${_ecx_arch_flags} --specs=nano.specs -fno-exceptions -fno-rtti")
