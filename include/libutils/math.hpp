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
[[nodiscard]] bool is_even(T n)
{
    static_assert(std::is_integral_v<T>);
    return (n & 1) == 0;
}

template <typename T>
[[nodiscard]] bool is_odd(T n)
{
    static_assert(std::is_integral_v<T>);
    return (n & 1) == 1;
}

template <typename T>
[[nodiscard]] bool nearly_equal(T lhs, T rhs,
                                T epsilon = std::numeric_limits<T>::epsilon())
{
    static_assert(std::is_floating_point_v<T>);

    T diff = std::abs(lhs - rhs);
    if (diff <= epsilon) {
        return true;
    }

    return (diff <= (std::max(std::abs(lhs), std::abs(rhs)) * epsilon));
}

template <typename T>
[[nodiscard]] T random(T min, T max)
{
    static_assert(std::is_integral_v<T>);

    auto thread_local static engine =
        std::default_random_engine{std::random_device{}()};
    return std::uniform_int_distribution<T>{min, max}(engine);
}

template <std::size_t N, typename input_t = std::uint32_t,
          typename sum_t = std::uint64_t>
class simple_moving_average
{
    static_assert(N <= 255, "N must fit in uint8_t index");

public:
    input_t operator()(input_t const input)
    {
        sum_ -= previous_inputs_[index_];
        sum_ += input;
        previous_inputs_[index_] = input;
        if (++index_ == N) {
            index_ = 0;
        }
        return (sum_ + (N / 2)) / N;
    }

private:
    std::uint8_t index_ = 0;
    std::array<input_t, N> previous_inputs_ = {0};
    sum_t sum_ = 0;
};
} // namespace utils::math
