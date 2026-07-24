#pragma once

#include <cstdint>
#include <iostream>
#include <source_location>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace utils::testing
{
//
// Declaration only for `TypeDisplayer`, no definition!
// Usage: utils::testing::TypeDisplayer<decltype(x)> xType;
//
template <typename T>
class TypeDisplayer;

//
// Google Test better std::cout
//
#define PRINTF(...)                                                            \
    do {                                                                       \
        printf("%s", "[      OUT>] ");                                         \
        printf(__VA_ARGS__);                                                   \
    } while (0)

// C++ stream interface
class gtest_cout : public std::stringstream
{
public:
    gtest_cout() = default;
    gtest_cout(gtest_cout const&) = delete;
    gtest_cout(gtest_cout&&) = delete;
    gtest_cout& operator=(gtest_cout const&) = delete;
    gtest_cout& operator=(gtest_cout&&) = delete;
    ~gtest_cout() override { PRINTF("%s", str().c_str()); } // NOLINT
};

// `GCOUT` is in the global namespace!
#define GCOUT utils::testing::gtest_cout()

//
// Lifetime Statistics
//

// Defined at namespace scope (not inside StatsCounter) so that Lifetime<T>
// can use `using enum LifetimeStats` without hitting the "dependent type"
// restriction that applies to `using enum` on inherited nested types.
enum class LifetimeStats : std::uint8_t
{
    DefaultConstructor,
    Constructor,
    CopyConstructor,
    MoveConstructor,
    Destructor,
    CopyAssignment,
    MoveAssignment,
    MemberSwap,
    NonMemberSwap,
    ObjectCount,
    ObjectTotalCount,
};

namespace internal
{
// CRTP base: each Lifetime<T, Print> instantiation gets its own stats_ map,
// so Lifetime<int> and Lifetime<std::string> counters never interfere.
template <typename Derived>
struct StatsCounter
{
    static void print_stats(char const* header, std::ostream& os = std::cout)
    {
        os << "----- " << header << '\n';
        os << "default constructor calls = "
           << stats_[LifetimeStats::DefaultConstructor] << '\n';
        os << "        constructor calls = "
           << stats_[LifetimeStats::Constructor] << '\n';
        os << "   copy constructor calls = "
           << stats_[LifetimeStats::CopyConstructor] << '\n';
        os << "   move constructor calls = "
           << stats_[LifetimeStats::MoveConstructor] << '\n';
        os << "         destructor calls = "
           << stats_[LifetimeStats::Destructor] << '\n';
        os << "    copy assignment calls = "
           << stats_[LifetimeStats::CopyAssignment] << '\n';
        os << "    move assignment calls = "
           << stats_[LifetimeStats::MoveAssignment] << '\n';
        os << "        member swap calls = "
           << stats_[LifetimeStats::MemberSwap] << '\n';
        os << "    non member swap calls = "
           << stats_[LifetimeStats::NonMemberSwap] << '\n';
        os << "            objects count = "
           << stats_[LifetimeStats::ObjectCount] << '\n';
        os << "      total objects count = "
           << stats_[LifetimeStats::ObjectTotalCount] << '\n';
    }

    static void clear_stat(LifetimeStats const key) { stats_.at(key) = 0; }
    static void clear_stats()
    {
        for (auto& [_, value] : stats_) {
            value = 0;
        }
    }
    static void increment_stat(LifetimeStats const key) { stats_.at(key)++; }
    static void increment_stats(std::initializer_list<LifetimeStats const> il)
    {
        for (LifetimeStats const key : il) {
            increment_stat(key);
        }
    }
    static void decrement_stat(LifetimeStats const key) { stats_.at(key)--; }
    static void decrement_stats(std::initializer_list<LifetimeStats const> il)
    {
        for (LifetimeStats const key : il) {
            decrement_stat(key);
        }
    }
    static std::size_t get_stat(LifetimeStats const key)
    {
        return stats_.at(key);
    }

private:
    // NOLINTNEXTLINE
    inline static std::unordered_map<LifetimeStats, std::size_t> stats_ = {
        {LifetimeStats::DefaultConstructor, 0},
        {LifetimeStats::Constructor, 0},
        {LifetimeStats::CopyConstructor, 0},
        {LifetimeStats::MoveConstructor, 0},
        {LifetimeStats::Destructor, 0},
        {LifetimeStats::CopyAssignment, 0},
        {LifetimeStats::MoveAssignment, 0},
        {LifetimeStats::MemberSwap, 0},
        {LifetimeStats::NonMemberSwap, 0},
        {LifetimeStats::ObjectCount, 0},
        {LifetimeStats::ObjectTotalCount, 0},
    };
};
} // namespace internal

template <typename T, bool Print = false>
class Lifetime : public internal::StatsCounter<Lifetime<T, Print>>
{
    using Base = internal::StatsCounter<Lifetime<T, Print>>;

public:
    // Alias for backward compatibility: Lifetime<int>::Stats::DefaultConstructor
    using Stats = LifetimeStats;
    // Bring all enumerators into class scope: Lifetime<int>::DefaultConstructor
    using enum LifetimeStats;

    Lifetime() noexcept : value_()
    {
        Base::increment_stats(
            {DefaultConstructor, ObjectCount, ObjectTotalCount});
        if constexpr (Print) {
            print(std::source_location::current(), this);
        }
    }
    Lifetime(T value) noexcept : value_(value) // NOLINT (explicit-conversion)
    {
        Base::increment_stats({Constructor, ObjectCount, ObjectTotalCount});
        if constexpr (Print) {
            print(std::source_location::current(), this);
        }
    }
    Lifetime(Lifetime const& rhs) noexcept : value_(rhs.value_)
    {
        Base::increment_stats({CopyConstructor, ObjectCount, ObjectTotalCount});
        if constexpr (Print) {
            print(std::source_location::current(), this, &rhs);
        }
    }
    Lifetime(Lifetime&& rhs) noexcept : value_(std::move(rhs.value_))
    {
        Base::increment_stats({MoveConstructor, ObjectCount, ObjectTotalCount});
        if constexpr (Print) {
            print(std::source_location::current(), this, &rhs);
        }
    }
    ~Lifetime() noexcept
    {
        Base::increment_stat(Destructor);
        Base::decrement_stat(ObjectCount);
        if constexpr (Print) {
            print(std::source_location::current(), this);
        }
    }
    Lifetime& operator=(Lifetime const& rhs) noexcept // NOLINT (don't care
                                                      // about self assignment)
    {
        value_ = rhs.value_;
        Base::increment_stat(CopyAssignment);
        if constexpr (Print) {
            print(std::source_location::current(), this, &rhs);
        }
        return *this;
    }
    Lifetime& operator=(
        Lifetime&& rhs) noexcept // NOLINT (don't care about self assignment)
    {
        value_ = std::move(rhs.value_);
        Base::increment_stat(MoveAssignment);
        if constexpr (Print) {
            print(std::source_location::current(), this, &rhs);
        }
        return *this;
    }

    [[nodiscard]] T value() const { return value_; }
    void swap(Lifetime& rhs) noexcept
    {
        Base::increment_stat(MemberSwap);
        using std::swap;
        swap(value_, rhs.value_);
    }
    friend void swap(Lifetime& lhs, Lifetime& rhs) noexcept
    {
        lhs.increment_stat(LifetimeStats::NonMemberSwap);
        lhs.swap(rhs);
    }
    friend bool operator==(Lifetime const& lhs, Lifetime const& rhs)
    {
        return lhs.value_ == rhs.value_;
    }
    friend bool operator!=(Lifetime const& lhs, Lifetime const& rhs)
    {
        return !(lhs == rhs);
    }
    friend std::ostream& operator<<(std::ostream& out, Lifetime const& l)
    {
        out << l.value_;
        return out;
    }

private:
    T value_;

    void print(std::source_location const& loc, Lifetime const* self = nullptr,
               Lifetime const* other = nullptr) const
    {
        std::cout << loc.function_name();
        if (self != nullptr) {
            std::cout << "\n\t self=" << self->value_ << " @ " << self;
        }
        if (other != nullptr) {
            std::cout << "\n\t  rhs=" << other->value_ << " @ " << other;
        }
        std::cout << '\n';
    }
};
} // namespace utils::testing
