#pragma once

#include <bitset>

class Int128
{
private:
    std::bitset<128> m_data;
    friend Int128 div_mod(Int128 a, Int128 b, bool is_div);
    uint64_t get_64bit(int pos) const;
    bool less_unsigned(const Int128 & value) const;

public:
    Int128() = default;
    Int128(const std::bitset<128> & mData);
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
    friend std::ostream & operator<<(std::ostream & os, const Int128 & value);
    friend Int128 operator+(const Int128 & a, const Int128 & b);
    friend Int128 operator/(const Int128 & a, const Int128 & b);
    friend Int128 operator%(const Int128 & a, const Int128 & b);
    friend Int128 operator-(const Int128 & a);
    friend Int128 operator-(const Int128 & a, const Int128 & b);
    friend Int128 operator*(Int128 a, Int128 b);
    Int128 & operator+=(const Int128 & a);
    Int128 & operator-=(const Int128 & a);
    Int128 & operator/=(const Int128 & a);
    Int128 & operator*=(const Int128 & a);
};
