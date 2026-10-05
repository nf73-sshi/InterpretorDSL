#pragma once
#include <string>
#include <vector>
#include "Lexer.h"
#include <iostream>

#include "Types.h"
struct Environment;

enum StmtType
{
	EXPR_STMT,
	ASSIGN_STMT,
	BLOCK_STMT,

	IF_STMT,
	WHILE_STMT,
	FOR_STMT,

	PARAM_STMT,

	TROOP_STMT,
	VAL_STMT,
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

	virtual void Execute(Environment* e) = 0;
};

enum ExprType
{
	EXPR_NONE,

	CALL,
	LITERAL,
	VARIABLE,

	GROUPING,
	UNARY,
	BINARY
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

	virtual Object Eval(Environment* e) = 0;
};

struct AssignStmt : public Stmt
{
	std::string name = "";
	std::unique_ptr<Expr> expr = nullptr;

	AssignStmt(std::string _name, std::unique_ptr<Expr> _expr) : Stmt(ASSIGN_STMT)
	{
		name = _name; expr = std::move(_expr);
	}

	void Print() override
	{
		std::cout << "ASSIGNEMENT | " << name << " = ";
		expr->Print();
		std::cout << "\n";
	}

	void Execute(Environment* e) override;
};

struct ParamStmt : public Stmt
{
	std::string name = "";
	std::unique_ptr<Expr> expr;

	ParamStmt(std::string _name , std::unique_ptr<Expr> _expr) : Stmt(PARAM_STMT)
	{
		name = _name;
		expr = std::move(_expr);
	}

	void Print() override
	{
		std::cout << "PARAM | " << name << ": ";
		expr->Print();
		std::cout << "\n";
	}

	void Execute(Environment* e) override;
};

struct TroopStmt : public Stmt
{
	std::string var_name = "";
	std::vector<std::unique_ptr<Stmt>> params;

	TroopStmt(std::string _var_name) : Stmt(TROOP_STMT)
	{
		var_name = _var_name;
	}

	void Print() override
	{
		std::cout << "TROOP | " << var_name << " = ";
		for (std::unique_ptr<Stmt>& p : params)
		{
			p->Print();
		}
		std::cout << "\n";
	}

	void Execute(Environment* e) override;
};

struct BlockStmt : public Stmt
{
	std::vector<std::unique_ptr<Stmt>> statements;

	BlockStmt() : Stmt(BLOCK_STMT) {}

	void Print() override
	{
		std::cout << "BLOCK |\n{\n";
		for (std::unique_ptr<Stmt>& s : statements)
		{
			s->Print();
		}
		std::cout << "}\n";
	}

	void Execute(Environment* e) override;
};

struct IfStmt : public Stmt
{	
	std::unique_ptr<Expr> condition = nullptr;
	std::unique_ptr<Stmt> then = nullptr;

	std::unique_ptr<Stmt> else_branch = nullptr;

	IfStmt(std::unique_ptr<Expr> _condition, std::unique_ptr<Stmt> _then, std::unique_ptr<Stmt> _else_branch) : Stmt(IF_STMT)
	{
		condition = std::move(_condition);
		then = std::move(_then);
		else_branch = std::move(_else_branch);
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

	void Execute(Environment* e) override;
};

struct WhileStmt : public Stmt
{
	std::unique_ptr<Expr> condition = nullptr;
	std::unique_ptr<Stmt> then = nullptr;

	WhileStmt(std::unique_ptr<Expr> _condition, std::unique_ptr<Stmt> _then) : Stmt(WHILE_STMT)
	{
		condition = std::move(_condition);
		then = std::move(_then);
	}

	void Print() override
	{
		std::cout << "WHILE | ";
		condition->Print();
		std::cout << "\n";
		then->Print();
	}

	void Execute(Environment* e) override;
};

struct ForStmt : public Stmt
{
	std::unique_ptr<Stmt> init = nullptr; //Assign -> i = 0
	std::unique_ptr<Expr> n = nullptr; //for(i = 0 ,i < n, ++i) | increment i until n
	std::unique_ptr<Stmt> then = nullptr;

	ForStmt(std::unique_ptr<Stmt> _init, std::unique_ptr<Expr> _n, std::unique_ptr<Stmt> _then) : Stmt(FOR_STMT)
	{
		init = std::move(_init);
		n = std::move(_n);
		then = std::move(_then);
	}

