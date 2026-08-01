#include <libutils/polyfill.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

// These tests target the polyfills in polyfill.hpp. When compiled at C++20 or
// later, utils::span/make_unique/exchange/bit_cast resolve to the std versions;
// below that they are the fallbacks in this header. The suite is written to
// pass identically on both so the fallbacks are pinned to the std behavior. The
// polyfill_cxx17 target in CMakeLists.txt compiles this file at C++17 to
// exercise the fallback path.

TEST_CASE("polyfill - make_unique")
{
    auto p = utils::make_unique<int>(42);
    REQUIRE(p != nullptr);
    REQUIRE(*p == 42);

    auto arr = utils::make_unique<int[]>(3);
    arr[0] = 1;
    arr[2] = 3;
    REQUIRE(arr[0] == 1);
    REQUIRE(arr[2] == 3);
}

TEST_CASE("polyfill - exchange")
{
    int x = 1;
    int const old = utils::exchange(x, 2);
    REQUIRE(old == 1);
    REQUIRE(x == 2);
}

TEST_CASE("polyfill - bit_cast")
{
    float const f = 1.0f;
    auto const bits = utils::bit_cast<std::uint32_t>(f);
    REQUIRE(bits == 0x3F800000u); // IEEE-754 representation of 1.0f
    REQUIRE(utils::bit_cast<float>(bits) == 1.0f);
}

TEST_CASE("span - default constructed is empty")
{
    utils::span<int> s;
    REQUIRE(s.size() == 0u);
    REQUIRE(s.empty());
    REQUIRE(s.data() == nullptr);
    REQUIRE(s.begin() == s.end());
}

TEST_CASE("span - pointer and size construction")
{
    int arr[] = {1, 2, 3, 4};
    utils::span<int> s(arr, 4);

    REQUIRE(s.size() == 4u);
    REQUIRE_FALSE(s.empty());
    REQUIRE(s.data() == arr);
    REQUIRE(s[0] == 1);
    REQUIRE(s[3] == 4);
    REQUIRE(s.front() == 1);
    REQUIRE(s.back() == 4);
    REQUIRE(s.size_bytes() == 4u * sizeof(int));
}

TEST_CASE("span - pointer pair construction")
{
    int arr[] = {5, 6, 7};
    utils::span<int> s(arr, arr + 3);

    REQUIRE(s.size() == 3u);
    REQUIRE(s.front() == 5);
    REQUIRE(s.back() == 7);
}

TEST_CASE("span - C-array construction deduces size")
{
    int arr[] = {10, 20, 30};
    utils::span<int> s(arr);

    REQUIRE(s.size() == 3u);
    REQUIRE(s.data() == arr);
}

TEST_CASE("span - range-for iteration")
{
    int arr[] = {1, 2, 3, 4};
    utils::span<int> s(arr);

    int sum = 0;
    for (int v : s) {
        sum += v;
    }
    REQUIRE(sum == 10);
}

TEST_CASE("span - mutable view over a vector writes through")
{
    std::vector<int> vec{1, 2, 3};
    utils::span<int> s(vec);

    REQUIRE(s.size() == 3u);
    REQUIRE(s.data() == vec.data());

    s[0] = 100;
    s.back() = 300;
    REQUIRE(vec[0] == 100);
    REQUIRE(vec[2] == 300);
}

TEST_CASE("span - constness of the span does not propagate to elements")
{
    // A const span is like a const pointer: the pointee stays mutable.
    int arr[] = {1, 2, 3};
    utils::span<int> const s(arr);

    s[0] = 42;
    s.front() = 7;
    REQUIRE(arr[0] == 7);
    REQUIRE(arr[1] == 2);

    static_assert(std::is_same<decltype(s[0]), int&>::value,
                  "operator[] on a const span must yield a mutable reference");
}

TEST_CASE("span - view of const elements over a const vector")
{
    std::vector<int> const vec{1, 2, 3};
    utils::span<int const> s(vec);

    REQUIRE(s.size() == 3u);
    REQUIRE(s[1] == 2);

    static_assert(std::is_same<decltype(s[0]), int const&>::value,
                  "span<int const> must yield a const reference");
}

TEST_CASE("span - first, last and subspan")
{
    int arr[] = {0, 1, 2, 3, 4};
    utils::span<int> s(arr);

    auto head = s.first(2);
    REQUIRE(head.size() == 2u);
    REQUIRE(head.front() == 0);
    REQUIRE(head.back() == 1);

    auto tail = s.last(2);
    REQUIRE(tail.size() == 2u);
    REQUIRE(tail.front() == 3);
    REQUIRE(tail.back() == 4);

    auto mid = s.subspan(1, 3);
    REQUIRE(mid.size() == 3u);
    REQUIRE(mid.front() == 1);
    REQUIRE(mid.back() == 3);

    // Default count runs to the end.
    auto rest = s.subspan(2);
    REQUIRE(rest.size() == 3u);
    REQUIRE(rest.front() == 2);
    REQUIRE(rest.back() == 4);

    // Subviews are views: writing through one mutates the original buffer.
    mid[0] = 99;
    REQUIRE(arr[1] == 99);
}

TEST_CASE("span - reverse iteration")
{
    int arr[] = {1, 2, 3};
    utils::span<int> s(arr);

    std::vector<int> reversed(s.rbegin(), s.rend());
    REQUIRE(reversed == std::vector<int>{3, 2, 1});
}

TEST_CASE("span - member typedefs are present")
{
    using span_t = utils::span<int>;
    static_assert(std::is_same<span_t::element_type, int>::value, "");
    static_assert(std::is_same<span_t::value_type, int>::value, "");
    static_assert(std::is_same<span_t::size_type, std::size_t>::value, "");
    static_assert(std::is_same<span_t::pointer, int*>::value, "");
    static_assert(std::is_same<span_t::reference, int&>::value, "");

    using cspan_t = utils::span<int const>;
    static_assert(std::is_same<cspan_t::value_type, int>::value,
                  "value_type strips const");
}
