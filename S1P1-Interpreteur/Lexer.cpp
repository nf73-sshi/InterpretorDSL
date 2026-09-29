#include "Lexer.h"
#include <sstream>
#include <iostream>
#include <regex>

void Lexer::Tokenize(const char* file)
{
	std::string_view f(file);
	std::string_view line;
	std::string_view elt; //element

	while (!f.empty())
	{
		size_t line_pos = f.find("\n");

		if (line_pos == std::string::npos)
		{
			line = f.substr(0, f.size() - 1);
			f = {};
		}
		else
		{
			line = f.substr(0, line_pos);
			f = f.substr(line_pos + 1, f.size() - line_pos - 1); //Keep the text after the first ;
		}

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

			std::cout << elt << " "; //TODO treat the element
		}

		std::cout << "\n";
	}
}
