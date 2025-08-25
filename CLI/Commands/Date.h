#pragma once
#include "Command.h"
using namespace std;
class Date : public Command {
public:
    void execute(ParsedCommand cmd) override;
};