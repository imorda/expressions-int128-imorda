#include "int128.h"

#include <array>

namespace int128_constants {
Int128 ONE = 1;
Int128 ZERO = 0;
Int128 M_ONE = -1;
Int128 TEN = 10;
} // namespace int128_constants

Int128 operator*(const Int128 & left, const Int128 & right)
{
    Int128 ans = left;
    return ans *= right;
}
Int128::Int128(std::string_view value)
{
    Int128 multiplier = int128_constants::ONE;
    for (auto i = value.rbegin(); i != value.rend(); ++i) {
        unsigned char ch = *i;
        if (isspace(ch)) {
            continue;
        }
        if (ch == '-') {
            (*this) = -(*this);
            break;
        }
        if (isdigit(ch)) {
            *this += (ch - '0') * multiplier;
            multiplier *= int128_constants::TEN;
        }
        else {
            break;
        }
    }
}
Int128 & Int128::operator*=(Int128 right)
{
    /*
     *  a4    a3    a2    a1
     *  b4    b3    b2    b1
     *  -----------------------
     *  b1*a4 b1*a3 b1*a2 b1*a1
     *  b2*a3 b2*a2 b2*a1
     *  b3*a2 b3*a1
     *  b4*a1
     */
    bool sign = ((*this) < 0) != (right < 0);
    if ((*this) < 0) {
        (*this) = -(*this);
    }
    if (right < 0) {
        right = -right;
    }

    std::array<std::uint64_t, 2> a_64;
    a_64[0] = get_64bit(0);
    a_64[1] = get_64bit(1);
    std::array<std::uint64_t, 2> b_64;
    b_64[0] = right.get_64bit(0);
    b_64[1] = right.get_64bit(1);
    (*this) = 0;

    uint64_t a32 = a_64[0] >> 32;
    uint64_t a00 = a_64[0] & 0xffffffff;
    uint64_t b32 = b_64[0] >> 32;
    uint64_t b00 = b_64[0] & 0xffffffff;
    m_low = a_64[1] * b_64[0] + a_64[0] * b_64[1] + a32 * b32;
    (*this) <<= 64;
    m_low = a00 * b00;

    Int128 carry;
    carry.m_low = a32 * b00;
    carry <<= 32;
    (*this) += carry;
    carry = 0;
    carry.m_low = a00 * b32;
    carry <<= 32;
    (*this) += carry;

    if (((*this) < 0) != sign) {
        (*this) = -(*this);
    }

    return *this;
}

std::ostream & operator<<(std::ostream & os, const Int128 & value)
{
    return os << value.str();
}

Int128::Int128(std::int64_t value)
    : m_high(value < 0 ? -1 : 0)
    , m_low(value)
{
}

Int128 operator+(const Int128 & a, const Int128 & b)
{
    Int128 ans = a;
    return ans += b;
}

Int128 operator-(const Int128 & a)
{
    Int128 ans = a;
    ans.m_low ^= UINT64_MAX;
    ans.m_high ^= -1;

    return ans += int128_constants::ONE;
}

