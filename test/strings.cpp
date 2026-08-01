#include <libutils/strings.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
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

TEST_CASE("Strings - wide-char case folding is not truncated")
{
    // ASCII folding through the wide-char path.
    REQUIRE(utils::strings::to_upper<wchar_t>(std::wstring{L"hello"}) ==
            L"HELLO");
    REQUIRE(utils::strings::to_lower<wchar_t>(std::wstring{L"WORLD"}) ==
            L"world");

    // A code point above 0xFF that the C locale leaves unchanged. With the old
    // `char` return type it was truncated to a low byte; it must round-trip now.
    wchar_t const wc = L'中'; // CJK '中', not cased in the C locale
    REQUIRE(utils::strings::my_toupper(wc) == wc);
    REQUIRE(utils::strings::my_tolower(wc) == wc);
}

TEST_CASE("Strings - to_hex does not sign-extend high bytes")
{
    // Signed char values >= 0x80 previously printed as "ffffff80" via
    // sign-extension; they must render as a single byte.
    std::vector<signed char> data{static_cast<signed char>(0x80),
                                  static_cast<signed char>(0xFF),
                                  static_cast<signed char>(0x7F)};
    REQUIRE(utils::strings::to_hex<char>(data, true, false) == "80FF7F");
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
    auto tokens = utils::strings::split<char>(std::string{"a,b,c"}, ',');
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[2] == "c");
}

