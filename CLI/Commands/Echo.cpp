#include "Echo.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

string Echo::execute(Invocation &inv)
{
    if (!inv.out || !inv.out->good())
    {
        return "Error - output stream is not valid or already closed";
    }
    // Echo ne sme da ima opcije
    if (inv.options && !inv.options->empty())
        return "Error - echo does not take options";

    // Previše argumenata
    if (inv.args && inv.args->size() > 1)
        return "Error - too many arguments";

    // Nema argumenata → prosleđuj input na output
    if (!inv.args || inv.args->empty())
    {
        ostringstream buffer;
        string line;
        while (getline(*inv.in, line))
        {
            buffer << line << '\n';
        }
        (*inv.out) << buffer.str();
        inv.out->flush();
        if (inv.in == &cin)
        {
            cin.clear();
        }
        return "";
    }

    if ((*inv.args)[0][0] != '*')
    {
        cout << "nije * " << (*inv.args)[0] << endl;
        (*inv.out) << (*inv.args)[0];
        inv.out->flush();
        return "";
    }
    else
    {
        string filename = (*inv.args)[0].substr(1); // ukloni '*'
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open())
            return "Error - cannot open file: " + filename;
        char ch;
        while (file.get(ch))
        {
            (*inv.out) << ch;
        }
        inv.out->flush();
        return "";
    }
}