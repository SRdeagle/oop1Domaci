#include "Wc.h"
#include <sstream>
#include <fstream>
#include <cctype>

string Wc::execute(Invocation &inv)
{
    // Mora imati opciju -w ili -c
    if (!inv.options || inv.options->empty())
        return "Error - wc requires option -w or -c";
    if (inv.options->size() != 1)
        return "Error - wc takes exactly one option";

    string opt = (*inv.options)[0];
    if (opt != "w" && opt != "c")
        return "Error - invalid option for wc: " + opt;

    // Odredi ulazni stream
    unique_ptr<istream> ownedInput;
    istream *input = nullptr;
    if (inv.args && !inv.args->empty())
    {
        string arg = (*inv.args)[0];
        if (arg.empty())
            return "Error - invalid argument";

        if (arg[0] == '*')
        {
            auto file = make_unique<ifstream>(arg.substr(1));
            if (!file->is_open())
                return "Error - cannot open file: " + arg.substr(1);
            input = file.get();
            ownedInput = move(file);
        }
        else
        {
            auto ss = make_unique<stringstream>();
            *ss << arg;
            input = ss.get();
            ownedInput = move(ss);
        }
    }
    else
    {
        input = inv.in; // standardni ulaz ili pipe
    }

    // Računanje
    size_t count = 0;
    if (opt == "c")
    {
        char ch;
        while (input->get(ch))
        {
            ++count;
        }
    }
    else if (opt == "w")
    {
        string word;
        while (*input >> word)
            ++count;
    }

    (*inv.out) << count;
    inv.out->flush();

    return "";
}