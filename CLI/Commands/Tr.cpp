#include "Tr.h"
#include <fstream>
#include <sstream>

string Tr::execute(Invocation &inv)
{
    if (!(*inv.options).empty())
    {
        return "Error: No options allowed for tr command.";
    }

    if ((*inv.args).empty())
    {
        return "Error: Missing arguments for tr command.";
    }

    istream *input = nullptr;
    ifstream fileStream;
    string what;
    string with;

    if ((*inv.args).size() == 1)
    {
        what = (*inv.args)[0];
        input = inv.in;
        with = "";
    }
    else if ((*inv.args).size() == 2)
    {
        what = (*inv.args)[0];
        with = (*inv.args)[1];
        input = inv.in;
    }
    else if ((*inv.args).size() == 3)
    {
        string argument = (*inv.args)[0];
        what = (*inv.args)[1];
        with = (*inv.args)[2];

        if (!argument.empty() && argument[0] == '*')
        {
            string filename = argument.substr(1);
            fileStream.open(filename);
            if (!fileStream.is_open())
            {
                return "Error: Unable to open file " + filename;
            }
            input = &fileStream;
        }
        else
        {
            // Treat as quoted string, remove quotes if present
            string arg = argument;
            if (arg.size() >= 2 && arg.front() == '"' && arg.back() == '"')
            {
                arg = arg.substr(1, arg.size() - 2);
            }
            stringstream *ss = new stringstream(arg);
            input = ss;
        }
    }
    else
    {
        return "Error: Invalid number of arguments for tr command.";
    }

    // Read full content from input
    stringstream buffer;
    buffer << input->rdbuf();
    string content = buffer.str();

    // Replace all occurrences of what with with
    size_t pos = 0;
    while ((pos = content.find(what, pos)) != string::npos && !what.empty())
    {
        content.replace(pos, what.length(), with);
        pos += with.length();
    }

    *inv.out << content;
    inv.out->flush();

    // If we allocated stringstream for quoted string argument, delete it
    if ((*inv.args).size() == 3 && !((*inv.args)[0].empty() || (*inv.args)[0][0] == '*'))
    {
        delete input;
    }

    return "";
}