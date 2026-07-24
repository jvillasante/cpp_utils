#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

// Pull in C++20 standard headers if available
#if __cplusplus >= 202002L
#include <bit>
#include <span>
#endif

// __cplusplus macro definition:
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
#if __cplusplus >= 201402L

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
#if __cplusplus >= 201402L

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
#if __cplusplus >= 202002L

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
#if __cplusplus >= 202002L

using std::span;

#else

// Pre-C++20 fallback implementation
template <typename T>
class span
{
public:
    span() = default;
    span(T* data, std::size_t size) : data_(data), size_(size) {}
    span(T* begin, T* end) : data_(begin), size_(end - begin) {}

    template <typename U>
    explicit span(std::vector<U> const& vec)
        : data_(vec.data()), size_(vec.size())
    {}

    T* begin() const { return data_; }
    T* end() const { return data_ + size_; }
    [[nodiscard]] std::size_t size() const { return size_; }

    T& operator[](std::size_t n) { return *(data_ + n); }
    T const& operator[](std::size_t n) const { return *(data_ + n); }

private:
    T* data_ = nullptr;
    std::size_t size_ = 0;
};

#endif
} // namespace utils
