#pragma once
#include <string>
#include <vector>
#include "../Structures.h"
using namespace std;

class Command
{
public:
    virtual string execute(Invocation &inv) = 0;
    virtual ~Command() = default;

private:
    vector<string> args;
    vector<string> options;
    IO io;
};