Int128 operator-(const Int128 & a, const Int128 & b)
{
    return a + (-b);
}
Int128 & Int128::operator+=(const Int128 & b)
{
    std::uint64_t new_low = m_low + b.m_low;

    if (new_low < m_low) {
        m_high = static_cast<std::uint64_t>(m_high) + 1;
    }
    m_low = new_low;
    m_high = static_cast<std::uint64_t>(b.m_high) + static_cast<std::uint64_t>(m_high);

    return *this;
}
Int128 & Int128::operator-=(const Int128 & a)
{
    return operator+=(-a);
}
Int128::operator std::int64_t() const
{
    return m_low;
}
Int128::operator double() const
{
    double ans = 0;

    ans += m_high;
    if (m_high < 0) {
        ans *= -1;
    }

    ans *= static_cast<double>(UINT64_MAX);
    ans += m_low ^ (UINT64_MAX * static_cast<int>(m_high < 0));

    if (m_high < 0) {
        ans *= -1;
    }
    return ans;
}
bool operator==(const Int128 & lhs, const Int128 & rhs)
{
    return lhs.m_high == rhs.m_high && lhs.m_low == rhs.m_low;
}
bool operator!=(const Int128 & lhs, const Int128 & rhs)
{
    return !(rhs == lhs);
}
std::uint64_t Int128::get_64bit(int pos) const
{
    if (pos == 0) {
        return m_low;
    }
    else {
        return m_high;
    }
}
Int128 operator/(const Int128 & left, const Int128 & right)
{
    return div_mod(left, right).first;
}
Int128 operator%(const Int128 & left, const Int128 & right)
{
    return div_mod(left, right).second;
}
bool operator<(const Int128 & lhs, const Int128 & rhs)
{
    if (lhs.m_high == rhs.m_high) {
        return lhs.m_low < rhs.m_low;
    }
    return lhs.m_high < rhs.m_high;
}
bool Int128::less_unsigned(const Int128 & value) const
{
    std::uint64_t u_l_high = m_high;
    std::uint64_t u_r_high = value.m_high;
    if (u_l_high == u_r_high) {
        return m_low < value.m_low;
    }
    return u_l_high < u_r_high;
}
bool operator>(const Int128 & lhs, const Int128 & rhs)
{
    return rhs < lhs;
}
bool operator<=(const Int128 & lhs, const Int128 & rhs)
{
    return !(rhs < lhs);
}
bool operator>=(const Int128 & lhs, const Int128 & rhs)
{
    return !(lhs < rhs);
}
Int128 & Int128::operator/=(const Int128 & a)
{
    (*this) = operator/(*this, a);
    return *this;
}
Int128 operator<<(const Int128 & lhs, std::size_t rhs)
{
    Int128 ans = lhs;
    return ans <<= rhs;
}
Int128 operator>>(const Int128 & lhs, std::size_t rhs)
{
    Int128 ans = lhs;
    return ans >>= rhs;
}
std::pair<Int128, Int128> div_mod(Int128 left, Int128 right)
{
    bool sign = (left < 0) != (right < 0);
    if (left < 0) {
        left = -left;
    }
    if (right < 0) {
        right = -right;
    }

    if (right == left) {
        return {(static_cast<int>(sign) ? int128_constants::M_ONE : int128_constants::ONE), int128_constants::ZERO};
    }
    if (left.less_unsigned(right)) {
        return {int128_constants::ZERO, left};
    }

    std::size_t left_significant_bits = Int128::find_msb(left);
    std::size_t right_significant_bits = Int128::find_msb(right);

    std::size_t delta_bits = left_significant_bits - right_significant_bits;

    right <<= delta_bits;

    Int128 ans = int128_constants::ZERO;

    for (std::size_t i = 0; i <= delta_bits; ++i) {
        ans <<= 1;
        if (!left.less_unsigned(right)) {
            left -= right;
            ans.m_low |= 1;
        }
        right >>= 1;
    }

    if ((ans < 0) != sign) {
        ans = -ans;
    }

    return {ans, left};
}
std::size_t Int128::find_msb(Int128 value)
{
    if (value < 0) {
        return 127;
    }
    std::size_t ans = 0;
    if (value.m_high) {
        ans += 64;
        while (value.m_high >>= 1) {
            ans++;
        }
    }
    else {
        while (value.m_low >>= 1) {
            ans++;
        }
    }

    return ans;
}
std::string Int128::str() const
{
    Int128 temp = *this;
    bool sign = temp < int128_constants::ZERO;
    std::string result;
    if (temp == int128_constants::ZERO) {
        return "0";
    }
    while (temp != int128_constants::ZERO) {
        auto new_val = div_mod(temp, int128_constants::TEN);
        result += static_cast<char>('0' + static_cast<std::int64_t>(new_val.second));
        temp = new_val.first;
    }
    if (sign) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}
Int128 & Int128::operator<<=(std::size_t rhs)
{
    if (rhs == 0) {
        return *this;
    }
    if (rhs >= 64) {
        m_high = m_low << (rhs - 64);
        m_low = 0;
    }
    else {
        m_high = static_cast<std::uint64_t>(m_high) << rhs;
        m_high |= m_low >> (64 - rhs);
        m_low <<= rhs;
    }
    return *this;
}
Int128 & Int128::operator>>=(std::size_t rhs)
{
    if (rhs == 0) {
        return *this;
    }
    if (rhs >= 64) {
        m_low = static_cast<std::uint64_t>(m_high) >> (rhs - 64);
        m_high = 0;
    }
    else {
        m_low >>= rhs;
        m_low |= static_cast<std::uint64_t>(m_high) << (64 - rhs);
        m_high = static_cast<std::uint64_t>(m_high) >> rhs;
    }
    return *this;
}
Int128::Int128()
    : Int128(0)
{
}
