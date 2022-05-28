#include "int128.h"

#include <array>

Int128 operator*(Int128 left, Int128 right)
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
    bool sign = left.m_data[127] != right.m_data[127];
    if (left.m_data[127]) {
        left.m_data.flip();
        left += 1;
    }
    if (right.m_data[127]) {
        right.m_data.flip();
        right += 1;
    }

    std::array<std::uint64_t, 2> a_64;
    a_64[0] = left.get_64bit(0);
    a_64[1] = left.get_64bit(1);
    std::array<std::uint64_t, 2> b_64;
    b_64[0] = right.get_64bit(0);
    b_64[1] = right.get_64bit(1);

    uint64_t a32 = a_64[0] >> 32;
    uint64_t a00 = a_64[0] & 0xffffffff;
    uint64_t b32 = b_64[0] >> 32;
    uint64_t b00 = b_64[0] & 0xffffffff;
    Int128 result = std::bitset<128>(a_64[1] * b_64[0] + a_64[0] * b_64[1] + a32 * b32);
    result.m_data <<= 64;
    result += std::bitset<128>(a00 * b00);

    Int128 carry = std::bitset<128>(a32 * b00);
    carry.m_data <<= 32;
    result += carry;
    carry = std::bitset<128>(a00 * b32);
    carry.m_data <<= 32;
    result += carry;

    if (result.m_data[127] != sign) {
        result.m_data.flip();
        result += 1;
    }

    return result;
}
Int128::Int128(std::string_view value)
{
    Int128 multiplier = 1;
    for (auto i = value.rbegin(); i != value.rend(); ++i) {
        unsigned char ch = *i;
        if (isspace(ch)) {
            continue;
        }
        if (ch == '-') {
            m_data.flip();
            (*this) += 1;
            break;
        }
        if (isdigit(ch)) {
            *this += (ch - '0') * multiplier;
            multiplier *= 10;
        }
        else {
            break;
        }
    }
}
Int128 & Int128::operator*=(const Int128 & a)
{
    m_data = operator*(*this, a).m_data;
    return *this;
}

Int128::Int128(const std::bitset<128> & mData)
    : m_data(mData)
{
}

std::ostream & operator<<(std::ostream & os, const Int128 & value)
{
    return os << value.str();
}
Int128::Int128(std::int64_t value)
    : m_data(value < 0 ? -value : value)
{
    if (value < 0) {
        m_data = operator-(*this).m_data;
    }
}

Int128 operator+(const Int128 & a, const Int128 & b)
{
    Int128 ans;
    bool carry = false;
    for (std::size_t i = 0; i < a.m_data.size(); ++i) {
        ans.m_data[i] = static_cast<bool>(a.m_data[i] ^ b.m_data[i]) ^ carry;
        if (a.m_data[i] == b.m_data[i]) {
            carry = a.m_data[i];
        }
    }
    return ans;
}

Int128 operator-(const Int128 & a)
{
    Int128 ans = a;
    ans.m_data.flip();
    return ans + Int128(std::bitset<128>(1));
}

Int128 operator-(const Int128 & a, const Int128 & b)
{
    return a + (-b);
}
Int128 & Int128::operator+=(const Int128 & a)
{
    m_data = operator+(*this, a).m_data;
    return *this;
}
Int128 & Int128::operator-=(const Int128 & a)
{
    m_data = operator-(*this, a).m_data;
    return *this;
}
Int128::operator std::int64_t() const
{
    return (m_data & std::bitset<128>(0xFFFFFFFFFFFFFFFF)).to_ullong();
}
Int128::operator double() const
{
    double multiplier = 1;
    double ans = 0;
    for (std::size_t i = 0; i < m_data.size() - 1; ++i) {
        ans += (m_data[i] ^ m_data[m_data.size() - 1]) * multiplier;
        multiplier *= 2;
    }
    ans *= (static_cast<int>(m_data[m_data.size() - 1]) * (-2)) + 1;
    ans -= static_cast<int>(m_data[m_data.size() - 1]);
    return ans;
}
bool operator==(const Int128 & lhs, const Int128 & rhs)
{
    return lhs.m_data == rhs.m_data;
}
bool operator!=(const Int128 & lhs, const Int128 & rhs)
{
    return !(rhs == lhs);
}
uint64_t Int128::get_64bit(int pos) const
{
    return ((m_data >> (pos * 64)) & std::bitset<128>(0xFFFFFFFFFFFFFFFF)).to_ullong();
}
Int128 operator/(const Int128 & left, const Int128 & right)
{
    return div_mod(left, right, true);
}
Int128 operator%(const Int128 & left, const Int128 & right)
{
    return div_mod(left, right, false);
}
bool operator<(const Int128 & lhs, const Int128 & rhs)
{
    if (rhs.m_data[127] && !lhs.m_data[127]) {
        return false;
    }
    if (!rhs.m_data[127] && lhs.m_data[127]) {
        return true;
    }
    bool reverse = rhs.m_data[127];
    for (std::size_t i = 127; i > 0; --i) {
        if (rhs.m_data[i - 1] && !(lhs.m_data[i - 1])) {
            return !reverse;
        }
        if (!rhs.m_data[i - 1] && (lhs.m_data[i - 1])) {
            return reverse;
        }
    }
    return false;
}
bool Int128::less_unsigned(const Int128 & value) const
{
    for (std::size_t i = 128; i > 0; --i) {
        if (value.m_data[i - 1] && !(m_data[i - 1])) {
            return true;
        }
        if (!value.m_data[i - 1] && (m_data[i - 1])) {
            return false;
        }
    }
    return false;
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
    m_data = operator/(*this, a).m_data;
    return *this;
}
Int128 div_mod(Int128 left, Int128 right, bool is_div)
{
    bool sign = left.m_data[127] != right.m_data[127];
    if (left.m_data[127]) {
        left.m_data.flip();
        left += 1;
    }
    if (right.m_data[127]) {
        right.m_data.flip();
        right += 1;
    }

    if (right == left) {
        return is_div ? static_cast<int>(sign) * (-2) + 1 : 0;
    }
    if (left.less_unsigned(right)) {
        return is_div ? 0 : left;
    }

    std::size_t left_signigicant_bits = 0;
    for (std::size_t i = 128; i > 0; --i) {
        if (left.m_data[i - 1]) {
            left_signigicant_bits = i;
            break;
        }
    }
    std::size_t right_signigicant_bits = 0;
    for (std::size_t i = 128; i > 0; --i) {
        if (right.m_data[i - 1]) {
            right_signigicant_bits = i;
            break;
        }
    }

    std::size_t delta_bits = left_signigicant_bits - right_signigicant_bits;

    right.m_data <<= delta_bits;

    Int128 ans = 0;

    for (std::size_t i = 0; i <= delta_bits; ++i) {
        ans.m_data <<= 1;
        if (!left.less_unsigned(right)) {
            left -= right;
            ans.m_data[0] = true;
        }
        right.m_data >>= 1;
    }

    if (ans.m_data[127] != sign) {
        ans.m_data.flip();
        ans += 1;
    }

    return is_div ? ans : left;
}
std::string Int128::str() const
{
    Int128 temp = *this;
    bool sign = temp < 0;
    std::string result;
    if (temp == 0) {
        result = "0";
        return result;
    }
    while (temp != 0) {
        auto x = static_cast<std::int64_t>(temp % 10);
        result += static_cast<char>('0' + x);
        temp /= 10;
    }
    if (sign) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}
