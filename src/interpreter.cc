// -*- mode: c++ -*-
// Copyright 2011, Florent Lamiraux, CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// dynamic-graph is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include "dynamic-graph/debug.h"
#include "dynamic-graph/python/interpreter.hh"
#include "link-to-python.hh"

#include <boost/python/errors.hpp>
#include <boost/python/object.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/str.hpp>
#include <boost/python/import.hpp>

namespace py=boost::python;

std::ofstream dg_debugfile( "/tmp/dynamic-graph-traces.txt", std::ios::trunc&std::ios::out );

// Python initialization commands
namespace dynamicgraph {
namespace python {
static const std::string pythonPrefix[5] = {
  "import traceback\n",
  "def display(s): return str(s) if not s is None else None",
  "class StdoutCatcher:\n"
  "    def __init__(self):\n"
  "        self.data = ''\n"
  "    def write(self, stuff):\n"
  "        self.data = self.data + stuff\n"
  "    def fetch(self):\n"
  "        s = self.data[:]\n"
  "        self.data = ''\n"
  "        return s\n"
  "stdout_catcher = StdoutCatcher()\n"
  "import sys\n"
  "sys.stdout = stdout_catcher\n"
  "dynamic_graph_remote_interpreter = True"
};
}
}

namespace dynamicgraph {
namespace python {

// Parse a python exception and return the corresponding error message
// http://thejosephturner.com/blog/post/embedding-python-in-c-applications-with-boostpython-part-2/
std::string parse_python_exception();

bool HandleErr(std::string & err,
               PyObject * traceback_format_exception,
               PyObject * globals_,
               int PythonInputType)
{
  dgDEBUGIN(15);
  err="";
  bool lres=false;

  if (PyErr_Occurred()) {
    PyObject *ptype, *pvalue, *ptraceback, *pyerr;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    if (ptraceback == NULL) {
      ptraceback = Py_None;
      // increase the Py_None count, to avoid a crash at the tuple destruction
      Py_INCREF(ptraceback);
    }
    PyObject* args = PyTuple_New(3);
    PyTuple_SET_ITEM(args, 0, ptype);
    PyTuple_SET_ITEM(args, 1, pvalue);
    PyTuple_SET_ITEM(args, 2, ptraceback);
    pyerr = PyObject_CallObject(traceback_format_exception, args);
    assert(PyList_Check(pyerr));
    Py_ssize_t size = PyList_GET_SIZE(pyerr);
    std::string stringRes;
    for (Py_ssize_t i=0; i<size; ++i)
      stringRes += std::string
          (PyString_AsString(PyList_GET_ITEM(pyerr, i)));
    Py_DecRef(pyerr);


    pyerr  = PyString_FromString(stringRes.c_str());
    err = PyString_AsString(pyerr);
    dgDEBUG(15) << "err: " << err << std::endl;
    Py_DecRef(pyerr);

    // Here if there is a syntax error and
    // and the interpreter input is set to Py_eval_input,
    // it is maybe a statement instead of an expression.
    // Therefore we indicate to re-evaluate the command.
    if (PyErr_GivenExceptionMatches(ptype, PyExc_SyntaxError) &&
        (PythonInputType==Py_eval_input))
    {
      dgDEBUG(15) << "Detected a syntax error " << std::endl;
      lres=false;
    }
    else
      lres=true;

    Py_CLEAR(args);

    PyErr_Clear();
  } else {
    dgDEBUG(15) << "no object generated but no error occured." << std::endl;
  }
  PyObject* stdout_obj = PyRun_String("stdout_catcher.fetch()",
                                      Py_eval_input, globals_,
                                      globals_);
  std::string out("");

  out = PyString_AsString(stdout_obj);
  // Local display for the robot (in debug mode or for the logs)
  if (out.length()!=0)
  { dgDEBUG(15) << std::endl; }
  else { dgDEBUG(15) << "No exception." << std::endl; }
  dgDEBUGOUT(15);
  Py_DecRef(stdout_obj);
  return lres;
}


Interpreter::Interpreter()
{
  // load python dynamic library
  // this is silly, but required to be able to import dl module.
#ifndef WIN32
  dlopen(libpython.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif
  Py_Initialize();
  PyEval_InitThreads();
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
  PyRun_SimpleString("import linecache");

  traceback_format_exception_ = PyDict_GetItemString
      (PyModule_GetDict(PyImport_AddModule("traceback")), "format_exception");
  assert(PyCallable_Check(traceback_format_exception_));
  Py_INCREF(traceback_format_exception_);

  // Allow threads
  _pyState = PyEval_SaveThread();
}

Interpreter::~Interpreter()
{
  PyEval_RestoreThread(_pyState);

  // Ideally, we should call Py_Finalize but this is not really supported by
  // Python.
  // Instead, we merelly remove variables.
  // Code was taken here: https://github.com/numpy/numpy/issues/8097#issuecomment-356683953
  {
    PyObject * poAttrList = PyObject_Dir(mainmod_);
    PyObject * poAttrIter = PyObject_GetIter(poAttrList);
    PyObject * poAttrName;

    while ((poAttrName = PyIter_Next(poAttrIter)) != NULL)
    {
      std::string oAttrName (PyString_AS_STRING(poAttrName));

      // Make sure we don't delete any private objects.
      if (oAttrName.compare(                 0,2,"__")!=0 ||
          oAttrName.compare(oAttrName.size()-2,2,"__")!=0)
      {
        PyObject * poAttr = PyObject_GetAttr(mainmod_, poAttrName);

        // Make sure we don't delete any module objects.
        if (poAttr && poAttr->ob_type != mainmod_->ob_type)
          PyObject_SetAttr(mainmod_, poAttrName, NULL);

        Py_DecRef(poAttr);
      }

      Py_DecRef(poAttrName);
    }

    Py_DecRef(poAttrIter);
    Py_DecRef(poAttrList);
  }

  Py_DECREF(mainmod_);
  Py_DECREF(globals_);
  Py_DECREF(traceback_format_exception_);
  //Py_Finalize();
}

std::string Interpreter::python( const std::string& command )
{
  std::string lerr(""),lout(""),lres("");
  python(command,lres,lout,lerr);
  return lres;
}


void Interpreter::python( const std::string& command, std::string& res,
                          std::string& out, std::string& err)
{
  res = "";
  out = "";
  err = "";

  // Check if the command is not a python comment or empty.
  std::string::size_type iFirstNonWhite = command.find_first_not_of (" \t");
  // Empty command
  if (iFirstNonWhite == std::string::npos) return;
  // Command is a comment. Ignore it.
  if (command[iFirstNonWhite] == '#') return;

  PyEval_RestoreThread(_pyState);

  std::cout << command.c_str() << std::endl;
  PyObject* result = PyRun_String(command.c_str(), Py_eval_input, globals_,
                                  globals_);
  // Check if the result is null.
  if (!result) {

    // Test if this is a syntax error (due to the evaluation of an expression)
    // else just output the problem.
    if (!HandleErr(err,
                   traceback_format_exception_, globals_,
                   Py_eval_input))
    {
      // If this is a statement, re-parse the command.
      result = PyRun_String(command.c_str(), Py_single_input, globals_, globals_);

      // If there is still an error build the appropriate err string.
      if (result == NULL) 
        HandleErr(err,
                  traceback_format_exception_, globals_,
                  Py_single_input);
      else 
        // If there is no error, make sure that the previous error message is erased.
        err="";
    }
    else 
    { dgDEBUG(15) << "Do not try a second time." << std::endl; }
  }

  PyObject* stdout_obj = 0;
  stdout_obj = PyRun_String("stdout_catcher.fetch()",
                            Py_eval_input, globals_,
                            globals_);
  out = PyString_AsString(stdout_obj);
  // Local display for the robot (in debug mode or for the logs)
  if (out.size()!=0)
    std::cout << "Output:" << out << std::endl;
  if (err.size()!=0)
    std::cout << "Error:" << err << std::endl;
  PyObject* result2 = PyObject_Repr(result);
  // If python cannot build a string representation of result
  // then results is equal to NULL. This will trigger a SEGV
  if (result2!=NULL)
  {
    dgDEBUG(15) << "For command :" << command << std::endl;
    res = PyString_AsString(result2);
    dgDEBUG(15) << "Result is: " << res <<std::endl;
    dgDEBUG(15) << "Out is: " << out <<std::endl;
    dgDEBUG(15) << "Err is :" << err << std::endl;
  }
  else
  { dgDEBUG(15) << "Result is empty" << std::endl; }
  Py_DecRef(stdout_obj);
  Py_DecRef(result2);
  Py_DecRef(result);

  _pyState = PyEval_SaveThread();

  return;
}

PyObject* Interpreter::globals()
{
  return globals_;
}

void Interpreter::runPythonFile( std::string filename )
{
  std::string err = "";
  runPythonFile(filename, err);
}

void Interpreter::runPythonFile( std::string filename, std::string& err)
{
  FILE* pFile = fopen( filename.c_str(),"r" );
  if (pFile==0x0)
  {
    err = filename + " cannot be open";
    return;
  }

  PyEval_RestoreThread(_pyState);

  err = "";
  PyObject* pymainContext = globals_;
  PyObject* run = PyRun_FileExFlags(pFile, filename.c_str(),
             Py_file_input, pymainContext,pymainContext, true, NULL);

  if (PyErr_Occurred())
  {
    err = parse_python_exception();
    std::cerr << err << std::endl;;
  }
  Py_DecRef(run);

  _pyState = PyEval_SaveThread();
}

void Interpreter::runMain( void )
{
  const char * argv [] = { "dg-embedded-pysh" };
  PyEval_RestoreThread(_pyState);
  Py_Main(1,const_cast<char**>(argv));
  _pyState = PyEval_SaveThread();
}

std::string Interpreter::processStream(std::istream& stream, std::ostream& os)
{
  char line[10000]; sprintf(line, "%s", "\n");
  std::string command;
  std::streamsize maxSize = 10000;
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

std::string parse_python_exception()
{
  PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
  PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
  std::string ret("Unfetchable Python error");

  if(type_ptr != NULL)
  {
    py::handle<> h_type(type_ptr);
    py::str type_pstr(h_type);
    py::extract<std::string> e_type_pstr(type_pstr);
    if(e_type_pstr.check())
      ret = e_type_pstr();
    else
      ret = "Unknown exception type";
  }

  if(value_ptr != NULL)
  {
    py::handle<> h_val(value_ptr);
    py::str a(h_val);
    py::extract<std::string> returned(a);
    if(returned.check())
      ret +=  ": " + returned();
    else
      ret += std::string(": Unparseable Python error: ");
  }

  if(traceback_ptr != NULL)
  {
    py::handle<> h_tb(traceback_ptr);
    py::object tb(py::import("traceback"));
    py::object fmt_tb(tb.attr("format_tb"));
    py::object tb_list(fmt_tb(h_tb));
    py::object tb_str(py::str("\n").join(tb_list));
    py::extract<std::string> returned(tb_str);
    if(returned.check())
      ret += ": " + returned();
    else
      ret += std::string(": Unparseable Python traceback");
  }
  return ret;
}
} //namespace python
} // namespace dynamicgraph
