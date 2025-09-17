#include "Rm.h"
#include <cstdio> // za std::remove

string Rm::execute(Invocation &inv)
{
    // rm ne prima opcije
    if (inv.options && !inv.options->empty())
        return "Error - rm does not take options";

    // mora postojati tačno jedan argument
    if (!inv.args || inv.args->size() != 1)
        return "Error - missing or too many arguments for rm";

    string arg = (*inv.args)[0];

    // argument mora počinjati sa '*'
    if (arg.empty() || arg[0] != '*')
        return "Error - invalid argument for rm: " + arg;

    string filename = arg.substr(1);

    // pokušaj da obrišeš fajl
    if (std::remove(filename.c_str()) != 0)
        return "Error - cannot remove file: " + filename;

    return "";
}
