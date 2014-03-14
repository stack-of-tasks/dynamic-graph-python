// The purpose of this unit test is to check the interpreter::runPythonFile method
#include "dynamic-graph/python/interpreter.hh"

int main(int argc, char ** argv)
{
  // execute numerous time the same file.
  // While running 1025, we can notice a change in the error.
  // unfortunately, it can not be shown using a redirection of the streams
  int numTest = 1025;
  if (argc > 1)
    numTest = atoi(argv[1]);

  dynamicgraph::python::Interpreter interp;

  for (int i=0; i<numTest; ++i)
  {
    interp.runPythonFile("test_python_error.py");
  }

  return 0;
}
