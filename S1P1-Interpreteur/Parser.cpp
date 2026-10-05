#include "Parser.h"
#include "Debug.h"

void Parser::Parse()
{
    while (current < tokens.size())
    {
        statements.push_back(std::move(ParseStmt()));
    }

    for (std::unique_ptr<Stmt>& stmt : statements)
    {
        //stmt->Print();
    }
}

std::unique_ptr<Stmt> Parser::ParseStmt()
{
    std::unique_ptr<Stmt> stmt = nullptr;
    Token* current = Peek();

    switch (current->type)
    {
    case TokenType::IF:
        stmt = ParseIf();
        break;
    case TokenType::WHILE:
        stmt = ParseWhile();
        break;
    case TokenType::FOR:
        stmt = ParseFor();
        break;
    case TokenType::LBRACKET:
        stmt = ParseBlock();
        break;

    case TokenType::VAL:
        if (Next()->type == TokenType::IDENTIFIER)
        {
            stmt = ParseVal();
        }
        break;

    case TokenType::TROOP:
        if (Next()->type == TokenType::IDENTIFIER)
        {
            stmt = ParseTroop();
        }
        break;

    case TokenType::IDENTIFIER:
        if (Next()->type == TokenType::ASSIGN)
        {
            stmt = ParseAssignement();
        }
        else if (Next()->type == TokenType::LPARENTHESIS)
        {
            stmt = ParseFunctionCall();
        }
        break;
    }

    if (stmt == nullptr)
        Debug::SyntaxError(current, "INVALID STATEMENT");

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseFunctionCall()
{
    Token* func_t = Advance();
    std::string func_name = func_t->GetLexeme();
    std::unique_ptr<FunctionCallStmt> func = std::make_unique<FunctionCallStmt>(FunctionCallStmt::FUNC_NONE, func_name);
    Advance();

    std::vector<std::unique_ptr<Stmt>> params;
    size_t params_count = 0;
    size_t needed_params = 0;

    while (true)
    {
        if (Peek()->type == TokenType::RPARENTHESIS)
        {
            Advance();
            break;
        }

        func->params.push_back(std::move(ParseParam()));
        params_count++;
    }

    bool handle_error = false;

    if (func_name == "GetAliveTroops")
    {
        func->id = FunctionCallStmt::GET_ALIVE_TROOPS;
        needed_params = 0; 
    }
    else if (func_name == "GetAliveTroopsInTeam")
    {
        func->id = FunctionCallStmt::GET_ALIVE_TROOPS_IN_TEAM;
        needed_params = 1; //team id
    }
    else if (func_name == "Spawn")
    {
        func->id = FunctionCallStmt::SPAWN;
        needed_params = 4; //troop, x, y, team_id
    }
    else if (func_name == "SpawnMultiple")
    {
        func->id = FunctionCallStmt::SPAWN_MULTIPLE;
        needed_params = 5; //troop, x, y, count, team_id
    }
    else if (func_name == "GetElapsedTime")
    {
        func->id = FunctionCallStmt::GET_ELAPSED_TIME;
        needed_params = 0;
    }
    else if (func_name == "TimeScale")
    {
        func->id = FunctionCallStmt::TIME_SCALE;
        needed_params = 1; //scale
    }
    else // Function not found !
        Debug::SyntaxError(func_t, "THIS FUNCTION DOESN'T EXISTS");

    if (params_count != needed_params && needed_params != -1)
        handle_error = true;

    if (handle_error)
    {
        std::string message = "THIS FUNCTION NEEDS " + std::to_string(needed_params) + " PARAMETERS";
        Debug::SyntaxError(func_t, message.c_str());
    }

    return func;
}

std::unique_ptr<Stmt> Parser::ParseAssignement()
{
    Token* current = Advance(); //identifier
    Advance();

    return std::make_unique<AssignStmt>(current->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseParam()
{
    Token* val = Advance();

    if (val->type != TokenType::IDENTIFIER)
    {
        Debug::SyntaxError(val, "IDENTIFIER EXPECTED");
        return nullptr;
    }

    Token* colon = Advance();

    if (colon->type != TokenType::COLON)
    {
        Debug::SyntaxError(colon, "COLON EXPECTED");
        return nullptr;
    }

    return std::make_unique<ParamStmt>(val->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseVal()
{
    Advance(); //Val
    Token* id = Advance(); //identifier
    Advance(); // =

    return std::make_unique<ValStmt>(id->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseTroop()
{
    Advance(); //Troop
    Token* id = Advance(); //identifier
    Token* op = Advance(); //=

    if (op == nullptr || op->type != TokenType::ASSIGN)
    {
        Debug::SyntaxError(id, "EXPECTED ASSIGNEMENT ");
        return nullptr;
    }

    std::unique_ptr<TroopStmt> troop = std::make_unique<TroopStmt>(id->GetLexeme());

    for (int i = 0; i < 8; ++i) // There are 8 params in the troop 
    {
        troop->params.push_back(std::move(ParseParam()));
    }

    return troop;
}

std::unique_ptr<Stmt> Parser::ParseBlock()
{
    Advance();
    std::unique_ptr<BlockStmt> block = std::make_unique<BlockStmt>();

    while (true)
    {
        Token* current = Peek();

        if (current->type == TokenType::RBRACKET)
        {
            Advance();
            break;
        }

        block->statements.push_back(std::move(ParseStmt()));
    }

    return block;
}

std::unique_ptr<Stmt> Parser::ParseIf()
{
    Advance(); //IF
    std::unique_ptr<Expr> condition = ParseExpr();

    std::unique_ptr<Stmt> then = ParseStmt();

    Token* else_t = Advance();

    std::unique_ptr<Stmt> else_then = nullptr;

    if (else_t != nullptr && else_t->type == TokenType::ELSE)
    {
        else_then = ParseStmt();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(then), std::move(else_then));
}

std::unique_ptr<Stmt> Parser::ParseWhile()
{
    Advance();
    std::unique_ptr<Expr> condition = ParseExpr();

    std::unique_ptr<Stmt> then = ParseStmt();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(then));
}

std::unique_ptr<Stmt> Parser::ParseFor()
{
    Token* for_t = Advance(); // for
    std::unique_ptr<Stmt> init = ParseStmt();

    if (init->type != StmtType::ASSIGN_STMT && init->type != StmtType::VAL_STMT)
    {
        Debug::SyntaxError(for_t, "ASSIGNEMENT OR INIT VAL EXPECTED");
        return nullptr;
    }

    Token* comma = Advance(); // ,
    if (comma->type != TokenType::COMMA)
    {
        Debug::SyntaxError(comma, "COMMA EXPECTED AFTER ASSIGNEMENT");
        return nullptr;
    }

    std::unique_ptr<Expr> n = ParseExpr();
    std::unique_ptr<Stmt> then = ParseStmt();

    return std::make_unique<ForStmt>(std::move(init), std::move(n), std::move(then));
}

std::unique_ptr<Expr> Parser::ParseExpr(int min_binding_power)
{
    std::unique_ptr<Expr> left = Prefix();

    while (true)
    {
        Token* op = Peek();
        int bp = BindingPower(op);          // || 1, && 2, == != 3, < <= > >= 4, + - 5, * / % 6

        if (bp < min_binding_power || bp == 0)
            break;

        Advance();
        auto rhs = ParseExpr(bp + 1);       // bp + 1 : associativité à gauche
        left = std::make_unique<Binary>(std::move(left), op->GetLexeme(), std::move(rhs));
    }

    return left;
}

int Parser::BindingPower(Token* op)
{
    if (op == nullptr)
        return 0;

    TokenType type = op->type;

    if (type == EQUAL || type == NOTEQUAL)
        return 3;

    else if (type == INFEQUAL || type == SUPEQUAL || type == INF || type == SUP)
        return 4;

    else if (type == ADD || type == SUB)
        return 5;

    else if (type == MUL || type == DIV)
        return 6;

    return 0;
}

std::unique_ptr<Expr> Parser::Prefix()
{
    Token* current = Peek();

    if (current->type == TokenType::LPARENTHESIS)
    {
       Advance(); // (
       std::unique_ptr<Expr>  expr = ParseExpr(0);

       if (Peek()->type == TokenType::RPARENTHESIS)
       {
           Advance(); // )
           return std::make_unique<Grouping>(std::move(expr));
       }

       return nullptr;
    }

    switch (current->type)
    {
    case TokenType::NUMBER:
        Advance();
        return std::make_unique<Literal>(std::stof(current->GetLexeme()));

    case TokenType::TRUE:
        Advance();
        return std::make_unique<Literal>(1);

    case TokenType::FALSE:
        Advance();
        return std::make_unique<Literal>(0);

    case TokenType::IDENTIFIER:
        Advance();
        return std::make_unique<Variable>(current->GetLexeme());

    case TokenType::SUB:
    case TokenType::NOT:
        Advance();
        std::unique_ptr<Expr> right = ParseExpr(BindingPower(current));

        return std::make_unique<Unary>(current->GetLexeme(), std::move(right));
    }

    return nullptr;
}
