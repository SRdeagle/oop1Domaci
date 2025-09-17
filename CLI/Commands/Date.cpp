#include "Date.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

string Date::execute(Invocation &inv)
{
    // Komanda date ne prima argumente ni opcije
    if (inv.args && !inv.args->empty())
        return "Error - date does not take arguments";
    if (inv.options && !inv.options->empty())
        return "Error - date does not take options";

    // Uzmi trenutno vreme iz system_clock
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    // Pretvori u lokalno vreme
    tm *local_tm = std::localtime(&now_time);
    if (!local_tm)
        return "Error - cannot get local date";

    // Format: YYYY-MM-DD
    (*inv.out) << put_time(local_tm, "%Y-%m-%d");
    inv.out->flush();

    return "";
}