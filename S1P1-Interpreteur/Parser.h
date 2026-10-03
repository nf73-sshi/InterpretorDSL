	#pragma once
#include <string>
#include <vector>
#include "Lexer.h"


enum StmtType
{
	ASSIGN_STMT,

	IF_STMT,
	ELIF_STMT,
	ELSE_STMT,

	PARAM_STMT,

	TROOP_STMT,
};

struct Stmt
{	
	StmtType type;

	Stmt(StmtType stmt)
	{
		type = stmt;
	}
	virtual ~Stmt() = default;
};

struct Assign : public Stmt
{
	const char* name = "";
	Expr* expr = nullptr;

	Assign(const char* _name, Expr* _expr) : Stmt(ASSIGN_STMT)
	{
		name = _name; expr = _expr;
	}
};

struct Param : public Stmt
{
	const char* name = "";
	Expr* right;

	Param(const char* _name ,Expr* _right) : Stmt(PARAM_STMT)
	{
		name = _name;
		right = _right;
	}
};

struct Troop : public Stmt
{
	const char* var_name = "";
	std::vector<Param*> params;

	Troop(const char* _var_name, std::vector<Param*> _params) : Stmt(TROOP_STMT)
	{
		var_name = _var_name;
		params = _params;
	}
};

struct ElseStmt : public Stmt
{
	Stmt* stmt = nullptr;

	ElseStmt(Stmt* _stmt) : Stmt(ELSE_STMT)
	{
		stmt = _stmt;
	}
};

struct ElifStmt : public Stmt
{
	Expr* condition = nullptr;
	Stmt* stmt = nullptr;

	ElifStmt(Expr* _condition, Stmt* _stmt) : Stmt(ELIF_STMT)
	{
		condition = _condition;
		stmt = _stmt;
	}
};

struct IfStmt : public Stmt
{	
	Expr* condition = nullptr;
	Stmt* stmt = nullptr; 

	IfStmt(Expr* _condition, Stmt* _stmt) : Stmt(IF_STMT)
	{
		condition = _condition;
		stmt = _stmt;
	}
};

enum ExprType
{
	EXPR_NONE,

	LITERAL,
	VARIABLE,

	UNARY,
	BINARY,
};

struct Expr
{
	ExprType type = EXPR_NONE;

	Expr(ExprType _type)
	{
		type = _type;
	}
	virtual ~Expr() = default;
};

struct Literal : public Expr
{
	float value = 0;

	Literal(float _value) : Expr(LITERAL)
	{
		value = _value;
	}
};

struct Variable : public Expr
{
	const char* name = "";

	Variable(const char* _name) : Expr(VARIABLE)
	{
		name = _name;
	}
};

struct Unary : public Expr
{
	const char* symbol = "";
	Expr* right = nullptr;

	Unary(const char* _symbol, Expr* _right) : Expr(UNARY)
	{
		right = _right;
	}
};

struct Binary : public Expr
{
	Expr* left = nullptr;
	const char* op = "";
	Expr* right = nullptr;

	Binary(Expr* _left, const char* _op, Expr* _right) : Expr(BINARY)
	{
		left = _left; op = _op; right = _right;
	}
};

struct Parser
{
	int current = 0;

	std::vector<Token> tokens = {};
	std::vector<Stmt*> statements = {};

public:
	Parser(std::vector<Token> tokenList) { tokens = tokenList; };
	Token& Peek() { return tokens[current]; }
	
	Token& Advance()
	{
		return tokens[current++];
	}
	Token& Previous()
	{
		return tokens[current-1];
	}
	Token& Next()
	{
		return tokens[current+1];
	}

	TokenType Check() { return Peek().type; }

	void Parse();

private:
	Stmt* ParseStmt();
	Expr* ParseExpr(int min_binding_power = 1);

	Stmt* ParseAssignement();

	int BindingPower(Token op);
	Expr* Prefix();
};
