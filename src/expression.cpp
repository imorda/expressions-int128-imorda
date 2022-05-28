#include "expression.h"

std::ostream & operator<<(std::ostream & out, const Expression & expression)
{
    return out << expression.str();
}

Const::Const(const Int128 & m_value)
    : m_value(m_value)
{
}
Int128 Const::eval(const std::map<std::string, Int128> &) const
{
    return m_value;
}
Expression * Const::clone() const
{
    return new Const(m_value);
}
std::string Const::str() const
{
    return m_value.str();
}
Variable::Variable(const std::string & m_value)
    : m_value(m_value)
{
}
Int128 Variable::eval(const std::map<std::string, Int128> & values) const
{
    return values.at(m_value);
}
Expression * Variable::clone() const
{
    return new Variable(m_value);
}
std::string Variable::str() const
{
    return m_value;
}
BinaryExpression::BinaryExpression(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right)
    : m_left(m_left)
    , m_right(m_right)
{
}
BinaryExpression::BinaryExpression(Expression * m_left, Expression * m_right)
    : m_left(m_left)
    , m_right(m_right)
{
}
std::string BinaryExpression::str() const
{
    return "(" + m_left->str() + " " + op_symbol() + " " + m_right->str() + ")";
}

Negate::Negate(const std::shared_ptr<Expression> & m_value)
    : m_value(m_value)
{
}
Int128 Negate::eval(const std::map<std::string, Int128> & values) const
{
    return -(m_value->eval(values));
}
Expression * Negate::clone() const
{
    return new Negate(m_value);
}
std::string Negate::str() const
{
    return "(-" + m_value->str() + ")";
}
Negate::Negate(const Expression & m_value)
    : m_value(m_value.clone())
{
}
Negate operator-(const Expression & val)
{
    return Negate(val);
}
#define BIN_EXPR(Name, OP)                                                                              \
    Name::Name(const Expression & m_left, const Expression & m_right)                                   \
        : BinaryExpression(m_left.clone(), m_right.clone())                                             \
    {                                                                                                   \
    }                                                                                                   \
    Name::Name(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right) \
        : BinaryExpression(m_left, m_right)                                                             \
    {                                                                                                   \
    }                                                                                                   \
    Int128 Name::eval(const std::map<std::string, Int128> & values) const                               \
    {                                                                                                   \
        return m_left->eval(values) OP m_right->eval(values);                                           \
    }                                                                                                   \
    Expression * Name::clone() const                                                                    \
    {                                                                                                   \
        return new Name(m_left, m_right);                                                               \
    }                                                                                                   \
    std::string Name::op_symbol() const                                                                 \
    {                                                                                                   \
        return #OP;                                                                                     \
    }                                                                                                   \
    Name operator OP(const Expression & a, const Expression & b)                                        \
    {                                                                                                   \
        return Name(a, b);                                                                              \
    }
#include "bin_exprs.inl"
