#include "collections.hpp"
#include "testing.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

TEST(Collections, QuickRemoveAt)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    utils::collections::quick_remove_at(v, 3);
    EXPECT_THAT(v, ::testing::ElementsAre(1, 2, 3, 10, 5, 6, 7, 8, 9));

    utils::collections::quick_remove_at(v, std::begin(v) + 5);
    EXPECT_THAT(v, ::testing::ElementsAre(1, 2, 3, 10, 5, 9, 7, 8));
}

TEST(Collections, InsertSorted)
{
    std::vector<int> v{1, 2, 3, 4, 5, 7, 8, 9, 10};

    utils::collections::insert_sorted(v, 6);
    EXPECT_TRUE(std::ranges::is_sorted(v));
}
