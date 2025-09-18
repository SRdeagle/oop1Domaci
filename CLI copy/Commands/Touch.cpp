#include "Touch.h"
#include <fstream>
#include <iostream>
using namespace std;

string Touch::execute(Invocation &inv)
{
    // touch ne sme da ima opcije
    if (inv.options && !inv.options->empty())
    {
        return "Error - touch does not take options";
    }
    // ulazna redirekcija nije dozvoljena
    if (inv.in && inv.in != &cin)
    {
        return "Error - touch does not take input redirection";
    }
    // izlazna redirekcija nije dozvoljena
    if (inv.out && inv.out != &cout)
    {
        return "Error - touch does not take output redirection";
    }
    // mora postojati bar jedan argument
    if (!inv.args || inv.args->empty())
    {
        return "Error - missing argument: filename";
    }
    // svaki argument mora poceti sa *
    for (const auto &arg : *inv.args)
    {
        if (arg.empty() || arg[0] != '*')
        {
            return "Error - invalid argument for touch: " + arg;
        }
    }
    // za svaki argument proveri da li fajl postoji ili ga napravi
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