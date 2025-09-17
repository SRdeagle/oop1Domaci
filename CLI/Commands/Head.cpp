#include "Head.h"
#include <sstream>
#include <fstream>
using namespace std;
string Head::execute(Invocation &inv)
{
    // Check options count
    if ((*inv.options).size() != 1)
    {
        return "Error: Exactly one option required.";
    }

    const string &opt = (*inv.options)[0];
    if (opt.substr(0, 1) != "n")
    {
        return "Error: Option must start with '-n' followed by a number.";
    }

    // Parse count
    int count = 0;
    try
    {
        count = stoi(opt.substr(1));
    }
    catch (...)
    {
        return "Error: Invalid number for -n option.";
    }

    if (count <= 0)
    {
        return "Error: Number of lines must be greater than zero.";
    }

    istream *input = nullptr;
    ifstream fileStream;
    istringstream stringStream;

    if ((*inv.args).size() == 0)
    {
        input = inv.in;
    }
    else if ((*inv.args).size() == 1)
    {
        const string &arg = (*inv.args)[0];
        if (!arg.empty() && arg[0] == '*')
        {
            string filename = arg.substr(1);
            fileStream.open(filename);
            if (!fileStream.is_open())
            {
                return "Error: Cannot open file '" + filename + "'.";
            }
            input = &fileStream;
        }
        else
        {
            string processed = arg;
            size_t pos = 0;
            while ((pos = processed.find("\\n", pos)) != string::npos)
            {
                processed.replace(pos, 2, "\n");
                pos += 1;
            }
            stringStream.str(processed);
            input = &stringStream;
        }
    }
    else
    {
        return "Error: Too many arguments.";
    }

    string line;
    int lineCount = 0;
    while (lineCount < count && getline(*input, line))
    {
        *inv.out << line << "\n";
        lineCount++;
    }
    inv.out->flush();

    return "";
}
