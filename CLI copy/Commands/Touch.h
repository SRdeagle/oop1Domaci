#pragma once
#include "Command.h"
#include "../Structures.h"
using namespace std;
class Touch : public Command
{
public:
  string execute(Invocation &inv) override;
};