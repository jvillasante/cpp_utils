#pragma once

namespace utils
{
/**
 * `overloaded` bundles several callables into one overload set, which is the
 * idiomatic way to write an inline visitor for std::visit:
 *
 *   std::variant<int, std::string> v = 42;
 *   std::visit(utils::overloaded{
 *       [](int i) { std::cout << "int: " << i; },
 *       [](std::string const& s) { std::cout << "str: " << s; },
 *   }, v);
 */
template <typename... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

// Deduction guide (not needed from C++20 on, but harmless and required for
// C++17).
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
} // namespace utils
