#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "SemanticAnalyser.h"
#include "TreeWalker.h"
#include "Environment.h"
#include "SimCalc.h"
//#define SFML_STATIC
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "advapi32.lib")
//
//#if _DEBUG
//#pragma comment(lib, "sfml-graphics-s-d.lib")
//#pragma comment(lib, "sfml-window-s-d.lib")
//#pragma comment(lib, "sfml-system-s-d.lib")
//#else
//#pragma comment(lib, "sfml-graphics-s.lib")
//#pragma comment(lib, "sfml-window-s.lib")
//#pragma comment(lib, "sfml-system-s.lib")
//#endif
//
//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/System.hpp>

int main()
{
    Lexer lexer;
    lexer.Tokenize("main.p2w");

    Parser parser(lexer.tokens);
    parser.Parse();

    SemanticAnalyser sem(parser.statements);
    sem.AnalyseStmt();

    Environment e;

    TreeWalker tree_walker(parser.statements);
    tree_walker.Execute(&e);

    std::vector<Troop*> troopList;
    Troop t =
    {
     t.hp = 50.f ,t.dmg = 1.f ,t.atkspeed = 2.F,t.fatkspeed = 0.5f,
     t.radius = 1.f
    };
    for (int i = 0; i < 8;i++)
    {
        troopList.push_back(&t);
    }
    SimCalc sim(troopList);
    int loop = 0;
    while (loop != 1)
    {
        sim.Update();
        loop++;
    }
    return 0;
}
