#pragma once

#include "int128.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>

struct Expression
{
    virtual Int128 eval(std::map<std::string, Int128> const & values = {}) const = 0;

    virtual Expression * clone() const = 0;

    virtual std::string str() const = 0;

    friend std::ostream & operator<<(std::ostream & out, const Expression & expression);

    virtual ~Expression() = default;
};

class Const : public Expression
{
private:
    const Int128 m_value;

public:
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;
    std::string str() const override;
    Const(const Int128 & m_value);
};

class Variable : public Expression
{
private:
    const std::string m_value;

public:
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;
    std::string str() const override;
    Variable(const std::string & m_value);
};

class BinaryExpression : public Expression
{
protected:
    const std::shared_ptr<Expression> m_left;
    const std::shared_ptr<Expression> m_right;

public:
    BinaryExpression(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right);
    BinaryExpression(Expression * m_left, Expression * m_right);
    std::string str() const override;

protected:
    virtual std::string op_symbol() const = 0;
};

class Add : public BinaryExpression
{
public:
    Add(const Expression & m_left, const Expression & m_right);
    Add(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & mRight);
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;

protected:
    std::string op_symbol() const override;
};

class Subtract : public BinaryExpression
{
public:
    Subtract(const Expression & m_left, const Expression & m_right);
    Subtract(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & mRight);
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;

protected:
    std::string op_symbol() const override;
};

class Divide : public BinaryExpression
{
public:
    Divide(const Expression & m_left, const Expression & m_right);
    Divide(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & mRight);
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;

protected:
    std::string op_symbol() const override;
};

class Multiply : public BinaryExpression
{
public:
    Multiply(const Expression & m_left, const Expression & m_right);
    Multiply(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & mRight);
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;

protected:
    std::string op_symbol() const override;
};

class Negate : public Expression
{
private:
    const std::shared_ptr<Expression> m_value;

public:
    Int128 eval(const std::map<std::string, Int128> & values = {}) const override;
    Expression * clone() const override;
    std::string str() const override;
    Negate(const std::shared_ptr<Expression> & m_value);
    Negate(const Expression & m_value);
};
Multiply operator*(const Expression & a, const Expression & b);
Negate operator-(const Expression & value);
Divide operator/(const Expression & a, const Expression & b);
Subtract operator-(const Expression & a, const Expression & b);
Add operator+(const Expression & a, const Expression & b);
