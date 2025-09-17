#include "CLIParser.h"
#include <sstream>
#include "Structures.h"
#include <vector>
using namespace std;

void removeWhitespace(string &s, int &i)
{
    while (i < s.length() - 1 && (s[i + 1] == ' ' || s[i + 1] == '\t'))
        i++;
}

string CLIParser::error = "";

bool CLIParser::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '/' || c == '_' || c == '-' || c == '.';
}

// --- Helper functions extracted from parseCommands ---
bool CLIParser::handleQuotation(char c, State &state, string &buffer, ParsedCommand &cmd, int i, string &error)
{
    if (c == '"')
    {
        // End of quotation
        // Next char must be end or separator
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

bool CLIParser::handleDash(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error)
{
    if (c == ' ' || c == '\t')
    {
        if (buffer.empty())
        {
            error = "expected option at " + to_string(i);
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
            error = "expected file name for output redirection at " + to_string(i);
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
            error = "pipe must follow a command at " + to_string(i);
            return false;
        }
        if (!buffer.empty())
        {
            cmd.options.push_back(buffer);
            buffer = "";
            i++;
            // break will be handled by parseCommands
            return false; // signal to break
        }
        else
        {
            error = "unexpected character at " + to_string(i);
            return false;
        }
    }
    else if (CLIParser::isLetter(c))
        buffer += c;
    else
    {
        error = "invalid syntax for command at " + to_string(i);
        return false;
    }
    return true;
}

bool CLIParser::HandleRest(char c, State &state, string &buffer, ParsedCommand &cmd, int &i, const string &line, string &error)
{
    if (c == ' ' || c == '\t')
    {
        cout << "usao u slucaj za space " << buffer << endl;
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
                error = "bad input order at " + to_string(i);
                return false;
            }
        }
        else if (c == '-')
        {
            if (state.phase == OPTION)
                state.dash = true;
            else
            {
                error = "bad order at " + to_string(i);
                return false;
            }
        }
        else if (c == '>' && !state.outRedir)
        {
            if (state.phase == ARGUMENT || state.phase == OPTION || state.phase == REDIRECTION)
            {
                if (i + 1 == line.length())
                {
                    error = "expected file name for output redirection at " + to_string(i);
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
                        error = "input source already specified (multiple inputs) at " + to_string(i);
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
                    error = "wrong order at " + to_string(i);
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
                        error = "error at " + to_string(i);
                        return false;
                    }
                    buffer = "";
                }
                state.inRedir = true;
                state.phase = REDIRECTION;
            }
            else
            {
                error = "unexpected < at " + to_string(i);
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
                            error = "unexpected redirection before pipe at " + to_string(i);
                            return false;
                        }
                    }
                }
                else
                {
                    error = "error before pipe at " + to_string(i);
                    return false;
                }
            }
            i++;
            buffer = "";
            if (i == line.length())
            {
                error = "expected command after pipe at the end";
                return false;
            }
            return false; // signal to break
        }
        else if (CLIParser::isLetter(c))
            buffer += c;
        else
        {
            error = "invalid syntax for command at " + to_string(i);
            return false;
        }
    }
    return true;
}

bool CLIParser::finalizeCommand(State &state, string &buffer, ParsedCommand &cmd, string &error)
{
    if (state.quotation)
        error = "expected ending \" at last word";
    if (state.dash)
    {
        if (state.phase == OPTION)
        {
            if (!buffer.empty())
                cmd.options.push_back(buffer);
            else
                error = "expected argument after - at last word";
            buffer = "";
        }
        else
        {
            error = "bad order at last word";
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
                    error = "input source already specified (multiple '<' or conflicting arg) at last word";
                    return false;
                }
                cmd.inFile = buffer;
                buffer = "";
            }
            else if (cmd.inFile.empty())
            {
                error = "expected redirection after < at last word";
            }
        }
        else
        {
            error = "bad syntax at last word";
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
                error = "expected redirection after > at last word";
            buffer = "";
        }
        else
        {
            error = "bad syntax at last word";
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
                error = "unexpected redirection at last word";
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
        error = "missing command";
        return false;
    }
    return true;
}

bool CLIParser::validatePipeline(vector<ParsedCommand> &cmds, string &error)
{
    if (cmds.size() >= 3)
    {
        for (int i = 1; i <= cmds.size() - 2; i++)
        {
            if (cmds[i].inFile != "" || cmds[i].outFile != "")
            {
                error = "redirection not allowed for inner commands";
                return false;
            }
        }
    }
    if (cmds.size() >= 2)
    {
        printParsedCommands(cmds);
        if (cmds[0].outFile != "")
        {
            error = "redirection not allowed for the first command";
            return false;
        }
        if (cmds[cmds.size() - 1].inFile != "")
        {
            error = "redirection not allowed for the last command";
            return false;
        }
    }
    return true;
}

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
            CLIParser::error = "command must start with a letter at " + to_string(i);
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
                // Only allow closing " if next char is separator or end
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
                        CLIParser::error = "unexpected character at " + to_string(i);
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
                        break; // break inner for loop for pipe
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
                        break; // break inner for loop for pipe
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
