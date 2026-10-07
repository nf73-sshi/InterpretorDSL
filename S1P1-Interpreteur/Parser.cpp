#include "Parser.h"
#include "Debug.h"
#include "Environment.h"

#include <variant>
#include <unordered_set>

void Parser::Parse()
{
    while (current < tokens.size())
    {
        statements.push_back(std::move(ParseStmt()));
    }

    for (std::unique_ptr<Stmt>& stmt : statements)
    {
       // stmt->Print();
    }
}

std::unique_ptr<Stmt> Parser::ParseStmt()
{
    std::unique_ptr<Stmt> stmt = nullptr;
    Token* current = Peek();

    switch (current->type)
    {
    case TokenType::COMMENTARY:
        ParseCommentary();
        current = Peek();
        stmt = ParseStmt();
        break;

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
        else
        {
            stmt = ParseExprStmt();
        }
        break;
    }

    if (stmt == nullptr)
        Debug::SyntaxError(current, "INVALID STATEMENT");

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseExprStmt()
{
    return std::make_unique<ExpressionStmt>(ParseExpr()); //Handle calls with no assign;
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

void Parser::ParseCommentary()
{
    Advance(); // //
    while (Peek()->type != TokenType::COMMENTARY)
    {
        Advance();
    }

    Advance();
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

std::unique_ptr<Expr> Parser::ParseCall()
{
    Token* call_t = Advance(); //identifier
    std::string call_name = call_t->GetLexeme();
    std::unique_ptr<Call> call = std::make_unique<Call>(Call::ID::FUNC_NONE, call_name);
    Advance(); // (

    size_t params_count = 0;
    size_t needed_params = 0;

    while (true)
    {
        if (Peek()->type == TokenType::RPARENTHESIS)
        {
            Advance();
            break;
        }

        call->params.push_back(std::move(ParseExpr()));
        
        if (Peek()->type == TokenType::COMMA)
            Advance();

        params_count++;
    }

    bool handle_error = false;

    if (call_name == "GetAliveTroops")
    {
        call->id = Call::ID::GET_ALIVE_TROOPS;
        call->return_type = Type::TYPE_FLOAT;
        needed_params = 0;
    }
    else if (call_name == "GetAliveTroopsInTeam")
    {
        call->id = Call::ID::GET_ALIVE_TROOPS_IN_TEAM;
        call->return_type = Type::TYPE_FLOAT;
        needed_params = 1; //team id
    }
    else if (call_name == "Spawn")
    {
        call->id = Call::ID::SPAWN;
        call->return_type = Type::TYPE_VOID;
        needed_params = 4; //troop, x, y, team_id
    }
    else if (call_name == "SpawnMultiple")
    {
        call->id = Call::ID::SPAWN_MULTIPLE;
        call->return_type = Type::TYPE_VOID;
        needed_params = 5; //troop, x, y, count, team_id
    }
    else if (call_name == "IsBattling")
    {
        call->id = Call::ID::IS_BATTLING;
        call->return_type = Type::TYPE_FLOAT;
        needed_params = 0;
    }
    else if (call_name == "GetWinnerTeam")
    {
        call->id = Call::ID::GET_WINNER_TEAM;
        call->return_type = Type::TYPE_FLOAT;
        needed_params = 0;
    }
    else if (call_name == "GetElapsedTime")
    {
        call->id = Call::ID::GET_ELAPSED_TIME;
        call->return_type = Type::TYPE_FLOAT;
        needed_params = 0;
    }
    else if (call_name == "Update")
    {
        call->id = Call::ID::UPDATE;
        call->return_type = Type::TYPE_VOID;
        needed_params = 0; //scale
    }
    else // Function not found !
        Debug::SyntaxError(call_t, "THIS FUNCTION DOESN'T EXISTS");

    if (params_count != needed_params && needed_params != -1)
        handle_error = true;

    if (handle_error)
    {
        std::string message = "THIS FUNCTION NEEDS " + std::to_string(needed_params) + " PARAMETERS";
        Debug::SyntaxError(call_t, message.c_str());
    }

    return call;
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
        return std::make_unique<Literal>(1.0f);

    case TokenType::FALSE:
        Advance();
        return std::make_unique<Literal>(0.f);

    case TokenType::IDENTIFIER:
        if (Next() != nullptr && (Next()->type == TokenType::LPARENTHESIS))
        {
            return ParseCall();
        }
        else
        {
            Token* previous = Previous();
            Type type = TYPE_FLOAT;

            if (previous != nullptr)
            {
                // If Val, it's float too

                if (previous->type == TokenType::TROOP) 
                {
                    type = TYPE_TROOP;
                }
            }

            Advance();
            return std::make_unique<Variable>(current->GetLexeme(), type);
        }

    case TokenType::SUB:
    case TokenType::NOT:
        Advance();
        std::unique_ptr<Expr> right = ParseExpr(BindingPower(current));

        return std::make_unique<Unary>(current->GetLexeme(), std::move(right));
    }

    return nullptr;
}

//Exe + Evals
void ExpressionStmt::Execute(Environment* e)
{
    expr->Eval(e);
}

void AssignStmt::Execute(Environment* e)
{
    if (e->Exists(name))
        e->Write(name, expr->Eval(e));
    else
        Debug::FatalError(name, " DOESN'T EXISTS");
}

void ParamStmt::Execute(Environment* e) //Only used in Troop so no need to do anything here
{
    return;
}

void TroopStmt::Execute(Environment* e)
{
    Troop t = {};
    std::unordered_set<int> params_setted; //size need to be 8 at the end

    for (auto& stmt : params)
    {
        ParamStmt* param = static_cast<ParamStmt*>(stmt.get());
        std::string name = param->name;

        float value = std::get<float>(param->expr->Eval(e));

        if (name == "cost")
        {
            t.cost = value;
            params_setted.insert(0);
        }
        else if (name == "hp")
        {
            t.hp = value;
            params_setted.insert(1);
        }
        else if (name == "radius")
        {
            t.radius = value;
            params_setted.insert(2);
        }
        else if (name == "dmg")
        {
            t.dmg = value;
            params_setted.insert(3);
        }
        else if (name == "fatkspeed")
        {
            t.fatkspeed = value;
            params_setted.insert(4);
        }
        else if (name == "atkspeed")
        {
            t.atkspeed = value;
            params_setted.insert(5);
        }
        else if (name == "speed")
        {
            t.speed = value;
            params_setted.insert(6);
        }
        else if (name == "range")
        {
            t.range = value;
            params_setted.insert(7);
        }
        else
            Debug::FatalError("TROOP ERROR, INVALID PARAMETER : ", name);
    }

    if (params_setted.size() != 8)
    {
        Debug::FatalError("TROOP ERROR, ALL PARAMETERS NEED TO BE SETTED");
    }

    e->Add(var_name, t);
}

void BlockStmt::Execute(Environment* e)
{
    e->AddBlock();
    for (auto& stmt : statements)
    {
        stmt->Execute(e);
    }
    e->AddBlock(true);
}

void IfStmt::Execute(Environment* e)
{
    if(std::get<float>(condition->Eval(e)) != 0.0f) //float != 0.0f
    {
        then->Execute(e);
    }
    else
    {
        if (else_branch == nullptr)
            return;

        else_branch->Execute(e);
    }
}

void WhileStmt::Execute(Environment* e)
{
    float c = std::get<float>(condition->Eval(e));

    while(c != 0.0f) //float != 0.0f
    {
        then->Execute(e);
        c = std::get<float>(condition->Eval(e));
    }
}

void ForStmt::Execute(Environment* e)
{
    int i = 0;
    std::string var_name = "";
    init->Execute(e);

    if (init->type == StmtType::VAL_STMT)
    {
        ValStmt* val = static_cast<ValStmt*>(init.get());
        i = (int)std::get<float>(val->expr->Eval(e));
        var_name = val->name;
    }
    else if (init->type == StmtType::ASSIGN_STMT)
    {
        AssignStmt* assign = static_cast<AssignStmt*>(init.get());
        assign->expr->Eval(e);
        i = (int)std::get<float>(assign->expr->Eval(e));
        var_name = assign->name;
    }

    const int amount = (int)std::get<float>(n->Eval(e));

    for (; i < amount;)
    {
        then->Execute(e);

        i++;
        e->Write(var_name, Object{ float(i) });
    }
}

void ValStmt::Execute(Environment* e)
{
    e->Add(name, expr->Eval(e));
}

Object Call::Eval(Environment* e)
{
    Object return_val = std::monostate();

    switch (id)
    {
    case Call::ID::GET_ALIVE_TROOPS:
        return_val = e->GetAliveTroops();
        break;

    case Call::ID::GET_ALIVE_TROOPS_IN_TEAM:
        return_val = e->GetAliveTroopsInTeam(std::get<float>(params[0]->Eval(e)));
        break;

    case Call::ID::GET_ELAPSED_TIME:
        return_val = e->GetElapsedTime();
        break;

    case Call::ID::SPAWN:
        e->Spawn(std::get<Troop>(params[0]->Eval(e)),
            std::get<float>(params[1]->Eval(e)),
            std::get<float>(params[2]->Eval(e)),
            std::get<float>(params[3]->Eval(e)));
        break;

    case Call::ID::SPAWN_MULTIPLE:
        e->SpawnMultiple(std::get<Troop>(params[0]->Eval(e)),
            std::get<float>(params[1]->Eval(e)),
            std::get<float>(params[2]->Eval(e)),
            std::get<float>(params[3]->Eval(e)),
            std::get<float>(params[4]->Eval(e)));
        break;

    case Call::ID::IS_BATTLING:
        return_val = e->IsBattling();
        break;

    case Call::ID::GET_WINNER_TEAM:
        return_val = e->GetWinnerTeam();
        break;

    case Call::ID::UPDATE:
        e->Update();
        break;

    default:
        Debug::FatalError("INVALID FUNCTION DETECTED");
        break;
    }

    return Object{return_val};
}

Object Literal::Eval(Environment* e)
{
    return Object{ value };
}

Object Variable::Eval(Environment* e)
{
    return e->Read(name);
}

Object Grouping::Eval(Environment* e)
{
    return expr->Eval(e);
}

Object Unary::Eval(Environment* e)
{
    Object a = right->Eval(e);
    float fa = 0;

    fa = std::get<float>(a);

    if (symbol == "!")
    {
        fa = !fa;
    }
    else if (symbol == "-")
    {
        fa = -fa;
    }

    return Object{fa};
}

Object Binary::Eval(Environment* e)
{
    Object a = left->Eval(e);
    Object b = right->Eval(e);

    float fa = std::get<float>(a);
    float fb = std::get<float>(b);

    if (op == "==")
        return Object{ float(fa == fb) };
    else if(op == "!=")
        return Object{ float(fa != fb) };

    else if (op == "<=")
        return Object{ float(fa <= fb) };

    else if (op == ">=")
        return Object{ float(fa >= fb) };

    else if (op == "<")
        return Object{ float(fa < fb) };

    else if (op == ">")
        return Object{ float(fa > fb) };

    else if (op == "+")
        return Object{ float(fa + fb) };

    else if (op == "-")
        return Object{ float(fa - fb) };

    else if (op == "*")
        return Object{ float(fa * fb) };

    else if (op == "/")
        return Object{ float(fa / fb) };

    Debug::FatalError("OPERATOR COULDNT BE EVALUATED");
    return Object{};
}