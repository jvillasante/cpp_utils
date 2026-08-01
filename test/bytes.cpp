#include <libutils/bytes.hpp>

#include <catch2/catch_test_macros.hpp>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

TEST_CASE("Bytes - from_bytes / as_bytes roundtrip")
{
    std::uint32_t const original = 0xDEADBEEFu;
    std::byte const* bytes = utils::bytes::as_bytes(original);
    auto const restored = utils::bytes::from_bytes<std::uint32_t>(bytes);
    REQUIRE(original == restored);
}

TEST_CASE("Bytes - to_byte_vector round-trips with from_bytes")
{
    std::uint32_t const original = 0xCAFEBABEu;
    auto const buf = utils::bytes::to_byte_vector(original);
    REQUIRE(buf.size() == sizeof(std::uint32_t));
    REQUIRE(utils::bytes::from_bytes<std::uint32_t>(buf.data()) == original);
}

TEST_CASE("Bytes - to_string_view from vector")
{
    std::vector<std::byte> data{std::byte{'H'}, std::byte{'i'}};
    auto sv = utils::bytes::to_string_view(data);
    REQUIRE(sv == "Hi");
}

TEST_CASE("Bytes - to_string_view from span")
{
    std::vector<std::byte> data{std::byte{'H'}, std::byte{'i'}};
    utils::span<std::byte const> sp{data};
    REQUIRE(utils::bytes::to_string_view(sp) == "Hi");
}

TEST_CASE("Bytes - endian conversions round-trip")
{
    std::uint32_t const v = 0x0A0B0C0Du;
    REQUIRE(utils::bytes::from_big_endian(utils::bytes::to_big_endian(v)) == v);
    REQUIRE(utils::bytes::from_little_endian(utils::bytes::to_little_endian(v)) ==
            v);

    // On any host, exactly one direction is identity and the other a byteswap.
    if constexpr (utils::endian::native == utils::endian::little)
    {
        REQUIRE(utils::bytes::to_little_endian(v) == v);
        REQUIRE(utils::bytes::to_big_endian(v) == utils::byteswap(v));
    }
    else
    {
        REQUIRE(utils::bytes::to_big_endian(v) == v);
        REQUIRE(utils::bytes::to_little_endian(v) == utils::byteswap(v));
    }
}

TEST_CASE("Bytes - store_be / load_be layout and roundtrip")
{
    std::array<std::byte, 4> buf{};
    utils::bytes::store_be<std::uint32_t>(buf.data(), 0x12345678u);
    // Big-endian layout: most-significant byte first.
    REQUIRE(buf[0] == std::byte{0x12});
    REQUIRE(buf[1] == std::byte{0x34});
    REQUIRE(buf[2] == std::byte{0x56});
    REQUIRE(buf[3] == std::byte{0x78});
    REQUIRE(utils::bytes::load_be<std::uint32_t>(buf.data()) == 0x12345678u);
}

TEST_CASE("Bytes - store_le / load_le layout and roundtrip")
{
    std::array<std::byte, 4> buf{};
    utils::bytes::store_le<std::uint32_t>(buf.data(), 0x12345678u);
    // Little-endian layout: least-significant byte first.
    REQUIRE(buf[0] == std::byte{0x78});
    REQUIRE(buf[1] == std::byte{0x56});
    REQUIRE(buf[2] == std::byte{0x34});
    REQUIRE(buf[3] == std::byte{0x12});
    REQUIRE(utils::bytes::load_le<std::uint32_t>(buf.data()) == 0x12345678u);
}

TEST_CASE("Bytes - load_be and load_le disagree on multi-byte values")
{
    std::array<std::byte, 2> buf{std::byte{0x12}, std::byte{0x34}};
    REQUIRE(utils::bytes::load_be<std::uint16_t>(buf.data()) == 0x1234u);
    REQUIRE(utils::bytes::load_le<std::uint16_t>(buf.data()) == 0x3412u);
}

TEST_CASE("Bytes - bounds-checked span load/store round-trips")
{
    std::array<std::byte, 4> buf{};
    utils::span<std::byte> const wr{buf.data(), buf.size()};
    utils::bytes::store_be<std::uint32_t>(wr, 0x12345678u);
    REQUIRE(buf[0] == std::byte{0x12});
    REQUIRE(buf[3] == std::byte{0x78});

    utils::span<std::byte const> const rd{buf.data(), buf.size()};
    REQUIRE(utils::bytes::load_be<std::uint32_t>(rd) == 0x12345678u);
    REQUIRE(utils::bytes::load_le<std::uint32_t>(rd) == 0x78563412u);
}

TEST_CASE("Bytes - bounds-checked span load throws on short buffer")
{
    std::array<std::byte, 2> buf{std::byte{0x12}, std::byte{0x34}};
    utils::span<std::byte const> const rd{buf.data(), buf.size()};
    REQUIRE_THROWS_AS(utils::bytes::load_be<std::uint32_t>(rd),
                      std::out_of_range);
    REQUIRE_THROWS_AS(utils::bytes::load_le<std::uint32_t>(rd),
                      std::out_of_range);
    // Exactly-sized read is fine.
    REQUIRE(utils::bytes::load_be<std::uint16_t>(rd) == 0x1234u);
}

TEST_CASE("Bytes - bounds-checked span store throws on short buffer")
{
    std::array<std::byte, 2> buf{};
    utils::span<std::byte> const wr{buf.data(), buf.size()};
    REQUIRE_THROWS_AS(utils::bytes::store_be<std::uint32_t>(wr, 0u),
                      std::out_of_range);
    REQUIRE_THROWS_AS(utils::bytes::store_le<std::uint32_t>(wr, 0u),
                      std::out_of_range);
}

