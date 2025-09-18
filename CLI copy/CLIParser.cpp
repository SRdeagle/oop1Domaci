#include "CLIParser.h"
#include <sstream>
#include "Structures.h"
#include <vector>
using namespace std;

// preskakanje belina u stringu
void removeWhitespace(string &s, int &i)
{
    while (i < s.length() - 1 && (s[i + 1] == ' ' || s[i + 1] == '\t'))
        i++;
}

string CLIParser::error = "";

// provera da li je karakter dozvoljen
bool CLIParser::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '/' || c == '_' || c == '-' || c == '.';
}

// obrada kada je parser u navodnicima
bool CLIParser::handleQuotation(char c, State &state, string &buffer, ParsedCommand &cmd, int i, string &error)
{
    if (c == '"')
    {
        state.quotation = false;
        cmd.args.push_back(buffer);
        buffer = "";
        return true;
    }
    else
    {
        buffer += c;
        return true;
    }
}

// obrada opcija
bool CLIParser::handleDash(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error)
{
    if (c == ' ' || c == '\t')
    {
        if (buffer.empty())
        {
            error = "Error: expected option at " + to_string(i);
            return false;
        }
        state.dash = false;
        cmd.options.push_back(buffer);
        buffer = "";
        removeWhitespace(const_cast<string &>(line), i);
    }
    else if (c == '>' && !state.outRedir)
    {
        if (i + 1 == line.length())
        {
            error = "Error: expected file name for output redirection at " + to_string(i);
            return false;
        }
        if (line[i + 1] == '>')
        {
            cmd.append = true;
            i++;
        }
        state.outRedir = true;
        state.phase = REDIRECTION;
        cmd.options.push_back(buffer);
        buffer = "";
    }
    else if (c == '<' && !state.inRedir)
    {
        state.inRedir = true;
        cmd.options.push_back(buffer);
        buffer = "";
        state.phase = REDIRECTION;
    }
    else if (c == '|')
    {
        if (cmd.command.empty())
        {
            error = "Error: pipe must follow a command at " + to_string(i);
            return false;
        }
        if (!buffer.empty())
        {
            cmd.options.push_back(buffer);
            buffer = "";
            i++;
            return false;
        }
        else
        {
            error = "Error: unexpected character at " + to_string(i);
            return false;
        }
    }
    else if (CLIParser::isLetter(c))
        buffer += c;
    else
    {
        error = "Error: invalid syntax for command at " + to_string(i);
        return false;
    }
    return true;
}

