#include "Batch.h"
#include "Interpreter.h"
#include "CLIParser.h"
#include "StreamHandler.h"
#include <fstream>
#include <sstream>

string Batch::execute(Invocation &inv)
{
    if (inv.options && !inv.options->empty())
        return "Error - batch does not take options";

    istream *input = nullptr;
    unique_ptr<istream> owned;

    // Argument (opciono): fajl ili string
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
        input = inv.in; // ako nema argumenta, koristi ulazni tok
    }

    CLIParser parser;
    StreamHandler sh;
    string line;

    while (getline(*input, line))
    {
        if (line.empty())
            continue;

        auto parsed = parser.parseCommands(line);
        if (!parser.getError().empty())
        {
            (*inv.out) << parser.getError() << "\n";
            parser.resetError();
            continue;
        }

        vector<Invocation> invs(parsed.size());
        sh.connectPipeline(invs, parsed);
        if (!sh.getError().empty())
        {
            (*inv.out) << sh.getError() << "\n";
            sh.resetError();
            continue;
        }

        for (auto &i : invs)
        {
            string err = interpreter.executeCommand(i); // ✅ koristi interpreter koji si prosledio
            if (!err.empty())
            {
                (*inv.out) << err << "\n";
                break;
            }
        }
    }

    return "";
}