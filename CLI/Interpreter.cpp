#include "Interpreter.h"
#include "Parser.h"
#include <iostream>
using namespace std;
void Interpreter::executeCommand(const ParsedCommand  &parsed) {
    if (commandMap.find(parsed.command) != commandMap.end()) {
        (this->commandMap[parsed.command])->execute(parsed);
    }
    else
        cout << "Command not found" << endl;
}

void output (const ParsedCommand &parsed) {
    cout << parsed.command << " option: " << parsed.option << " argument:" << parsed.argument<<endl;
}

void Interpreter::activate() {
    isActive = true;
    string input;
    ParsedCommand parsed;
    while (isActive) {
         cout<<"$";
         if (!getline(cin, input) || input == "exit")
             break;
        parsed = parser.parseCommand(input);
        if (!parsed.error.empty())
            cout<<parsed.error<<endl;
        else
            this->executeCommand(parsed);
        //output(parsed);
    }
    isActive = false;
}

void Interpreter::addCommand(string command, Command* commandPointer) {
    commandMap[command] = commandPointer;
}

