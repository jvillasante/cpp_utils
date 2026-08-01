#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>

#include <limits>
#include <string_view>
#include <type_traits>
#include <vector>

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

template <typename To>
[[nodiscard]] To from_bytes(std::byte const* bytes)
{
    static_assert(std::is_trivially_constructible_v<To>);
    static_assert(std::is_trivially_copyable_v<To>);

    To ret;
    std::memcpy(&ret, bytes, sizeof(To));
    return ret;
}

template <typename From>
[[nodiscard]] std::byte* as_bytes(From& from)
{
    static_assert(std::is_trivially_copyable_v<From>);

    // NOLINTNEXTLINE
    return reinterpret_cast<std::byte*>(&from);
}

template <typename From>
[[nodiscard]] std::byte const* as_bytes(From const& from)
{
    static_assert(std::is_trivially_copyable_v<From>);

    // NOLINTNEXTLINE
    return reinterpret_cast<std::byte const*>(&from);
}

[[nodiscard]] inline std::string_view to_string_view(std::byte const* data,
                                                     std::size_t size)
{
    // NOLINTNEXTLINE
    return {reinterpret_cast<char const*>(data), size};
}

[[nodiscard]] inline std::string_view
to_string_view(std::vector<std::byte> const& data)
{
    return to_string_view(data.data(), data.size());
}

template <typename T>
[[nodiscard]] T get_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);

    assert(pos < std::numeric_limits<T>::digits);
    return (num >> pos) & T{1};
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
