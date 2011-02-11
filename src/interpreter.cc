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
#include "dynamic-graph/python/interpreter.hh"
#include "link-to-python.hh"

// Python initialization commands
namespace dynamicgraph {
  namespace python {
    static const std::string pythonPrefix[5] = {
      "import traceback\n",
      "def display(s): return str(s) if not s is None else None"
    };
  }
}

using dynamicgraph::python::Interpreter;
using dynamicgraph::python::libpython;

Interpreter::Interpreter()
{
  // load python dynamic library
  // this is silly, but required to be able to import dl module.
  dlopen(libpython.c_str(), RTLD_LAZY | RTLD_GLOBAL);
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
  PyObject* result = PyRun_String(command.c_str(), Py_eval_input, globals_,
				  globals_);
  if (!result) {
    PyErr_Clear();
    result = PyRun_String(command.c_str(), Py_single_input, globals_,
				  globals_);
  }
  if (result == NULL) {
    if (PyErr_Occurred()) {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      if (ptraceback == NULL) {
	ptraceback = Py_None;
      }
      PyObject* args = PyTuple_New(3);
      PyTuple_SET_ITEM(args, 0, ptype);
      PyTuple_SET_ITEM(args, 1, pvalue);
      PyTuple_SET_ITEM(args, 2, ptraceback);
      result = PyObject_CallObject(traceback_format_exception_, args);
      assert(PyList_Check(result));
      unsigned int size = PyList_GET_SIZE(result);
      std::string stringRes;
      for (unsigned int i=0; i<size; i++) {
	stringRes += std::string(PyString_AsString(PyList_GET_ITEM(result, i)));
      }
      result  = PyString_FromString(stringRes.c_str());
      PyErr_Clear();
    } else {
      std::cout << "Result is NULL but no error occurred." << std::endl;
    }
  } else {
    result = PyObject_Str(result);
  }
  std::string value = PyString_AsString(result);
  return value;
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
