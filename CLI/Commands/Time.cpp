#include "Time.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

string Time::execute(Invocation &inv)
{
    // Komanda time ne prima argumente ni opcije
    if (inv.args && !inv.args->empty())
        return "Error - time does not take arguments";
    if (inv.options && !inv.options->empty())
        return "Error - time does not take options";

    // Uzmi trenutno vreme iz system_clock
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    // Konvertuj u lokalno vreme (nije thread-safe, ali je portable)
    tm *local_tm = std::localtime(&now_time);
    if (!local_tm)
        return "Error - cannot get local time";

    // Format: HH:MM:SS
    (*inv.out) << put_time(local_tm, "%H:%M:%S");
    inv.out->flush();

    return "";
}