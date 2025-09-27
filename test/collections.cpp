#include <libutils/collections.hpp>

#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("Collections - QuickRemoveAt")
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    utils::collections::quick_remove_at(v, 3);
    REQUIRE(v == std::vector<int>{1, 2, 3, 10, 5, 6, 7, 8, 9});

    utils::collections::quick_remove_at(v, std::begin(v) + 5);
    REQUIRE(v == std::vector<int>{1, 2, 3, 10, 5, 9, 7, 8});
}

// TEST(Collections, InsertSorted)
TEST_CASE("Collections - InsertSorted")
{
    std::vector<int> v{1, 2, 3, 4, 5, 7, 8, 9, 10};

    utils::collections::insert_sorted(v, 6);
    REQUIRE(std::ranges::is_sorted(v));
}
