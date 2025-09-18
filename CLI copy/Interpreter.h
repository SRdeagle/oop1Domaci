#pragma once
#include <unordered_map>
#include "Commands/Command.h"
#include "StreamHandler.h"
#include "CLIParser.h"
using namespace std;

class Interpreter
{
private:
  bool isActive = false;
  unordered_map<string, unique_ptr<Command>> commandMap;
  CLIParser parser;
  StreamHandler streamHandler;

  static string prompt;

public:
  void addCommand(const string &command, unique_ptr<Command> commandPointer);
  void activate();
  static void setPrompt(string p);
  string executeCommand(Invocation &inv);
};