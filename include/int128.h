#pragma once

#include <bitset>

class Int128
{
private:
    std::int64_t m_high = 0;
    std::uint64_t m_low = 0;

    friend std::pair<Int128, Int128> div_mod(Int128 left, Int128 right);
    std::uint64_t get_64bit(int pos) const;
    bool less_unsigned(const Int128 & value) const;
    static std::size_t find_msb(Int128 value);

public:
    Int128();
    Int128(std::int64_t value);
    Int128(std::string_view value);

    explicit operator std::int64_t() const;
    explicit operator double() const;
    std::string str() const;

    friend bool operator==(const Int128 & lhs, const Int128 & rhs);
    friend bool operator!=(const Int128 & lhs, const Int128 & rhs);

    friend bool operator<(const Int128 & lhs, const Int128 & rhs);
    friend bool operator>(const Int128 & lhs, const Int128 & rhs);
    friend bool operator<=(const Int128 & lhs, const Int128 & rhs);
    friend bool operator>=(const Int128 & lhs, const Int128 & rhs);

    friend Int128 operator<<(const Int128 & lhs, std::size_t rhs);
    Int128 & operator<<=(std::size_t rhs);

    friend Int128 operator>>(const Int128 & lhs, std::size_t rhs);
    Int128 & operator>>=(std::size_t rhs);

    friend std::ostream & operator<<(std::ostream & os, const Int128 & value);

    friend Int128 operator+(const Int128 & a, const Int128 & b);
    Int128 & operator+=(const Int128 & a);

    friend Int128 operator/(const Int128 & a, const Int128 & b);
    friend Int128 operator%(const Int128 & a, const Int128 & b);
    Int128 & operator/=(const Int128 & a);

    friend Int128 operator-(const Int128 & a);
    friend Int128 operator-(const Int128 & a, const Int128 & b);
    Int128 & operator-=(const Int128 & a);

    friend Int128 operator*(const Int128 & a, const Int128 & b);
    Int128 & operator*=(Int128 a);
};
