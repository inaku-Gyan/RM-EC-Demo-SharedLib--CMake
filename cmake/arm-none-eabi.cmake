# Compile-only smoke check with arm-none-eabi-gcc. No startup files, no linker
# script, no libc — we only produce a static library to verify headers parse
# and lower correctly under the real target toolchain.

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# Without libc/startup files, CMake's default try_compile (executable link)
# fails. STATIC_LIBRARY skips the link step during compiler detection.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Cortex-M4 with hard-FPU. Representative of STM32F4. STM32F7 (Cortex-M7) and
# other targets can override these via additional presets later.
set(_ecx_arch_flags "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS_INIT   "${_ecx_arch_flags} -ffreestanding")
set(CMAKE_CXX_FLAGS_INIT "${_ecx_arch_flags} -ffreestanding -fno-exceptions -fno-rtti")
