#include "Touch.h"
#include <fstream>
#include <iostream>
using namespace std;

void Touch::execute(ParsedCommand cmd) {
    ifstream file(cmd.argument);
    if (file.is_open()) {
        file.close();
        cout<<"file already exists"<<endl;
        return;
    }
    else {
        ofstream ofile(cmd.argument);
        if (!ofile.is_open()) {
            cout<<"can't open file"<<endl;
            return;
        }
        ofile.close();
    }

 }