#pragma once

#include <memory>

namespace utils
{
/**
 * @brief Converts std::unique_ptr of base type to std::unique_ptr of derived
 * type by using static_cast internally
 *
 * @details Ownership of the object is transferred unconditionally to the
 * returned std::unique_ptr. It is somewhat analogous to std::static_ptr_cast
 */
template <typename Derived, typename Base, typename Deleter>
[[nodiscard]] std::unique_ptr<Derived, Deleter>
static_ptr_cast(std::unique_ptr<Base, Deleter> base)
{
    auto derived_ptr = static_cast<Derived*>(base.release());
    return std::unique_ptr<Derived, Deleter>(derived_ptr,
                                             std::move(base.get_deleter()));
}

/**
 * @details A separate static_ptr_cast() version for std::default_delete<T> is
 * required to provide expected results in use-cases like
 *
 *   std::unique_ptr<Base> base(new Derived());
 *   auto derived = static_ptr_cast<Derived>(std::move(base));
 *
 * It is desirable for `derived` to have type `std::unique_ptr<Derived>`,
 * but if general version of static_ptr_cast() is used, it will actually
 * be `std::unique_ptr<Derived, std::default_delete<Base>>` which is very
 * inconvenient
 */
template <typename Derived, typename Base>
[[nodiscard]] std::unique_ptr<Derived>
static_ptr_cast(std::unique_ptr<Base> base) noexcept
{
    auto derived_ptr = static_cast<Derived*>(base.release());
    return std::unique_ptr<Derived>(derived_ptr);
}

/**
 * @brief Converts std::unique_ptr of base type to std::unique_ptr of derived
 * type by using dynamic_cast internally.
 *
 * @details Takes `base` by lvalue reference. If the cast succeeds, ownership
 * is transferred to the returned std::unique_ptr and `base` becomes null.
 * If the cast fails, `base` retains ownership and null is returned.
 */
template <typename Derived, typename Base, typename Deleter>
[[nodiscard]] std::unique_ptr<Derived, Deleter>
dynamic_ptr_cast(std::unique_ptr<Base, Deleter>& base)
{
    if (auto* derived = dynamic_cast<Derived*>(base.get());
        derived != nullptr) {
        base.release();
        return std::unique_ptr<Derived, Deleter>(derived,
                                                 std::move(base.get_deleter()));
    }

    return nullptr;
}

/**
 * @details see description to @see static_ptr_cast() version that works only
 * with `std::default_delete<T>` for explanation why this special version of
 * dynamic_ptr_cast() is required
 */
template <typename Derived, typename Base>
[[nodiscard]] std::unique_ptr<Derived>
dynamic_ptr_cast(std::unique_ptr<Base>& base) noexcept
{
    if (auto* derived = dynamic_cast<Derived*>(base.get());
        derived != nullptr) {
        base.release();
        return std::unique_ptr<Derived>(derived);
    }

    return nullptr;
}
} // namespace utils
