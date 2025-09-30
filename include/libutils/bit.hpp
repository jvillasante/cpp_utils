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
template <typename To>
To from_bytes(std::byte const* bytes)
{
    static_assert(std::is_trivially_constructible_v<To>);

    To ret{};
    std::memcpy(&ret, bytes, sizeof(To));
    return ret;
}

template <typename From>
std::byte* as_bytes(From& from)
{
    // NOLINTNEXTLINE
    return reinterpret_cast<std::byte*>(&from);
}

template <typename From>
std::byte const* as_bytes(From const& from)
{
    // NOLINTNEXTLINE
    return reinterpret_cast<std::byte const*>(&from);
}

inline std::string_view to_string_view(std::byte const* data, std::size_t size)
{
    // NOLINTNEXTLINE
    return {reinterpret_cast<char const*>(data), size};
}

inline std::string_view to_string_view(std::vector<std::byte> const& data)
{
    return to_string_view(data.data(), data.size());
}

template <typename T>
T get_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);

    assert(pos < std::numeric_limits<T>::digits);
    return (num >> pos) & 1;
}

template <typename T>
T set_bit(T num, std::size_t pos, bool value)
{
    static_assert(std::is_unsigned_v<T>);

    assert(pos < std::numeric_limits<T>::digits);
    return value ? (num | (1U << pos)) : (num & ~(1U << pos));
}

template <typename T>
T invert_bit(T num, std::size_t pos)
{
    static_assert(std::is_unsigned_v<T>);

    assert(pos < std::numeric_limits<T>::digits);
    return (num ^ (1U << pos));
}

template <typename T>
T get_bit_range(T num, std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);

    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    T mask = ((1U << (end - start + 1)) - 1) << start;
    return (num & mask) >> start;
}

template <typename T>
T set_bit_range(T num, std::size_t start, std::size_t end, std::size_t value)
{
    static_assert(std::is_unsigned_v<T>);

    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    std::size_t range_size = end - start + 1; // Determine the size of the range
    T mask = ((1U << range_size) - 1)
             << start; // Create a mask with 1s in the specified range
    num &= ~mask;      // Clear the bits in the range
    num |= (value & ((1U << range_size) - 1)) << start;
    return num;
}

template <typename T>
T invert_bit_range(T num, std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);

    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    std::size_t mask = ((1U << (end - start + 1)) - 1) << start;
    return num ^ mask;
}
} // namespace utils::bit
