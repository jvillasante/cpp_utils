#include <libutils/strings.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

TEST_CASE("Strings - to_string")
{
    REQUIRE(utils::strings::to_string(42) == "42");
    REQUIRE(utils::strings::to_string(3.14f) == std::to_string(3.14f));
}

TEST_CASE("Strings - to_upper / to_lower")
{
    REQUIRE(utils::strings::to_upper<char>("hello") == "HELLO");
    REQUIRE(utils::strings::to_lower<char>("WORLD") == "world");
}

TEST_CASE("Strings - reverse")
{
    REQUIRE(utils::strings::reverse<char>(std::string{"abcd"}) == "dcba");
}

TEST_CASE("Strings - trim")
{
    REQUIRE(utils::strings::trim<char>(std::string{"  hello  "}) == "hello");
    REQUIRE(utils::strings::trim<char>(std::string{"   "}) == "");
    REQUIRE(utils::strings::trimleft<char>(std::string{"  hi"}) == "hi");
    REQUIRE(utils::strings::trimright<char>(std::string{"hi  "}) == "hi");

    // All whitespace variants, not just space
    REQUIRE(utils::strings::trim<char>(std::string{"\t hello \n"}) == "hello");
    REQUIRE(utils::strings::trimleft<char>(std::string{"\n\thi"}) == "hi");
    REQUIRE(utils::strings::trimright<char>(std::string{"hi\r\n"}) == "hi");
    REQUIRE(utils::strings::trim<char>(std::string{"\t\r\n"}) == "");
}

TEST_CASE("Strings - trim with charset")
{
    REQUIRE(utils::strings::trim<char>(std::string{"--hello--"},
                                       std::string{"-"}) == "hello");
}

TEST_CASE("Strings - remove")
{
    REQUIRE(utils::strings::remove<char>(std::string{"hello world"}, ' ') ==
            "helloworld");
}

TEST_CASE("Strings - join")
{
    std::vector<std::string> v{"a", "b", "c"};
    REQUIRE(utils::strings::join<char>(v, ",") == "a,b,c");
}

TEST_CASE("Strings - split by char")
{
    auto tokens =
        utils::strings::split<char>(std::string{"a,b,c"}, ',');
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[2] == "c");
}

TEST_CASE("Strings - split by delimiters")
{
    auto tokens = utils::strings::split<char>(std::string{"a,b;c"},
                                              std::string{",;"});
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[1] == "b");
}

TEST_CASE("Strings - equal")
{
    REQUIRE(utils::strings::equal<char>("hello", "hello"));
    REQUIRE_FALSE(utils::strings::equal<char>("Hello", "hello"));
    REQUIRE(utils::strings::equal<char>("Hello", "hello", true));
}

TEST_CASE("Strings - to_hex (no trailing space)")
{
    std::vector<std::uint8_t> data{0xDE, 0xAD, 0xBE};
    auto hex = utils::strings::to_hex<char>(data, true, false);
    REQUIRE(hex == "DEADBE");
    auto spaced = utils::strings::to_hex<char>(data, false, true);
    REQUIRE(spaced == "de ad be");
}

TEST_CASE("Strings - hex_to_bytes")
{
    auto bytes = utils::strings::hex_to_bytes<char>("DEADBE");
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == std::byte{0xDE});
    REQUIRE(bytes[1] == std::byte{0xAD});
    REQUIRE(bytes[2] == std::byte{0xBE});
}

TEST_CASE("Strings - hex_to_bytes invalid")
{
    REQUIRE_THROWS_AS(utils::strings::hex_to_bytes<char>("ZZ"),
                      std::invalid_argument);
}

TEST_CASE("Strings - to_integral")
{
    REQUIRE(utils::strings::to_integral<int>("42") == 42);
    REQUIRE(utils::strings::to_integral<int>("-7") == -7);
    REQUIRE(utils::strings::to_integral<std::uint64_t>("18446744073709551615") ==
            18446744073709551615ULL);
    REQUIRE_THROWS_AS(utils::strings::to_integral<int>("abc"),
                      std::invalid_argument);
}

TEST_CASE("Strings - join with iterator range")
{
    std::vector<std::string> v{"x", "y", "z"};
    REQUIRE(utils::strings::join<char>(v.begin(), v.end(), ",") == "x,y,z");
}

TEST_CASE("Strings - mutable_version::trim")
{
    std::string s = "  \t hello \n ";
    utils::strings::mutable_version::trim<char>(s);
    REQUIRE(s == "hello");
}

TEST_CASE("Strings - mutable_version::trimleft")
{
    std::string s = "\n\thello";
    utils::strings::mutable_version::trimleft<char>(s);
    REQUIRE(s == "hello");
}

TEST_CASE("Strings - mutable_version::trimright")
{
    std::string s = "hello\r\n";
    utils::strings::mutable_version::trimright<char>(s);
    REQUIRE(s == "hello");
}

TEST_CASE("Strings - mutable_version::to_upper")
{
    std::string s = "hello";
    utils::strings::mutable_version::to_upper<char>(s);
    REQUIRE(s == "HELLO");
}

TEST_CASE("Strings - mutable_version::to_lower")
{
    std::string s = "WORLD";
    utils::strings::mutable_version::to_lower<char>(s);
    REQUIRE(s == "world");
}

TEST_CASE("Strings - mutable_version::reverse")
{
    std::string s = "abcd";
    utils::strings::mutable_version::reverse<char>(s);
    REQUIRE(s == "dcba");
}
