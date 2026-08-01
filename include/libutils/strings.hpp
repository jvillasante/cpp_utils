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

// Widen a single element to its unsigned byte value without sign-extension.
// Works for integral element types as well as std::byte.
template <typename T>
[[nodiscard]] constexpr unsigned to_byte_value(T const v) noexcept
{
    if constexpr (std::is_same_v<T, std::byte>) {
        return std::to_integer<unsigned>(v);
    } else {
        return static_cast<unsigned>(static_cast<std::make_unsigned_t<T>>(v));
    }
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
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

// ----------

template <typename CharT>
inline CharT my_tolower(CharT const ch, std::locale const& loc = std::locale())
{
    return std::tolower(ch, loc);
}

template <typename CharT>
inline CharT my_toupper(CharT const ch, std::locale const& loc = std::locale())
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
                   [](CharT const& ch) {
                       return my_toupper(ch);
                   });
}

template <typename CharT>
inline void to_lower(tstring<CharT>& text)
{
    std::transform(std::begin(text), std::end(text), std::begin(text),
                   [](CharT const& ch) {
                       return my_tolower(ch);
                   });
}

template <typename CharT>
inline void reverse(tstring<CharT>& text)
{
    std::reverse(std::begin(text), std::end(text));
}

template <typename CharT>
inline void trim(tstring<CharT>& text)
{
    text.erase(text.begin(), std::find_if_not(text.begin(), text.end(),
                                              detail::is_whitespace<CharT>));
    text.erase(std::find_if_not(text.rbegin(), text.rend(),
                                detail::is_whitespace<CharT>)
                   .base(),
               text.end());
}

