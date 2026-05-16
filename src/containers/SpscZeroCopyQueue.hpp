#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

#include "../_inc/configuration.hpp"
#include "../memory.hpp"

namespace ecx {

template <typename T, std::size_t kCapacity, CachePolicy kDCachePolicy = CachePolicy::None>
    requires(std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>)
class SpscZeroCopyQueue {

    // 多分配 1 个槽，用 (writer == reader) 表示空、(next(writer) == reader) 表示满，
    // 避免引入共享的 size_ 原子量，从而省掉每次 commit 的 RMW。
    static constexpr std::size_t kNSlots_ = kCapacity + 1;

    // 单个槽位类型（含 cache-line padding，当 kDCachePolicy != None 时）
    using TSlot_ = AlignedElement<T, AlignmentByCachePolicy<kDCachePolicy>>;

    static constexpr std::size_t S_next(std::size_t i) noexcept {
        return (i + 1 == kNSlots_) ? 0 : i + 1;
    }

    // 生产者写、消费者读
    ECX_ALIGNAS_DCACHE_LINE(std::atomic_size_t)           writer_{0};
    // 消费者写、生产者读
    ECX_ALIGNAS_DCACHE_LINE(std::atomic_size_t)           reader_{0};
    // 与 writer_/reader_ 分行，避免数据写入污染其控制字段所在的缓存行
    ECX_ALIGNAS_DCACHE_LINE(std::array<TSlot_, kNSlots_>) buffer_{};

#if ECX_USE_USAGE_CHECK
    // 0: idle, 0b01: reading, 0b10: writing
    // 读位仅由消费者改、写位仅由生产者改，两位互不相干，无需 acquire/release
    mutable std::atomic_uint8_t state_{0};

    void M_write_begin() { state_.fetch_or(0b10, std::memory_order_relaxed); }

    void M_write_end() { state_.fetch_and(~0b10, std::memory_order_relaxed); }

    void M_read_begin() const { state_.fetch_or(0b01, std::memory_order_relaxed); }

    void M_read_end() const { state_.fetch_and(~0b01, std::memory_order_relaxed); }

    bool M_is_writing() const { return (state_.load(std::memory_order_relaxed) & 0b10) != 0; }

    bool M_is_reading() const { return (state_.load(std::memory_order_relaxed) & 0b01) != 0; }
#endif  // ECX_USE_USAGE_CHECK

public:
    // 默认构造：对 Invalidate / CleanInvalidate 策略，把 buffer_{} 零初始化产生的
    // 「脏零」cache 行 clean 到主存，避免后续被驱逐覆盖外设写入的数据。
    SpscZeroCopyQueue() noexcept {
        if constexpr (kDCachePolicy == CachePolicy::Invalidate
                      || kDCachePolicy == CachePolicy::CleanInvalidate) {
            ECX_DCACHE_CLEAN(reinterpret_cast<uint32_t*>(buffer_.data()), sizeof(buffer_));
        }
    }
    // 析构函数 / 赋值操作符使用默认实现
    // （atomic 成员非可拷贝/可移动，因此本类型同样不可拷贝/移动）

    const T* read_acquire() const noexcept {
        ECX_USAGE_ASSERT(!M_is_reading());

        // reader_ 仅由本线程（消费者）修改，relaxed 读取自己写入的值即可
        const auto r = reader_.load(std::memory_order_relaxed);
        // 与生产者 write_commit 中 writer_.store(release) 配对：
        // 看到 writer_ 的新值时，必然也能看到生产者 commit 前对 buffer_ 的写入
        const auto w = writer_.load(std::memory_order_acquire);
        if (r == w) {
            return nullptr;  // 队列为空
        }

#if ECX_USE_USAGE_CHECK
        M_read_begin();
#endif  // ECX_USE_USAGE_CHECK

        // 外设（DMA 等）已把数据写入主存；CPU 的 D-Cache 中本槽位可能持有 stale 行。
        // invalidate 丢弃这些行，让随后用户解引用返回值时从主存拉取新数据。
        if constexpr (kDCachePolicy == CachePolicy::Invalidate
                      || kDCachePolicy == CachePolicy::CleanInvalidate) {
            ECX_DCACHE_INVALIDATE(
                reinterpret_cast<uint32_t*>(const_cast<TSlot_*>(&buffer_[r])), sizeof(buffer_[r])
            );
        }

        return &buffer_[r].value;
    }

    void read_commit() noexcept {
        ECX_USAGE_ASSERT(M_is_reading());

        const auto r = reader_.load(std::memory_order_relaxed);
        // release 与生产者 write_acquire 中 reader_.load(acquire) 配对：
        // 生产者看到 reader_ 推进后，才可以安全覆写之前的那个槽
        reader_.store(S_next(r), std::memory_order_release);

#if ECX_USE_USAGE_CHECK
        M_read_end();
#endif  // ECX_USE_USAGE_CHECK
    }

    T* write_acquire() noexcept {
        ECX_USAGE_ASSERT(!M_is_writing());

        const auto w = writer_.load(std::memory_order_relaxed);
        // 与消费者 read_commit 中 reader_.store(release) 配对
        const auto r = reader_.load(std::memory_order_acquire);
        if (S_next(w) == r) {
            return nullptr;  // 队列已满
        }

#if ECX_USE_USAGE_CHECK
        M_write_begin();
#endif  // ECX_USE_USAGE_CHECK

        return &buffer_[w].value;
    }

    void write_commit() noexcept {
        ECX_USAGE_ASSERT(M_is_writing());

        const auto w = writer_.load(std::memory_order_relaxed);

        // CPU 通过 D-Cache 写入了数据；clean 把脏行刷回主存，
        // 这样外设从主存读时拿到最新值。必须发生在 writer_.store(release) 之前——
        // 让观察到 writer_ 推进的一侧能看到新数据。
        if constexpr (kDCachePolicy == CachePolicy::Clean
                      || kDCachePolicy == CachePolicy::CleanInvalidate) {
            ECX_DCACHE_CLEAN(reinterpret_cast<uint32_t*>(&buffer_[w]), sizeof(buffer_[w]));
        }

        // release 与消费者 read_acquire 中 writer_.load(acquire) 配对
        writer_.store(S_next(w), std::memory_order_release);

#if ECX_USE_USAGE_CHECK
        M_write_end();
#endif  // ECX_USE_USAGE_CHECK
    }

    std::size_t size() const noexcept {
        const auto w = writer_.load(std::memory_order_relaxed);
        const auto r = reader_.load(std::memory_order_relaxed);
        return (w >= r) ? (w - r) : (kNSlots_ - (r - w));
    }

    bool empty() const noexcept {
        return writer_.load(std::memory_order_relaxed) == reader_.load(std::memory_order_relaxed);
    }

    bool full() const noexcept {
        const auto w = writer_.load(std::memory_order_relaxed);
        const auto r = reader_.load(std::memory_order_relaxed);
        return S_next(w) == r;
    }

    static constexpr std::size_t capacity() noexcept { return kCapacity; }
};

}  // namespace ecx
