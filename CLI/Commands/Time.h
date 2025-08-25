#pragma once
#include "Command.h"
using namespace std;
class Time : public Command {
public:
    void execute(ParsedCommand cmd) override;
};