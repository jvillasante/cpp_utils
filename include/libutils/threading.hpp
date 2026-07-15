#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>

namespace utils::threading
{
struct pcout : public std::stringstream
{
    static inline std::mutex mtx_;
    ~pcout() override
    {
        std::lock_guard<std::mutex> _{mtx_};
        std::cout << rdbuf();
        std::cout.flush();
    }

    pcout() = default;
    pcout(pcout const&) = delete;
    pcout(pcout&&) noexcept = delete;
    pcout& operator=(pcout const&) = delete;
    pcout& operator=(pcout&&) noexcept = delete;
};

template <typename Iter>
void join_all(Iter first, Iter last)
{
    std::for_each(first, last, [](std::thread& t) {
        if (t.joinable()) { t.join(); }
    });
}

template <typename Collection>
void join_all(Collection& collection)
{
    join_all(std::begin(collection), std::end(collection));
}

/**
 * The anti-lock unlocks a `mutex` at construction and locks it at destruction.
 * Requires a Guard type that exposes a mutex() method (e.g. std::unique_lock).
 * std::lock_guard does not qualify — use std::unique_lock instead.
 */
template <typename Guard>
struct anti_lock
{
    using mutex_type = typename Guard::mutex_type;

    static_assert(
        requires(Guard& g) { g.mutex(); },
        "anti_lock requires a guard with a mutex() method; "
        "use std::unique_lock, not std::lock_guard");

    explicit anti_lock(Guard& guard) : mutex_(guard.mutex())
    {
        if (mutex_) { mutex_->unlock(); }
    }

private:
    struct anti_lock_deleter
    {
        void operator()(mutex_type* mutex) { mutex->lock(); }
    };

    std::unique_ptr<mutex_type, anti_lock_deleter> mutex_;
};
} // namespace utils::threading
