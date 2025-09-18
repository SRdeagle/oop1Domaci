#include "Batch.h"
#include "Interpreter.h"
#include "CLIParser.h"
#include "StreamHandler.h"
#include <fstream>
#include <sstream>

string Batch::execute(Invocation &inv)
{
    // provera da li su prosledjene opcije sto nije dozvoljeno
    if (inv.options && !inv.options->empty())
        return "Error - batch does not take options";

    istream *input = nullptr;
    unique_ptr<istream> owned;

    // ako je prosledjen argument koristi ga kao fajl ili string
    if (inv.args && !inv.args->empty())
    {
        string arg = (*inv.args)[0];
        if (!arg.empty() && arg[0] == '*')
        {
            auto f = make_unique<ifstream>(arg.substr(1));
            if (!f->is_open())
                return "Error - cannot open file: " + arg.substr(1);
            input = f.get();
            owned = move(f);
        }
        else
        {
            auto ss = make_unique<stringstream>();
            (*ss) << arg;
            input = ss.get();
            owned = move(ss);
        }
    }
    else
    {
        // ako nema argumenta koristi ulaz koji je vec prosledjen
        input = inv.in;
    }

    CLIParser parser;
    StreamHandler sh;
    string line;

    // citanje linija iz ulaza red po red
    while (getline(*input, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        auto parsed = parser.parseCommands(line);
        // ako parser vrati gresku ispisi je i nastavi dalje
        if (!parser.getError().empty())
        {
            (*inv.out) << parser.getError() << "\n";
            parser.resetError();
            continue;
        }

        vector<Invocation> invs(parsed.size());
        // povezivanje komandi i provera gresaka stream handlera
        sh.connectPipeline(invs, parsed);
        if (!sh.getError().empty())
        {
            (*inv.out) << sh.getError() << "\n";
            sh.resetError();
            continue;
        }

        // izvrsavanje svake komande iz niza
        for (auto &i : invs)
        {
            string err = interpreter.executeCommand(i); // koristi interpreter koji je prosledjen
            if (!err.empty())
            {
                (*inv.out) << err << "\n";
                break;
            }
        }
    }

    return "";
}