#include <libutils/iterators.hpp>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <vector>

TEST_CASE("iterators - ostream_joiner separates elements")
{
    std::vector<int> const v{1, 2, 3};
    std::ostringstream os;
    std::copy(v.begin(), v.end(),
              utils::iterators::make_ostream_joiner(os, ", "));
    REQUIRE(os.str() == "1, 2, 3");
}

TEST_CASE("iterators - ostream_joiner with a single element has no delimiter")
{
    std::vector<int> const v{7};
    std::ostringstream os;
    std::copy(v.begin(), v.end(),
              utils::iterators::make_ostream_joiner(os, "-"));
    REQUIRE(os.str() == "7");
}

TEST_CASE("iterators - ostream_joiner on an empty range writes nothing")
{
    std::vector<int> const v;
    std::ostringstream os;
    std::copy(v.begin(), v.end(),
              utils::iterators::make_ostream_joiner(os, ","));
    REQUIRE(os.str().empty());
}

TEST_CASE("iterators - ostream_joiner supports a char delimiter")
{
    std::vector<std::string> const v{"a", "b", "c"};
    std::ostringstream os;
    std::copy(v.begin(), v.end(),
              utils::iterators::make_ostream_joiner(os, '|'));
    REQUIRE(os.str() == "a|b|c");
}
