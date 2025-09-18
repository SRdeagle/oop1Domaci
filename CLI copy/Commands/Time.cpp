#include "Time.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

string Time::execute(Invocation &inv)
{
    // komanda time ne sme da ima argumente ili opcije
    if (inv.args && !inv.args->empty())
        return "Error - time does not take arguments";
    if (inv.options && !inv.options->empty())
        return "Error - time does not take options";

    // dohvatanje trenutnog vremena
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    // konverzija vremena u lokalno
    tm *local_tm = std::localtime(&now_time);
    if (!local_tm)
        return "Error - cannot get local time";

    // ispis vremena u formatu hh mm ss
    (*inv.out) << put_time(local_tm, "%H:%M:%S");
    inv.out->flush();
    if (inv.out == &cout)
        cout << endl;
    return "";
}