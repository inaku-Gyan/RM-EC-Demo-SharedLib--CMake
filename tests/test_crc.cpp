#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "proto/crc.hpp"

namespace {

using ecx::proto::crc16;

TEST(Crc16Test, StandardCheckVector) {
    constexpr std::array<uint8_t, 9> data{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    EXPECT_EQ(crc16(data), 0x29B1U);
}

TEST(Crc16Test, EmptyInputReturnsInitValue) {
    constexpr std::array<uint8_t, 0> empty{};
    EXPECT_EQ(crc16(empty), 0xFFFFU);
}

}  // namespace
