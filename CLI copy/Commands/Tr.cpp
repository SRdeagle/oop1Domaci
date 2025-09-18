#include "Tr.h"
#include <fstream>
#include <sstream>

string Tr::execute(Invocation &inv)
{
    // provera da li su prosledjene opcije sto nije dozvoljeno
    if (!(*inv.options).empty())
    {
        return "Error - No options allowed for tr command.";
    }

    // provera da li su prosledjeni argumenti
    if ((*inv.args).empty())
    {
        return "Error - Missing arguments for tr command.";
    }

    istream *input = nullptr;
    ifstream fileStream;
    string what;
    string with;

    // odredjivanje nacina rada na osnovu broja argumenata
    if ((*inv.args).size() == 1)
    {
        // jedan argument znaci sta zamenjujemo a sa cim ostaje prazno
        what = (*inv.args)[0];
        input = inv.in;
        with = "";
    }
    else if ((*inv.args).size() == 2)
    {
        // dva argumenta znaci zameni prvi drugim
        what = (*inv.args)[0];
        with = (*inv.args)[1];
        input = inv.in;
    }
    else if ((*inv.args).size() == 3)
    {
        // tri argumenta znaci da je prvi ulaz a druga dva su sta i cim
        string argument = (*inv.args)[0];
        what = (*inv.args)[1];
        with = (*inv.args)[2];

        if (!argument.empty() && argument[0] == '*')
        {
            string filename = argument.substr(1);
            fileStream.open(filename);
            if (!fileStream.is_open())
            {
                return "Error - Unable to open file " + filename;
            }
            input = &fileStream;
        }
        else
        {
            // ako je prosledjen string sa navodnicima napravi stringstream
            string arg = argument;
            if (arg.size() >= 2 && arg.front() == '"' && arg.back() == '"')
            {
                arg = arg.substr(1, arg.size() - 2);
            }
            stringstream *ss = new stringstream(arg);
            input = ss;
        }
    }
    else
    {
        return "Error - Invalid number of arguments for tr command.";
    }

    // ucitavanje celog sadrzaja iz ulaza
    stringstream buffer;
    buffer << input->rdbuf();
    string content = buffer.str();

    // zamena svih pojavljivanja podstringa
    size_t pos = 0;
    while ((pos = content.find(what, pos)) != string::npos && !what.empty())
    {
        content.replace(pos, what.length(), with);
        pos += with.length();
    }

    // ispis rezultata na izlaz
    *inv.out << content;
    inv.out->flush();
    if (input != &cin && inv.out == &cout)
        cout << endl;

    // brisanje stringstream objekta ako je koriscen za quoted string
    if ((*inv.args).size() == 3 && !((*inv.args)[0].empty() || (*inv.args)[0][0] == '*'))
    {
        delete input;
    }

    return "";
}