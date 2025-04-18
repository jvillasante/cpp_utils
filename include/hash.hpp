#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <functional>

/**
 * hash_combine taken from boost. Example implementation for a Point type:
 *     struct Point { int x, y; };
 *
 *     bool operator==(Point const& lhs, Point const& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
 *
 *     namespace std {
 *     template <>
 *     struct hash<Point> {
 *         using argument_type = Point;
 *         using result_type = std::size_t;
 *         result_type operator()(argument_type const& c) const
 *         {
 *             std::size_t seed(0);
 *             utils::hash::combine(seed, c.x);
 *             utils::hash::combine(seed, c.y);
 *             return seed;
 *         }
 *     };
 *     }
 */

namespace utils::hash
{
namespace detail
{
template <std::size_t Bits>
struct hash_mix_impl;

// hash_mix for 64 bit size_t
//
// The general "xmxmx" form of state of the art 64 bit mixers originates
// from Murmur3 by Austin Appleby, which uses the following function as
// its "final mix":
//
//  k ^= k >> 33;
//  k *= 0xff51afd7ed558ccd;
//  k ^= k >> 33;
//  k *= 0xc4ceb9fe1a85ec53;
//  k ^= k >> 33;
//
// (https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp)
//
// It has subsequently been improved multiple times by different authors
// by changing the constants. The most well known improvement is the
// so-called "variant 13" function by David Stafford:
//
//  k ^= k >> 30;
//  k *= 0xbf58476d1ce4e5b9;
//  k ^= k >> 27;
//  k *= 0x94d049bb133111eb;
//  k ^= k >> 31;
//
// (https://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html)
//
// This mixing function is used in the splitmix64 RNG:
// http://xorshift.di.unimi.it/splitmix64.c
//
// We use Jon Maiga's implementation from
// http://jonkagstrom.com/mx3/mx3_rev2.html
//
//  x ^= x >> 32;
//  x *= 0xe9846af9b1a615d;
//  x ^= x >> 32;
//  x *= 0xe9846af9b1a615d;
//  x ^= x >> 28;
//
// An equally good alternative is Pelle Evensen's Moremur:
//
//  x ^= x >> 27;
//  x *= 0x3C79AC492BA7B653;
//  x ^= x >> 33;
//  x *= 0x1C69B3F74AC4AE35;
//  x ^= x >> 27;
//
// (https://mostlymangling.blogspot.com/2019/12/stronger-better-morer-moremur-better.html)

template <>
struct hash_mix_impl<64>
{
    static std::uint64_t fn(std::uint64_t x)
    {
        std::uint64_t const m = 0xe9846af9b1a615d;
        x ^= x >> 32U;
        x *= m;
        x ^= x >> 32U;
        x *= m;
        x ^= x >> 28U;
        return x;
    }
};

// hash_mix for 32 bit size_t
//
// We use the "best xmxmx" implementation from
// https://github.com/skeeto/hash-prospector/issues/19

template <>
struct hash_mix_impl<32>
{
    static std::uint32_t fn(std::uint32_t x)
    {
        std::uint32_t const m1 = 0x21f0aaad;
        std::uint32_t const m2 = 0x735a2d97;
        x ^= x >> 16U;
        x *= m1;
        x ^= x >> 15U;
        x *= m2;
        x ^= x >> 15U;
        return x;
    }
};

inline std::size_t hash_mix(std::size_t v)
{
    return hash_mix_impl<sizeof(std::size_t) * CHAR_BIT>::fn(v);
}
} // namespace detail

template <typename T>
inline void combine(std::size_t& seed, T const& v)
{
    seed = detail::hash_mix(seed + 0x9e3779b9 + std::hash<T>()(v));
}
} // namespace utils::hash
