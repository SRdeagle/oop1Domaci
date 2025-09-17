#pragma once
#include <string>
#include "Structures.h"
#include <vector>
using namespace std;

class CLIParser
{
public:
  vector<ParsedCommand> parseCommands(const string &input);
  string getError() { return error; };
  void resetError() { error = ""; };

private:
  static bool isLetter(char c);
  static string error;
};
