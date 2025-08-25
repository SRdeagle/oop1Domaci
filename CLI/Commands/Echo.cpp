#include "Echo.h"
#include <iostream>
#include <fstream>
using namespace std;

void Echo::execute(ParsedCommand cmd) {
    if (!cmd.option.empty()) {
        cout <<"invalid syntax for command"<<endl;
        return;
    }
    if (!cmd.argument.empty()) {
        if (cmd.isPath) {
            cmd.argument = "/Users/aleksandarbrozek/Documents/domaciProg/oop1Domaci/CLI/text.txt";
            ifstream ifs(cmd.argument);
            ifs.open(cmd.argument);
            if (!ifs.is_open()) {
                cout << "Error opening file " /*<< filesystem::current_path()*/ << endl;
                return;
            }
            string line;
            while (getline(ifs, line)) {
                cout << line << endl;
            }
            ifs.close();
        }
        else {
            cout << cmd.argument << endl;
        }
    }
    else {
        string input,buffer;
        while (getline(cin, input)) {
            buffer += input +'\n';
        }
        if (input.length() == cin.eof()) {
            clearerr(stdin);
            cin.clear();
        }
        cout << buffer << endl;
        return;
    }
}