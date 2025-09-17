#include "StreamHandler.h"

#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

void StreamHandler::resolveInput(Invocation &inv, ParsedCommand &parsed)
{
    currentIn = &cin;
    if (!parsed.inFile.empty())
    {
        auto f = make_unique<ifstream>(parsed.inFile);
        if (!f->is_open())
        {
            StreamHandler::error = "Error: cannot open input file '" + parsed.inFile + "'\n";
            return;
        }
        in_holder = move(f);
        currentIn = in_holder.get();
    }
    // else if (!parsed.args.empty())
    // {
    //     const string &arg = parsed.args[0];
    //     if (!arg.empty())
    //     {
    //         // Quoted string: use stringstream
    //         auto ss = make_unique<stringstream>(arg.substr(1));
    //         in_holder = move(ss);
    //         currentIn = in_holder.get();
    //     }
    //     else
    //     {
    //         // Default to standard input
    //         currentIn = &cin;
    //     }
    // }
    // for (int j = 0; j < parsed.args.size(); j++)
    // {
    //     if (parsed.args[j][0] == '*')
    //     {
    //         if (parsed.inFile.empty())
    //         {
    //             auto f = make_unique<ifstream>(parsed.args[j].substr(1));
    //             if (!f->is_open())
    //             {
    //                 StreamHandler::error = "Error: cannot open input file '" + parsed.args[j] + "'\n";
    //                 return;
    //             }
    //             in_holder = move(f);
    //             currentIn = in_holder.get();
    //         }
    //         else
    //         {
    //             StreamHandler::error = "Error: multiple input sources\n";
    //             return;
    //         }
    //     }
    // }
    inv.in = currentIn;
}

void StreamHandler::resolveOutput(Invocation &inv, ParsedCommand &parsed)
{
    if (!parsed.outFile.empty())
    {
        auto f = make_unique<ofstream>(parsed.outFile, parsed.append ? ios::app : ios::out);
        if (!f->is_open())
        {
            cerr << "Error: cannot open output file '" << parsed.outFile << "'\n";
            return;
        }
        out_holder = move(f);
        currentOut = out_holder.get();
    }
    else
    {
        currentOut = &cout;
    }
    inv.out = currentOut;
}

void StreamHandler::connectPipeline(vector<Invocation> &invs,
                                    vector<ParsedCommand> &parsed)
{
    // 0) ensure enough pipes (N-1) and reset them
    // cout << "0" << endl;
    size_t n = parsed.size();
    invs[0].args = &parsed[0].args;
    invs[0].options = &parsed[0].options;
    invs[0].append = parsed[0].append;
    invs[0].command = parsed[0].command;
    // cout << "1" << endl;
    if (n == 1)
    {
        resolveInput(invs[0], parsed[0]);
        resolveOutput(invs[0], parsed[0]);
        return;
    }

    size_t need = (n > 0 ? n - 1 : 0);
    if (pipes.size() <= need)
    {
        while (pipes.size() < need)
            pipes.push_back(make_unique<stringstream>());
    }
    else
    {
        pipes.resize(need);
    }
    // cout << "2" << endl;
    for (size_t i = 0; i < need; i++)
    {
        pipes[i]->str("");
        pipes[i]->clear();
    }
    // cout << "3" << endl;
    resolveOutput(invs[n - 1], parsed[n - 1]);
    resolveInput(invs[0], parsed[0]);
    // 2) wire pipes between neighboring commands
    // cout << "4" << endl;
    for (size_t i = 0; i < need; i++)
    {
        // i-th writes to pipe[i], (i+1)-th reads from the same pipe
        invs[i].out = pipes[i].get();
        invs[i + 1].in = pipes[i].get();
        invs[i + 1].args = &parsed[i + 1].args;
        invs[i + 1].options = &parsed[i + 1].options;
        invs[i + 1].append = parsed[i + 1].append;
        invs[i + 1].command = parsed[i + 1].command;
    }
    // cout << "5" << endl;
}