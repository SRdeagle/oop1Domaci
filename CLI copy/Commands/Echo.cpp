#include "Echo.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

string Echo::execute(Invocation &inv)
{
    // provera da li je izlazni stream ispravan
    if (!inv.out || !inv.out->good())
    {
        return "Error - output stream is not valid or already closed";
    }
    // echo ne sme da ima opcije
    if (inv.options && !inv.options->empty())
        return "Error - echo does not take options";

    // provera da li je prosledjeno previse argumenata
    if (inv.args && inv.args->size() > 1)
        return "Error - too many arguments";

    // provera da li je navedeno vise ulaza sto nije dozvoljeno
    if (inv.in != &cin && inv.args && inv.args->size())
        return "Error - multiple inputs specified for echo";

    // ako nema argumenata cita se sa ulaza
    if (!inv.args || inv.args->empty())
    {
        ostringstream buffer;
        string line;
        while (getline(*inv.in, line))
        {
            buffer << line;
            if (!inv.in->eof())
                buffer << '\n';
        }
        (*inv.out) << buffer.str();
        if (inv.out == &cout && inv.in != &cin)
        {
            cout << endl;
        }
        inv.out->flush();
        if (inv.in == &cin)
        {
            cin.clear();
        }
        return "";
    }

    // ako je prosledjen string ispisuje se direktno
    if ((*inv.args)[0][0] != '*')
    {
        (*inv.out) << (*inv.args)[0];
        if (inv.out == &cout)
        {
            cout << endl;
        }
        inv.out->flush();
        return "";
    }
    // ako je prosledjen fajl ispisi njegov sadrzaj
    else
    {
        string filename = (*inv.args)[0].substr(1);
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open())
            return "Error - cannot open file: " + filename;
        char ch;
        while (file.get(ch))
        {
            (*inv.out) << ch;
        }
        if (inv.out == &cout)
        {
            cout << endl;
        }
        inv.out->flush();
        return "";
    }
}