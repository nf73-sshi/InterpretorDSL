#pragma once
#include <string>
#include <vector>
#include "Lexer.h"
#include <iostream>

enum StmtType
{
	ASSIGN_STMT,
	BLOCK_STMT,

	IF_STMT,

	PARAM_STMT,

	TROOP_STMT,
	VAL_STMT
};

struct Stmt
{	
	StmtType type;

	Stmt(StmtType stmt)
	{
		type = stmt;
	}
	virtual ~Stmt() = default;
	virtual void Print() = 0;
};

enum ExprType
{
	EXPR_NONE,

	LITERAL,
	VARIABLE,

	GROUPING,
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
	virtual void Print() = 0;
};

struct AssignStmt : public Stmt
{
	std::string name = "";
	Expr* expr = nullptr;

	AssignStmt(std::string _name, Expr* _expr) : Stmt(ASSIGN_STMT)
	{
		name = _name; expr = _expr;
	}

	void Print() override
	{
		std::cout << "ASSIGNEMENT | " << name << " = ";
		expr->Print();
		std::cout << "\n";
	}
};

struct ParamStmt : public Stmt
{
	std::string name = "";
	Expr* expr;

	ParamStmt(std::string _name ,Expr* _expr) : Stmt(PARAM_STMT)
	{
		name = _name;
		expr = _expr;
	}

	void Print() override
	{
		std::cout << "PARAM | " << name << ": ";
		expr->Print();
		std::cout << "\n";
	}
};

struct TroopStmt : public Stmt
{
	std::string var_name = "";
	std::vector<ParamStmt*> params;

	TroopStmt(std::string _var_name, std::vector<ParamStmt*> _params) : Stmt(TROOP_STMT)
	{
		var_name = _var_name;
		params = _params;
	}

	void Print() override
	{
		std::cout << "TROOP | " << var_name << " = ";
		for (ParamStmt* p : params)
		{
			p->Print();
		}
		std::cout << "\n";
	}
};

struct BlockStmt : public Stmt
{
	std::vector<Stmt*> statements;

	BlockStmt(std::vector<Stmt*> _s) : Stmt(BLOCK_STMT)
	{
		statements = _s;
	}

	void Print() override
	{
		std::cout << "BLOCK |\n{\n";
		for (Stmt* s : statements)
		{
			s->Print();
		}
		std::cout << "}\n";
	}
};

struct IfStmt : public Stmt
{	
	Expr* condition = nullptr;
	Stmt* then = nullptr; 

	Stmt* else_branch = nullptr;

	IfStmt(Expr* _condition, Stmt* _then, Stmt* _else_branch) : Stmt(IF_STMT)
	{
		condition = _condition;
		then = _then;
		else_branch = _else_branch;
	}

	void Print() override
	{
		std::cout << "IF | ";
		condition->Print();
		std::cout << "\n";
		then->Print();

		if (else_branch != nullptr)
		{
			std::cout << "ELSE | ";
			else_branch->Print();
		}
		std::cout << "\n";
	}
};

struct ValStmt : public Stmt
{
	std::string name = "";
	Expr* expr = nullptr;

	ValStmt(std::string _name, Expr* _expr) : Stmt(VAL_STMT)
	{
		name = _name;
		expr = _expr;
	}

	void Print() override
	{
		std::cout << "VAL | " << name << " = ";
		expr->Print();
		std::cout << "\n";
	}
};

struct Literal : public Expr
{
	float value = 0;

	Literal(float _value) : Expr(LITERAL)
	{
		value = _value;
	}

	void Print() override
	{
		std::cout << value;
	}
};

struct Variable : public Expr
{
	std::string name = "";

	Variable(std::string _name) : Expr(VARIABLE)
	{
		name = _name;
	}

	void Print() override
	{
		std::cout << name;
	}
};

struct Grouping : public Expr
{
	Expr* expr = nullptr;

	Grouping(Expr* _expr) : Expr(GROUPING)
	{
		expr = _expr;
	}

	void Print() override
	{
		std::cout << "(";
		expr->Print();
		std::cout << ")";
	}
};

struct Unary : public Expr
{
	std::string symbol = "";
	Expr* right = nullptr;

	Unary(std::string _symbol, Expr* _right) : Expr(UNARY)
	{
		symbol = _symbol;
		right = _right;
	}

	void Print() override
	{
		std::cout << symbol;
		right->Print();
	}
};

struct Binary : public Expr
{
	Expr* left = nullptr;
	std::string op = "";
	Expr* right = nullptr;

	Binary(Expr* _left, std::string _op, Expr* _right) : Expr(BINARY)
	{
		left = _left; op = _op; right = _right;
	}

	void Print() override
	{
		left->Print();
		std::cout << " " << op << " ";
		right->Print();
	}
};

struct Parser
{
	int current = 0;

	std::vector<Token> tokens = {};
	std::vector<Stmt*> statements = {};

public:
	Parser(std::vector<Token> tokenList) { tokens = tokenList; };

	bool IsAtEnd() const { return current >= tokens.size() || current < 0; }

	Token* Peek() 
	{ 
		if (IsAtEnd())
			return nullptr;
		
		return &tokens[current]; 
	}
	
	Token* Advance()
	{
		if (IsAtEnd())
			return nullptr;

		return &tokens[current++];
	}
	Token* Previous()
	{
		if (IsAtEnd())
			return nullptr;

		return &tokens[current-1];
	}
	Token* Next()
	{
		if (IsAtEnd())
			return nullptr;

		return &tokens[current+1];
	}

	void Parse();

private:
	Stmt* ParseStmt();
	Stmt* ParseAssignement();
	Stmt* ParseParam();

	Stmt* ParseVal();
	Stmt* ParseTroop();
	Stmt* ParseBlock();
	Stmt* ParseIf();

	Expr* ParseExpr(int min_binding_power = 1);

	int BindingPower(Token* op);
	Expr* Prefix();
};
