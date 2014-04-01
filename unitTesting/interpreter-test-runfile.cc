// The purpose of this unit test is to check the interpreter::runPythonFile method
#include <cstring>
#include <iostream>

#include "dynamic-graph/python/interpreter.hh"


int main(int argc, char ** argv)
{
  // execute numerous time the same file.
  // While running 1025, we can notice a change in the error.
  // unfortunately, it can not be shown using a redirection of the streams
  int numTest = 1025;
  if (argc > 1)
    numTest = atoi(argv[1]);

  std::string empty_err = "";
  dynamicgraph::python::Interpreter interp;

  for (int i=0; i<numTest; ++i)
  {
    interp.runPythonFile("test_python_ok.py", empty_err);
    if (empty_err != "")
    {
      std::cerr << "At iteration " << i << ", the error was not empty:" << std::endl;
      std::cerr << " err " << empty_err << std::endl;
      return -1;
    }
  }

  // check that the error remains the same, despite of the number of executions
  std::string old_err;
  interp.runPythonFile("test_python_error.py", old_err);
  std::string new_err = old_err;
  for (int i=0; i<numTest; ++i)
  {
    interp.runPythonFile("test_python_error.py", new_err);
    if (old_err != new_err)
    {
      std::cerr << "At iteration " << i << ", the error changed:" << std::endl;
      std::cerr << " old " << old_err << std::endl;
      std::cerr << " new " << new_err << std::endl;
      return -1;
    }
  }

  return 0;
}
