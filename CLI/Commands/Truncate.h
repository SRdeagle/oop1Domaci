#pragma once
#include "Command.h"
#include "../Structures.h"
using namespace std;

class Truncate : public Command
{
public:
    string execute(Invocation &inv) override;
};