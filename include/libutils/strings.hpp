#pragma once

#include <libutils/iterators.hpp>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace utils::strings
{

namespace detail
{
template <typename CharT>
constexpr bool is_whitespace(CharT ch) noexcept
{
    return ch == CharT{' '} || ch == CharT{'\t'} || ch == CharT{'\n'} ||
           ch == CharT{'\r'} || ch == CharT{'\v'} || ch == CharT{'\f'};
}
} // namespace detail

template <typename CharT>
using tstring =
    std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

template <typename CharT>
using tstringview = std::basic_string_view<CharT, std::char_traits<CharT>>;

template <typename CharT>
using tstringstream = std::basic_stringstream<CharT, std::char_traits<CharT>,
                                              std::allocator<CharT>>;

// ----------

template <typename Val>
[[nodiscard]] inline std::enable_if_t<std::is_arithmetic_v<Val>, std::string>
to_string(Val const val)
{
    return std::to_string(val);
}

template <typename Val>
[[nodiscard]] inline std::enable_if_t<!std::is_arithmetic_v<Val>, std::string>
to_string(Val const& val)
{
    return static_cast<std::ostringstream const&>(std::ostringstream() << val)
        .str();
}

// ----------

template <typename CharT>
inline char my_tolower(CharT const ch, std::locale const& loc = std::locale())
{
    return std::tolower(ch, loc);
}

template <typename CharT>
inline char my_toupper(CharT const ch, std::locale const& loc = std::locale())
{
    return std::toupper(ch, loc);
}

// ----------

namespace mutable_version
{
template <typename CharT>
inline void to_upper(tstring<CharT>& text)
{
    std::transform(std::begin(text), std::end(text), std::begin(text),
                   [](CharT const& ch) { return my_toupper(ch); });
}

template <typename CharT>
inline void to_lower(tstring<CharT>& text)
{
    std::transform(std::begin(text), std::end(text), std::begin(text),
                   [](CharT const& ch) { return my_tolower(ch); });
}

template <typename CharT>
inline void reverse(tstring<CharT>& text)
{
    std::reverse(std::begin(text), std::end(text));
}

template <typename CharT>
inline void trim(tstring<CharT>& text)
{
    text.erase(text.begin(),
               std::find_if_not(text.begin(), text.end(),
                                detail::is_whitespace<CharT>));
    text.erase(std::find_if_not(text.rbegin(), text.rend(),
                                detail::is_whitespace<CharT>)
                   .base(),
               text.end());
}

template <typename CharT>
inline void trimleft(tstring<CharT>& text)
{
    text.erase(text.begin(),
               std::find_if_not(text.begin(), text.end(),
                                detail::is_whitespace<CharT>));
}

template <typename CharT>
inline void trimright(tstring<CharT>& text)
{
    text.erase(std::find_if_not(text.rbegin(), text.rend(),
                                detail::is_whitespace<CharT>)
                   .base(),
               text.end());
}
} // namespace mutable_version

template <typename CharT>
[[nodiscard, deprecated("Use std::basic_string_view::starts_with() (C++20)")]]
inline bool starts_with(tstringview<CharT> const str,
                        tstringview<CharT> const prefix)
{
    return str.substr(0, prefix.size()) == prefix;
}

template <typename CharT>
[[nodiscard, deprecated("Use std::basic_string_view::ends_with() (C++20)")]]
inline bool ends_with(tstringview<CharT> const input,
                      tstringview<CharT> const suffix)
{
    return (input.size() >= suffix.size()) &&
           (0 ==
            input.compare(input.size() - suffix.size(), suffix.size(), suffix));
}

template <typename CharT>
[[nodiscard, deprecated("Use std::basic_string_view::contains() (C++23)")]]
bool contains(tstringview<CharT> const input, tstringview<CharT> const needle,
              bool const ignore_case = false)
{
    if (input.size() >= needle.size())
    {
        return std::search(std::begin(input), std::end(input),
                           std::begin(needle), std::end(needle),
                           [&](CharT const c1, CharT const c2) {
                               return ignore_case
                                          ? my_tolower(c1) == my_tolower(c2)
                                          : c1 == c2;
                           }) != std::end(input);
    }

    return false;
}

template <typename CharT>
[[nodiscard]] bool equal(tstringview<CharT> const str1,
                         tstringview<CharT> const str2,
                         bool const ignore_case = false)
{
    if (str1.size() == str2.size())
    {
        return std::equal(str1.begin(), str1.end(), str2.begin(),
                          [=](CharT const c1, CharT const c2) {
                              return ignore_case
                                         ? my_tolower(c1) == my_tolower(c2)
                                         : c1 == c2;
                          });
    }

    return false;
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> to_upper(tstring<CharT> text)
{
    mutable_version::to_upper(text);
    return text;
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> to_lower(tstring<CharT> text)
{
    mutable_version::to_lower(text);
    return text;
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> reverse(tstring<CharT> text)
{
    mutable_version::reverse(text);
    return text;
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trim(tstring<CharT> const& text)
{
    auto const first = std::find_if_not(text.begin(), text.end(),
                                        detail::is_whitespace<CharT>);
    if (first == text.end()) { return {}; }
    auto const last = std::find_if_not(text.rbegin(), text.rend(),
                                       detail::is_whitespace<CharT>);
    return tstring<CharT>(first, last.base());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimleft(tstring<CharT> const& text)
{
    return tstring<CharT>(
        std::find_if_not(text.begin(), text.end(), detail::is_whitespace<CharT>),
        text.end());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimright(tstring<CharT> const& text)
{
    return tstring<CharT>(
        text.begin(),
        std::find_if_not(text.rbegin(), text.rend(), detail::is_whitespace<CharT>)
            .base());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trim(tstring<CharT> const& text,
                                         tstring<CharT> const& chars)
{
    auto const first{text.find_first_not_of(chars)};
    if (first == std::string::npos) { return {}; }

    auto const last{text.find_last_not_of(chars)};
    return text.substr(first, (last - first + 1));
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimleft(tstring<CharT> const& text,
                                             tstring<CharT> const& chars)
{
    auto const first{text.find_first_not_of(chars)};
    if (first == std::string::npos) { return {}; }

    return text.substr(first, text.size() - first);
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimright(tstring<CharT> const& text,
                                              tstring<CharT> const& chars)
{
    auto const last{text.find_last_not_of(chars)};
    return text.substr(0, last + 1);
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> remove(tstring<CharT> text, CharT const ch)
{
    auto const start = std::remove_if(std::begin(text), std::end(text),
                                      [=](CharT const c) { return c == ch; });
    text.erase(start, std::end(text));
    return text;
}

template <typename CharT, typename Iter>
[[nodiscard]] inline tstring<CharT> join(Iter begin, Iter end,
                                         CharT const* const separator)
{
    tstringstream<CharT> oss;
    std::copy(begin, end,
              utils::iterators::make_ostream_joiner(oss, separator));
    return oss.str();
}

template <typename CharT, typename C>
[[nodiscard]] inline tstring<CharT> join(C const& c,
                                         CharT const* const separator)
{
    return join(std::cbegin(c), std::cend(c), separator);
}

template <typename CharT>
[[nodiscard]] inline std::vector<tstring<CharT>> split(tstring<CharT> text,
                                                       CharT const delimiter)
{
    tstringstream<CharT> sstr{std::move(text)};
    std::vector<tstring<CharT>> tokens;
    tstring<CharT> token;
    while (std::getline(sstr, token, delimiter))
    {
        if (!token.empty()) { tokens.emplace_back(token); }
    }

    return tokens;
}

template <typename CharT>
[[nodiscard]] inline std::vector<tstring<CharT>>
split(tstring<CharT> const& text, tstring<CharT> const& delimiters)
{
    std::vector<tstring<CharT>> tokens;
    std::size_t pos = 0;
    std::size_t prev_pos = 0;
    while ((pos = text.find_first_of(delimiters, prev_pos)) !=
           std::string::npos)
    {
        if (pos > prev_pos)
        {
            tokens.emplace_back(text.substr(prev_pos, pos - prev_pos));
        }

        prev_pos = pos + 1;
    }

    if (prev_pos < text.size())
    {
        tokens.emplace_back(text.substr(prev_pos, std::string::npos));
    }

    return tokens;
}

template <typename CharT, typename Iter>
[[nodiscard]] inline tstring<CharT> to_hex(Iter begin, Iter end,
                                           bool use_uppercase = true,
                                           bool insert_spaces = false)
{
    tstringstream<CharT> oss;
    if (use_uppercase) { oss.setf(std::ios_base::uppercase); }

    for (auto current = begin; current != end; ++current)
    {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(*current);
        if (insert_spaces && std::next(current) != end) { oss << ' '; }
    }

    return oss.str();
}

template <typename CharT, typename C>
[[nodiscard]] inline tstring<CharT> to_hex(C const& c,
                                           bool use_uppercase = true,
                                           bool insert_spaces = false)
{
    return to_hex<CharT>(std::cbegin(c), std::cend(c), use_uppercase,
                         insert_spaces);
}

// Converts a hexadecimal string to a byte vector.
template <typename CharT>
[[nodiscard]] std::vector<std::byte> hex_to_bytes(tstringview<CharT> const str)
{
    auto const hexchar_to_int = [](CharT const ch) {
        if (ch >= '0' && ch <= '9') { return ch - '0'; }
        if (ch >= 'A' && ch <= 'F') { return ch - 'A' + 10; }
        if (ch >= 'a' && ch <= 'f') { return ch - 'a' + 10; }

        throw std::invalid_argument("Invalid hexadecimal character");
    };

    std::vector<std::byte> result;
    auto const size{str.size()};

    std::size_t i{0};
    if (size % 2 != 0)
    {
        result.push_back(
            static_cast<std::byte>((hexchar_to_int('0') << 4) |
                                   hexchar_to_int(str[i])));
        ++i;
    }

    for (; i < size; i += 2)
    {
        result.push_back(
            static_cast<std::byte>((hexchar_to_int(str[i]) << 4) |
                                   hexchar_to_int(str[i + 1])));
    }

    return result;
}

// Kept for backwards compatibility.
template <typename CharT>
[[nodiscard, deprecated("Use hex_to_bytes()")]]
std::vector<std::byte> to_bytes(tstringview<CharT> const str)
{
    return hex_to_bytes(str);
}

// Converts a string to an integral type using std::from_chars.
// Throws std::invalid_argument on failure.
template <typename T, typename StringLike>
[[nodiscard]] T to_integral(StringLike&& str)
{
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    T value{};
    auto const sv = std::string_view(str);
    auto const [ptr, ec] =
        std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc{})
    {
        throw std::invalid_argument("to_integral: conversion failed");
    }
    return value;
}
} // namespace utils::strings
