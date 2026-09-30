#pragma once
#define WHITE   "\033[37m" 
#define COLOR   "\033[33m"    

#define RESET "\033[39m"

#include <iostream>

struct Debug
{
	template<typename ...Args>
	static void Log(Args&& ... terms);

	template<typename ...Args>
	static void FatalError(Args&& ... terms);
};

template<typename ...Args>
inline void Debug::Log(Args&& ... terms)
{
	(std::cout << ... << terms);
}

template<typename ...Args>
inline void Debug::FatalError(Args&& ... terms)
{
	(std::cerr << ... << terms) << std::endl;
	abort();
}
