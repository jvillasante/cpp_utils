#include <libutils/functional.hpp>

#include <catch2/catch_test_macros.hpp>
#include <map>
#include <queue>
#include <string>
#include <vector>

TEST_CASE("Functional - mapf range")
{
    std::vector<int> v{1, 2, 3, 4};
    auto result = utils::functional::mapf(
        [](int x) {
            return x * 2;
        },
        v);
    REQUIRE(result == std::vector<int>{2, 4, 6, 8});
}

TEST_CASE("Functional - mapf queue")
{
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    auto result = utils::functional::mapf(
        [](int x) {
            return x + 10;
        },
        q);
    REQUIRE(result.front() == 11);
    result.pop();
    REQUIRE(result.front() == 12);
}

TEST_CASE("Functional - foldl")
{
    std::vector<int> v{1, 2, 3, 4, 5};
    auto sum = utils::functional::foldl(
        [](int acc, int x) {
            return acc + x;
        },
        v, 0);
    REQUIRE(sum == 15);
}

TEST_CASE("Functional - foldr")
{
    // foldr on a string concat should produce right-associative order
    std::vector<std::string> v{"a", "b", "c"};
    auto result = utils::functional::foldr(
        [](std::string const& a, std::string const& b) {
            return a + b;
        },
        v, std::string{});
    REQUIRE(result == "abc");
}

TEST_CASE("Functional - foldl queue (by value, non-destructive to caller)")
{
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    auto sum = utils::functional::foldl(
        [](int acc, int x) {
            return acc + x;
        },
        q, 0);
    REQUIRE(sum == 6);
    // original queue is untouched
    REQUIRE(q.size() == 3);
}

TEST_CASE("Functional - map / filter transducers")
{
    // Transducers compose right-to-left: filter first, then double.
    // filter(is_even)(map(double)(accumulate)): keep evens, then double them.
    std::vector<int> v{1, 2, 3, 4, 5};
    auto push = [](std::vector<int> acc, int x) {
        acc.push_back(x);
        return acc;
    };
    auto xf = utils::functional::filter([](int x) {
        return x % 2 == 0;
    })(utils::functional::map([](int x) {
        return x * 2;
    })(push));

    std::vector<int> result;
    for (int x : v) {
        result = xf(result, x);
    }
    REQUIRE(result == std::vector<int>{4, 8});
}

TEST_CASE("Functional - mapf std::map")
{
    std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}};
    auto result = utils::functional::mapf(
        [](std::pair<std::string const, int> const& kvp) {
            return std::make_pair(kvp.first, kvp.second * 10);
        },
        m);
    REQUIRE(result["a"] == 10);
    REQUIRE(result["b"] == 20);
    REQUIRE(result["c"] == 30);
}

TEST_CASE("Functional - concat composes left-to-right (outer applied last)")
{
    // concat(f, g)(x) = f(g(x))
    auto times2 = [](int x) {
        return x * 2;
    };
    auto add1 = [](int x) {
        return x + 1;
    };
    auto f = utils::functional::concat(add1, times2); // add1(times2(x))
    REQUIRE(f(3) == 7);                               // 3*2=6, 6+1=7
}

TEST_CASE("Functional - concat three functions")
{
    auto times2 = [](int x) {
        return x * 2;
    };
    auto add1 = [](int x) {
        return x + 1;
    };
    auto negate = [](int x) {
        return -x;
    };
    // concat(negate, add1, times2)(x) = negate(add1(times2(x)))
    auto f = utils::functional::concat(negate, add1, times2);
    REQUIRE(f(3) == -7); // 3*2=6, 6+1=7, negate(7)=-7
}

TEST_CASE("Functional - combine applies two functions and merges results")
{
    auto square = [](int x) {
        return x * x;
    };
    auto negate = [](int x) {
        return -x;
    };
    // combine(plus, square, negate)(3) = square(3) + negate(3) = 9 + (-3) = 6
    auto f = utils::functional::combine(
        [](int a, int b) {
            return a + b;
        },
        square, negate);
    REQUIRE(f(3) == 6);
    REQUIRE(f(5) == 20); // 25 + (-5)
}
