// The purpose of this unit test is to check the interpreter::runPythonFile
// method
#include <cstring>
#include <iostream>

#include "dynamic-graph/python/interpreter.hh"

bool testFile(const std::string& filename, const std::string& expectedOutput,
              int numTest) {
  std::string err = "";
  dynamicgraph::python::Interpreter interp;
  for (int i = 0; i < numTest; ++i) {
    interp.runPythonFile(filename, err);
    if (err != expectedOutput) {
      std::cerr << "At iteration " << i
                << ", the output was not the one expected:" << std::endl;
      std::cerr << " expected: " << expectedOutput << std::endl;
      std::cerr << " err:      " << err << std::endl;
      return false;
    }
  }
  return true;
}

bool testInterpreterDestructor(const std::string& filename,
                               const std::string& expectedOutput) {
  std::string err = "";
  {
    dynamicgraph::python::Interpreter interp;
    interp.runPythonFile(filename, err);
  }
  {
    dynamicgraph::python::Interpreter interp;
    interp.runPythonFile(filename, err);
    if (err != expectedOutput) {
      std::cerr << "The output was not the one expected:" << std::endl;
      std::cerr << " expected: " << expectedOutput << std::endl;
      std::cerr << " err:      " << err << std::endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  // execute numerous time the same file.
  // While running 1025, we can notice a change in the error.
  // unfortunately, it can not be shown using a redirection of the streams
  int numTest = 1025;
  if (argc > 1) numTest = atoi(argv[1]);

  bool res = true;
  // This test succeeds only because it is launched before "test_python-ok.py"
  // because re as been imported in a previous test and it is not
  // safe to delete imported module...
  res = testFile(PATH "test_python-name_error.py",
                 std::string(
                     "Traceback (most recent call last):\n"
                     "  File \"" PATH
                     "test_python-name_error.py\", line 7, in <module>\n"
                     "    pathList = re.split(\":\", pkgConfigPath)  # noqa\n"
                     "NameError: name 're' is not defined\n"),
                 numTest) &&
        res;

  res = testFile(PATH "test_python-ok.py", "", numTest) && res;
  res = testFile(PATH "unexistant_file.py",
                 PATH "unexistant_file.py cannot be open", numTest) &&
        res;
  res = testFile(PATH "test_python-syntax_error.py",
                 std::string("  File \"" PATH
                             "test_python-syntax_error.py\", line 2\n"
                             "    hello world\n"
#if PY_MINOR_VERSION >= 10
                             "          ^^^^^\n"
#elif PY_MINOR_VERSION >= 8
                             "          ^\n"
#else
                             "              ^\n"
#endif
                             "SyntaxError: invalid syntax\n"),
                 numTest) &&
        res;
  res = testInterpreterDestructor(PATH "test_python-restart_interpreter.py",
                                  "") &&
        res;
  return (res ? 0 : 1);
}
