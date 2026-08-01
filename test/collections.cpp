#include <libutils/collections.hpp>

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <iterator>
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
    REQUIRE(std::is_sorted(std::begin(v), std::end(v)));
}

TEST_CASE("Collections - areaof generic uses size")
{
    std::array<int, 4> arr{1, 2, 3, 4};
    std::size_t const expected = (arr.size() * sizeof(int)) + sizeof(arr);
    REQUIRE(utils::collections::areaof(arr) == expected);
}

TEST_CASE("Collections - areaof vector uses capacity not size")
{
    std::vector<int> v;
    v.reserve(10);
    v.push_back(1);
    v.push_back(2);
    // size=2, capacity=10: areaof must use capacity
    std::size_t const expected = (v.capacity() * sizeof(int)) + sizeof(v);
    REQUIRE(utils::collections::areaof(v) == expected);
    REQUIRE(utils::collections::areaof(v) !=
            v.size() * sizeof(int) + sizeof(v));
}

TEST_CASE("Collections - memory_utilization generic")
{
    std::array<int, 4> arr{1, 2, 3, 4};
    auto const useful = static_cast<double>(arr.size() * sizeof(int));
    auto const total = static_cast<double>(utils::collections::areaof(arr));
    REQUIRE(utils::collections::memory_utilization(arr) == useful / total);
}

TEST_CASE("Collections - memory_utilization vector partial fill")
{
    std::vector<int> v;
    v.reserve(10);
    v.push_back(1);
    v.push_back(2);
    // useful = 2 elements, total based on capacity=10
    auto const useful = static_cast<double>(v.size() * sizeof(int));
    auto const total = static_cast<double>(utils::collections::areaof(v));
    REQUIRE(utils::collections::memory_utilization(v) == useful / total);
    REQUIRE(utils::collections::memory_utilization(v) < 1.0);
}