// obrada ostalih karaktera
bool CLIParser::HandleRest(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error)
{
    if (c == ' ' || c == '\t')
    {
        if (state.inRedir && !buffer.empty())
        {
            if (!cmd.inFile.empty())
            {
                error = "input source already specified (multiple '<' or conflicting arg) at " + to_string(i);
                return false;
            }
            cmd.inFile = buffer;
            buffer = "";
        }
        else if (state.outRedir && !buffer.empty())
        {
            cmd.outFile = buffer;
            buffer = "";
        }
        else
        {
            if (!buffer.empty())
            {
                if (state.phase == COMMAND)
                {
                    cmd.command = buffer;
                    state.phase = OPTION;
                    buffer = "";
                }
                else if (state.phase == ARGUMENT)
                {
                    cmd.args.push_back('*' + buffer);
                    buffer = "";
                }
                else
                {
                    if (!cmd.inFile.empty())
                    {
                        error = "input source already specified (multiple inputs) at " + to_string(i);
                        return false;
                    }
                    cmd.args.push_back('*' + buffer);
                    buffer = "";
                    state.phase = ARGUMENT;
                }
            }
        }
        removeWhitespace(const_cast<string &>(line), i);
    }
    else
    {
        if (c == '"')
        {
            if (state.phase == ARGUMENT || state.phase == OPTION)
            {
                state.quotation = true;
                state.phase = ARGUMENT;
            }
            else
            {
                error = "Error: bad input order at " + to_string(i);
                return false;
            }
        }
        else if (c == '-')
        {
            if (state.phase == OPTION)
                state.dash = true;
            else
            {
                error = "Error: bad order at " + to_string(i);
                return false;
            }
        }
        else if (c == '>' && !state.outRedir)
        {
            if (state.phase == ARGUMENT || state.phase == OPTION || state.phase == REDIRECTION)
            {
                if (i + 1 == line.length())
                {
                    error = "Error: expected file name for output redirection at " + to_string(i);
                    return false;
                }
                if (line[i + 1] == '>')
                {
                    cmd.append = true;
                    i++;
                    state.outRedir = true;
                    state.phase = REDIRECTION;
                    return true;
                }
                if (!buffer.empty())
                {
                    if (cmd.inFile.empty() && state.inRedir)
                        cmd.inFile = buffer;
                    else if (state.phase == ARGUMENT)
                        cmd.args.push_back('*' + buffer);
                    else
                    {
                        error = "Error: input source already specified (multiple inputs) at " + to_string(i);
                        return false;
                    }
                    buffer = "";
                }
                state.outRedir = true;
                state.phase = REDIRECTION;
            }
            else
            {
                if (!buffer.empty())
                {
                    cmd.command = buffer;
                    buffer = "";
                    state.outRedir = true;
                    state.phase = REDIRECTION;
                }
                else
                {
                    error = "Error: wrong order at " + to_string(i);
                    return false;
                }
                state.phase = REDIRECTION;
                state.outRedir = true;
            }
        }
        else if (c == '<')
        {
            if (!state.inRedir)
            {
                if (!buffer.empty())
                {
                    if (cmd.command.empty() && state.phase == COMMAND)
                        cmd.command = buffer;
                    else if (state.phase == ARGUMENT || state.phase == OPTION)
                    {
                        cmd.args.push_back('*' + buffer);
                        buffer = "";
                    }
                    else if (state.phase == REDIRECTION && cmd.outFile.empty())
                    {
                        cmd.outFile = buffer;
                    }
                    else
                    {
                        error = "Error: error at " + to_string(i);
                        return false;
                    }
                    buffer = "";
                }
                state.inRedir = true;
                state.phase = REDIRECTION;
            }
            else
            {
                error = "Error: unexpected < at " + to_string(i);
                return false;
            }
        }
        else if (c == '|')
        {
            if (!buffer.empty())
            {
                if (cmd.command.empty())
                    cmd.command = buffer;
                else if (state.phase == ARGUMENT || state.phase == OPTION)
                {
                    cmd.args.push_back('*' + buffer);
                    buffer = "";
                }
                else if (state.phase == REDIRECTION)
                {
                    if (state.outRedir)
                        cmd.outFile = buffer;
                    else if (state.inRedir)
                        cmd.inFile = buffer;
                    else
                    {
                        if (cmd.inFile.empty())
                            cmd.inFile = buffer;
                        else if (cmd.outFile.empty())
                            cmd.outFile = buffer;
                        else
                        {
                            error = "Error: unexpected redirection before pipe at " + to_string(i);
                            return false;
                        }
                    }
                }
                else
                {
                    error = "Error: error before pipe at " + to_string(i);
                    return false;
                }
            }
            i++;
            buffer = "";
            if (i == line.length())
            {
                error = "Error: expected command after pipe at the end";
                return false;
            }
            return false;
        }
        else if (CLIParser::isLetter(c))
            buffer += c;
        else
        {
            error = "Error: invalid syntax for command at " + to_string(i);
            return false;
        }
    }
    return true;
}

