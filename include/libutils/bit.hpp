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
[[nodiscard]] To from_bytes(std::byte const* bytes)
{
    static_assert(std::is_trivially_constructible_v<To>);
    static_assert(std::is_trivially_copyable_v<To>);

    To ret{};
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
    assert(start != end);
    T mask = ((T{1} << (end - start + 1)) - 1) << start;
    return (num & mask) >> start;
}

template <typename T>
[[nodiscard]] T set_bit_range(T num, std::size_t start, std::size_t end,
                              std::size_t value)
{
    static_assert(std::is_unsigned_v<T>);

    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    std::size_t range_size = end - start + 1;
    T mask = ((T{1} << range_size) - 1) << start;
    num &= ~mask;
    num |= (value & ((T{1} << range_size) - 1)) << start;
    return num;
}

template <typename T>
[[nodiscard]] T invert_bit_range(T num, std::size_t start, std::size_t end)
{
    static_assert(std::is_unsigned_v<T>);

    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    T mask = ((T{1} << (end - start + 1)) - 1) << start;
    return num ^ mask;
}
} // namespace utils::bit
