#pragma once
#include "Command.h"
#include "../Structures.h"
class Interpreter; // forward declaration

class Batch : public Command
{
    Interpreter &interpreter;

public:
    Batch(Interpreter &i) : interpreter(i) {}
    string execute(Invocation &inv) override;
};