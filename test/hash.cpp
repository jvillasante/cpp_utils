#include <libutils/hash.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <string>

TEST_CASE("Hash - combine produces consistent results")
{
    std::size_t seed1 = 0;
    utils::hash::combine(seed1, 42);
    utils::hash::combine(seed1, std::string{"hello"});

    std::size_t seed2 = 0;
    utils::hash::combine(seed2, 42);
    utils::hash::combine(seed2, std::string{"hello"});

    REQUIRE(seed1 == seed2);
}

TEST_CASE("Hash - combine order matters")
{
    std::size_t seed_ab = 0;
    utils::hash::combine(seed_ab, 1);
    utils::hash::combine(seed_ab, 2);

    std::size_t seed_ba = 0;
    utils::hash::combine(seed_ba, 2);
    utils::hash::combine(seed_ba, 1);

    REQUIRE(seed_ab != seed_ba);
}

TEST_CASE("Hash - different values produce different seeds")
{
    std::size_t s1 = 0;
    utils::hash::combine(s1, 1);

    std::size_t s2 = 0;
    utils::hash::combine(s2, 2);

    REQUIRE(s1 != s2);
}
