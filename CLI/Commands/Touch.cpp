#include "Touch.h"
#include <fstream>
#include <iostream>
using namespace std;

string Touch::execute(Invocation &inv)
{
    // Check for options
    if (inv.options && !inv.options->empty())
    {
        return "Error - touch does not take options";
    }
    // Check input redirection
    if (inv.in && inv.in != &cin)
    {
        return "Error - touch does not take input redirection";
    }
    // Check output redirection
    if (inv.out && inv.out != &cout)
    {
        return "Error - touch does not take output redirection";
    }
    // Check arguments
    if (!inv.args || inv.args->empty())
    {
        return "Error - missing argument: filename";
    }
    for (const auto &arg : *inv.args)
    {
        if (arg.empty() || arg[0] != '*')
        {
            return "Error - invalid argument for touch: " + arg;
        }
    }
    // Process each argument
    for (const auto &arg : *inv.args)
    {
        string filename = arg.substr(1);
        if (filename.empty())
        {
            return "Error - invalid argument for touch: " + arg;
        }
        ifstream f(filename);
        if (f.good())
        {
            return "Error - file already exists: " + filename;
        }
        ofstream file(filename, ios::app);
        if (!file)
        {
            return "Error - cannot create file: " + filename;
        }
    }
    return "";
}