#include "int128.h"

#include <array>

Int128 Int128::ONE = 1;
Int128 Int128::ZERO = 0;
Int128 Int128::M_ONE = -1;
Int128 Int128::TEN = 10;

Int128 operator*(const Int128 & left, const Int128 & right)
{
    Int128 ans = left;
    return ans *= right;
}
Int128::Int128(std::string_view value)
{
    Int128 multiplier = ONE;
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
            multiplier *= TEN;
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
    bool sign = m_data[127] != right.m_data[127];
    if (m_data[127]) {
        m_data.flip();
        (*this) += ONE;
    }
    if (right.m_data[127]) {
        right.m_data.flip();
        right += ONE;
    }

    std::array<std::uint64_t, 2> a_64;
    a_64[0] = get_64bit(0);
    a_64[1] = get_64bit(1);
    std::array<std::uint64_t, 2> b_64;
    b_64[0] = right.get_64bit(0);
    b_64[1] = right.get_64bit(1);

    uint64_t a32 = a_64[0] >> 32;
    uint64_t a00 = a_64[0] & 0xffffffff;
    uint64_t b32 = b_64[0] >> 32;
    uint64_t b00 = b_64[0] & 0xffffffff;
    (*this) = std::bitset<128>(a_64[1] * b_64[0] + a_64[0] * b_64[1] + a32 * b32);
    m_data <<= 64;
    (*this) += std::bitset<128>(a00 * b00);

    Int128 carry = std::bitset<128>(a32 * b00);
    carry.m_data <<= 32;
    (*this) += carry;
    carry = std::bitset<128>(a00 * b32);
    carry.m_data <<= 32;
    (*this) += carry;

    if (m_data[127] != sign) {
        m_data.flip();
        (*this) += ONE;
    }

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
    Int128 ans = a;
    return ans += b;
}

Int128 operator-(const Int128 & a)
{
    Int128 ans = a;
    ans.m_data.flip();
    return ans += Int128::ONE;
}

Int128 operator-(const Int128 & a, const Int128 & b)
{
    return a + (-b);
}
Int128 & Int128::operator+=(const Int128 & b)
{
    bool carry = false;
    for (std::size_t i = 0; i < m_data.size(); ++i) {
        bool new_val = static_cast<bool>(m_data[i] ^ b.m_data[i]) ^ carry;
        if (m_data[i] == b.m_data[i]) {
            carry = m_data[i];
        }
        m_data[i] = new_val;
    }
    return *this;
}
Int128 & Int128::operator-=(const Int128 & a)
{
    return operator+=(-a);
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
        left = -left;
    }
    if (right.m_data[127]) {
        right = -right;
    }

    if (right == left) {
        return is_div ? (static_cast<int>(sign) ? Int128::M_ONE : Int128::ONE) : Int128::ZERO;
    }
    if (left.less_unsigned(right)) {
        return is_div ? Int128::ZERO : left;
    }

    size_t left_significant_bits = left.count_sig_bits();
    std::size_t right_significant_bits = right.count_sig_bits();

    std::size_t delta_bits = left_significant_bits - right_significant_bits;

    right.m_data <<= delta_bits;

    Int128 ans = Int128::ZERO;

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
        ans += Int128::ONE;
    }

    return is_div ? ans : left;
}
size_t Int128::count_sig_bits() const
{
    std::size_t ans = 0;
    for (std::size_t i = 128; i > 0; --i) {
        if (m_data[i - 1]) {
            ans = i;
            break;
        }
    }
    return ans;
}
std::string Int128::str() const
{
    Int128 temp = *this;
    bool sign = temp < ZERO;
    std::string result;
    if (temp == ZERO) {
        result = "0";
        return result;
    }
    while (temp != ZERO) {
        auto x = static_cast<std::int64_t>(temp % TEN);
        result += static_cast<char>('0' + x);
        temp /= TEN;
    }
    if (sign) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}
