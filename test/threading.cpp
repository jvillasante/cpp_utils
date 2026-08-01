#include <libutils/threading.hpp>

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

TEST_CASE("threading - join_all joins every joinable thread")
{
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    threads.reserve(4);
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&counter] {
            ++counter;
        });
    }

    utils::threading::join_all(threads);

    REQUIRE(counter.load() == 4);
    for (auto const& t : threads) {
        REQUIRE_FALSE(t.joinable());
    }
}

TEST_CASE(
    "threading - anti_lock unlocks on construction, relocks on destruction")
{
    std::mutex m;
    std::unique_lock<std::mutex> guard(m);
    REQUIRE(guard.owns_lock());

    auto other_can_lock = [&m] {
        bool locked = false;
        std::thread t([&] {
            if (m.try_lock()) {
                locked = true;
                m.unlock();
            }
        });
        t.join();
        return locked;
    };

    {
        utils::threading::anti_lock<std::unique_lock<std::mutex>> anti(guard);
        // While the anti_lock is alive the mutex is unlocked.
        REQUIRE(other_can_lock());
    }
    // The anti_lock destructor relocked the mutex.
    REQUIRE_FALSE(other_can_lock());

    guard.unlock();
}

TEST_CASE("threading - pcout writes to std::cout on destruction")
{
    std::ostringstream captured;
    auto* const old = std::cout.rdbuf(captured.rdbuf());
    {
        utils::threading::pcout{} << "hello " << 42;
    }
    std::cout.rdbuf(old);

    REQUIRE(captured.str() == "hello 42");
}
