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
  bool handleQuotation(char c, State &state, string &buffer, ParsedCommand &cmd, int i, string &error);
  bool handleDash(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error);
  bool HandleRest(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error);
  bool finalizeCommand(State &state, string &buffer, ParsedCommand &cmd, string &error);
  bool validatePipeline(vector<ParsedCommand> &cmds, string &error);
};
