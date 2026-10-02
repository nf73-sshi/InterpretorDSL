#pragma once
#include <string>
#include <vector>
#include "Lexer.h"

enum NodeType
{
	NODE_NONE,

	EXPR,
	ASSIGNEMENT,
};

enum ExprType
{
	EXPR_NONE, 

	LITERAL,
	VARIABLE,

	UNARY,
	BINARY,
};

struct Node
{
	NodeType node_type = NODE_NONE;
};

struct Assign : public Node
{
	Token id; //identifier
	Token op;
	Expr* expr;

	Assign(Token _id, Token _op, Expr* _expr)
	{
		node_type = ASSIGNEMENT;
		id = _id; op = _op; expr = _expr;
	}
};

struct Expr : public Node
{
	ExprType type = EXPR_NONE;

	Expr()
	{
		node_type = EXPR;
	}
};

struct Literal : public Expr
{
	Token token;

	Literal(Token _token)
	{
		type = LITERAL;
		token = _token;
	}
};

struct Variable : public Expr
{
	Token token;

	Variable(Token _token)
	{
		type = VARIABLE;
		token = _token;
	}
};

struct Unary : public Expr
{
	const char* symbol = "";
	Expr* right = nullptr;

	Unary(const char* _symbol, Expr* _right)
	{
		type = UNARY;
		right = _right;
	}
};

struct Binary : public Expr
{
	Expr* left = nullptr;
	Token op;
	Expr* right = nullptr;

	Binary(Expr* _left, Token _op, Expr* _right)
	{
		type = BINARY;
		left = _left; op = _op; right = _right;
	}
};

struct Parser
{
	int m_actual = 0;
	std::vector<Token> m_tokenList;
public:
	Parser(std::vector<Token> tokenList) { m_tokenList = tokenList; };
	Token& peek() { return m_tokenList[m_actual]; }
	
	Token& advance()
	{
		if (m_actual >= m_tokenList.size())
			return peek();

		else
			return m_tokenList[++m_actual];
	}

	Token& previous()
	{
		if (m_actual == 0)
			return m_tokenList[0];

		return m_tokenList[m_actual - 1];
	}

	Token& next()
	{
		if (m_actual >= m_tokenList.size())
			return m_tokenList.back();

		return m_tokenList[m_actual + 1];
	}

	TokenType check() { return peek().type; }

	void Parse();

	Assign* parse_assign();
	Expr* parse_expr(int min_binding_power = 1);

private:
	int binding_power(Token op);
	Expr* prefix();
};
