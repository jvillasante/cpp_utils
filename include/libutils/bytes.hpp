#pragma once

#include <libutils/polyfill.hpp>

#include <cstddef>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

namespace utils::bytes
{
// ----------
// Object <-> raw byte representation
// ----------

// Reconstruct a trivially-copyable object from a byte buffer.
template <typename To>
[[nodiscard]] To from_bytes(std::byte const* bytes)
{
    static_assert(std::is_trivially_constructible_v<To>);
    static_assert(std::is_trivially_copyable_v<To>);

    To ret;
    std::memcpy(&ret, bytes, sizeof(To));
    return ret;
}

// View an object's underlying bytes (mutable).
template <typename From>
[[nodiscard]] std::byte* as_bytes(From& from)
{
    static_assert(std::is_trivially_copyable_v<From>);
    return reinterpret_cast<std::byte*>(&from);
}

// View an object's underlying bytes (const).
template <typename From>
[[nodiscard]] std::byte const* as_bytes(From const& from)
{
    static_assert(std::is_trivially_copyable_v<From>);
    return reinterpret_cast<std::byte const*>(&from);
}

// Copy a trivially-copyable object into an owning byte vector.
template <typename From>
[[nodiscard]] std::vector<std::byte> to_byte_vector(From const& from)
{
    static_assert(std::is_trivially_copyable_v<From>);
    std::vector<std::byte> out(sizeof(From));
    std::memcpy(out.data(), &from, sizeof(From));
    return out;
}

// ----------
// Byte buffers as text
// ----------

[[nodiscard]] inline std::string_view to_string_view(std::byte const* data,
                                                     std::size_t size)
{
    return {reinterpret_cast<char const*>(data), size};
}

[[nodiscard]] inline std::string_view
to_string_view(std::vector<std::byte> const& data)
{
    return to_string_view(data.data(), data.size());
}

[[nodiscard]] inline std::string_view
to_string_view(utils::span<std::byte const> data)
{
    return to_string_view(data.data(), data.size());
}

// ----------
// Byte views over contiguous ranges
//
// Reinterpret a string or typed span as a span of bytes, e.g. to hand
// .data()/.size() to a socket send/recv. Named distinctly from as_bytes (which
// views a single object's representation) to avoid silently viewing the bytes
// of a container object instead of its elements.
// ----------

// Read-only byte view over a string's characters.
[[nodiscard]] inline utils::span<std::byte const> byte_view(std::string_view s)
{
    return {reinterpret_cast<std::byte const*>(s.data()), s.size()};
}

// Read-only byte view over a span of trivially-copyable elements. Accepts spans
// of const or non-const T; the result is always read-only.
template <typename T>
[[nodiscard]] utils::span<std::byte const> byte_view(utils::span<T> s)
{
    static_assert(std::is_trivially_copyable_v<T>);
    return {reinterpret_cast<std::byte const*>(s.data()), s.size() * sizeof(T)};
}

// Writable byte view over a span of trivially-copyable, non-const elements.
template <typename T>
[[nodiscard]] utils::span<std::byte> writable_byte_view(utils::span<T> s)
{
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(!std::is_const_v<T>,
                  "writable_byte_view requires a non-const element type");
    return {reinterpret_cast<std::byte*>(s.data()), s.size() * sizeof(T)};
}

// ----------
// Endianness
// ----------

// Convert a host-order integer to its big-endian representation. Because
// std::byteswap is its own inverse, from_big_endian is the same operation;
// both names exist for call-site readability.
template <typename T>
[[nodiscard]] constexpr T to_big_endian(T value) noexcept
{
    static_assert(std::is_integral_v<T>);
    if constexpr (utils::endian::native == utils::endian::big) {
        return value;
    } else {
        return utils::byteswap(value);
    }
}

template <typename T>
[[nodiscard]] constexpr T to_little_endian(T value) noexcept
{
    static_assert(std::is_integral_v<T>);
    if constexpr (utils::endian::native == utils::endian::little) {
        return value;
    } else {
        return utils::byteswap(value);
    }
}

template <typename T>
[[nodiscard]] constexpr T from_big_endian(T value) noexcept
{
    return to_big_endian(value);
}

template <typename T>
[[nodiscard]] constexpr T from_little_endian(T value) noexcept
{
    return to_little_endian(value);
}

// ----------
// Endian-aware load/store against a byte buffer
// ----------

// Read a T from a big-endian byte buffer, returning the host-order value.
template <typename T>
[[nodiscard]] T load_be(std::byte const* data)
{
    static_assert(std::is_integral_v<T>);
    return from_big_endian(from_bytes<T>(data));
}

// Read a T from a little-endian byte buffer, returning the host-order value.
template <typename T>
[[nodiscard]] T load_le(std::byte const* data)
{
    static_assert(std::is_integral_v<T>);
    return from_little_endian(from_bytes<T>(data));
}

// Write a host-order value into a byte buffer as big-endian.
template <typename T>
void store_be(std::byte* data, T value)
{
    static_assert(std::is_integral_v<T>);
    T const be = to_big_endian(value);
    std::memcpy(data, &be, sizeof(T));
}

// Write a host-order value into a byte buffer as little-endian.
template <typename T>
void store_le(std::byte* data, T value)
{
    static_assert(std::is_integral_v<T>);
    T const le = to_little_endian(value);
    std::memcpy(data, &le, sizeof(T));
}

// ----------
// Bounds-checked load/store over a span
//
// These overloads verify the span is large enough to hold a T before touching
// memory, throwing std::out_of_range otherwise. Prefer them when parsing
// untrusted or externally-sized buffers; the raw-pointer versions above assume
// the caller has already guaranteed the size.
// ----------

template <typename T>
[[nodiscard]] T load_be(utils::span<std::byte const> data)
{
    static_assert(std::is_integral_v<T>);
    if (data.size() < sizeof(T)) {
        throw std::out_of_range("load_be: buffer too small for T");
    }
    return load_be<T>(data.data());
}

template <typename T>
[[nodiscard]] T load_le(utils::span<std::byte const> data)
{
    static_assert(std::is_integral_v<T>);
    if (data.size() < sizeof(T)) {
        throw std::out_of_range("load_le: buffer too small for T");
    }
    return load_le<T>(data.data());
}

template <typename T>
void store_be(utils::span<std::byte> data, T value)
{
    static_assert(std::is_integral_v<T>);
    if (data.size() < sizeof(T)) {
        throw std::out_of_range("store_be: buffer too small for T");
    }
    store_be<T>(data.data(), value);
}

template <typename T>
void store_le(utils::span<std::byte> data, T value)
{
    static_assert(std::is_integral_v<T>);
    if (data.size() < sizeof(T)) {
        throw std::out_of_range("store_le: buffer too small for T");
    }
    store_le<T>(data.data(), value);
}

// ----------
// Sequential cursors over a byte buffer
//
// byte_reader / byte_writer track a position into a span and read/write
// sequentially. Each operation comes in two forms:
//   - try_*  : noexcept; reports underrun via std::optional (reads) or a bool
//              (writes) and does not advance on failure. Pay nothing for
//              exceptions on the hot path.
//   - plain  : a thin wrapper that throws std::out_of_range on underrun.
// Mix freely: use try_* where failure is expected (parsing untrusted input) and
// the throwing form where a short buffer is a programming error.
// ----------

class byte_reader
{
public:
    explicit byte_reader(utils::span<std::byte const> data) noexcept
        : data_(data)
    {}

    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    [[nodiscard]] std::size_t position() const noexcept { return pos_; }
    [[nodiscard]] std::size_t remaining() const noexcept
    {
        return data_.size() - pos_;
    }
    [[nodiscard]] bool exhausted() const noexcept
    {
        return pos_ >= data_.size();
    }

