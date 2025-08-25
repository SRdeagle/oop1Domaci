#pragma once
#include <string>
#include <vector>
#include "Structures.h"
using namespace std;

class Command {
public:
    virtual void execute(ParsedCommand cmd) = 0;
    virtual ~Command() = default;
};


