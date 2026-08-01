#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

// Effective C++ standard version. MSVC does not update __cplusplus unless
// /Zc:__cplusplus is passed, but it always reflects the real standard in
// _MSVC_LANG. Gate every feature check on this instead of __cplusplus so the
// fallbacks are not wrongly selected on MSVC.
#if defined(_MSVC_LANG)
#define UTILS_CPLUSPLUS _MSVC_LANG
#else
#define UTILS_CPLUSPLUS __cplusplus
#endif

// Pull in C++20 standard headers if available. <version> defines the
// __cpp_lib_* feature-test macros used below to detect library support
// precisely rather than inferring it from the language version alone.
#if UTILS_CPLUSPLUS >= 202002L
#include <bit>
#include <span>
#include <version>
#endif

// __cplusplus / _MSVC_LANG values:
//     201103L - Cpp11
//     201402L - Cpp14
//     201703L - Cpp17
//     202002L - Cpp20
//     202302L - Cpp23

namespace utils
{

// ==========================================
// make_unique (Introduced in C++14)
// ==========================================
#if defined(__cpp_lib_make_unique) || UTILS_CPLUSPLUS >= 201402L

using std::make_unique;

#else

// C++11 fallback implementation proposed by Stephan T. Lavavej in N3656
template <typename T>
struct _Unique_if
{
    typedef std::unique_ptr<T> _Single_object;
};

template <typename T>
struct _Unique_if<T[]>
{
    typedef std::unique_ptr<T[]> _Unknown_bound;
};

template <typename T, std::size_t N>
struct _Unique_if<T[N]>
{
    typedef void _Known_bound;
};

template <typename T, typename... Args>
typename _Unique_if<T>::_Single_object make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
typename _Unique_if<T>::_Unknown_bound make_unique(std::size_t n)
{
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

template <typename T, typename... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args&&...) = delete;

#endif

// ==========================================
// exchange (Introduced in C++14)
// ==========================================
#if defined(__cpp_lib_exchange_function) || UTILS_CPLUSPLUS >= 201402L

using std::exchange;

#else

// C++11 fallback implementation
template <typename T, typename U = T>
T exchange(T& obj, U&& new_value) noexcept(
    std::is_nothrow_move_constructible<T>::value &&
    std::is_nothrow_assignable<T&, U>::value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}

#endif

// ==========================================
// bit_cast (Introduced in C++20)
// ==========================================
#if defined(__cpp_lib_bit_cast) || UTILS_CPLUSPLUS >= 202002L

using std::bit_cast;

#else

// C++11/14/17 fallback implementation
template <typename To, typename From>
typename std::enable_if<sizeof(To) == sizeof(From) &&
                            std::is_trivially_copyable<From>::value &&
                            std::is_trivially_copyable<To>::value,
                        To>::type
bit_cast(From const& src) noexcept
{
    static_assert(std::is_trivially_constructible<To>::value,
                  "This implementation additionally requires "
                  "destination type to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

#endif

// ==========================================
// span (Introduced in C++20)
// ==========================================
#if defined(__cpp_lib_span) || UTILS_CPLUSPLUS >= 202002L

using std::dynamic_extent;
using std::span;

#else

constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

// Pre-C++20 fallback implementation (dynamic-extent subset of std::span)
template <typename T>
class span
{
public:
    using element_type = T;
    using value_type = typename std::remove_cv<T>::type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using iterator = T*;
    using reverse_iterator = std::reverse_iterator<iterator>;

    constexpr span() noexcept = default;
    constexpr span(T* data, std::size_t size) noexcept
        : data_(data), size_(size)
    {}
    span(T* begin, T* end) noexcept
        : data_(begin), size_(static_cast<std::size_t>(end - begin))
    {}

    template <std::size_t N>
    constexpr span(T (&arr)[N]) noexcept : data_(arr), size_(N)
    {}

    // Non-const vector: valid for span<T> and span<T const>.
    span(std::vector<value_type>& vec) noexcept
        : data_(vec.data()), size_(vec.size())
    {}

    // Const vector: only compiles when T is const-qualified, which prevents
    // silently stripping const to obtain a mutable view.
    span(std::vector<value_type> const& vec) noexcept
        : data_(vec.data()), size_(vec.size())
    {}

    constexpr T* begin() const noexcept { return data_; }
    constexpr T* end() const noexcept { return data_ + size_; }
    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }
    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }
    constexpr T* data() const noexcept { return data_; }

    constexpr std::size_t size() const noexcept { return size_; }
    constexpr std::size_t size_bytes() const noexcept
    {
        return size_ * sizeof(T);
    }
    constexpr bool empty() const noexcept { return size_ == 0; }

    // View semantics: element access does not depend on the span's constness,
    // so this is a single const-qualified overload returning T& (like a
    // pointer), matching std::span.
    constexpr T& operator[](std::size_t n) const noexcept { return data_[n]; }

    constexpr T& front() const noexcept { return data_[0]; }
    constexpr T& back() const noexcept { return data_[size_ - 1]; }

    // Subviews. count == dynamic_extent means "to the end", matching
    // std::span's dynamic-extent overloads.
    constexpr span first(std::size_t count) const noexcept
    {
        return span(data_, count);
    }
    constexpr span last(std::size_t count) const noexcept
    {
        return span(data_ + (size_ - count), count);
    }
    constexpr span subspan(std::size_t offset,
                           std::size_t count = dynamic_extent) const noexcept
    {
        return span(data_ + offset,
                    count == dynamic_extent ? size_ - offset : count);
    }

private:
    T* data_ = nullptr;
    std::size_t size_ = 0;
};

#endif

// ==========================================
// endian (Introduced in C++20)
// ==========================================
#if defined(__cpp_lib_endian) || UTILS_CPLUSPLUS >= 202002L

using std::endian;

#else

// Pre-C++20 fallback. GCC and Clang expose the target byte order through
// built-in macros; MSVC targets are always little-endian.
#if defined(_MSVC_LANG)
enum class endian : std::uint8_t
{
    little = 0,
    big = 1,
    native = little,
};
#else
enum class endian // NOLINT
{
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__,
};
#endif

#endif

// ==========================================
// byteswap (Introduced in C++23)
// ==========================================
#if defined(__cpp_lib_byteswap) || UTILS_CPLUSPLUS >= 202302L

using std::byteswap;

#else

// Pre-C++23 fallback: reverse the object representation of an integer. The
// standard restricts byteswap to integral types with no padding bits, which
// every standard integer type satisfies.
template <typename T>
constexpr T byteswap(T value) noexcept
{
    static_assert(std::is_integral<T>::value,
                  "byteswap requires an integral type");
    using U = typename std::make_unsigned<T>::type;
    U const in = static_cast<U>(value);
    U out = 0;
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        out = static_cast<U>(
            static_cast<U>(out << 8) |
            static_cast<U>((in >> (i * 8)) & static_cast<U>(0xFF)));
    }
    return static_cast<T>(out);
}