TEST_CASE("Strings - split by delimiters")
{
    auto tokens =
        utils::strings::split<char>(std::string{"a,b;c"}, std::string{",;"});
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
    REQUIRE(utils::strings::to_integral<std::uint64_t>(
                "18446744073709551615") == 18446744073709551615ULL);
    REQUIRE_THROWS_AS(utils::strings::to_integral<int>("abc"),
                      std::invalid_argument);
    // Trailing garbage must be rejected, not silently truncated to 123.
    REQUIRE_THROWS_AS(utils::strings::to_integral<int>("123abc"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(utils::strings::to_integral<int>("12 34"),
                      std::invalid_argument);
}

TEST_CASE("Strings - split_view (non-owning)")
{
    std::string const text = "a,b;c";
    auto tokens =
        utils::strings::split_view<char>(text, std::string_view{",;"});
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == "b");
    REQUIRE(tokens[2] == "c");
    // Views point back into the original buffer (no copies).
    REQUIRE(tokens[0].data() == text.data());

    auto by_char = utils::strings::split_view<char>(text, ',');
    REQUIRE(by_char.size() == 2);
    REQUIRE(by_char[0] == "a");
    REQUIRE(by_char[1] == "b;c");

    // Empty tokens are skipped, matching split().
    auto skipped =
        utils::strings::split_view<char>(std::string_view{",,a,,b,,"}, ',');
    REQUIRE(skipped.size() == 2);
    REQUIRE(skipped[0] == "a");
    REQUIRE(skipped[1] == "b");
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

TEST_CASE("Strings - replace_all substring")
{
    REQUIRE(utils::strings::replace_all<char>("a.b.c", ".", "::") ==
            "a::b::c");
    // to contains from: must not re-scan the replacement (no infinite loop).
    REQUIRE(utils::strings::replace_all<char>("aaa", "a", "aa") == "aaaaaa");
    // Deletion.
    REQUIRE(utils::strings::replace_all<char>("a-b-c", "-", "") == "abc");
    // Empty from is a no-op.
    REQUIRE(utils::strings::replace_all<char>("abc", "", "x") == "abc");
    // No match.
    REQUIRE(utils::strings::replace_all<char>("abc", "z", "y") == "abc");
}

TEST_CASE("Strings - replace_all single char")
{
    REQUIRE(utils::strings::replace_all<char>(std::string{"a.b.c"}, '.', '_') ==
            "a_b_c");
}

TEST_CASE("Strings - replace_first")
{
    REQUIRE(utils::strings::replace_first<char>("a.b.c", ".", "::") ==
            "a::b.c");
    REQUIRE(utils::strings::replace_first<char>("abc", "z", "y") == "abc");
    REQUIRE(utils::strings::replace_first<char>("abc", "", "y") == "abc");
}

TEST_CASE("Strings - starts_with / ends_with with ignore_case")
{
    REQUIRE(utils::strings::starts_with<char>("hello", "he"));
    REQUIRE_FALSE(utils::strings::starts_with<char>("hello", "HE"));
    REQUIRE(utils::strings::starts_with<char>("hello", "HE", true));
    REQUIRE_FALSE(utils::strings::starts_with<char>("hi", "hello"));

    REQUIRE(utils::strings::ends_with<char>("hello", "lo"));
    REQUIRE_FALSE(utils::strings::ends_with<char>("hello", "LO"));
    REQUIRE(utils::strings::ends_with<char>("hello", "LO", true));
    REQUIRE_FALSE(utils::strings::ends_with<char>("hi", "hello"));
}

TEST_CASE("Strings - split keep_empty")
{
    auto skipped = utils::strings::split<char>(std::string_view{"a,,c"}, ',');
    REQUIRE(skipped.size() == 2);
    REQUIRE(skipped[0] == "a");
    REQUIRE(skipped[1] == "c");

    auto kept =
        utils::strings::split<char>(std::string_view{"a,,c"}, ',', true);
    REQUIRE(kept.size() == 3);
    REQUIRE(kept[0] == "a");
    REQUIRE(kept[1].empty());
    REQUIRE(kept[2] == "c");

    // Leading and trailing empties are preserved too.
    auto edges =
        utils::strings::split<char>(std::string_view{",a,"}, ',', true);
    REQUIRE(edges.size() == 3);
    REQUIRE(edges[0].empty());
    REQUIRE(edges[1] == "a");
    REQUIRE(edges[2].empty());
}

TEST_CASE("Strings - split_view keep_empty")
{
    auto kept = utils::strings::split_view<char>(std::string_view{"a,,c"}, ',',
                                                 true);
    REQUIRE(kept.size() == 3);
    REQUIRE(kept[1].empty());
}

TEST_CASE("Strings - split_on whole delimiter")
{
    auto tokens =
        utils::strings::split_on<char>(std::string_view{"a->b->c"},
                                       std::string_view{"->"});
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == "b");
    REQUIRE(tokens[2] == "c");

    // Contrast with the char-set split, which would break on '-' and '>'.
    auto as_set = utils::strings::split<char>(std::string_view{"a->b"},
                                              std::string_view{"->"});
    REQUIRE(as_set.size() == 2);
    REQUIRE(as_set[0] == "a");
    REQUIRE(as_set[1] == "b");

    // keep_empty across adjacent delimiters.
    auto kept = utils::strings::split_on<char>(std::string_view{"a--b"},
                                               std::string_view{"-"}, true);
    REQUIRE(kept.size() == 3);
    REQUIRE(kept[1].empty());

    // Empty delimiter yields the whole input.
    auto whole = utils::strings::split_on_view<char>(std::string_view{"abc"},
                                                     std::string_view{""});
    REQUIRE(whole.size() == 1);
    REQUIRE(whole[0] == "abc");
}

TEST_CASE("Strings - join separator overloads")
{
    std::vector<std::string> v{"a", "b", "c"};
    REQUIRE(utils::strings::join<char>(v, ',') == "a,b,c");
    REQUIRE(utils::strings::join<char>(v, std::string{", "}) == "a, b, c");
    REQUIRE(utils::strings::join<char>(v, "--") == "a--b--c");
}

TEST_CASE("Strings - pad_left / pad_right / center")
{
    REQUIRE(utils::strings::pad_left<char>("42", 5) == "   42");
    REQUIRE(utils::strings::pad_left<char>("42", 5, '0') == "00042");
    REQUIRE(utils::strings::pad_right<char>("42", 5) == "42   ");
    REQUIRE(utils::strings::center<char>("ab", 6, '*') == "**ab**");
    // Odd padding: extra goes right.
    REQUIRE(utils::strings::center<char>("ab", 5, '*') == "*ab**");
    // Already at/over width: unchanged.
    REQUIRE(utils::strings::pad_left<char>("hello", 3) == "hello");
    REQUIRE(utils::strings::center<char>("hello", 3) == "hello");
}

TEST_CASE("Strings - repeat")
{
    REQUIRE(utils::strings::repeat<char>("ab", 3) == "ababab");
    REQUIRE(utils::strings::repeat<char>('x', 4) == "xxxx");
    REQUIRE(utils::strings::repeat<char>("ab", 0).empty());
}

#if defined(__cpp_lib_to_chars)
TEST_CASE("Strings - to_floating")
{
    REQUIRE(utils::strings::to_floating<double>("3.5") == 3.5);
    REQUIRE(utils::strings::to_floating<double>("-0.25") == -0.25);
    REQUIRE_THROWS_AS(utils::strings::to_floating<double>("abc"),
                      std::invalid_argument);
    // Trailing garbage rejected.
    REQUIRE_THROWS_AS(utils::strings::to_floating<double>("3.5x"),
                      std::invalid_argument);
}
#endif

TEST_CASE("Strings - replace_all forms new matches but does not re-scan")
{
    // Each original char expands; inserted text must not be re-replaced.
    REQUIRE(utils::strings::replace_all<char>("xx", "x", "xx") == "xxxx");
    // Overlapping source collapses correctly.
    REQUIRE(utils::strings::replace_all<char>("aaaa", "aa", "b") == "bb");
    // Whole string is one match.
    REQUIRE(utils::strings::replace_all<char>("abc", "abc", "z") == "z");
}

TEST_CASE("Strings - split all-delimiters and boundaries")
{
    // All delimiters, skip vs keep.
    REQUIRE(utils::strings::split<char>(std::string_view{",,,"}, ',').empty());
    auto kept = utils::strings::split<char>(std::string_view{",,,"}, ',', true);
    REQUIRE(kept.size() == 4);
    for (auto const& t : kept) {
        REQUIRE(t.empty());
    }

    // Empty input.
    REQUIRE(utils::strings::split<char>(std::string_view{""}, ',').empty());
    auto empty_kept =
        utils::strings::split<char>(std::string_view{""}, ',', true);
    REQUIRE(empty_kept.size() == 1);
    REQUIRE(empty_kept[0].empty());
}

TEST_CASE("Strings - split_on delimiter at start and end")
{
    auto t = utils::strings::split_on<char>(std::string_view{"::a::b::"},
                                            std::string_view{"::"});
    // Leading and trailing empties skipped by default.
    REQUIRE(t.size() == 2);
    REQUIRE(t[0] == "a");
    REQUIRE(t[1] == "b");

    auto kept = utils::strings::split_on<char>(std::string_view{"::a::"},
                                               std::string_view{"::"}, true);
    REQUIRE(kept.size() == 3);
    REQUIRE(kept[0].empty());
    REQUIRE(kept[1] == "a");
    REQUIRE(kept[2].empty());

    // Delimiter longer than text.
    auto none = utils::strings::split_on<char>(std::string_view{"ab"},
                                               std::string_view{"abcd"});
    REQUIRE(none.size() == 1);
    REQUIRE(none[0] == "ab");
}

TEST_CASE("Strings - generic over wchar_t (no locale paths)")
{
    using namespace std::literals;
    REQUIRE(utils::strings::replace_all<wchar_t>(L"a.b.c", L".", L"::") ==
            L"a::b::c");
    REQUIRE(utils::strings::pad_left<wchar_t>(L"42", 5, L'0') == L"00042");
    REQUIRE(utils::strings::repeat<wchar_t>(L"ab", 3) == L"ababab");

    auto tokens = utils::strings::split_on<wchar_t>(std::wstring_view{L"a->b"},
                                                    std::wstring_view{L"->"});
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == L"a");
    REQUIRE(tokens[1] == L"b");

    std::vector<std::wstring> v{L"x", L"y", L"z"};
    REQUIRE(utils::strings::join<wchar_t>(v, L',') == L"x,y,z");
}
