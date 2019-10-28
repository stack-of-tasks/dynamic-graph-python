// The purpose of this unit test is to evaluate the memory consumption
// when call the interpreter.
#include "dynamic-graph/python/interpreter.hh"

int main(int argc, char** argv) {
  int numTest = 1;
  if (argc > 1) numTest = atoi(argv[1]);

  dynamicgraph::python::Interpreter interp;
  std::string command;
  std::string result;
  std::string out;
  std::string err;

  for (int i = 0; i < numTest; ++i) {
    // correct input
    interp.python("print('I am the interpreter')", result, out, err);
    assert(out.compare("I am the interpreter"));
    assert(err.length() == 0);
    // incorrect input
    interp.python("print I am the interpreter", result, out, err);
    assert(result.length() == 0);
    assert(out.length() == 0);
    assert(err.length() > 50);
  }
  return 0;
}
