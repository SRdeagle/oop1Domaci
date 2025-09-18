#include "Truncate.h"
#include <fstream>

// truncate ne sme da ima opcije
string Truncate::execute(Invocation &inv)
{
    if (inv.options && !inv.options->empty())
        return "Error - truncate does not take options";

    // mora postojati tacno jedan argument
    if (!inv.args || inv.args->size() != 1)
        return "Error - missing or too many arguments for truncate";

    string arg = (*inv.args)[0];

    // argument mora poceti sa *
    if (arg.empty() || arg[0] != '*')
        return "Error - invalid argument for truncate: " + arg;

    string filename = arg.substr(1);
    // otvaranje fajla u trunc rezimu brise sadrzaj
    ofstream file(filename, ios::trunc);
    if (!file.is_open())
        return "Error - cannot open file: " + filename;

    file.close();
    return "";
}