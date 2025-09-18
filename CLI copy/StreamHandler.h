#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "Structures.h"
#include <string>
using namespace std;

class StreamHandler
{
private:
    unique_ptr<std::istream> in_holder;
    unique_ptr<std::ostream> out_holder;
    istream *currentIn = nullptr;
    ostream *currentOut = nullptr;
    vector<unique_ptr<stringstream>> pipes;
    string error = "";

public:
    // 1. Povezuje IO za jednu komandu (koristi se i unutar connectPipeline)
    void resolveInput(Invocation &inv, ParsedCommand &parsed);
    void resolveOutput(Invocation &inv, ParsedCommand &parsed);

    // 2. Povezuje sve komande u cevovodu
    void connectPipeline(vector<Invocation> &invs,
                         vector<ParsedCommand> &parsed);
    string getError() { return error; };
    void resetError() { error = ""; };
};