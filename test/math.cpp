#include <libutils/math.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Math - is_even / is_odd")
{
    REQUIRE(utils::math::is_even(0));
    REQUIRE(utils::math::is_even(4));
    REQUIRE_FALSE(utils::math::is_even(3));
    REQUIRE(utils::math::is_odd(1));
    REQUIRE(utils::math::is_odd(7));
    REQUIRE_FALSE(utils::math::is_odd(8));
}

TEST_CASE("Math - nearly_equal")
{
    REQUIRE(utils::math::nearly_equal(1.0, 1.0));
    REQUIRE(utils::math::nearly_equal(0.1 + 0.2, 0.3));
    REQUIRE_FALSE(utils::math::nearly_equal(1.0, 2.0));

    REQUIRE(utils::math::nearly_equal(1.0f, 1.0f));
    REQUIRE_FALSE(utils::math::nearly_equal(1.0f, 1.1f));
}

TEST_CASE("Math - random range")
{
    for (int i = 0; i < 100; ++i) {
        auto v = utils::math::random(1, 10);
        REQUIRE(v >= 1);
        REQUIRE(v <= 10);
    }
}

TEST_CASE("Math - simple_moving_average")
{
    utils::math::simple_moving_average<4> sma;
    sma(10);
    sma(20);
    sma(30);
    auto avg = sma(40);
    REQUIRE(avg == 25); // (10+20+30+40)/4
}
