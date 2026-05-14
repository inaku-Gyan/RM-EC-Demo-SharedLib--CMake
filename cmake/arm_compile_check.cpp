// build-armgcc preset 使用的驱动翻译单元：强制实例化模板、保留 inline 函数代码，
// 让 algo/ 与 proto/ 中的 header-only 内容真正经过 arm-none-eabi-gcc 的代码生成。
// 故意不 include rtos/，因为它依赖 FreeRTOS，本仓库目前未提供其头文件。

#include <array>
#include <cstdint>
#include <span>

#include "algo/pid.hpp"
#include "proto/cobs.hpp"
#include "proto/crc.hpp"

template class ecx::algo::Pid<float>;
template class ecx::algo::Pid<double>;

namespace {

[[gnu::used]] void exercise_proto() {
    std::array<uint8_t, 8> buf{};
    const std::span<const uint8_t> in{buf.data(), 4};
    const std::span<uint8_t>       out{buf.data(), buf.size()};
    (void)ecx::proto::cobs_encode(in, out);
    (void)ecx::proto::cobs_decode(in, out);
    (void)ecx::proto::crc16(in);
}

}  // namespace
