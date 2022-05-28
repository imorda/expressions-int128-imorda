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

#define BIN_EXPR(Name, OP)                                                                             \
    class Name : public BinaryExpression                                                               \
    {                                                                                                  \
    public:                                                                                            \
        Name(const Expression & m_left, const Expression & m_right);                                   \
        Name(const std::shared_ptr<Expression> & m_left, const std::shared_ptr<Expression> & m_right); \
        Int128 eval(const std::map<std::string, Int128> & values = {}) const override;                 \
        Expression * clone() const override;                                                           \
                                                                                                       \
    protected:                                                                                         \
        std::string op_symbol() const override;                                                        \
    };                                                                                                 \
    Name operator OP(const Expression & a, const Expression & b);
#include "bin_exprs.inl"

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
Negate operator-(const Expression & value);
