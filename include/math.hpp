#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

namespace utils::math
{
template <typename T>
T get_bit(T num, std::size_t pos)
    requires(std::is_unsigned_v<T>)
{
    assert(pos < std::numeric_limits<T>::digits);
    return (num >> pos) & 1;
}

template <typename T>
T set_bit(T num, std::size_t pos, bool value)
    requires(std::is_unsigned_v<T>)
{
    assert(pos < std::numeric_limits<T>::digits);
    return value ? (num | (1U << pos)) : (num & ~(1U << pos));
}

template <typename T>
T invert_bit(T num, std::size_t pos)
    requires(std::is_unsigned_v<T>)
{
    assert(pos < std::numeric_limits<T>::digits);
    return (num ^ (1U << pos));
}

template <typename T>
T get_bit_range(T num, std::size_t start, std::size_t end)
    requires(std::is_unsigned_v<T>)
{
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    T mask = ((1U << (end - start + 1)) - 1) << start;
    return (num & mask) >> start;
}

template <typename T>
T set_bit_range(T num, std::size_t start, std::size_t end, std::size_t value)
    requires(std::is_unsigned_v<T>)
{
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
    requires(std::is_unsigned_v<T>)
{
    assert(start < std::numeric_limits<T>::digits);
    assert(end < std::numeric_limits<T>::digits);
    assert(start != end);
    std::size_t mask = ((1U << (end - start + 1)) - 1) << start;
    return num ^ mask;
}

template <typename T>
bool is_even(T n)
    requires(std::is_integral_v<T>)
{
    return (n & 1) == 0;
}

template <typename T>
bool is_odd(T n)
    requires(std::is_integral_v<T>)
{
    return (n & 1) == 1;
}

template <typename T>
bool nearly_equal(T lhs, T rhs, T epsilon = std::numeric_limits<T>::epsilon())
    requires(std::is_floating_point_v<T>)
{
    // Check if the numbers are really close -- needed when comparing numbers
    // near zero
    T diff = std::fabs(lhs - rhs);
    if (diff <= epsilon) { return true; }

    // Otherwise fall back to Knuth's algorithm
    return (diff <= (std::max(std::fabs(lhs), std::fabs(rhs)) * epsilon));
}

template <typename T>
T random(T min, T max)
    requires(std::is_integral_v<T>)
{
    // One engine instance per thread
    auto thread_local static engine =
        std::default_random_engine{std::random_device{}()};
    return std::uniform_int_distribution<T>{min, max}(engine);
}

// Useful little function to delay a computation based on a simple counter.
// When `count` reaches `max` then `count` will be reset and the function will
// return `true` otherwise `count` will be incremented and the function will
// return `false`
template <typename T>
bool inline throttle(T& count, T const max)
    requires(std::is_integral_v<T>)
{
    if (++count == max)
    {
        count = 0;
        return true;
    }

    return false;
}

template <std::size_t N, typename input_t = std::uint32_t,
          typename sum_t = std::uint64_t>
class simple_moving_average
{
public:
    input_t operator()(input_t const input)
    {
        sum_ -= previous_inputs_[index_];
        sum_ += input;
        previous_inputs_[index_] = input;
        if (++index_ == N) { index_ = 0; }
        return (sum_ + (N / 2)) / N;
    }

private:
    std::uint8_t index_ = 0;
    std::array<input_t, N> previous_inputs_ = {0};
    sum_t sum_ = 0;
};
} // namespace utils::math
