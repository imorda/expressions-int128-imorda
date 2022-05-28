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

Add::Add(const Expression & m_left, const Expression & m_right)
    : BinaryExpression(m_left.clone(), m_right.clone())
{
}
Add::Add(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right)
    : BinaryExpression(m_left, m_right)
{
}
Int128 Add::eval(const std::map<std::string, Int128> & values) const
{
    return m_left->eval(values) + m_right->eval(values);
}
Expression * Add::clone() const
{
    return new Add(m_left, m_right);
}
std::string Add::op_symbol() const
{
    return "+";
}

Subtract::Subtract(const Expression & m_left, const Expression & m_right)
    : BinaryExpression(m_left.clone(), m_right.clone())
{
}
Subtract::Subtract(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right)
    : BinaryExpression(m_left, m_right)
{
}
Int128 Subtract::eval(const std::map<std::string, Int128> & values) const
{
    return m_left->eval(values) - m_right->eval(values);
}
Expression * Subtract::clone() const
{
    return new Subtract(m_left, m_right);
}
std::string Subtract::op_symbol() const
{
    return "-";
}

Divide::Divide(const Expression & m_left, const Expression & m_right)
    : BinaryExpression(m_left.clone(), m_right.clone())
{
}
Divide::Divide(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right)
    : BinaryExpression(m_left, m_right)
{
}
Int128 Divide::eval(const std::map<std::string, Int128> & values) const
{
    return m_left->eval(values) / m_right->eval(values);
}
Expression * Divide::clone() const
{
    return new Divide(m_left, m_right);
}
std::string Divide::op_symbol() const
{
    return "/";
}

Multiply::Multiply(const Expression & m_left, const Expression & m_right)
    : BinaryExpression(m_left.clone(), m_right.clone())
{
}
Multiply::Multiply(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right)
    : BinaryExpression(m_left, m_right)
{
}
Int128 Multiply::eval(const std::map<std::string, Int128> & values) const
{
    return m_left->eval(values) * m_right->eval(values);
}
Expression * Multiply::clone() const
{
    return new Multiply(m_left, m_right);
}
std::string Multiply::op_symbol() const
{
    return "*";
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

Add operator+(const Expression & a, const Expression & b)
{
    return Add(a, b);
}
Subtract operator-(const Expression & a, const Expression & b)
{
    return Subtract(a, b);
}
Negate operator-(const Expression & val)
{
    return Negate(val);
}
Multiply operator*(const Expression & a, const Expression & b)
{
    return Multiply(a, b);
}
Divide operator/(const Expression & a, const Expression & b)
{
    return Divide(a, b);
}
