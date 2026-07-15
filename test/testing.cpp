#include <libutils/testing.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>

using utils::testing::LifetimeStats;

template <typename T>
using LT = utils::testing::Lifetime<T>;

TEST_CASE("Lifetime - default constructor")
{
    LT<int>::clear_stats();
    {
        LT<int> a;
        REQUIRE(LT<int>::get_stat(LifetimeStats::DefaultConstructor) == 1);
        REQUIRE(LT<int>::get_stat(LifetimeStats::ObjectCount) == 1);
        REQUIRE(LT<int>::get_stat(LifetimeStats::ObjectTotalCount) == 1);
    }
    REQUIRE(LT<int>::get_stat(LifetimeStats::Destructor) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::ObjectCount) == 0);
}

TEST_CASE("Lifetime - value constructor")
{
    LT<int>::clear_stats();
    LT<int> a{42};
    REQUIRE(a.value() == 42);
    REQUIRE(LT<int>::get_stat(LifetimeStats::Constructor) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::DefaultConstructor) == 0);
}

TEST_CASE("Lifetime - copy constructor")
{
    LT<int>::clear_stats();
    LT<int> a{1};
    LT<int> b{a};
    REQUIRE(b.value() == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::CopyConstructor) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::ObjectCount) == 2);
}

TEST_CASE("Lifetime - move constructor")
{
    LT<int>::clear_stats();
    LT<int> a{7};
    LT<int> b{std::move(a)};
    REQUIRE(b.value() == 7);
    REQUIRE(LT<int>::get_stat(LifetimeStats::MoveConstructor) == 1);
}

TEST_CASE("Lifetime - copy assignment")
{
    LT<int>::clear_stats();
    LT<int> a{1};
    LT<int> b;
    b = a;
    REQUIRE(b.value() == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::CopyAssignment) == 1);
}

TEST_CASE("Lifetime - move assignment")
{
    LT<int>::clear_stats();
    LT<int> a{5};
    LT<int> b;
    b = std::move(a);
    REQUIRE(b.value() == 5);
    REQUIRE(LT<int>::get_stat(LifetimeStats::MoveAssignment) == 1);
}

TEST_CASE("Lifetime - member swap")
{
    LT<int>::clear_stats();
    LT<int> a{1};
    LT<int> b{2};
    a.swap(b);
    REQUIRE(a.value() == 2);
    REQUIRE(b.value() == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::MemberSwap) == 1);
}

TEST_CASE("Lifetime - non-member swap")
{
    LT<int>::clear_stats();
    LT<int> a{1};
    LT<int> b{2};
    swap(a, b);
    REQUIRE(a.value() == 2);
    REQUIRE(b.value() == 1);
    // non-member swap delegates to member swap, so both counters go up
    REQUIRE(LT<int>::get_stat(LifetimeStats::NonMemberSwap) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::MemberSwap) == 1);
}

TEST_CASE("Lifetime - CRTP isolation: Lifetime<int> and Lifetime<string> have separate counters")
{
    LT<int>::clear_stats();
    LT<std::string>::clear_stats();

    // Create 1 int by value constructor
    { LT<int> i{42}; }

    // Create 3 strings by default constructor
    {
        LT<std::string> s1;
        LT<std::string> s2;
        LT<std::string> s3;
    }

    // int counters must not include string operations
    REQUIRE(LT<int>::get_stat(LifetimeStats::Constructor) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::DefaultConstructor) == 0);
    REQUIRE(LT<int>::get_stat(LifetimeStats::Destructor) == 1);
    REQUIRE(LT<int>::get_stat(LifetimeStats::ObjectCount) == 0);

    // string counters must not include int operations
    REQUIRE(LT<std::string>::get_stat(LifetimeStats::DefaultConstructor) == 3);
    REQUIRE(LT<std::string>::get_stat(LifetimeStats::Constructor) == 0);
    REQUIRE(LT<std::string>::get_stat(LifetimeStats::Destructor) == 3);
    REQUIRE(LT<std::string>::get_stat(LifetimeStats::ObjectCount) == 0);
}

TEST_CASE("Lifetime - Stats alias is LifetimeStats")
{
    static_assert(std::is_same_v<LT<int>::Stats, LifetimeStats>);
    LT<int>::clear_stats();
    LT<int> a;
    REQUIRE(LT<int>::get_stat(LT<int>::Stats::DefaultConstructor) == 1);
}
