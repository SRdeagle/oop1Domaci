#include "Head.h"
#include <sstream>
#include <fstream>
using namespace std;
// provera da li je prosledjena tacno jedna opcija
string Head::execute(Invocation &inv)
{
    if ((*inv.options).size() != 1)
    {
        return "Error: Exactly one option required.";
    }

    const string &opt = (*inv.options)[0];
    // opcija mora poceti sa -n
    if (opt.substr(0, 1) != "n")
    {
        return "Error: Option must start with '-n' followed by a number.";
    }

    // parsiranje broja linija iz opcije
    int count = 0;
    try
    {
        count = stoi(opt.substr(1));
    }
    catch (...)
    {
        return "Error: Invalid number for -n option.";
    }

    // broj linija mora biti veci od nule
    if (count <= 0)
    {
        return "Error: Number of lines must be greater than zero.";
    }

    // odabir izvora ulaza
    istream *input = nullptr;
    ifstream fileStream;
    istringstream stringStream;

    if ((*inv.args).size() == 0)
    {
        input = inv.in;
    }
    else if ((*inv.args).size() == 1)
    {
        const string &arg = (*inv.args)[0];
        // ako je argument fajl otvori ga
        if (!arg.empty() && arg[0] == '*')
        {
            string filename = arg.substr(1);
            fileStream.open(filename);
            if (!fileStream.is_open())
            {
                return "Error: Cannot open file '" + filename + "'.";
            }
            input = &fileStream;
        }
        // ako je argument string pretvori u stream
        else
        {
            string processed = arg;
            size_t pos = 0;
            while ((pos = processed.find("\\n", pos)) != string::npos)
            {
                processed.replace(pos, 2, "\n");
                pos += 1;
            }
            stringStream.str(processed);
            input = &stringStream;
        }
    }
    // ako je previse argumenata prijavi gresku
    else
    {
        return "Error: Too many arguments.";
    }

    string line;
    int lineCount = 0;
    // ako se cita sa tastature cuvaj u buffer i ispisi
    if (input == &cin)
    {
        ostringstream buffer;
        while (getline(*input, line))
        {
            if (lineCount < count)
            {
                buffer << line << "\n";
                lineCount++;
            }
        }
        *inv.out << buffer.str();
        inv.out->flush();
        cin.clear();
    }
    // ako se cita iz fajla ili stringa ispisi redom
    else
    {
        while (lineCount < count && getline(*input, line))
        {
            *inv.out << line << "\n";
            lineCount++;
        }
        inv.out->flush();
    }

    return "";
}
