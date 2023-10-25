// -*- mode: c++ -*-
// Copyright 2011, Florent Lamiraux, CNRS.

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>

#include "dynamic-graph/debug.h"
#include "dynamic-graph/python/interpreter.hh"

std::ofstream dg_debugfile("/tmp/dynamic-graph-traces.txt",
                           std::ios::trunc& std::ios::out);

// Python initialization commands
namespace dynamicgraph {
namespace python {
static const std::string pythonPrefix[8] = {
    "from __future__ import print_function\n",
    "import traceback\n",
    "class StdoutCatcher:\n"
    "    def __init__(self):\n"
    "        self.data = ''\n"
    "    def write(self, stuff):\n"
    "        self.data = self.data + stuff\n"
    "    def fetch(self):\n"
    "        s = self.data[:]\n"
    "        self.data = ''\n"
    "        return s\n",
    "stdout_catcher = StdoutCatcher()\n",
    "stderr_catcher = StdoutCatcher()\n",
    "import sys\n",
    "sys.stdout = stdout_catcher",
    "sys.stderr = stderr_catcher"};

bool HandleErr(std::string& err, PyObject* globals_, int PythonInputType) {
  dgDEBUGIN(15);
  err = "";
  bool lres = false;

  if (PyErr_Occurred() != NULL) {
    bool is_syntax_error = PyErr_ExceptionMatches(PyExc_SyntaxError);
    PyErr_Print();
    PyObject* stderr_obj = PyRun_String("stderr_catcher.fetch()", Py_eval_input,
                                        globals_, globals_);
    err = obj_to_str(stderr_obj);
    Py_DECREF(stderr_obj);

    // Here if there is a syntax error and
    // and the interpreter input is set to Py_eval_input,
    // it is maybe a statement instead of an expression.
    // Therefore we indicate to re-evaluate the command.
    if (is_syntax_error && PythonInputType == Py_eval_input) {
      dgDEBUG(15) << "Detected a syntax error " << std::endl;
      lres = false;
    } else
      lres = true;

    PyErr_Clear();
  } else {
    dgDEBUG(15) << "no object generated but no error occured." << std::endl;
  }
  return lres;
}

Interpreter::Interpreter() {
  // load python dynamic library
  // this is silly, but required to be able to import dl module.
#ifndef WIN32
  dlopen(PYTHON_LIBRARY, RTLD_LAZY | RTLD_GLOBAL);
#endif
  Py_Initialize();
#if PY_MAJOR_VERSION < 3 || PY_MINOR_VERSION < 7
  PyEval_InitThreads();
#endif
  mainmod_ = PyImport_AddModule("__main__");
  Py_INCREF(mainmod_);
  globals_ = PyModule_GetDict(mainmod_);
  assert(globals_);
  Py_INCREF(globals_);
  PyRun_SimpleString(pythonPrefix[0].c_str());
  PyRun_SimpleString(pythonPrefix[1].c_str());
  PyRun_SimpleString(pythonPrefix[2].c_str());
  PyRun_SimpleString(pythonPrefix[3].c_str());
  PyRun_SimpleString(pythonPrefix[4].c_str());
  PyRun_SimpleString(pythonPrefix[5].c_str());
  PyRun_SimpleString(pythonPrefix[6].c_str());
  PyRun_SimpleString(pythonPrefix[7].c_str());
  PyRun_SimpleString("import linecache");

  // Allow threads
  _pyState = PyEval_SaveThread();
}

Interpreter::~Interpreter() {
  PyEval_RestoreThread(_pyState);

  // Ideally, we should call Py_Finalize but this is not really supported by
  // Python.
  // Instead, we merelly remove variables.
  // Code was taken here:
  // https://github.com/numpy/numpy/issues/8097#issuecomment-356683953
  {
    PyObject* poAttrList = PyObject_Dir(mainmod_);
    PyObject* poAttrIter = PyObject_GetIter(poAttrList);
    PyObject* poAttrName;

    while ((poAttrName = PyIter_Next(poAttrIter)) != NULL) {
      std::string oAttrName(obj_to_str(poAttrName));

      // Make sure we don't delete any private objects.
      if (oAttrName.compare(0, 2, "__") != 0 ||
          oAttrName.compare(oAttrName.size() - 2, 2, "__") != 0) {
        PyObject* poAttr = PyObject_GetAttr(mainmod_, poAttrName);

        // Make sure we don't delete any module objects.
        if (poAttr && poAttr->ob_type != mainmod_->ob_type)
          PyObject_SetAttr(mainmod_, poAttrName, NULL);

        Py_DECREF(poAttr);
      }

      Py_DECREF(poAttrName);
    }

    Py_DECREF(poAttrIter);
    Py_DECREF(poAttrList);
  }

  Py_DECREF(mainmod_);
  Py_DECREF(globals_);
  // Py_Finalize();
}

std::string Interpreter::python(const std::string& command) {
  std::string lerr(""), lout(""), lres("");
  python(command, lres, lout, lerr);
  return lres;
}

void Interpreter::python(const std::string& command, std::string& res,
                         std::string& out, std::string& err) {
  res = "";
  out = "";
  err = "";

  // Check if the command is not a python comment or empty.
  std::string::size_type iFirstNonWhite = command.find_first_not_of(" \t");
  // Empty command
  if (iFirstNonWhite == std::string::npos) return;
  // Command is a comment. Ignore it.
  if (command[iFirstNonWhite] == '#') return;

  PyEval_RestoreThread(_pyState);

  std::cout << command.c_str() << std::endl;
  PyObject* result =
      PyRun_String(command.c_str(), Py_eval_input, globals_, globals_);
  // Check if the result is null.
  if (result == NULL) {
    // Test if this is a syntax error (due to the evaluation of an expression)
    // else just output the problem.
    if (!HandleErr(err, globals_, Py_eval_input)) {
      // If this is a statement, re-parse the command.
      result =
          PyRun_String(command.c_str(), Py_single_input, globals_, globals_);

      // If there is still an error build the appropriate err string.
      if (result == NULL) HandleErr(err, globals_, Py_single_input);
      // If there is no error, make sure that the previous error message is
      // erased.
      else
        err = "";
    } else
      dgDEBUG(15) << "Do not try a second time." << std::endl;
  }

  PyObject* stdout_obj = 0;
  stdout_obj =
      PyRun_String("stdout_catcher.fetch()", Py_eval_input, globals_, globals_);
  out = obj_to_str(stdout_obj);
  Py_DECREF(stdout_obj);
  // Local display for the robot (in debug mode or for the logs)
  if (out.size() != 0) std::cout << "Output:" << out << std::endl;
  if (err.size() != 0) std::cout << "Error:" << err << std::endl;
  // If python cannot build a string representation of result
  // then results is equal to NULL. This will trigger a SEGV
  dgDEBUG(15) << "For command: " << command << std::endl;
  if (result != NULL) {
    res = obj_to_str(result);
    dgDEBUG(15) << "Result is: " << res << std::endl;
    Py_DECREF(result);
  } else {
    dgDEBUG(15) << "Result is: empty" << std::endl;
  }
  dgDEBUG(15) << "Out is: " << out << std::endl;
  dgDEBUG(15) << "Err is :" << err << std::endl;

  _pyState = PyEval_SaveThread();

  return;
}

PyObject* Interpreter::globals() { return globals_; }

void Interpreter::runPythonFile(std::string filename) {
  std::string err = "";
  runPythonFile(filename, err);
}

void Interpreter::runPythonFile(std::string filename, std::string& err) {
  FILE* pFile = fopen(filename.c_str(), "r");
  if (pFile == 0x0) {
    err = filename + " cannot be open";
    return;
  }

  PyEval_RestoreThread(_pyState);

  err = "";
  PyObject* run =
      PyRun_File(pFile, filename.c_str(), Py_file_input, globals_, globals_);
  if (run == NULL) {
    HandleErr(err, globals_, Py_file_input);
    std::cerr << err << std::endl;
  }
  Py_DecRef(run);

  _pyState = PyEval_SaveThread();
  fclose(pFile);
}

void Interpreter::runMain(void) {
  PyEval_RestoreThread(_pyState);
#if PY_MAJOR_VERSION >= 3
  const Py_UNICODE* argv[] = {L"dg-embedded-pysh"};
  Py_Main(1, const_cast<Py_UNICODE**>(argv));
#else
  const char* argv[] = {"dg-embedded-pysh"};
  Py_Main(1, const_cast<char**>(argv));
#endif
  _pyState = PyEval_SaveThread();
}

std::string Interpreter::processStream(std::istream& stream, std::ostream& os) {
  char line[10000];
  std::streamsize maxSize = 10000;
  snprintf(line, static_cast<size_t>(maxSize), "%s", "\n");
  std::string command;
#if 0
  while (line != std::string("")) {
    stream.getline(line, maxSize, '\n');
    command += std::string(line) + std::string("\n");
  };
#else
  os << "dg> ";
  stream.getline(line, maxSize, ';');
  command += std::string(line);
#endif
  return command;
}

}  // namespace python
}  // namespace dynamicgraph
