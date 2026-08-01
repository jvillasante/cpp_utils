#include <libutils/bit.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

TEST_CASE("Bit - get_bit")
{
    REQUIRE(utils::bit::get_bit(0b1010u, 1) == 1u);
    REQUIRE(utils::bit::get_bit(0b1010u, 0) == 0u);
    REQUIRE(utils::bit::get_bit(0b1010u, 3) == 1u);

    // 64-bit: bit at position 33 (would UB with 1U << 33)
    std::uint64_t const val = std::uint64_t{1} << 33;
    REQUIRE(utils::bit::get_bit(val, 33) == 1u);
    REQUIRE(utils::bit::get_bit(val, 32) == 0u);
}

TEST_CASE("Bit - set_bit")
{
    REQUIRE(utils::bit::set_bit(0b0000u, 1, true) == 0b0010u);
    REQUIRE(utils::bit::set_bit(0b1111u, 1, false) == 0b1101u);

    // 64-bit: set bit beyond 32
    std::uint64_t const val = utils::bit::set_bit(std::uint64_t{0}, 33, true);
    REQUIRE(val == (std::uint64_t{1} << 33u));
}

TEST_CASE("Bit - invert_bit")
{
    REQUIRE(utils::bit::invert_bit(0b1010u, 0) == 0b1011u);
    REQUIRE(utils::bit::invert_bit(0b1010u, 1) == 0b1000u);

    std::uint64_t const val = utils::bit::invert_bit(std::uint64_t{0}, 40);
    REQUIRE(val == (std::uint64_t{1} << 40u));
}

TEST_CASE("Bit - get_bit_range")
{
    // bits 1..3 of 0b11110 = 0b111
    REQUIRE(utils::bit::get_bit_range(0b11110u, 1u, 3u) == 0b111u);
    REQUIRE(utils::bit::get_bit_range(0b11110u, 1u, 2u) == 0b11u);
}

TEST_CASE("Bit - set_bit_range")
{
    // set bits 1..3 of 0b00000 to 0b101 => 0b01010
    REQUIRE(utils::bit::set_bit_range(0u, 1u, 3u, 0b101u) == 0b01010u);
}

TEST_CASE("Bit - single-bit range (start == end)")
{
    // A one-bit range is now valid and behaves like get_bit.
    REQUIRE(utils::bit::get_bit_range(0b100u, 2u, 2u) == 1u);
    REQUIRE(utils::bit::get_bit_range(0b100u, 0u, 0u) == 0u);
    REQUIRE(utils::bit::set_bit_range(0u, 3u, 3u, 1u) == 0b1000u);
    REQUIRE(utils::bit::invert_bit_range(0b0000u, 2u, 2u) == 0b0100u);
}

TEST_CASE("Bit - full-width range does not overflow the shift")
{
    // start == 0, end == digits-1 previously shifted by the full width (UB).
    std::uint32_t const all32 = 0xFFFFFFFFu;
    REQUIRE(utils::bit::get_bit_range(all32, 0u, 31u) == all32);
    REQUIRE(utils::bit::set_bit_range(std::uint32_t{0}, 0u, 31u,
                                      std::size_t{0xFFFFFFFFu}) == all32);
    REQUIRE(utils::bit::invert_bit_range(std::uint32_t{0}, 0u, 31u) == all32);

    std::uint64_t const all64 = 0xFFFFFFFFFFFFFFFFull;
    REQUIRE(utils::bit::get_bit_range(all64, 0u, 63u) == all64);
    REQUIRE(utils::bit::invert_bit_range(std::uint64_t{0}, 0u, 63u) == all64);

    // A high-end sub-range that reaches the top bit.
    REQUIRE(utils::bit::get_bit_range(all32, 16u, 31u) == 0xFFFFu);
}

TEST_CASE("Bit - invert_bit_range")
{
    REQUIRE(utils::bit::invert_bit_range(0b00000u, 1u, 3u) == 0b01110u);
    REQUIRE(utils::bit::invert_bit_range(0b11111u, 1u, 3u) == 0b10001u);
}

TEST_CASE("Bit - from_bytes / as_bytes roundtrip")
{
    std::uint32_t const original = 0xDEADBEEFu;
    std::byte const* bytes = utils::bit::as_bytes(original);
    auto const restored = utils::bit::from_bytes<std::uint32_t>(bytes);
    REQUIRE(original == restored);
}

TEST_CASE("Bit - to_string_view")
{
    std::vector<std::byte> data{std::byte{'H'}, std::byte{'i'}};
    auto sv = utils::bit::to_string_view(data);
    REQUIRE(sv == "Hi");
}
