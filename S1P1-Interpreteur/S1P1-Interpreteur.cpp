#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "Lexer.h"
#include "Parser.h"

#define SFML_STATIC
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

#if _DEBUG
#pragma comment(lib, "sfml-graphics-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

#include <SFML/Window.hpp>


int main()
{
    Lexer lexer;
    lexer.Tokenize("main.p2w");

    Parser parser(lexer.tokens);
    parser.Parse();

    //sf::Window window(sf::VideoMode(800, 600), "My window");

    //// run the program as long as the window is open
    //while (window.isOpen())
    //{
    //    // check all the window's events that were triggered since the last iteration of the loop
    //    sf::Event event;
    //    while (window.pollEvent(event))
    //    {
    //        // "close requested" event: we close the window
    //        if (event.type == sf::Event::Closed)
    //            window.close();
    //    }
    //}

    //return 0;
}
