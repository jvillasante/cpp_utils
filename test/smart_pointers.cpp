#include <libutils/smart_pointers.hpp>

#include <catch2/catch_test_macros.hpp>
#include <memory>

namespace
{
struct Base
{
    virtual ~Base() = default;
    virtual int id() const { return 0; }
};

struct Derived : Base
{
    int id() const override { return 1; }
};

struct OtherDerived : Base
{
    int id() const override { return 2; }
};
} // namespace

TEST_CASE("SmartPointers - static_ptr_cast default deleter")
{
    std::unique_ptr<Base> base = std::make_unique<Derived>();
    auto derived = utils::static_ptr_cast<Derived>(std::move(base));
    REQUIRE(derived != nullptr);
    REQUIRE(derived->id() == 1);
}

TEST_CASE("SmartPointers - dynamic_ptr_cast success transfers ownership")
{
    std::unique_ptr<Base> base = std::make_unique<Derived>();
    auto derived = utils::dynamic_ptr_cast<Derived>(base);
    REQUIRE(derived != nullptr);
    REQUIRE(derived->id() == 1);
    REQUIRE(base == nullptr); // ownership transferred
}

TEST_CASE("SmartPointers - dynamic_ptr_cast failure preserves ownership")
{
    std::unique_ptr<Base> base = std::make_unique<Derived>();
    auto other = utils::dynamic_ptr_cast<OtherDerived>(base);
    REQUIRE(other == nullptr);
    REQUIRE(base != nullptr); // ownership retained on failed cast
    REQUIRE(base->id() == 1);
}
