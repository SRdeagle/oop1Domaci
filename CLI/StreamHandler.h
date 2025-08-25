#include <iostream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

class StreamHandler
{
public:
    // 1. Povezuje IO za jednu komandu (koristi se i unutar connectPipeline)
    void resolveInput(const Cmd &cmd, IO &io);
    void resolveOutput(const Cmd &cmd, IO &io);

    // 2. Povezuje sve komande u cevovodu
    void connectPipeline(const vector<Cmd> &cmds,
                         vector<IO> &ios,
                         vector<stringstream> &pipes);

    // 3. Resetuje stanje cevi pre nego što sledeća komanda čita
    void rewindPipe(stringstream &pipe);
}