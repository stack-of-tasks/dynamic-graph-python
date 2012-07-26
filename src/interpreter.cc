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
      "sys.stdout = stdout_catcher"
    };
  }
}

using dynamicgraph::python::Interpreter;
using dynamicgraph::python::libpython;

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
    }
    PyObject* args = PyTuple_New(3);
    PyTuple_SET_ITEM(args, 0, ptype);
    PyTuple_SET_ITEM(args, 1, pvalue);
    PyTuple_SET_ITEM(args, 2, ptraceback);
    pyerr = PyObject_CallObject(traceback_format_exception, args);
    assert(PyList_Check(pyerr));
    unsigned int size = PyList_GET_SIZE(pyerr);
    std::string stringRes;
    for (unsigned int i=0; i<size; i++) {
      stringRes += std::string(PyString_AsString(PyList_GET_ITEM(pyerr, i)));
    }
    pyerr  = PyString_FromString(stringRes.c_str());
    err = PyString_AsString(pyerr);
    dgDEBUG(15) << "err: " << err << std::endl;

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
  traceback_format_exception_ = PyDict_GetItemString
    (PyModule_GetDict(PyImport_AddModule("traceback")), "format_exception");
  assert(PyCallable_Check(traceback_format_exception_));
  Py_INCREF(traceback_format_exception_);
}

Interpreter::~Interpreter()
{
  //Py_DECREF(mainmod_);
  //Py_DECREF(globals_);
  //Py_DECREF(traceback_format_exception_);
  Py_Finalize();
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

  PyObject* stdout_obj = PyRun_String("stdout_catcher.fetch()",
                                      Py_eval_input, globals_,
                                      globals_);
  out = PyString_AsString(stdout_obj);
  // Local display for the robot (in debug mode or for the logs)
  std::cout << out;
  result = PyObject_Repr(result);
  // If python cannot build a string representation of result
  // then results is equal to NULL. This will trigger a SEGV
  if (result!=NULL)
    {
      dgDEBUG(15) << "For command :" << command << std::endl;
      res = PyString_AsString(result);
      dgDEBUG(15) << "Result is: " << res <<std::endl;
      dgDEBUG(15) << "Out is: " << out <<std::endl;
      dgDEBUG(15) << "Err is :" << err << std::endl;
    }
  else 
    { dgDEBUG(15) << "Result is empty" << std::endl; }
  return;
}

PyObject* Interpreter::globals()
{
  return globals_;
}

void Interpreter::runPythonFile( std::string filename )
{
  PyObject* pymainContext = globals_;
  PyRun_File(fopen( filename.c_str(),"r" ), filename.c_str(),
	     Py_file_input, pymainContext,pymainContext);
  if (PyErr_Occurred())
    {
      std::cout << "Error occures..." << std::endl;
      PyErr_Print();
    }
}

void Interpreter::runMain( void )
{
  const char * argv [] = { "dg-embedded-pysh" };
  Py_Main(1,const_cast<char**>(argv));
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
