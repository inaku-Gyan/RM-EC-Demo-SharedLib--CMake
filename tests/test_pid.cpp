#include <gtest/gtest.h>

#include "algo/pid.hpp"

namespace {

using ecx::algo::Pid;

TEST(PidTest, ZeroErrorYieldsZeroOutput) {
    Pid<float> pid(1.0F, 0.0F, 0.0F, 100.0F);
    EXPECT_FLOAT_EQ(pid.update(0.0F, 0.0F, 0.01F), 0.0F);
}

TEST(PidTest, ProportionalSignIsCorrect) {
    Pid<float> pid(2.0F, 0.0F, 0.0F, 100.0F);
    EXPECT_FLOAT_EQ(pid.update(5.0F, 3.0F, 0.01F), 4.0F);
    pid.reset();
    EXPECT_FLOAT_EQ(pid.update(0.0F, 5.0F, 0.01F), -10.0F);
}

TEST(PidTest, OutputIsClampedToLimit) {
    Pid<float> pid(1000.0F, 0.0F, 0.0F, 10.0F);
    EXPECT_FLOAT_EQ(pid.update(1.0F, 0.0F, 0.01F), 10.0F);
    pid.reset();
    EXPECT_FLOAT_EQ(pid.update(-1.0F, 0.0F, 0.01F), -10.0F);
}

}  // namespace
