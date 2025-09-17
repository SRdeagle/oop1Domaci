#include "Interpreter.h"
#include "CLIParser.h"
#include <iostream>
#include <vector>
using namespace std;
string Interpreter::prompt = "$";
string Interpreter::executeCommand(Invocation &inv)
{
    if (commandMap.find(inv.command) != commandMap.end())
    {
        return (commandMap[inv.command])->execute(inv);
    }
    else
        return "Command not found";
}

void output(const vector<ParsedCommand> &parsed)
{
    for (int i = 0; i < parsed.size(); i++)
        cout << parsed[i].command << " option: " << parsed[i].options[0] << " argument:" << parsed[i].args[0] << "  i: " << parsed.size() << endl;
}

void Interpreter::activate()
{
    isActive = true;
    string input, err = "";
    vector<ParsedCommand> parsed;
    while (isActive)
    {
        cout << prompt;
        if (!getline(cin, input) || input == "exit")
            break;
        parsed = parser.parseCommands(input);
        if (parser.getError() != "")
        {
            cout << parser.getError() << endl;
            continue;
        }
        printParsedCommands(parsed);
        vector<Invocation> invs(parsed.size());
        cout << "ulazimo sad u sh\n";
        streamHandler.connectPipeline(invs, parsed);
        printInvocations(invs);
        if (streamHandler.getError() != "")
        {
            cout << streamHandler.getError() << endl;
            continue;
        }
        for (int i = 0; i < invs.size(); i++)
        {
            err = executeCommand(invs[i]);
            if (err != "")
            {
                cout << err << endl;
                break;
            }
        }
        parser.resetError();
        streamHandler.resetError();
    }
    isActive = false;
}

void Interpreter::addCommand(const string &command, unique_ptr<Command> commandPointer)
{
    commandMap[command] = move(commandPointer);
}
void Interpreter::setPrompt(string p)
{
    prompt = p;
}
