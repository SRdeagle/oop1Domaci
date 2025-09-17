#pragma once
#include "Command.h"
using namespace std;
class Echo : public Command
{
public:
  string execute(Invocation &inv) override;
};