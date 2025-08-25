#include "Echo.h"
#include "Interpreter.h"
using namespace std;
int main() {
  Interpreter interpreter;
  interpreter.addCommand("echo", new Echo());
  interpreter.activate();
  return 0;
}