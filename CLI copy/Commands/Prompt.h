#pragma once
#include "Command.h"
#include "../Interpreter.h"
using namespace std;

class Prompt : public Command
{
public:
    string execute(Invocation &inv) override;
};