    template <typename T>
    [[nodiscard]] std::optional<T> try_read_be() noexcept
    {
        static_assert(std::is_integral_v<T>);
        if (remaining() < sizeof(T)) {
            return std::nullopt;
        }
        T const value = load_be<T>(data_.data() + pos_);
        pos_ += sizeof(T);
        return value;
    }

    template <typename T>
    [[nodiscard]] std::optional<T> try_read_le() noexcept
    {
        static_assert(std::is_integral_v<T>);
        if (remaining() < sizeof(T)) {
            return std::nullopt;
        }
        T const value = load_le<T>(data_.data() + pos_);
        pos_ += sizeof(T);
        return value;
    }

    template <typename T>
    [[nodiscard]] T read_be()
    {
        std::optional<T> const value = try_read_be<T>();
        if (!value) {
            throw std::out_of_range("byte_reader::read_be: buffer underrun");
        }
        return *value;
    }

    template <typename T>
    [[nodiscard]] T read_le()
    {
        std::optional<T> const value = try_read_le<T>();
        if (!value) {
            throw std::out_of_range("byte_reader::read_le: buffer underrun");
        }
        return *value;
    }

    // Read a raw sub-view of n bytes (e.g. a variable-length payload).
    [[nodiscard]] std::optional<utils::span<std::byte const>>
    try_read_bytes(std::size_t n) noexcept
    {
        if (remaining() < n) {
            return std::nullopt;
        }
        utils::span<std::byte const> const out = data_.subspan(pos_, n);
        pos_ += n;
        return out;
    }

