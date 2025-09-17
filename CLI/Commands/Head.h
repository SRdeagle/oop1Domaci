#pragma once
#include "Command.h"
#include <string>
class Head : public Command
{
public:
    std::string execute(Invocation &inv) override;
};