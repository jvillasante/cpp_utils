#include "algorithms.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <list>
#include <string>

TEST(Algorithms, Split)
{
    std::string const s{"a-b-c-d-e-f-g"};
    auto binfunc([](auto it_a, auto it_b) { return std::string(it_a, it_b); });

    std::list<std::string> l;
    utils::algorithms::split(std::begin(s), std::end(s), std::back_inserter(l),
                             '-', binfunc);

    EXPECT_THAT(l, ::testing::ElementsAre("a", "b", "c", "d", "e", "f", "g"));
}

TEST(Algorithms, Gather)
{
    auto is_a([](char c) { return c == 'a'; });
    std::string a{"a_a_a_a_a_a_a_a_a_a_a"};

    auto middle(std::begin(a) +
                static_cast<std::string::difference_type>(std::size(a) / 2));
    utils::algorithms::gather(std::begin(a), std::end(a), middle, is_a);
    EXPECT_STREQ(a.c_str(), "_____aaaaaaaaaaa_____");

    utils::algorithms::gather(std::begin(a), std::end(a), std::begin(a), is_a);
    EXPECT_STREQ(a.c_str(), "aaaaaaaaaaa__________");

    utils::algorithms::gather(std::begin(a), std::end(a), std::end(a), is_a);
    EXPECT_STREQ(a.c_str(), "__________aaaaaaaaaaa");

    // This will NOT work as naively expected
    utils::algorithms::gather(std::begin(a), std::end(a), middle, is_a);
    EXPECT_STREQ(a.c_str(), "__________aaaaaaaaaaa");

    std::string b{"_9_2_4_7_3_8_1_6_5_0_"};
    utils::algorithms::gather_sort(
        std::begin(b), std::end(b),
        std::begin(b) +
            static_cast<std::string::difference_type>(std::size(b) / 2));
    EXPECT_STREQ(b.c_str(), "_____9743201568______");
}
