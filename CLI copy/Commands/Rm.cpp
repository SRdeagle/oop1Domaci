#include "Rm.h"
#include <cstdio> // za std::remove

string Rm::execute(Invocation &inv)
{
    // rm ne sme da ima opcije
    if (inv.options && !inv.options->empty())
        return "Error - rm does not take options";

    // mora postojati tacno jedan argument
    if (!inv.args || inv.args->size() != 1)
        return "Error - missing or too many arguments for rm";

    // argument mora poceti sa *
    string arg = (*inv.args)[0];

    if (arg.empty() || arg[0] != '*')
        return "Error - invalid argument for rm: " + arg;

    string filename = arg.substr(1);

    // pokusaj brisanja fajla
    if (std::remove(filename.c_str()) != 0)
        return "Error - cannot remove file: " + filename;

    return "";
}
