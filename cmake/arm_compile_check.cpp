// Driver TU for the build-armgcc preset. Forces template instantiation and
// inline-function codegen so that header-only contents in algo/ and proto/ are
// actually exercised by arm-none-eabi-gcc. rtos/ is intentionally excluded
// (depends on FreeRTOS, which is not provided here).

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
