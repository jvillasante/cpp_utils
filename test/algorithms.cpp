#include <libutils/algorithms.hpp>

#include <catch2/catch_test_macros.hpp>
#include <list>
#include <string>

TEST_CASE("Algorithms - Split")
{
    std::string const s{"a-b-c-d-e-f-g"};
    auto binfunc([](auto it_a, auto it_b) { return std::string(it_a, it_b); });

    std::list<std::string> l;
    utils::algorithms::split(std::begin(s), std::end(s), std::back_inserter(l),
                             '-', binfunc);

    REQUIRE(l == std::list<std::string>{"a", "b", "c", "d", "e", "f", "g"});
}

TEST_CASE("Algorithms - Gather")
{
    auto is_a([](char c) { return c == 'a'; });
    std::string a{"a_a_a_a_a_a_a_a_a_a_a"};

    auto middle(std::begin(a) +
                static_cast<std::string::difference_type>(std::size(a) / 2));
    auto [first1, last1] =
        utils::algorithms::gather(std::begin(a), std::end(a), middle, is_a);
    REQUIRE(a == "_____aaaaaaaaaaa_____");
    REQUIRE(std::distance(first1, last1) == 11);

    auto [first2, last2] = utils::algorithms::gather(
        std::begin(a), std::end(a), std::begin(a), is_a);
    REQUIRE(a == "aaaaaaaaaaa__________");
    REQUIRE(first2 == std::begin(a));

    auto [first3, last3] = utils::algorithms::gather(
        std::begin(a), std::end(a), std::end(a), is_a);
    REQUIRE(a == "__________aaaaaaaaaaa");
    REQUIRE(last3 == std::end(a));

    // This will NOT work as naively expected — already-sorted range ignored
    [[maybe_unused]] auto [first4, last4] =
        utils::algorithms::gather(std::begin(a), std::end(a), middle, is_a);
    REQUIRE(a == "__________aaaaaaaaaaa");

    std::string b{"_9_2_4_7_3_8_1_6_5_0_"};
    utils::algorithms::gather_sort(
        std::begin(b), std::end(b),
        std::begin(b) +
            static_cast<std::string::difference_type>(std::size(b) / 2));
    REQUIRE(b == "_____9743201568______");
}
