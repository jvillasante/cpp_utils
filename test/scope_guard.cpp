#include <libutils/scope_guard.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ScopeGuard - make_guard runs on scope exit")
{
    int counter = 0;
    {
        auto guard = utils::make_guard([&]() noexcept { ++counter; });
    }
    REQUIRE(counter == 1);
}

TEST_CASE("ScopeGuard - dismiss prevents execution")
{
    int counter = 0;
    {
        auto guard = utils::make_guard([&]() noexcept { ++counter; });
        guard.dismiss();
    }
    REQUIRE(counter == 0);
}

TEST_CASE("ScopeGuard - rehire after dismiss re-enables execution")
{
    int counter = 0;
    {
        auto guard = utils::make_guard([&]() noexcept { ++counter; });
        guard.dismiss();
        guard.rehire();
    }
    REQUIRE(counter == 1);
}

TEST_CASE("ScopeGuard - ON_SCOPE_EXIT macro")
{
    int counter = 0;
    {
        ON_SCOPE_EXIT { ++counter; };
    }
    REQUIRE(counter == 1);
}

TEST_CASE("ScopeGuard - ON_SCOPE_EXIT_NAMED macro with dismiss")
{
    int counter = 0;
    {
        ON_SCOPE_EXIT_NAMED(guard) { ++counter; };
        guard.dismiss();
    }
    REQUIRE(counter == 0);
}