// zavrsna obrada komande i provera
bool CLIParser::finalizeCommand(State &state, string &buffer, ParsedCommand &cmd, string &error)
{
    if (state.quotation)
        error = "Error: expected ending \" at last word";
    if (state.dash)
    {
        if (state.phase == OPTION)
        {
            if (!buffer.empty())
                cmd.options.push_back(buffer);
            else
                error = "Error: expected argument after - at last word";
            buffer = "";
        }
        else
        {
            error = "Error: bad order at last word";
            return false;
        }
    }
    if (state.inRedir && !state.outRedir)
    {
        if (state.phase == OPTION || state.phase == ARGUMENT || state.phase == REDIRECTION)
        {
            if (!buffer.empty())
            {
                if (!cmd.inFile.empty())
                {
                    error = "Error: input source already specified (multiple '<' or conflicting arg) at last word";
                    return false;
                }
                cmd.inFile = buffer;
                buffer = "";
            }
            else if (cmd.inFile.empty())
            {
                error = "Error: expected redirection after < at last word";
            }
        }
        else
        {
            error = "Error: bad syntax at last word";
            return false;
        }
    }
    if (state.outRedir && !state.inRedir)
    {
        if (state.phase == OPTION || state.phase == ARGUMENT || state.phase == REDIRECTION)
        {
            if (!buffer.empty())
                cmd.outFile = buffer;
            else if (cmd.outFile.empty())
                error = "Error: expected redirection after > at last word";
            buffer = "";
        }
        else
        {
            error = "Error: bad syntax at last word";
            return false;
        }
    }
    if (state.inRedir && state.outRedir)
    {
        if (!buffer.empty())
        {
            if (cmd.inFile.empty())
                cmd.inFile = buffer;
            else if (cmd.outFile.empty())
                cmd.outFile = buffer;
            else
            {
                error = "Error: unexpected redirection at last word";
                return false;
            }
            buffer = "";
        }
    }
    if (!buffer.empty() && !state.inRedir && !state.outRedir && !state.dash && state.phase == COMMAND)
    {
        if (cmd.command.empty())
            cmd.command = buffer;
        else
            cmd.args.push_back(buffer);
    }
    else if (!buffer.empty())
    {
        cmd.args.push_back('*' + buffer);
    }
    if (cmd.command.empty())
    {
        error = "Error: missing command";
        return false;
    }
    return true;
}

// provera pravila za pipeline komande
bool CLIParser::validatePipeline(vector<ParsedCommand> &cmds, string &error)
{
    if (cmds.size() >= 3)
    {
        for (int i = 1; i <= cmds.size() - 2; i++)
        {
            if (cmds[i].inFile != "" || cmds[i].outFile != "")
            {
                error = "Error: redirection not allowed for inner commands";
                return false;
            }
        }
    }
    if (cmds.size() >= 2)
    {
        printParsedCommands(cmds);
        if (cmds[0].outFile != "")
        {
            error = "Error: redirection not allowed for the first command";
            return false;
        }
        if (cmds[cmds.size() - 1].inFile != "")
        {
            error = "Error: redirection not allowed for the last command";
            return false;
        }
    }
    return true;
}

// glavna funkcija za parsiranje ulaznog stringa
vector<ParsedCommand> CLIParser::parseCommands(const string &input)
{
    CLIParser::error = "";
    string line = input, buffer;
    char c;
    int i = 0, j = -1;
    vector<ParsedCommand> cmds;
    State state;
    while (i < line.length())
    {
        removeWhitespace(line, i);
        if (i == line.length())
            break;
        if (line[i] == ' ' || line[i] == '\t')
            i++;
        if (!isLetter(line[i]))
        {
            CLIParser::error = "Error: command must start with a letter at " + to_string(i);
            return cmds;
        }
        cmds.push_back({});
        j++;
        buffer = "";
        state = {false, false, false, false, COMMAND};
        for (; i < line.length(); i++)
        {
            c = line[i];
            bool cont = true;
            if (state.quotation)
            {
                if (c == '"')
                {
                    if (i + 1 == line.length() || line[i + 1] == ' ' || line[i + 1] == '\t' || line[i + 1] == '|' || line[i + 1] == '>' || line[i + 1] == '<')
                    {
                        state.quotation = false;
                        cmds[j].args.push_back(buffer);
                        buffer = "";
                        continue;
                    }
                    else
                    {
                        CLIParser::error = "Error: unexpected character at " + to_string(i);
                        return cmds;
                    }
                }
                else
                {
                    buffer += c;
                }
            }
            else if (state.dash)
            {
                string err;
                cont = handleDash(c, state, buffer, cmds[j], i, line, err);
                if (!cont)
                {
                    if (!err.empty())
                    {
                        CLIParser::error = err;
                        return cmds;
                    }
                    else
                        break;
                }
            }
            else
            {
                string err;
                cont = HandleRest(c, state, buffer, cmds[j], i, line, err);
                if (!cont)
                {
                    if (!err.empty())
                    {
                        CLIParser::error = err;
                        return cmds;
                    }
                    else
                        break;
                }
            }
        }
        string err;
        if (!finalizeCommand(state, buffer, cmds[j], err))
        {
            CLIParser::error = err;
            return cmds;
        }
        if (!err.empty())
            CLIParser::error = err;
        if (!CLIParser::error.empty())
            return cmds;
    }
    string err;
    if (!validatePipeline(cmds, err))
    {
        CLIParser::error = err;
        return cmds;
    }
    return cmds;
}
