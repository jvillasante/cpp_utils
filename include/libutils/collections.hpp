#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace utils::collections
{
template <typename T>
void quick_remove_at(std::vector<T>& v,
                     typename std::vector<T>::size_type const idx)
{
    if (idx < v.size()) {
        v[idx] = std::move(v.back());
        v.pop_back();
    }
}

template <typename T>
void quick_remove_at(std::vector<T>& v,
                     typename std::vector<T>::iterator const it)
{
    if (it != std::end(v)) {
        *it = std::move(v.back());
        v.pop_back();
    }
}

template <typename C, typename T>
void insert_sorted(C& c, T const& value)
{
    assert(std::is_sorted(std::begin(c), std::end(c)) == true);
    auto const it = std::lower_bound(std::begin(c), std::end(c), value);
    c.insert(it, value);
}

// Number of bytes a collection's storage occupies (generic: uses size()).
// For std::vector specifically, see the overload below which uses capacity().
template <typename Collection>
[[nodiscard]] std::size_t areaof(Collection const& x)
{
    using value_type = typename Collection::value_type;
    return (std::size(x) * sizeof(value_type)) + sizeof(Collection);
}

// std::vector overload: reports allocated (capacity) bytes, not just used bytes.
template <typename T, typename Allocator>
[[nodiscard]] std::size_t areaof(std::vector<T, Allocator> const& x)
{
    return (x.capacity() * sizeof(T)) + sizeof(x);
}

// Ratio of live-element bytes to total allocated bytes.
template <typename Collection>
[[nodiscard]] double memory_utilization(Collection const& x)
{
    using value_type = typename Collection::value_type;
    double useful{std::size(x) * sizeof(value_type)};
    auto total{static_cast<double>(areaof(x))};
    return useful / total;
}

// std::vector overload: useful = size, total = capacity.
template <typename T, typename Allocator>
[[nodiscard]] double memory_utilization(std::vector<T, Allocator> const& x)
{
    auto useful{static_cast<double>(x.size() * sizeof(T))};
    auto total{static_cast<double>(areaof(x))};
    return useful / total;
}
} // namespace utils::collections
