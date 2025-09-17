#include "Commands/Echo.h"
#include "Commands/Touch.h"
#include "Interpreter.h"
#include "Commands/Prompt.h"
#include "Commands/Time.h"
#include "Commands/Date.h"
#include "Commands/Truncate.h"
#include "Commands/Rm.h"
#include "Commands/Wc.h"
#include "Commands/Tr.h"
#include "Commands/Head.h"
#include "Commands/Batch.h"
using namespace std;
int main()
{
  Interpreter interpreter;
  interpreter.addCommand("touch", make_unique<Touch>());
  interpreter.addCommand("echo", make_unique<Echo>());
  interpreter.addCommand("prompt", make_unique<Prompt>());
  interpreter.addCommand("time", make_unique<Time>());
  interpreter.addCommand("date", make_unique<Date>());
  interpreter.addCommand("truncate", make_unique<Truncate>());
  interpreter.addCommand("rm", make_unique<Rm>());
  interpreter.addCommand("wc", make_unique<Wc>());
  interpreter.addCommand("tr", make_unique<Tr>());
  interpreter.addCommand("head", make_unique<Head>());
  interpreter.addCommand("batch", make_unique<Batch>(interpreter));
  interpreter.activate();
  return 0;
}