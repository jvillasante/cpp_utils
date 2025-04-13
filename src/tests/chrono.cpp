#include "chrono.hpp"
#include "testing.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

TEST(Chrono, PerfTimer)
{
    auto f = [](int ms_sleep) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
    };

    auto t =
        utils::chrono::bench::perf_timer<std::chrono::microseconds>::duration(
            f, 10);
    auto microseconds = std::chrono::duration<double, std::micro>(t).count();
    GCOUT << "Took: " << microseconds << " microseconds" << '\n';

    auto milliseconds = std::chrono::duration<double, std::milli>(t).count();
    GCOUT << "Took: " << milliseconds << " milliseconds" << '\n';
}
