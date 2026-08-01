#include <libutils/chrono.hpp>

#include <catch2/catch_test_macros.hpp>
#include <chrono>

TEST_CASE("chrono - perf_timer invokes the callable and returns a duration")
{
    bool called = false;
    auto const d = utils::chrono::perf_timer<>::duration([&] {
        called = true;
    });

    REQUIRE(called);
    REQUIRE(d.count() >= 0);
}

TEST_CASE("chrono - perf_timer forwards arguments")
{
    int sum = 0;
    auto const d =
        utils::chrono::perf_timer<std::chrono::nanoseconds>::duration(
            [&](int a, int b) {
                sum = a + b;
            },
            2, 3);

    REQUIRE(sum == 5);
    REQUIRE(d.count() >= 0);
}

TEST_CASE("chrono - perf_timer duration_with_result returns time and value")
{
    auto const [d, result] = utils::chrono::perf_timer<>::duration_with_result(
        [](int a, int b) {
            return a * b;
        },
        6, 7);

    REQUIRE(result == 42);
    REQUIRE(d.count() >= 0);
}
