#include <libutils/overloaded.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <variant>

TEST_CASE("overloaded - visits the active alternative")
{
    std::variant<int, std::string> v = 42;

    auto stringify = utils::overloaded{
        [](int i) {
            return "int:" + std::to_string(i);
        },
        [](std::string const& s) {
            return "str:" + s;
        },
    };

    REQUIRE(std::visit(stringify, v) == "int:42");

    v = std::string{"hi"};
    REQUIRE(std::visit(stringify, v) == "str:hi");
}

TEST_CASE("overloaded - is directly callable as an overload set")
{
    auto f = utils::overloaded{
        [](int) {
            return 1;
        },
        [](double) {
            return 2;
        },
    };

    REQUIRE(f(0) == 1);
    REQUIRE(f(0.0) == 2);
}
