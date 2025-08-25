#pragma once
#include <unordered_map>
#include "Command.h"
#include "Parser.h"
using namespace std;

class Interpreter {
  private:
    bool isActive = false;
    unordered_map<string, Command* > commandMap;
    Parser parser;
    void executeCommand(const ParsedCommand &parsed);
  public:
      void addCommand(string command, Command* commandPointer);
      void activate ();
};