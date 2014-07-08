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

using namespace boost::python;

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
  "sys.stdout = stdout_catcher"
};
}
}

namespace dynamicgraph {
namespace python {

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

  
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

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
  PyRun_SimpleString("import linecache");

  traceback_format_exception_ = PyDict_GetItemString
      (PyModule_GetDict(PyImport_AddModule("traceback")), "format_exception");
  assert(PyCallable_Check(traceback_format_exception_));
  Py_INCREF(traceback_format_exception_);

  PyGILState_Release(gstate);
}

Interpreter::~Interpreter()
{

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  //Py_DECREF(mainmod_);
  //Py_DECREF(globals_);
  //Py_DECREF(traceback_format_exception_);
  Py_Finalize();

  PyGILState_Release(gstate);
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

  std::cout << command.c_str() << std::endl;

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

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

  PyGILState_Release(gstate);

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
  err = "";

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  PyObject* pymainContext = globals_;
  PyObject* run = PyRun_FileExFlags(fopen( filename.c_str(),"r" ), filename.c_str(),
             Py_file_input, pymainContext,pymainContext, true, NULL);
  if (PyErr_Occurred())
  {
    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);

    handle<> hTraceback(ptraceback);
    object traceback(hTraceback);

    //Extract error message
    std::string strErrorMessage = extract<std::string>(pvalue);
    std::ostringstream errstream;

    //TODO does not work for now for a single command.
    do
      {
	//Extract line number (top entry of call stack)
	// if you want to extract another levels of call stack
	// also process traceback.attr("tb_next") recurently
	long lineno = extract<long> (traceback.attr("tb_lineno"));
	std::string filename = extract<std::string>
	  (traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
	std::string funcname = extract<std::string>
	  (traceback.attr("tb_frame").attr("f_code").attr("co_name"));
	errstream << " File \"" << filename  <<"\", line "
		  << lineno << ", in "<< funcname << std::endl;

	// get the corresponding line.
	std::ostringstream cmd;
	cmd << "linecache.getline('"<<filename<<"', "<<lineno <<")";
	PyObject* line_obj = PyRun_String(cmd.str().c_str(),
					  Py_eval_input, globals_, globals_);
	std::string line = PyString_AsString(line_obj);
	Py_DecRef(line_obj);

	// remove the spaces at the beginning of the line.
	size_t index = line.find_first_not_of (" \t");
	errstream << "  " << line.substr(index, line.size()-index);
	
	// go to the next line.
	traceback = traceback.attr("tb_next");
      }
    while (traceback);

    // recreate the error message
    errstream << strErrorMessage << std::endl;
    err =errstream.str();
    std::cerr << err;
  }

  Py_DecRef(run);

  PyGILState_Release(gstate);

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
} //namespace python
} // namespace dynamicgraph
