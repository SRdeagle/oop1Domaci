#include "Date.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

string Date::execute(Invocation &inv)
{
    // provera da li su prosledjeni argumenti ili opcije sto nije dozvoljeno
    if (inv.args && !inv.args->empty())
        return "Error - date does not take arguments";
    if (inv.options && !inv.options->empty())
        return "Error - date does not take options";

    // dohvatanje trenutnog vremena iz system clock
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    // pretvaranje vremena u lokalno
    tm *local_tm = std::localtime(&now_time);
    if (!local_tm)
        return "Error - cannot get local date";

    // formatiranje i ispis datuma u obliku godina mesec dan
    (*inv.out) << put_time(local_tm, "%Y-%m-%d");
    inv.out->flush();
    if (inv.out == &cout)
        cout << endl;

    return "";
}