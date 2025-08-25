#pragma once
#include "Command.h"
using namespace std;
class Wc : public Command {
public:
    void execute(ParsedCommand cmd) override;
};