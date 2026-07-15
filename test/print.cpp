#include <libutils/print.hpp>

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

TEST_CASE("Print - line with fill char only")
{
    std::ostringstream oss;
    utils::print::line('=', 5, oss);
    REQUIRE(oss.str() == "=====\n");
}

TEST_CASE("Print - line with header shorter than width")
{
    std::ostringstream oss;
    // "===" + "hi" + std::string(10 - 2 - 3, '=') + '\n'  =>  "===hi=====\n"
    utils::print::line("hi", '=', 10, oss);
    REQUIRE(oss.str() == "===hi=====\n");
}

TEST_CASE("Print - line with header longer than width falls back to header only")
{
    std::ostringstream oss;
    utils::print::line("toolongheader", '=', 5, oss);
    REQUIRE(oss.str() == "toolongheader\n");
}

TEST_CASE("Print - line header at underflow boundary falls back to header only")
{
    // header.size() = s - 1 would underflow s - size - 3 as size_t: must fall back
    std::ostringstream oss;
    utils::print::line("123456789", '=', 10, oss); // size=9, s=10: 10-9-3 underflows
    REQUIRE(oss.str() == "123456789\n");
}

TEST_CASE("Print - line header at exact fill boundary produces prefix only")
{
    // header.size() + 3 == s: no fill chars, still falls back (header only)
    std::ostringstream oss;
    utils::print::line("1234567", '=', 10, oss); // size=7, s=10: 7+3==10, not < 10
    REQUIRE(oss.str() == "1234567\n");
}

TEST_CASE("Print - new_line")
{
    std::ostringstream oss;
    utils::print::new_line("\n", oss);
    REQUIRE(oss.str() == "\n");
}

TEST_CASE("Print - collection from range collection")
{
    std::ostringstream oss;
    std::vector<int> v{1, 2, 3};
    utils::print::collection(v, '[', ']', oss);
    REQUIRE(oss.str() == "[1, 2, 3]\n");
}

TEST_CASE("Print - collection from iterators")
{
    std::ostringstream oss;
    std::vector<int> v{10, 20};
    utils::print::collection(v.begin(), v.end(), '{', '}', oss);
    REQUIRE(oss.str() == "{10, 20}\n");
}

TEST_CASE("Print - pair operator<<")
{
    std::ostringstream oss;
    oss << std::make_pair(1, 2);
    REQUIRE(oss.str() == "(1:2)");
}
