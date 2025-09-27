#pragma once

#include <cassert>
#include <utility>

/**
 * Defines `UniqueHandle` class template which allows us to interact with C
 * resources in a C++ compliant way.
 *
 * Original idea:
 *    https://docs.microsoft.com/en-us/archive/msdn-magazine/2011/july/msdn-magazine-windows-with-c-c-and-the-windows-api
 *
 * Example usage:
 * struct CFileHandleTraits {
 *     using handle = FILE *;
 *     static handle invalid() noexcept { return nullptr; }
 *     static void destroy(handle value) noexcept {
 *         if (value != CFileHandleTraits::invalid()) {
 *             std::fclose(value);
 *         }
 *     }
 * };
 *
 * utils::UniqueHandle<CFileHandleTraits> handle(std::fopen("file.txt", "wb"));
 * if (!handle) { return; }
 *
 * // use `handle.get()`
 * // `handle` will be correctly closed at the end of the scope
 */
namespace utils
{
template <typename Traits>
class UniqueHandle
{
    using handle = typename Traits::handle;
    handle value_;

public:
    explicit UniqueHandle(handle value = Traits::invalid()) noexcept
        : value_(value)
    {}
    UniqueHandle(UniqueHandle const&) = delete;
    UniqueHandle& operator=(UniqueHandle const&) = delete;
    UniqueHandle(UniqueHandle&& other) noexcept : value_(other.release()) {}
    UniqueHandle& operator=(UniqueHandle&& other) noexcept
    {
        UniqueHandle{std::move(other)}.swap(*this);
        return *this;
    }
    ~UniqueHandle() noexcept { Traits::destroy(value_); }

    explicit operator bool() const noexcept
    {
        return value_ != Traits::invalid();
    }
    handle const& get() const noexcept { return value_; }
    handle const& operator*() const noexcept { return value_; }
    handle release() noexcept
    {
        return std::exchange(value_, Traits::invalid());
    }
    void reset(handle value = Traits::invalid()) noexcept
    {
        // NOTE: A test for self-reset, i.e. whether `value` points to an object already managed by `*this`, is not performed!
        assert(value_ != value);

        Traits::destroy(value_);
        value_ = value;
    }
    void swap(UniqueHandle<Traits>& other) noexcept
    {
        using std::swap;
        swap(value_, other.value_);
    }
};

template <typename Traits>
void swap(UniqueHandle<Traits>& lhs, UniqueHandle<Traits>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Traits>
bool operator==(UniqueHandle<Traits> const& lhs,
                UniqueHandle<Traits> const& rhs) noexcept
{
    return lhs.get() == rhs.get();
}

template <typename Traits>
bool operator!=(UniqueHandle<Traits> const& lhs,
                UniqueHandle<Traits> const& rhs) noexcept
{
    return !operator==(lhs, rhs);
}
} // namespace utils
