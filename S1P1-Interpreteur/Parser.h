	#pragma once
#include <string>
#include <vector>
#include "Lexer.h"

enum NodeType
{
	NODE_NONE,
	EXPR,
	ASSIGN,
	PARAM,
	TROOP_NODE,
	BLOCK,
	STMT
};
enum StmtType
{
	STMT_NONE,
	STMT_IF,
	STMT_ELSE
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

struct Stmt : public Node
{
	Token id;
	
	StmtType stmt_type;
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

struct Block : public Node
{
	Token id;
	std::vector<Expr*> exprList;
	Block(Token _id, std::vector<Expr*> _exprList)
	{
		node_type = BLOCK;
		id = _id;
		for (Expr* _expr : _exprList)
		{
			exprList.push_back(_expr);
		}
	}
};

struct Param : public Node
{
	Token id;
	Expr* right;
	Param(Token _id ,Expr* _right)
	{
		node_type = PARAM;
		id = _id;
		right = _right;
	}
};

struct Troop : public Node
{
	Token id;
	std::vector<Expr*> expList;
	Troop(Token _id, std::vector<Param> _expList)
	{
		node_type = TROOP_NODE;
		id = _id;
		for (Param p : _expList)
		{
			expList.push_back(p.right);
		}
	}
};
struct ElseStmt : public Stmt
{
	Block* block;

	ElseStmt(Token _id, Block* _block)
	{
		node_type = STMT;
		stmt_type = STMT_ELSE;
		id = _id;
		block = _block;
	}
};
struct IfStmt : public Stmt
{	
	Expr* parametrer;
	Block* block;

	IfStmt(Token _id, Expr* _parametrer, Block* _block)
	{
		node_type = STMT;
		stmt_type = STMT_IF;
		id = _id;
		parametrer = _parametrer;
		block = _block;
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
