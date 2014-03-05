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

  for (int i=0; i<numTest; ++i)
  {
    //correct input
    command = "print \"Hello world\"";
    interp.python(command);

    //incorrect input
    command = "print Hello";
    interp.python(command);

    std::string result;
    std::string out;
    std::string err;
    //correct input
    interp.python("print \"I am the interpreter\"", result, out, err);
    //incorrect input
    interp.python("print I am the interpreter", result, out, err);
  }
  return 0;
}
