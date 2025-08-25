#pragma once
#include <string>
#include "Structures.h"
using namespace std;

class Parser {
  public:
    ParsedCommand parseCommand(const string &input);
  private:
    static bool isValid(const string &input);
};