template <typename CharT>
inline void trimleft(tstring<CharT>& text)
{
    text.erase(text.begin(), std::find_if_not(text.begin(), text.end(),
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
[[nodiscard]]
inline bool starts_with(tstringview<CharT> const str,
                        tstringview<CharT> const prefix,
                        bool const ignore_case = false)
{
    if (str.size() < prefix.size()) {
        return false;
    }
    if (!ignore_case) {
        return str.substr(0, prefix.size()) == prefix;
    }
    return std::equal(prefix.begin(), prefix.end(), str.begin(),
                      [](CharT const c1, CharT const c2) {
                          return my_tolower(c1) == my_tolower(c2);
                      });
}

template <typename CharT>
[[nodiscard]]
inline bool ends_with(tstringview<CharT> const input,
                      tstringview<CharT> const suffix,
                      bool const ignore_case = false)
{
    if (input.size() < suffix.size()) {
        return false;
    }
    auto const tail = input.substr(input.size() - suffix.size());
    if (!ignore_case) {
        return tail == suffix;
    }
    return std::equal(suffix.begin(), suffix.end(), tail.begin(),
                      [](CharT const c1, CharT const c2) {
                          return my_tolower(c1) == my_tolower(c2);
                      });
}

template <typename CharT>
[[nodiscard]]
bool contains(tstringview<CharT> const input, tstringview<CharT> const needle,
              bool const ignore_case = false)
{
    if (input.size() >= needle.size()) {
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
    if (str1.size() == str2.size()) {
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
    if (first == text.end()) {
        return {};
    }
    auto const last = std::find_if_not(text.rbegin(), text.rend(),
                                       detail::is_whitespace<CharT>);
    return tstring<CharT>(first, last.base());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimleft(tstring<CharT> const& text)
{
    return tstring<CharT>(std::find_if_not(text.begin(), text.end(),
                                           detail::is_whitespace<CharT>),
                          text.end());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimright(tstring<CharT> const& text)
{
    return tstring<CharT>(text.begin(),
                          std::find_if_not(text.rbegin(), text.rend(),
                                           detail::is_whitespace<CharT>)
                              .base());
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trim(tstring<CharT> const& text,
                                         tstring<CharT> const& chars)
{
    auto const first{text.find_first_not_of(chars)};
    if (first == std::string::npos) {
        return {};
    }

    auto const last{text.find_last_not_of(chars)};
    return text.substr(first, (last - first + 1));
}

template <typename CharT>
[[nodiscard]] inline tstring<CharT> trimleft(tstring<CharT> const& text,
                                             tstring<CharT> const& chars)
{
    auto const first{text.find_first_not_of(chars)};
    if (first == std::string::npos) {
        return {};
    }

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
    auto const start =
        std::remove_if(std::begin(text), std::end(text), [=](CharT const c) {
            return c == ch;
        });
    text.erase(start, std::end(text));
    return text;
}

// Replace every non-overlapping occurrence of `from` with `to`. An empty `from`
// is a no-op (returns the input unchanged) rather than looping forever.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> replace_all(tstring<CharT> text,
                                                tstringview<CharT> const from,
                                                tstringview<CharT> const to)
{
    if (from.empty()) {
        return text;
    }

    std::size_t pos = 0;
    while ((pos = text.find(from, pos)) != tstring<CharT>::npos) {
        text.replace(pos, from.size(), to.data(), to.size());
        pos += to.size(); // skip the replacement so `to` is not re-scanned
    }
    return text;
}

// Single-character replace convenience overload.
template <typename CharT>
[[nodiscard]] inline tstring<CharT>
replace_all(tstring<CharT> text, CharT const from, CharT const to)
{
    std::replace(std::begin(text), std::end(text), from, to);
    return text;
}

// Replace only the first occurrence of `from` with `to`.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> replace_first(tstring<CharT> text,
                                                  tstringview<CharT> const from,
                                                  tstringview<CharT> const to)
{
    if (from.empty()) {
        return text;
    }

    auto const pos = text.find(from);
    if (pos != tstring<CharT>::npos) {
        text.replace(pos, from.size(), to.data(), to.size());
    }
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

// Single-character separator convenience overload.
template <typename CharT, typename C>
[[nodiscard]] inline tstring<CharT> join(C const& c, CharT const separator)
{
    tstring<CharT> const sep(1, separator);
    return join<CharT>(std::cbegin(c), std::cend(c), sep.c_str());
}

// String separator convenience overload.
template <typename CharT, typename C>
[[nodiscard]] inline tstring<CharT> join(C const& c,
                                         tstring<CharT> const& separator)
{
    return join<CharT>(std::cbegin(c), std::cend(c), separator.c_str());
}

// Non-owning split on a SET of single-character delimiters: returns views into
// `text`, which must outlive the result. Allocates only the token vector, not
// the tokens themselves. With keep_empty == false (the default) empty tokens
// between adjacent/leading/trailing delimiters are skipped; with keep_empty ==
// true they are preserved (e.g. for CSV fields).
template <typename CharT>
[[nodiscard]] inline std::vector<tstringview<CharT>>
split_view(tstringview<CharT> const text, tstringview<CharT> const delimiters,
           bool const keep_empty = false)
{
    std::vector<tstringview<CharT>> tokens;
    std::size_t pos = 0;
    std::size_t prev_pos = 0;
    while ((pos = text.find_first_of(delimiters, prev_pos)) !=
           tstringview<CharT>::npos) {
        if (pos > prev_pos || keep_empty) {
            tokens.push_back(text.substr(prev_pos, pos - prev_pos));
        }

        prev_pos = pos + 1;
    }

    if (prev_pos < text.size() || keep_empty) {
        tokens.push_back(text.substr(prev_pos));
    }

    return tokens;
}

// Single-character delimiter convenience overload.
template <typename CharT>
[[nodiscard]] inline std::vector<tstringview<CharT>>
split_view(tstringview<CharT> const text, CharT const delimiter,
           bool const keep_empty = false)
{
    return split_view<CharT>(text, tstringview<CharT>(&delimiter, 1),
                             keep_empty);
}

// Non-owning split on a WHOLE multi-character delimiter (the entire `delimiter`
// sequence is matched), as opposed to split_view() which treats its argument as
// a set of single-character delimiters. An empty delimiter yields the whole
// input as a single token.
template <typename CharT>
[[nodiscard]] inline std::vector<tstringview<CharT>>
split_on_view(tstringview<CharT> const text, tstringview<CharT> const delimiter,
              bool const keep_empty = false)
{
    std::vector<tstringview<CharT>> tokens;
    if (delimiter.empty()) {
        tokens.push_back(text);
        return tokens;
    }

    std::size_t pos = 0;
    std::size_t prev_pos = 0;
    while ((pos = text.find(delimiter, prev_pos)) != tstringview<CharT>::npos) {
        if (pos > prev_pos || keep_empty) {
            tokens.push_back(text.substr(prev_pos, pos - prev_pos));
        }

        prev_pos = pos + delimiter.size();
    }

    if (prev_pos < text.size() || keep_empty) {
        tokens.push_back(text.substr(prev_pos));
    }

    return tokens;
}

// Owning splits: materialize the views from the *_view functions into strings.
template <typename CharT>
[[nodiscard]] inline std::vector<tstring<CharT>>
split(tstringview<CharT> const text, CharT const delimiter,
      bool const keep_empty = false)
{
    std::vector<tstring<CharT>> tokens;
    for (auto const token : split_view<CharT>(text, delimiter, keep_empty)) {
        tokens.emplace_back(token);
    }
    return tokens;
}

template <typename CharT>
[[nodiscard]] inline std::vector<tstring<CharT>>
split(tstringview<CharT> const text, tstringview<CharT> const delimiters,
      bool const keep_empty = false)
{
    std::vector<tstring<CharT>> tokens;
    for (auto const token : split_view<CharT>(text, delimiters, keep_empty)) {
        tokens.emplace_back(token);
    }
    return tokens;
}

template <typename CharT>
[[nodiscard]] inline std::vector<tstring<CharT>>
split_on(tstringview<CharT> const text, tstringview<CharT> const delimiter,
         bool const keep_empty = false)
{
    std::vector<tstring<CharT>> tokens;
    for (auto const token : split_on_view<CharT>(text, delimiter, keep_empty)) {
        tokens.emplace_back(token);
    }
    return tokens;
}

template <typename CharT, typename Iter>
[[nodiscard]] inline tstring<CharT> to_hex(Iter begin, Iter end,
                                           bool use_uppercase = true,
                                           bool insert_spaces = false)
{
    tstringstream<CharT> oss;
    if (use_uppercase) {
        oss.setf(std::ios_base::uppercase);
    }

    for (auto current = begin; current != end; ++current) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << detail::to_byte_value(*current);
        if (insert_spaces && std::next(current) != end) {
            oss << ' ';
        }
    }

    return oss.str();
}

template <typename CharT, typename C>
[[nodiscard]] inline tstring<CharT>
to_hex(C const& c, bool use_uppercase = true, bool insert_spaces = false)
{
    return to_hex<CharT>(std::cbegin(c), std::cend(c), use_uppercase,
                         insert_spaces);
}

// Converts a hexadecimal string to a byte vector.
template <typename CharT>
[[nodiscard]] std::vector<std::byte> hex_to_bytes(tstringview<CharT> const str)
{
    auto const hexchar_to_int = [](CharT const ch) {
        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        }
        if (ch >= 'A' && ch <= 'F') {
            return ch - 'A' + 10;
        }
        if (ch >= 'a' && ch <= 'f') {
            return ch - 'a' + 10;
        }

        throw std::invalid_argument("Invalid hexadecimal character");
    };

    std::vector<std::byte> result;
    auto const size{str.size()};

    std::size_t i{0};
    if (size % 2 != 0) {
        result.push_back(static_cast<std::byte>((hexchar_to_int('0') << 4) |
                                                hexchar_to_int(str[i])));
        ++i;
    }

    for (; i < size; i += 2) {
        result.push_back(static_cast<std::byte>((hexchar_to_int(str[i]) << 4) |
                                                hexchar_to_int(str[i + 1])));
    }

    return result;
}

// Kept for backwards compatibility.
template <typename CharT>
[[nodiscard]]
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
    // Reject both parse errors and trailing garbage ("123abc" must not yield
    // 123): require that the whole input was consumed.
    if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
        throw std::invalid_argument("to_integral: conversion failed");
    }
    return value;
}

// Converts a string to a floating-point type using std::from_chars.
// Throws std::invalid_argument on failure. Only available when the standard
// library provides floating-point from_chars (libstdc++ >= 11, recent libc++);
// __cpp_lib_to_chars is defined exactly when that support is complete.
#if defined(__cpp_lib_to_chars)
template <typename T, typename StringLike>
[[nodiscard]] T to_floating(StringLike&& str)
{
    static_assert(std::is_floating_point_v<T>,
                  "T must be a floating-point type");
    T value{};
    auto const sv = std::string_view(str);
    auto const [ptr, ec] =
        std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
        throw std::invalid_argument("to_floating: conversion failed");
    }
    return value;
}
#endif

// ----------
// Fixed-width formatting
// ----------

// Left-pad `text` with `fill` up to `width`. Shorter-than-width only; longer
// strings are returned unchanged.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> pad_left(tstring<CharT> text,
                                             std::size_t const width,
                                             CharT const fill = CharT{' '})
{
    if (text.size() < width) {
        text.insert(text.begin(), width - text.size(), fill);
    }
    return text;
}

// Right-pad `text` with `fill` up to `width`.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> pad_right(tstring<CharT> text,
                                              std::size_t const width,
                                              CharT const fill = CharT{' '})
{
    if (text.size() < width) {
        text.append(width - text.size(), fill);
    }
    return text;
}

// Center `text` within `width`, padding both sides with `fill`. When the
// padding is odd, the extra character goes on the right.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> center(tstring<CharT> const& text,
                                           std::size_t const width,
                                           CharT const fill = CharT{' '})
{
    if (text.size() >= width) {
        return text;
    }
    std::size_t const total = width - text.size();
    std::size_t const left = total / 2;
    std::size_t const right = total - left;
    tstring<CharT> out;
    out.reserve(width);
    out.append(left, fill);
    out.append(text);
    out.append(right, fill);
    return out;
}

// ----------
// Repetition
// ----------

// Repeat a multi-character unit `count` times.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> repeat(tstringview<CharT> const unit,
                                           std::size_t const count)
{
    tstring<CharT> out;
    out.reserve(unit.size() * count);
    for (std::size_t i = 0; i < count; ++i) {
        out.append(unit.data(), unit.size());
    }
    return out;
}

// Repeat a single character `count` times.
template <typename CharT>
[[nodiscard]] inline tstring<CharT> repeat(CharT const ch,
                                           std::size_t const count)
{
    return tstring<CharT>(count, ch);
}
} // namespace utils::strings
