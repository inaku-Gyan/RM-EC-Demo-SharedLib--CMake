#include <array>
#include <cstdint>

#include <gtest/gtest.h>

#include "proto/cobs.hpp"

namespace {

using ecx::proto::cobs_decode;
using ecx::proto::cobs_encode;

TEST(CobsTest, RoundTripPayloadWithZeros) {
    constexpr std::array<uint8_t, 6> payload{0x11, 0x00, 0x22, 0x00, 0x00, 0x33};

    std::array<uint8_t, 16> encoded{};
    const size_t            encoded_len = cobs_encode(payload, encoded);
    ASSERT_GT(encoded_len, 0U);
    ASSERT_EQ(encoded[encoded_len - 1], 0x00);

    std::array<uint8_t, 16> decoded{};
    const size_t            decoded_len =
        cobs_decode({encoded.data(), encoded_len - 1}, decoded);
    ASSERT_EQ(decoded_len, payload.size());
    for (size_t i = 0; i < payload.size(); ++i) { EXPECT_EQ(decoded[i], payload[i]); }
}

TEST(CobsTest, EncodeFailsWhenDstTooSmall) {
    constexpr std::array<uint8_t, 4> payload{1, 2, 3, 4};
    std::array<uint8_t, 4>           dst{};  // Need src.size() + 2 = 6
    EXPECT_EQ(cobs_encode(payload, dst), 0U);
}

}  // namespace