    [[nodiscard]] utils::span<std::byte const> read_bytes(std::size_t n)
    {
        std::optional<utils::span<std::byte const>> const out =
            try_read_bytes(n);
        if (!out) {
            throw std::out_of_range("byte_reader::read_bytes: buffer underrun");
        }
        return *out;
    }

    [[nodiscard]] bool try_skip(std::size_t n) noexcept
    {
        if (remaining() < n) {
            return false;
        }
        pos_ += n;
        return true;
    }

    void skip(std::size_t n)
    {
        if (!try_skip(n)) {
            throw std::out_of_range("byte_reader::skip: buffer underrun");
        }
    }

private:
    utils::span<std::byte const> data_;
    std::size_t pos_ = 0;
};

class byte_writer
{
public:
    explicit byte_writer(utils::span<std::byte> data) noexcept : data_(data) {}

    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    [[nodiscard]] std::size_t position() const noexcept { return pos_; }
    [[nodiscard]] std::size_t remaining() const noexcept
    {
        return data_.size() - pos_;
    }
    [[nodiscard]] bool exhausted() const noexcept
    {
        return pos_ >= data_.size();
    }

    template <typename T>
    [[nodiscard]] bool try_write_be(T value) noexcept
    {
        static_assert(std::is_integral_v<T>);
        if (remaining() < sizeof(T)) {
            return false;
        }
        store_be<T>(data_.data() + pos_, value);
        pos_ += sizeof(T);
        return true;
    }

    template <typename T>
    [[nodiscard]] bool try_write_le(T value) noexcept
    {
        static_assert(std::is_integral_v<T>);
        if (remaining() < sizeof(T)) {
            return false;
        }
        store_le<T>(data_.data() + pos_, value);
        pos_ += sizeof(T);
        return true;
    }

    template <typename T>
    void write_be(T value)
    {
        if (!try_write_be<T>(value)) {
            throw std::out_of_range("byte_writer::write_be: buffer overrun");
        }
    }

    template <typename T>
    void write_le(T value)
    {
        if (!try_write_le<T>(value)) {
            throw std::out_of_range("byte_writer::write_le: buffer overrun");
        }
    }

    [[nodiscard]] bool
    try_write_bytes(utils::span<std::byte const> src) noexcept
    {
        if (remaining() < src.size()) {
            return false;
        }
        if (!src.empty()) {
            std::memcpy(data_.data() + pos_, src.data(), src.size());
        }
        pos_ += src.size();
        return true;
    }

    void write_bytes(utils::span<std::byte const> src)
    {
        if (!try_write_bytes(src)) {
            throw std::out_of_range("byte_writer::write_bytes: buffer overrun");
        }
    }

private:
    utils::span<std::byte> data_;
    std::size_t pos_ = 0;
};
} // namespace utils::bytes
