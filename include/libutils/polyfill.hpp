#pragma once

#include <cstddef>
#include <cstring>
#include <iterator>
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
} // namespace utils

#undef UTILS_CPLUSPLUS