#endif

// ==========================================
// bit operations from <bit> (Introduced in C++20)
//   rotl, rotr, countl_zero, countl_one, countr_zero, countr_one, popcount,
//   has_single_bit, bit_width, bit_ceil, bit_floor
// ==========================================
#if defined(__cpp_lib_bitops) || UTILS_CPLUSPLUS >= 202002L

using std::bit_ceil;
using std::bit_floor;
using std::bit_width;
using std::countl_one;
using std::countl_zero;
using std::countr_one;
using std::countr_zero;
using std::has_single_bit;
using std::popcount;
using std::rotl;
using std::rotr;

#else

// Pre-C++20 fallbacks. All operate on unsigned integer types, matching the
// standard's constraints, and mirror the std semantics so the behavior is
// identical whether the fallback or the std version is selected.

template <typename T>
constexpr int popcount(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    int count = 0;
    while (x != 0) {
        x = static_cast<T>(x & static_cast<T>(x - 1));
        ++count;
    }
    return count;
}

template <typename T>
constexpr int countr_zero(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    constexpr int digits = std::numeric_limits<T>::digits;
    for (int i = 0; i < digits; ++i) {
        if (((x >> i) & T{1}) != 0) {
            return i;
        }
    }
    return digits;
}

template <typename T>
constexpr int countl_zero(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    constexpr int digits = std::numeric_limits<T>::digits;
    for (int i = 0; i < digits; ++i) {
        if (((x >> (digits - 1 - i)) & T{1}) != 0) {
            return i;
        }
    }
    return digits;
}

template <typename T>
constexpr int countr_one(T x) noexcept
{
    return countr_zero(static_cast<T>(~x));
}

template <typename T>
constexpr int countl_one(T x) noexcept
{
    return countl_zero(static_cast<T>(~x));
}

template <typename T>
constexpr int bit_width(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    return std::numeric_limits<T>::digits - countl_zero(x);
}

template <typename T>
constexpr bool has_single_bit(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    return x != 0 && (x & static_cast<T>(x - 1)) == 0;
}

template <typename T>
constexpr T bit_floor(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    return x == 0 ? T{0} : static_cast<T>(T{1} << (bit_width(x) - 1));
}

template <typename T>
constexpr T bit_ceil(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    return x <= 1 ? T{1}
                  : static_cast<T>(T{1} << bit_width(static_cast<T>(x - 1)));
}

template <typename T>
constexpr T rotl(T x, int s) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    constexpr int digits = std::numeric_limits<T>::digits;
    int r = s % digits;
    if (r < 0) {
        r += digits;
    }
    if (r == 0) {
        return x;
    }
    return static_cast<T>(static_cast<T>(x << r) |
                          static_cast<T>(x >> (digits - r)));
}

template <typename T>
constexpr T rotr(T x, int s) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "bit operations require an unsigned integer type");
    constexpr int digits = std::numeric_limits<T>::digits;
    int r = s % digits;
    if (r < 0) {
        r += digits;
    }
    if (r == 0) {
        return x;
    }
    return static_cast<T>(static_cast<T>(x >> r) |
                          static_cast<T>(x << (digits - r)));
}

#endif
} // namespace utils

#undef UTILS_CPLUSPLUS
