#pragma once
#include <string>
#include <vector>
#include "Lexer.h"

class function
{
public:
	virtual function() = default;
};

class Statement
{
public:
	virtual Statement() = default;
};

class VariableStatement : public Statement
{
	std::string name;
	std::vector<function> functionsList;
};

struct Parser
{
	int m_actual = 0;
	std::vector<Token> m_token_list;
public:
	Parser(std::vector<Token> token_list) { m_token_list = token_list; };
	Token peek() { return m_token_list[m_actual]; }
	Token previous() { return m_token_list[m_actual - 1]; }
	bool check(TokenType type) { return peek().type; }
};
