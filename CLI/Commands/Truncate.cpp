#include "Truncate.h"
#include <fstream>

string Truncate::execute(Invocation &inv)
{
    // Truncate ne sme imati opcije
    if (inv.options && !inv.options->empty())
        return "Error - truncate does not take options";

    // Mora postojati tačno jedan argument
    if (!inv.args || inv.args->size() != 1)
        return "Error - missing or too many arguments for truncate";

    string arg = (*inv.args)[0];

    // Argument mora počinjati sa '*'
    if (arg.empty() || arg[0] != '*')
        return "Error - invalid argument for truncate: " + arg;

    string filename = arg.substr(1);
    ofstream file(filename, ios::trunc); // otvara fajl i briše sadržaj
    if (!file.is_open())
        return "Error - cannot open file: " + filename;

    file.close();
    return "";
}