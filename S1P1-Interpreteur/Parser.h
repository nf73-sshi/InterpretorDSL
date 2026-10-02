#pragma once
#include <string>
#include <vector>
#include "Lexer.h"

class Function
{
public:
	virtual Function() = default;
};

class Statement
{
public:
	virtual Statement() = default;
};

class VariableStatement : public Statement
{
	std::string m_name;
	std::vector<Function> m_functionsList;
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
