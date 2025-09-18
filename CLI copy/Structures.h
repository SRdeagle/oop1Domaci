#pragma once
#include <vector>
#include <string>
#include <iostream>
using namespace std;

struct ParsedCommand
{
    vector<string> args;
    bool append = false;
    string inFile = "";
    string outFile = "";
    string command = "";
    vector<string> options;
    string error = "";
};

inline void printParsedCommands(const vector<ParsedCommand> &cmds)
{
    for (size_t i = 0; i < cmds.size(); i++)
    {
        const auto &c = cmds[i];
        cout << "Command " << i << ":\n";
        cout << "  command: " << (c.command.empty() ? "<none>" : c.command) << "\n";

        cout << "  options: ";
        if (c.options.empty())
            cout << "<none>";
        else
        {
            for (auto &o : c.options)
                cout << o << " ";
        }
        cout << "\n";

        cout << "  args: ";
        if (c.args.empty())
            cout << "<none>";
        else
        {
            for (auto &a : c.args)
                cout << a << " ";
        }
        cout << "\n";

        cout << "  inFile: " << (c.inFile.empty() ? "<none>" : c.inFile) << "\n";
        cout << "  outFile: " << (c.outFile.empty() ? "<none>" : c.outFile) << "\n";
        cout << "  append: " << (c.append ? "true" : "false") << "\n";
        cout << "  error: " << (c.error.empty() ? "<none>" : c.error) << "\n";
        cout << "------------------------------------\n";
    }
}

struct IO
{
    istream *input;
    ostream *output;
};

struct Invocation
{
    string command;
    vector<string> *args;
    vector<string> *options;
    istream *in;
    ostream *out;
    bool append;
};
enum Phase
{
    COMMAND,
    ARGUMENT,
    OPTION,
    REDIRECTION
};
struct State
{
    bool quotation = false;
    bool dash = false;
    bool inRedir = false;
    bool outRedir = false;
    Phase phase = COMMAND;
};

inline void printInvocations(const vector<Invocation> &invs)
{
    for (size_t i = 0; i < invs.size(); i++)
    {
        const auto &inv = invs[i];
        cout << "Invocation " << i << ":\n";

        cout << "  args: ";
        if ((*inv.args).empty())
            cout << "<none>";
        else
        {
            for (auto &a : *inv.args)
                cout << a << " ";
        }
        cout << "\n";

        cout << "  options: ";
        if ((*inv.options).empty())
            cout << "<none>";
        else
        {
            for (auto &o : *inv.options)
                cout << o << " ";
        }
        cout << "\n";

        // Print input stream info
        if (inv.in == &cin)
        {
            cout << "  in: cin\n";
        }
        else
        {
            cout << "  in: " << inv.in;
            if (inv.in == nullptr)
                cout << " (nullptr)";
            cout << "\n";
        }

        // Print output stream info
        if (inv.out == &cout)
        {
            cout << "  out: cout\n";
        }
        else
        {
            cout << "  out: " << inv.out;
            if (inv.out == nullptr)
                cout << " (nullptr)";
            cout << "\n";
        }

        cout << "  append: " << (inv.append ? "true" : "false") << "\n";
        cout << "------------------------------------\n";
    }
}