#pragma once

#include <cstddef>
#include <vector>

namespace utils
{
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
} // namespace utils
