#include "Lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

void Lexer::Tokenize(const char* path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "File not found !\n";
		abort();
		return;
	}

	std::string line; ::
	std::string elt; //element

	while (getline(file, line))
	{
		while (!line.empty())
		{
			size_t elt_pos = line.find(" ");

			if (elt_pos == std::string::npos)
			{
				elt = line.substr(0, line.size());
				line = {};
			}
			else
			{
				elt = line.substr(0, elt_pos);
				line = line.substr(elt_pos + 1, line.size() - elt_pos - 1);
			}

			std::cout << "[" << elt << "]"; //TODO treat the element
			HandleElement(elt.c_str());
		}

		std::cout << "\n";
	}
}

void Lexer::HandleElement(const char* element)
{

}
