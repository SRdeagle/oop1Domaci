#pragma once
#include "Command.h"
using namespace std;
 class Echo : public Command {
   public:
     void execute(ParsedCommand cmd) override;
 };