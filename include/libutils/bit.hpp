#pragma once

#include <cassert>
#include <cstddef>

#include <limits>
#include <type_traits>

namespace utils::bit
{
namespace detail
{
// Mask with the low `width` bits set. Handles width == digits without the
// undefined behavior of shifting by the full bit width of T.
template <typename T>
[[nodiscard]] constexpr T low_bits_mask(std::size_t width)
{
    static_assert(std::is_unsigned_v<T>);
    return (width >= static_cast<std::size_t>(std::numeric_limits<T>::digits))
               ? static_cast<T>(~T{0})
               : static_cast<T>((T{1} << width) - T{1});
}
} // namespace detail

// A mask of type T with bits [start, end] (inclusive) set and all others clear.
// Handy alongside the *_bit_range functions when you need the mask directly.
template <typename T>
[[nodiscard]] constexpr T mask(std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start <= end);
    return static_cast<T>(detail::low_bits_mask<T>(end - start + 1) << start);
}

template <typename T>
[[nodiscard]] T get_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);
    assert(pos < std::numeric_limits<T>::digits);
    return (num >> pos) & T{1};
}

// Predicate form of get_bit: true when the bit at pos is set. Reads more
// naturally in a condition than `get_bit(x, pos) != 0`.
template <typename T>
[[nodiscard]] bool test_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);
    assert(pos < std::numeric_limits<T>::digits);
    return ((num >> pos) & T{1}) != 0;
}

template <typename T>
[[nodiscard]] T set_bit(T num, std::size_t pos, bool value)
{
    static_assert(std::is_unsigned_v<T>);
    assert(pos < std::numeric_limits<T>::digits);
    return value ? (num | (T{1} << pos)) : (num & ~(T{1} << pos));
}

template <typename T>
[[nodiscard]] T invert_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);
    assert(pos < std::numeric_limits<T>::digits);
    return (num ^ (T{1} << pos));
}

template <typename T>
[[nodiscard]] T get_bit_range(T num, std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start <= end);
    T const low = detail::low_bits_mask<T>(end - start + 1);
    return (num >> start) & low;
}

template <typename T>
[[nodiscard]] T set_bit_range(T num, std::size_t start, std::size_t end,
                              std::size_t value)
{
    static_assert(std::is_unsigned_v<T>);
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start <= end);
    T const low = detail::low_bits_mask<T>(end - start + 1);
    T const mask = static_cast<T>(low << start);
    num &= static_cast<T>(~mask);
    num |= static_cast<T>((static_cast<T>(value) & low) << start);
    return num;
}

template <typename T>
[[nodiscard]] T invert_bit_range(T num, std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start <= end);
    T const low = detail::low_bits_mask<T>(end - start + 1);
    return num ^ static_cast<T>(low << start);
}
} // namespace utils::bit
