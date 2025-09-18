#include "StreamHandler.h"

#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

// podesavanje ulaznog streama za komandu
void StreamHandler::resolveInput(Invocation &inv, ParsedCommand &parsed)
{
    currentIn = &cin;
    if (!parsed.inFile.empty())
    {
        // ako fajl ne moze da se otvori prijavi gresku i vrati cin
        auto f = make_unique<ifstream>(parsed.inFile);
        if (!f->is_open())
        {
            StreamHandler::error = "Error: cannot open input file '" + parsed.inFile + "'\n";
            currentIn = &cin;
            in_holder.reset();
            inv.in = currentIn;
            return;
        }
        in_holder = std::move(f);
        currentIn = in_holder.get();
    }
    inv.in = currentIn;
}

// podesavanje izlaznog streama za komandu
void StreamHandler::resolveOutput(Invocation &inv, ParsedCommand &parsed)
{
    if (!parsed.outFile.empty())
    {
        // ako fajl ne moze da se otvori prijavi gresku i vrati cout
        auto f = make_unique<ofstream>(parsed.outFile, parsed.append ? ios::app : ios::out);
        if (!f->is_open())
        {
            cerr << "Error: cannot open output file '" << parsed.outFile << "'\n";
            currentOut = &cout;
            out_holder.reset();
            inv.out = currentOut;
            return;
        }
        out_holder = std::move(f);
        currentOut = out_holder.get();
    }
    else
    {
        // ako nema izlazne redirekcije koristi cout
        currentOut = &cout;
    }
    inv.out = currentOut;
}

// povezivanje komandi u pipeline
void StreamHandler::connectPipeline(vector<Invocation> &invs,
                                    vector<ParsedCommand> &parsed)
{
    size_t n = parsed.size();
    invs[0].args = &parsed[0].args;
    invs[0].options = &parsed[0].options;
    invs[0].append = parsed[0].append;
    invs[0].command = parsed[0].command;
    if (n == 1)
    {
        resolveInput(invs[0], parsed[0]);
        resolveOutput(invs[0], parsed[0]);
        return;
    }

    size_t need = (n > 0 ? n - 1 : 0);
    // napravi ili resetuj pipe objekte
    if (pipes.size() <= need)
    {
        while (pipes.size() < need)
            pipes.push_back(make_unique<stringstream>());
    }
    else
    {
        pipes.resize(need);
    }
    for (size_t i = 0; i < need; i++)
    {
        pipes[i]->str("");
        pipes[i]->clear();
    }
    resolveOutput(invs[n - 1], parsed[n - 1]);
    resolveInput(invs[0], parsed[0]);
    // povezi ulaz i izlaz izmedju susednih komandi
    for (size_t i = 0; i < need; i++)
    {
        invs[i].out = pipes[i].get();
        invs[i + 1].in = pipes[i].get();
        invs[i + 1].args = &parsed[i + 1].args;
        invs[i + 1].options = &parsed[i + 1].options;
        invs[i + 1].append = parsed[i + 1].append;
        invs[i + 1].command = parsed[i + 1].command;
    }
}