#include <gtest/gtest.h>

#include "containers/SpscZeroCopyQueue.hpp"

namespace {

using ecx::SpscZeroCopyQueue;

TEST(SpscZeroCopyQueueBadUsageDeathTest, ReadAcquireTwiceWithoutCommit) {
    SpscZeroCopyQueue<int, 4> q;

    // 先写入一个元素，让随后首次 read_acquire 真正进入 reading 状态
    auto* w = q.write_acquire();
    ASSERT_NE(w, nullptr);
    *w = 1;
    q.write_commit();

    [[maybe_unused]] const auto* r = q.read_acquire();
    ASSERT_NE(r, nullptr);
    // 未 commit 就再次 acquire，应触发断言
    EXPECT_DEATH({ (void)q.read_acquire(); }, "");
}

TEST(SpscZeroCopyQueueBadUsageDeathTest, WriteAcquireTwiceWithoutCommit) {
    SpscZeroCopyQueue<int, 4> q;
    [[maybe_unused]] auto* w = q.write_acquire();
    ASSERT_NE(w, nullptr);
    // 未 commit 就再次 acquire，应触发断言
    EXPECT_DEATH({ (void)q.write_acquire(); }, "");
}

TEST(SpscZeroCopyQueueBadUsageDeathTest, ReadCommitWithoutAcquire) {
    SpscZeroCopyQueue<int, 4> q;
    EXPECT_DEATH({ q.read_commit(); }, "");
}

TEST(SpscZeroCopyQueueBadUsageDeathTest, WriteCommitWithoutAcquire) {
    SpscZeroCopyQueue<int, 4> q;
    EXPECT_DEATH({ q.write_commit(); }, "");
}

// 失败的 acquire（空队列读 / 满队列写）不应进入 reading/writing 状态，
// 因此可以重复调用而不触发断言。
TEST(SpscZeroCopyQueueBadUsageTest, ReadAcquireOnEmptyDoesNotEnterReadState) {
    SpscZeroCopyQueue<int, 4> const q;
    EXPECT_EQ(q.read_acquire(), nullptr);
    EXPECT_EQ(q.read_acquire(), nullptr);
}

TEST(SpscZeroCopyQueueBadUsageTest, WriteAcquireOnFullDoesNotEnterWriteState) {
    SpscZeroCopyQueue<int, 4> q;
    for (int i = 0; i < 4; ++i) {
        auto* w = q.write_acquire();
        ASSERT_NE(w, nullptr);
        *w = i;
        q.write_commit();
    }
    EXPECT_EQ(q.write_acquire(), nullptr);
    EXPECT_EQ(q.write_acquire(), nullptr);
}

}  // namespace
