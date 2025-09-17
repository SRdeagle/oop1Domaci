#include "Prompt.h"
#include <string>
using namespace std;

string Prompt::execute(Invocation &inv)
{
    if ((*inv.args).size() != 1)
        return "Error - prompt takes one argument";
    Interpreter::setPrompt((*inv.args)[0]);
    return "";
}