	void Print() override
	{
		std::cout << "FOR | ";
		init->Print();
		std::cout << ", "; n->Print();
		std::cout << "\n"; then->Print();
		std::cout << "\n";
	}

	void Execute(Environment* e) override;
};

struct ValStmt : public Stmt
{
	std::string name = "";
	std::unique_ptr<Expr> expr = nullptr;

	ValStmt(std::string _name, std::unique_ptr<Expr> _expr) : Stmt(VAL_STMT)
	{
		name = _name;
		expr = std::move(_expr);
	}

	void Print() override
	{
		std::cout << "VAL | " << name << " = ";
		expr->Print();
		std::cout << "\n";
	}

	void Execute(Environment* e) override;
};

struct ExpressionStmt : public Stmt
{
	std::unique_ptr<Expr> expr = nullptr;

	ExpressionStmt(std::unique_ptr<Expr> _expr) : Stmt(EXPR_STMT)
	{
		expr = std::move(_expr);
	}

	void Print() override
	{
		expr->Print();
	}

	void Execute(Environment* e) override;
};

struct Call : public Expr
{
	enum ID
	{
		FUNC_NONE,
		SPAWN,
		SPAWN_MULTIPLE,
		GET_ALIVE_TROOPS,
		GET_ALIVE_TROOPS_IN_TEAM,
		GET_ELAPSED_TIME,
		TIME_SCALE,
	};

	ID id = FUNC_NONE;
	std::string name = "";
	std::vector<std::unique_ptr<Expr>> params;

	Call(Call::ID _id, std::string _name) : Expr(CALL)
	{
		id = _id;
		name = _name;
	}

	void Print() override
	{
		std::cout << "FUNCTION | " << name << "(";
		for (std::unique_ptr<Expr>& stmt : params)
		{
			stmt->Print();
		}
		std::cout << ")\n";
	}

	Object Eval(Environment* e) override;
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

	Object Eval(Environment* e) override;
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

	Object Eval(Environment* e) override;
};

struct Grouping : public Expr
{
	std::unique_ptr<Expr> expr = nullptr;

	Grouping(std::unique_ptr<Expr> _expr) : Expr(GROUPING)
	{
		expr = std::move(_expr);
	}

	void Print() override
	{
		std::cout << "(";
		expr->Print();
		std::cout << ")";
	}

	Object Eval(Environment* e) override;
};

struct Unary : public Expr
{
	std::string symbol = "";
	std::unique_ptr<Expr> right = nullptr;

	Unary(std::string _symbol, std::unique_ptr<Expr> _right) : Expr(UNARY)
	{
		symbol = _symbol;
		right = std::move(_right);
	}

	void Print() override
	{
		std::cout << symbol;
		right->Print();
	}

	Object Eval(Environment* e) override;
};

struct Binary : public Expr
{
	std::unique_ptr<Expr> left = nullptr;
	std::string op = "";
	std::unique_ptr<Expr> right = nullptr;

	Binary(std::unique_ptr<Expr> _left, std::string _op, std::unique_ptr<Expr> _right) : Expr(BINARY)
	{
		left = std::move(_left); op = _op; right = std::move(_right);
	}

	void Print() override
	{
		left->Print();
		std::cout << " " << op << " ";
		right->Print();
	}

	Object Eval(Environment* e) override;
};

struct Parser
{
	int current = 0;

	std::vector<Token> tokens = {};
	std::vector<std::unique_ptr<Stmt>> statements = {};

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
	std::unique_ptr<Stmt> ParseStmt();
	std::unique_ptr<Stmt> ParseExprStmt();
	std::unique_ptr<Stmt> ParseAssignement();
	std::unique_ptr<Stmt> ParseParam();

	std::unique_ptr<Stmt> ParseVal();
	std::unique_ptr<Stmt> ParseTroop();
	std::unique_ptr<Stmt> ParseBlock();
	std::unique_ptr<Stmt> ParseIf();
	std::unique_ptr<Stmt> ParseWhile();
	std::unique_ptr<Stmt> ParseFor();

	std::unique_ptr<Expr> ParseCall();
	std::unique_ptr<Expr> ParseExpr(int min_binding_power = 1);

	int BindingPower(Token* op);
	std::unique_ptr<Expr> Prefix();
};