TEST_CASE("Bytes - byte_view over string_view")
{
    std::string_view const s = "Hi";
    auto const v = utils::bytes::byte_view(s);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == std::byte{'H'});
    REQUIRE(v[1] == std::byte{'i'});
    // Round-trips back through to_string_view.
    REQUIRE(utils::bytes::to_string_view(v) == "Hi");
}

TEST_CASE("Bytes - byte_view over a typed span scales by sizeof")
{
    std::array<std::uint32_t, 2> data{0x11223344u, 0x55667788u};
    utils::span<std::uint32_t> const sp{data.data(), data.size()};
    auto const v = utils::bytes::byte_view(sp);
    REQUIRE(v.size() == 2 * sizeof(std::uint32_t));
}

TEST_CASE("Bytes - writable_byte_view mutates the underlying buffer")
{
    std::array<std::uint16_t, 1> data{0};
    utils::span<std::uint16_t> const sp{data.data(), data.size()};
    auto wv = utils::bytes::writable_byte_view(sp);
    REQUIRE(wv.size() == sizeof(std::uint16_t));

    utils::bytes::store_be<std::uint16_t>(wv, 0x1234u);
    // Big-endian byte layout is host-independent.
    REQUIRE(wv[0] == std::byte{0x12});
    REQUIRE(wv[1] == std::byte{0x34});
    // The store landed in the original array (host-order value is endian-dep).
    REQUIRE(data[0] != 0);
}

TEST_CASE("Bytes - byte_reader sequential reads and position tracking")
{
    // A little frame: u16 length (BE), then a 3-byte payload.
    std::array<std::byte, 5> buf{std::byte{0x00}, std::byte{0x03},
                                 std::byte{'a'},  std::byte{'b'},
                                 std::byte{'c'}};
    utils::bytes::byte_reader r{
        utils::span<std::byte const>{buf.data(), buf.size()}};

    REQUIRE(r.size() == 5);
    REQUIRE(r.remaining() == 5);

    REQUIRE(r.read_be<std::uint16_t>() == 0x0003u);
    REQUIRE(r.position() == 2);
    REQUIRE(r.remaining() == 3);

    auto const payload = r.read_bytes(3);
    REQUIRE(utils::bytes::to_string_view(payload) == "abc");
    REQUIRE(r.exhausted());
}

TEST_CASE("Bytes - byte_reader try_read reports underrun without advancing")
{
    std::array<std::byte, 2> buf{std::byte{0x12}, std::byte{0x34}};
    utils::bytes::byte_reader r{
        utils::span<std::byte const>{buf.data(), buf.size()}};

    // Not enough for a u32: nullopt, position unchanged.
    REQUIRE_FALSE(r.try_read_be<std::uint32_t>().has_value());
    REQUIRE(r.position() == 0);

    // A u16 fits.
    auto const v = r.try_read_be<std::uint16_t>();
    REQUIRE(v.has_value());
    REQUIRE(*v == 0x1234u);
    REQUIRE(r.exhausted());

    // Now empty: another read is nullopt.
    REQUIRE_FALSE(r.try_read_le<std::uint16_t>().has_value());
}

TEST_CASE("Bytes - byte_reader throwing read underruns")
{
    std::array<std::byte, 2> buf{};
    utils::bytes::byte_reader r{
        utils::span<std::byte const>{buf.data(), buf.size()}};
    REQUIRE_THROWS_AS(r.read_be<std::uint32_t>(), std::out_of_range);
    REQUIRE_THROWS_AS(r.read_bytes(3), std::out_of_range);
    REQUIRE_FALSE(r.try_skip(3));
    REQUIRE_THROWS_AS(r.skip(3), std::out_of_range);
}

TEST_CASE("Bytes - byte_writer round-trips through byte_reader")
{
    std::array<std::byte, 8> buf{};
    utils::bytes::byte_writer w{
        utils::span<std::byte>{buf.data(), buf.size()}};

    REQUIRE(w.try_write_be<std::uint16_t>(0x1234u));
    REQUIRE(w.try_write_le<std::uint32_t>(0xDEADBEEFu));
    std::array<std::byte, 2> const tail{std::byte{'h'}, std::byte{'i'}};
    REQUIRE(w.try_write_bytes(
        utils::span<std::byte const>{tail.data(), tail.size()}));
    REQUIRE(w.position() == 8);
    REQUIRE(w.exhausted());

    utils::bytes::byte_reader r{
        utils::span<std::byte const>{buf.data(), buf.size()}};
    REQUIRE(r.read_be<std::uint16_t>() == 0x1234u);
    REQUIRE(r.read_le<std::uint32_t>() == 0xDEADBEEFu);
    REQUIRE(utils::bytes::to_string_view(r.read_bytes(2)) == "hi");
}

TEST_CASE("Bytes - byte_writer reports and throws on overrun")
{
    std::array<std::byte, 2> buf{};
    utils::bytes::byte_writer w{
        utils::span<std::byte>{buf.data(), buf.size()}};

    // No room for a u32: try_ returns false and does not advance.
    REQUIRE_FALSE(w.try_write_be<std::uint32_t>(0u));
    REQUIRE(w.position() == 0);

    // Throwing form.
    REQUIRE_THROWS_AS(w.write_be<std::uint32_t>(0u), std::out_of_range);

    // A u16 fits exactly.
    REQUIRE(w.try_write_le<std::uint16_t>(0xABCDu));
    REQUIRE(w.exhausted());
}
