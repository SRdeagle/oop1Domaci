#include "Parser.h"
#include <sstream>
#include "Structures.h"
#include <vector>
using namespace std;

void removeWhitespace(string &s, int &i)
{
    while (i < s.length() - 1 && (s[i + 1] == ' ' || s[i + 1] == '\t'))
        i++;
}

string Parser::error = "";

bool Parser::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '/' || c == '_' || c == '-' || c == '.';
}

vector<ParsedCommand> Parser::parseCommands(const string &input)
{
    Parser::error = "";
    string line = input;
    char c, p = ' ';
    int i = 0;
    int j = -1;
    vector<ParsedCommand> cmds;
    string buffer;
    enum Phase
    {
        COMMAND,
        ARGUMENT,
        OPTION,
        REDIRECTION
    };
    struct State
    {
        bool quotation = false;
        bool dash = false;
        bool inRedir = false;
        bool outRedir = false;
        Phase phase = COMMAND;
    };
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
            Parser::error = "command must start with a letter at " + to_string(i);
            return cmds;
        }
        cmds.push_back({});
        j++;
        buffer = "";
        state = {false, false, false, false, COMMAND};
        for (; i < line.length(); i++)
        {
            c = line[i];
            // cout << c << endl;
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
                        Parser::error = "unexpected character at " + to_string(i);
                        return cmds;
                    }
                }

                else
                    buffer += c;
            }
            else if (state.dash)
            {
                if (c == ' ' || c == '\t')
                {
                    if (buffer.empty())
                    {
                        Parser::error = "expected option at " + to_string(i);
                        return cmds;
                    }
                    state.dash = false;
                    cmds[j].options.push_back(buffer);
                    buffer = "";
                    removeWhitespace(line, i);
                }
                else if (c == '>' && !state.outRedir)
                {
                    if (i + 1 == line.length())
                    {
                        Parser::error = "expected file name for output redirection at " + to_string(i);
                        return cmds;
                    }
                    if (line[i + 1] == '>')
                    {
                        cmds[j].append = true;
                        i++;
                    }

                    state.outRedir = true;
                    state.phase = REDIRECTION;
                    cmds[j].options.push_back(buffer);
                    buffer = "";
                }
                else if (c == '<' && !state.inRedir)
                {
                    state.inRedir = true;
                    cmds[j].options.push_back(buffer);
                    buffer = "";
                    state.phase = REDIRECTION;
                }
                else if (c == '|')
                {
                    if (cmds[j].command.empty())
                    {
                        Parser::error = "pipe must follow a command at " + to_string(i);
                        return cmds;
                    }
                    if (!buffer.empty())
                    {
                        cmds[j].options.push_back(buffer);
                        buffer = "";
                        i++;
                        break;
                    }
                    else
                    {
                        Parser::error = "unexpected character at " + to_string(i);
                        return cmds;
                    }
                }

                else if (Parser::isLetter(c))
                    buffer += c;
                else
                {
                    Parser::error = "invalid syntax for command at " + to_string(i);
                    return cmds;
                }
            }
            else
            {
                if (c == ' ' || c == '\t')
                {
                    if (state.inRedir && !buffer.empty())
                    {
                        if (!cmds[j].inFile.empty())
                        {
                            Parser::error = "input source already specified (multiple '<' or conflicting arg) at " + to_string(i);
                            return cmds;
                        }
                        cmds[j].inFile = buffer;
                        buffer = "";
                    }
                    else if (state.outRedir && !buffer.empty())
                    {
                        cmds[j].outFile = buffer;
                        buffer = "";
                    }
                    else
                    {
                        if (!buffer.empty())
                        {
                            if (state.phase == COMMAND)

                            {
                                cmds[j].command = buffer; // first word -> command name
                                state.phase = OPTION;
                                buffer = "";
                            }
                            else if (state.phase == ARGUMENT)
                            {
                                /*if (!cmds[j].inFile.empty())
                                {
                                    Parser::error = "input source already specified (multiple inputs) at " + to_string(i);
                                    return cmds;
                                }*/
                                cmds[j].args.push_back('*' + buffer);
                                buffer = "";
                            }
                            else
                            {
                                if (!cmds[j].inFile.empty())
                                {
                                    Parser::error = "input source already specified (multiple inputs) at " + to_string(i);
                                    return cmds;
                                }
                                cmds[j].args.push_back('*' + buffer);
                                buffer = "";
                                state.phase = ARGUMENT;
                            }
                        }
                    }
                    removeWhitespace(line, i);
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
                            Parser::error = "bad input order at " + to_string(i);
                            return cmds;
                        }
                    }
                    else if (c == '-')
                    {
                        if (state.phase == OPTION)
                            state.dash = true;
                        else
                        {
                            Parser::error = "bad order at " + to_string(i);
                            return cmds;
                        }
                    }
                    else if (c == '>' && !state.outRedir)
                    {
                        if (state.phase == ARGUMENT || state.phase == OPTION || state.phase == REDIRECTION)
                        {
                            if (i + 1 == line.length())
                            {
                                Parser::error = "expected file name for output redirection at " + to_string(i);
                                return cmds;
                            }
                            if (line[i + 1] == '>')
                            {
                                cmds[j].append = true;
                                i++;
                                state.outRedir = true;
                                state.phase = REDIRECTION;
                                continue;
                            }

                            if (!buffer.empty())
                            {

                                if (cmds[j].inFile.empty() && state.inRedir)
                                    cmds[j].inFile = buffer;
                                else if (state.phase == ARGUMENT)
                                    cmds[j].args.push_back('*' + buffer);
                                else
                                {
                                    Parser::error = "input source already specified (multiple inputs) at " + to_string(i);
                                    return cmds;
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
                                cmds[j].command = buffer;
                                buffer = "";
                                state.outRedir = true;
                                state.phase = REDIRECTION;
                                // cout << "buffer: " << buffer << endl;
                            }
                            else
                            {
                                Parser::error = "wrong order at " + to_string(i);
                                return cmds;
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
                                if (cmds[j].command.empty() && state.phase == COMMAND)
                                    cmds[j].command = buffer;
                                else if (state.phase == ARGUMENT || state.phase == OPTION)
                                {
                                    cmds[j].args.push_back('*' + buffer);
                                    buffer = "";
                                }
                                else if (state.phase == REDIRECTION && cmds[j].outFile.empty())
                                {
                                    cmds[j].outFile = buffer;
                                }
                                else
                                {
                                    Parser::error = "error at " + to_string(i);
                                    return cmds;
                                }
                                buffer = "";
                            }
                            state.inRedir = true;
                            state.phase = REDIRECTION;
                        }
                        else
                        {
                            Parser::error = "unexpected < at " + to_string(i);
                            return cmds;
                        }
                    }
                    else if (c == '|')
                    {
                        if (!buffer.empty())
                        {
                            if (cmds[j].command.empty())
                                cmds[j].command = buffer;
                            else if (state.phase == ARGUMENT || state.phase == OPTION)
                            {
                                cmds[j].args.push_back('*' + buffer);
                                buffer = "";
                            }
                            else if (state.phase == REDIRECTION)
                            {
                                if (state.outRedir)
                                    cmds[j].outFile = buffer;
                                else if (state.inRedir)
                                    cmds[j].inFile = buffer;
                                else
                                {
                                    if (cmds[j].inFile.empty())
                                        cmds[j].inFile = buffer;
                                    else if (cmds[j].outFile.empty())
                                        cmds[j].outFile = buffer;
                                    else
                                    {
                                        Parser::error = "unexpected redirection before pipe at " + to_string(i);
                                        return cmds;
                                    }
                                }
                            }
                            else
                            {
                                Parser::error = "error before pipe at " + to_string(i);
                                return cmds;
                            }
                        }
                        i++;
                        buffer = "";
                        if (i == line.length())
                        {
                            Parser::error = "expected command after pipe at the end";
                            return cmds;
                        }
                        break;
                    }
                    else if (Parser::isLetter(c))
                        buffer += c;
                    else
                    {
                        Parser::error = "invalid syntax for command at " + to_string(i);
                        return cmds;
                    }
                }
            }
        }
        // cout << (state.dash ? "dash" : "no dash") << " buffer: " << buffer << endl;
        if (state.quotation)
            Parser::error = "expected ending \" at last word";
        if (state.dash)
        {
            if (state.phase == OPTION)

            {
                if (!buffer.empty())
                    cmds[j].options.push_back(buffer);
                else
                    Parser::error = "expected argument after - at last word";
                buffer = "";
            }
            else
            {
                Parser::error = "bad order at last word";
                return cmds;
            }
        }
        if (state.inRedir && !state.outRedir)
        {
            if (state.phase == OPTION || state.phase == ARGUMENT || state.phase == REDIRECTION)
            {
                if (!buffer.empty())
                {
                    if (!cmds[j].inFile.empty())
                    {
                        Parser::error = "input source already specified (multiple '<' or conflicting arg) at last word";
                        return cmds;
                    }
                    cmds[j].inFile = buffer;
                    buffer = "";
                }
                else
                {
                    Parser::error = "expected redirection after < at last word";
                }
            }
            else
            {
                Parser::error = "bad syntax at last word";
                return cmds;
            }
        }
        if (state.outRedir && !state.inRedir)
        {
            if (state.phase == OPTION || state.phase == ARGUMENT || state.phase == REDIRECTION)
            {
                if (!buffer.empty())
                    cmds[j].outFile = buffer;
                else
                    Parser::error = "expected redirection after > at last word";
                buffer = "";
            }
            else
            {
                Parser::error = "bad syntax at last word";
                return cmds;
            }
        }
        if (state.inRedir && state.outRedir)
        {
            if (!buffer.empty())
            {
                if (cmds[j].inFile.empty())
                    cmds[j].inFile = buffer;
                else if (cmds[j].outFile.empty())
                    cmds[j].outFile = buffer;
                else
                {
                    Parser::error = "unexpected redirection at last word";
                    return cmds;
                }
                buffer = "";
            }
        }

        if (!buffer.empty() && !state.inRedir && !state.outRedir && !state.dash && state.phase == COMMAND)
        {
            if (cmds[j].command.empty())
                cmds[j].command = buffer;
            else
                cmds[j].args.push_back(buffer);
        }
        else if (!buffer.empty())
        {
            cmds[j].args.push_back('*' + buffer);
        }
        if (cmds[j].command.empty())
        {
            Parser::error = "missing command";
            return cmds;
        }
    }
    if (cmds.size() >= 3)
    {
        for (int i = 1; i <= cmds.size() - 2; i++)
        {
            if (cmds[i].inFile != "" || cmds[i].outFile != "")
            {
                Parser::error = "redirection not allowed for inner commands";
                return cmds;
            }
        }
    }
    if (cmds.size() >= 2)
    {
        printParsedCommands(cmds);
        if (cmds[0].outFile != "")
        {
            Parser::error = "redirection not allowed for the first command";
            return cmds;
        }
        if (cmds[cmds.size() - 1].inFile != "")
        {
            Parser::error = "redirection not allowed for the last command";
            return cmds;
        }
    }
    return cmds;
}

string Parser::getError()
{
    return Parser::error;
}
void Parser::resetError()
{
    Parser::error = "";
}
