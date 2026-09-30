#include "Lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

void Lexer::Tokenize(const char* path)
{
	ReadFile(path);

	while(current < text.size())
	{
		char c = GetNextChar();

		switch (c)
		{

		}

		std::cout << c;
	}
}

char Lexer::GetNextChar()
{
	return text.at(current++);
}

void Lexer::ReadFile(const char* path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "File not found !\n";
		abort();
		return;
	}

	text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
}

void Lexer::HandleElement(std::string elt, size_t i)
{
	auto isDigit = [](char c) -> bool
	{
		return (c >= 48 && c <= 57);
	};

	if (elt.find("continue") != std::string::npos)
		std::cout << "CONTINUE found" << std::endl;

	else if (elt.find("Troop") != std::string::npos)
		std::cout << "TROOP found" << std::endl;

	else if (elt.find("while") != std::string::npos)
		std::cout << "WHILE found" << std::endl;

	else if (elt.find("break") != std::string::npos)
		std::cout << "BREAK found" << std::endl;

	else if (elt.find("elif") != std::string::npos)
		std::cout << "ELIF found" << std::endl;

	else if (elt.find("else") != std::string::npos)
		std::cout << "ELSE found" << std::endl;

	else if (elt.find("for") != std::string::npos)
		std::cout << "FOR found" << std::endl;

	else if (elt.find("if") != std::string::npos)
		std::cout << "IF found" << std::endl;

	else if (elt.find("==") != std::string::npos)
		std::cout << "COMPARE found" << std::endl;

	else if (elt.find(">=") != std::string::npos)
		std::cout << "SUPEQUAL found" << std::endl;

	else if (elt.find("<=") != std::string::npos)
		std::cout << "INFEQUAL found" << std::endl;

	else if (elt.find("(") != std::string::npos)
		std::cout << "LPARENTHESIS found" << std::endl;

	else if (elt.find(")") != std::string::npos)
		std::cout << "RPARENTHESIS found" << std::endl;

	else if (elt.find(",") != std::string::npos)
		std::cout << "COMMA found" << std::endl;

	else if (elt.find(";") != std::string::npos)
		std::cout << "SEMICOLON found" << std::endl;

	else if (elt.find(":") != std::string::npos)
		std::cout << "COLON found" << std::endl;

	else if (elt.find("\"") != std::string::npos)
		std::cout << "QUOTE found" << std::endl;

	else if (elt.find("+") != std::string::npos)
		std::cout << "ADD found" << std::endl;

	else if (elt.find("-") != std::string::npos)
		std::cout << "SUB found" << std::endl;

	else if (elt.find("*") != std::string::npos)
		std::cout << "MUL found" << std::endl;

	else if (elt.find("/") != std::string::npos)
		std::cout << "DIV found" << std::endl;

	else if (elt.find(">") != std::string::npos)
		std::cout << "SUP found" << std::endl;

	else if (elt.find("<") != std::string::npos)
		std::cout << "INF found" << std::endl;

	else if (elt.find("=") != std::string::npos)
		std::cout << "EQUAL found" << std::endl;



	//NUMBER
	else if (isDigit(elt[0])) 
	{
		int number_type = 1; //0 = invalid | 1 = int | 2 = float

		if (elt.back() == '.')
			number_type = 0;
		else
		{
			for (int i = 0; i < elt.size(); ++i)
			{
				if (isDigit(elt[i]) == false)
				{
					if (number_type == 2) // Multiple .
					{
						number_type = 0;
						break;
					}

					if (elt[i] == '.') // First .
						number_type = 2;
					else
					{
						number_type = 0;
						break;
					}
				}

				continue;
			}
		}
	}
}

void Lexer::AddToken(const char* lexeme, TokenType type)
{
	token_lexemes.push_back(lexeme);
	token_types.push_back(type);
	token_positions.push_back(Pos(line, col));
}
