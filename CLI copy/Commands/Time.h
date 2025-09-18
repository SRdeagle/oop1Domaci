#pragma once
#include "Command.h"
#include "../Structures.h"
using namespace std;
class Time : public Command
{
public:
    string execute(Invocation &inv) override;
};