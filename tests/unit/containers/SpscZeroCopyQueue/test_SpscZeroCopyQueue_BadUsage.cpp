#include <gtest/gtest.h>

#include "containers/SpscZeroCopyQueue.hpp"

namespace {

using ecx::SpscZeroCopyQueue;

// 仅在 ECX_USE_USAGE_CHECK=1 变体编译，验证对队列的误用会触发断言（std::abort）。
// 本变体 ecx_config.hpp 覆盖了 ECX_USAGE_ASSERT，使其不依赖 NDEBUG。

TEST(SpscZeroCopyQueueUsageAssertDeathTest, ReadAcquireTwiceWithoutCommit) {
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

TEST(SpscZeroCopyQueueUsageAssertDeathTest, WriteAcquireTwiceWithoutCommit) {
    SpscZeroCopyQueue<int, 4> q;
    [[maybe_unused]] auto* w = q.write_acquire();
    ASSERT_NE(w, nullptr);
    // 未 commit 就再次 acquire，应触发断言
    EXPECT_DEATH({ (void)q.write_acquire(); }, "");
}

TEST(SpscZeroCopyQueueUsageAssertDeathTest, ReadCommitWithoutAcquire) {
    SpscZeroCopyQueue<int, 4> q;
    EXPECT_DEATH({ q.read_commit(); }, "");
}

TEST(SpscZeroCopyQueueUsageAssertDeathTest, WriteCommitWithoutAcquire) {
    SpscZeroCopyQueue<int, 4> q;
    EXPECT_DEATH({ q.write_commit(); }, "");
}

// 失败的 acquire（空队列读 / 满队列写）不应进入 reading/writing 状态，
// 因此可以重复调用而不触发断言。
TEST(SpscZeroCopyQueueUsageAssertTest, ReadAcquireOnEmptyDoesNotEnterReadState) {
    SpscZeroCopyQueue<int, 4> const q;
    EXPECT_EQ(q.read_acquire(), nullptr);
    EXPECT_EQ(q.read_acquire(), nullptr);
}

TEST(SpscZeroCopyQueueUsageAssertTest, WriteAcquireOnFullDoesNotEnterWriteState) {
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
