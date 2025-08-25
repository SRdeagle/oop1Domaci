#include "Parser.h"
#include <sstream>
#include "Structures.h"
using namespace std;
bool Parser::isValid(const string &input) {
    return true;
}

ParsedCommand Parser::parseCommand(const string &input) {
    stringstream ss(input);
    char c,p=' ';
    ParsedCommand cmd;
    ss >> cmd.command;
    bool quotation = false, dash = false;
    string buffer;
    while (ss.get(c)) {
        if (c == '"') {
            cmd.isPath = false;
            if (quotation) {
                cmd.argument = buffer;
                buffer.clear();
                cmd.isPath = false;
                quotation = false;
            }
            else if (isspace(p))
                quotation = !quotation;
            else {
                cmd.error = "los format argumenata";
                break;
            }
        }
        else if (quotation)
            buffer += c;


        else {
            if (c == '-' && isspace(p)) {
                dash = true;
            }
            else if (!isspace(c)) {
                buffer += c;
            }
            else {
                if (dash) {
                    cmd.option = buffer;
                    buffer.clear();
                    dash = false;
                }
            }
        }
        p=c;
    }
    if (quotation)
        cmd.error = "los format argumenata";
    if (dash && !buffer.empty())
        cmd.option = buffer;
    if (!buffer.empty())
        cmd.argument = buffer;
    return cmd;
}
