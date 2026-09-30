#include <iostream>
#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <regex>
//
void tokenTest(char c)
{

	switch (c)
	{
	case('c'):
		std::cout << "CONTINUE found" << std::endl;
		break;
	case('T'):
		std::cout << "TROOP found" << std::endl;
		break;
	case('f'):
		std::cout << "FOR found" << std::endl;
		break;
	case('i'):
		std::cout << "IF found" << std::endl;
		break;
	case('b'):
		std::cout << "BREAK found" << std::endl;
		break;
	case('w'):
		std::cout << "WHILE found" << std::endl;
		break;
	case('('):
		std::cout << "LPARENTHESIS found" << std::endl;
		break;
	case(')'):
		std::cout << "RPARENTHESIS found" << std::endl;
		break;
	case('{'):
		std::cout << "LBRACKET found" << std::endl;
		break;
	case('}'):
		std::cout << "RBRACKET found" << std::endl;
		break;
	case(','):
		std::cout << "COMMA found" << std::endl;
		break;
	case(';'):
		std::cout << "SEMICOLON found" << std::endl;
		break;
	case(':'):
		std::cout << "COLON found" << std::endl;
		break;
	case('"'):
		std::cout << "QUOTE found" << std::endl;
		break;
	case('+'):
		std::cout << "ADD found" << std::endl;
		break;
	case('-'):
		std::cout << "SUB found" << std::endl;
		break;
	case('*'):
		std::cout << "MUL found" << std::endl;
		break;
	case('/'):
		std::cout << "DIV found" << std::endl;
		break;
	case('>'):
		std::cout << "SUP found" << std::endl;
		break;
	case('<'):
		std::cout << "INF found" << std::endl;
		break;
	case('='):
		std::cout << "EQUAL found" << std::endl;
		break;
	default:
		break;
	}
  
}

int main()
{
    Lexer l;
    l.Tokenize("main.p2w");

    std::string test = "({*,;+-:<>/=\"})";
	for (char c : test)
	{
		tokenTest(c);
	}
}
