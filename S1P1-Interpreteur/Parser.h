#pragma once
#include <string>
#include <vector>
#include "Lexer.h"

enum NodeType
{
	NODE_NONE,

	EXPR,
	ASSIGN,
};

enum ExprType
{
	EXPR_NONE, 

	LITERAL,
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
		node_type = ASSIGN;
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

struct Unary : public Expr
{
	char symbol = ' ';
	Expr* right = nullptr;

	Unary(char _symbol, Expr* _right)
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
	Token peek() { return m_tokenList[m_actual]; }
	Token previous() { return m_tokenList[m_actual - 1]; }
	bool check(TokenType type) { return peek().type; }
};
