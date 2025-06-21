#pragma once

#include <initializer_list>
#include <type_traits>

template <typename From, typename To>
concept convertible_to = requires(From f) { static_cast<To>(f); };

// clang-format off

template <typename Enum, typename Int = int> class QmFlags {
    static_assert(std::is_integral_v<Int>, "Int must be a integer type!");

public:
    constexpr inline QmFlags() noexcept : i_(0) {}
    constexpr inline QmFlags(Enum flags) noexcept : i_(Int(flags)) {}
    constexpr inline QmFlags(Int flags) noexcept : i_(flags) {}

    constexpr inline QmFlags(std::initializer_list<Enum> flags) noexcept
        : i_(initializer_list_helper(flags.begin(), flags.end())) {}

    constexpr static inline QmFlags fromInt(Int i) noexcept { return QmFlags(static_cast<Enum>(i)); }
    constexpr inline Int toInt() const noexcept { return i_; }

    constexpr inline QmFlags &operator&=(QmFlags mask) noexcept { i_ &= mask.i_; return *this; }
    constexpr inline QmFlags &operator&=(Enum mask) noexcept { i_ &= Int(mask); return *this; }
    constexpr inline QmFlags &operator|=(QmFlags other) noexcept { i_ |= other.i_; return *this; }
    constexpr inline QmFlags &operator|=(Enum other) noexcept { i_ |= Int(other); return *this; }
    constexpr inline QmFlags &operator^=(QmFlags other) noexcept { i_ ^= other.i_; return *this; }
    constexpr inline QmFlags &operator^=(Enum other) noexcept { i_ ^= Int(other); return *this; }

    constexpr inline explicit operator Int() const noexcept { return i_; }
    constexpr inline explicit operator bool() const noexcept { return i_; }

    constexpr inline QmFlags operator|(QmFlags other) const noexcept { return QmFlags(QFlag(i_ | other.i_)); }
    constexpr inline QmFlags operator|(Enum other) const noexcept { return QmFlags(QFlag(i_ | Int(other))); }
    constexpr inline QmFlags operator^(QmFlags other) const noexcept { return QmFlags(QFlag(i_ ^ other.i_)); }
    constexpr inline QmFlags operator^(Enum other) const noexcept { return QmFlags(QFlag(i_ ^ Int(other))); }
    constexpr inline QmFlags operator&(QmFlags other) const noexcept { return QmFlags(QFlag(i_ & other.i_)); }
    constexpr inline QmFlags operator&(Enum other) const noexcept { return QmFlags(QFlag(i_ & Int(other))); }
    constexpr inline QmFlags operator~() const noexcept { return QmFlags(QFlag(~i_)); }

    constexpr inline void operator+(QmFlags other) const noexcept = delete;
    constexpr inline void operator+(Enum other) const noexcept = delete;
    constexpr inline void operator+(int other) const noexcept = delete;
    constexpr inline void operator-(QmFlags other) const noexcept = delete;
    constexpr inline void operator-(Enum other) const noexcept = delete;
    constexpr inline void operator-(int other) const noexcept = delete;

    template<typename OtherEnum>
        requires convertible_to<OtherEnum, Enum>
    constexpr inline bool testFlag(OtherEnum flag) const noexcept { return testFlags(static_cast<Enum>(flag)); }

    constexpr inline bool testFlag(Enum flag) const noexcept { return testFlags(flag); }
    constexpr inline bool testFlags(QmFlags flags) const noexcept { return flags.i_ ? ((i_ & flags.i_) == flags.i_) : i_ == Int(0); }
    constexpr inline bool testAnyFlag(Enum flag) const noexcept { return testAnyFlags(flag); }
    constexpr inline bool testAnyFlags(QmFlags flags) const noexcept { return (i_ & flags.i_) != Int(0); }
    constexpr inline QmFlags &setFlag(Enum flag, bool on = true) noexcept
    {
        return on ? (*this |= flag) : (*this &= ~QmFlags(flag));
    }

    constexpr inline QmFlags &setFlag(Int flag, bool on = true) noexcept
    {
        return on ? (*this |= flag) : (*this &= ~QmFlags(flag));
    }

    constexpr inline QmFlags &orFlags(const QmFlags &flags) noexcept
    {
        *this |= flags;
        return *this;
    }


    friend constexpr inline bool operator==(QmFlags lhs, QmFlags rhs) noexcept
    { return lhs.i_ == rhs.i_; }
    friend constexpr inline bool operator!=(QmFlags lhs, QmFlags rhs) noexcept
    { return lhs.i_ != rhs.i_; }
    friend constexpr inline bool operator==(QmFlags lhs, Enum rhs) noexcept
    { return lhs == QmFlags(rhs); }
    friend constexpr inline bool operator!=(QmFlags lhs, Enum rhs) noexcept
    { return lhs != QmFlags(rhs); }
    friend constexpr inline bool operator==(Enum lhs, QmFlags rhs) noexcept
    { return QmFlags(lhs) == rhs; }
    friend constexpr inline bool operator!=(Enum lhs, QmFlags rhs) noexcept
    { return QmFlags(lhs) != rhs; }

private:
    constexpr static inline Int initializer_list_helper(typename std::initializer_list<Enum>::const_iterator it,
        typename std::initializer_list<Enum>::const_iterator end)
    noexcept
    {
        return (it == end ? Int(0) : (Int(*it) | initializer_list_helper(it + 1, end)));
    }

    Int i_;
};

#define QM_DECLARE_FLAGS(Flags, Enum, Int)\
typedef QmFlags<Enum, Int> Flags;

// clang-format on
