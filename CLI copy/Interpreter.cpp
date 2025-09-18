#include "Interpreter.h"
#include "CLIParser.h"
#include <iostream>
#include <vector>
using namespace std;
string Interpreter::prompt = "$";

// pronalazi i izvrsava komandu
string Interpreter::executeCommand(Invocation &inv)
{
    if (commandMap.find(inv.command) != commandMap.end())
    {
        return (commandMap[inv.command])->execute(inv);
    }
    else
        return "Error: command not found";
}

// pomocna funkcija za debug ispis parsed komandi
void output(const vector<ParsedCommand> &parsed)
{
    for (int i = 0; i < parsed.size(); i++)
        cout << parsed[i].command << " option: " << parsed[i].options[0] << " argument:" << parsed[i].args[0] << "  i: " << parsed.size() << endl;
}

// glavna petlja interpretera
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
        // ako parser prijavi gresku ispisi je i nastavi
        if (parser.getError() != "")
        {
            cout << parser.getError() << endl;
            continue;
        }
        // povezivanje komandi preko stream handlera
        vector<Invocation> invs(parsed.size());
        streamHandler.connectPipeline(invs, parsed);
        // ako stream handler prijavi gresku ispisi je i nastavi
        if (streamHandler.getError() != "")
        {
            cout << streamHandler.getError() << endl;
            parser.resetError();
            streamHandler.resetError();
            continue;
        }
        // izvrsavanje komandi redom
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

// dodavanje nove komande u mapu
void Interpreter::addCommand(const string &command, unique_ptr<Command> commandPointer)
{
    commandMap[command] = move(commandPointer);
}
// podesavanje prompt teksta
void Interpreter::setPrompt(string p)
{
    prompt = p;
}
