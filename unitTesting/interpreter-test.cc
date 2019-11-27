// The purpose of this unit test is to evaluate the memory consumption 
// when call the interpreter.
#include "dynamic-graph/python/interpreter.hh"

int main(int argc, char ** argv)
{
  int numTest = 1;
  if (argc > 1)
    numTest = atoi(argv[1]);

  dynamicgraph::python::Interpreter interp;
  std::string command;
  std::string result;
  std::string out;
  std::string err;

  for (int i=0; i<numTest; ++i)
  {
    //correct input
    interp.python("print \"I am the interpreter\"", result, out, err);
    //incorrect input
    interp.python("print I am the interpreter", result, out, err);
    // Test that variable dynamic_graph_remote_interpreter is set to True
    interp.python("globals ().has_key (\"dynamic_graph_remote_interpreter\")",
                  result, out, err);
    if (result != "True") return -1;
    interp.python("dynamic_graph_remote_interpreter",
                  result, out, err);
    if (result != "True") return -1;
  }
  return 0;
}
