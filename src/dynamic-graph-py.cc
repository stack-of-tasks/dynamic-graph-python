/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
#include <sstream>
#include <string>

#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/interpreter-helper.h>
#include <dynamic-graph/functions.h>

namespace dynamicgraph {
  namespace python {

    // Declare functions defined in other source files
    namespace signalBase {
      extern PyObject* create(PyObject* self, PyObject* args);
      extern PyObject* getTime(PyObject* self, PyObject* args);
      extern PyObject* getValue(PyObject* self, PyObject* args);
      extern PyObject* setValue(PyObject* self, PyObject* args);
    }
    namespace entity {
      extern PyObject* create(PyObject* self, PyObject* args);
      extern PyObject* getName(PyObject* self, PyObject* args);
      extern PyObject* getSignal(PyObject* self, PyObject* args);
      extern PyObject* displaySignals(PyObject* self, PyObject* args);
      extern PyObject* executeCommand(PyObject* self, PyObject* args);
    }

    namespace factory {
      PyObject* getEntityClassList(PyObject* self, PyObject* args);
    }
    PyObject* error;

    static dynamicgraph::InterpreterHelper interpreter;

    /**
       \brief plug a signal into another one.
    */
    PyObject*
    plug(PyObject* self, PyObject* args)
    {
      char* objOut = NULL;
      char* objIn = NULL;
      char* sigOut = NULL;
      char* sigIn = NULL;
      if (!PyArg_ParseTuple(args,"ssss", &objIn, &sigIn, &objOut, &sigOut))
	return NULL;

      std::ostringstream os;
      try {
	interpreter.cmdPlug(objIn, sigIn, objOut, sigOut, os);
      } catch (dynamicgraph::ExceptionFactory& exc) {
	PyErr_SetString(error, exc.getStringMessage().c_str());
	return NULL;
      }

      return Py_BuildValue("");
    }

    PyObject*
    enableTrace(PyObject* self, PyObject* args)
    {
      char* trueFalse = NULL;
      char* filename = NULL;
      std::stringstream ss;
      std::ostringstream os;

      if (PyArg_ParseTuple(args,"ss", &trueFalse, &filename)) {
	ss << std::string(trueFalse) << " " << std::string(filename);
      } else if (PyArg_ParseTuple(args,"s", &trueFalse)) {
	ss << std::string(trueFalse);
      } else {
	return NULL;
      }

      std::istringstream cmdArg(ss.str());
      try {
	ShellFunctions::cmdEnableTrace(std::string("debugtrace"), cmdArg, os);
      } catch (dynamicgraph::ExceptionFactory& exc) {
	PyErr_SetString(error, exc.getStringMessage().c_str());
	return NULL;
      }

      return Py_BuildValue("");
    }
  }
}

/**
   \brief List of python functions
*/
static PyMethodDef dynamicGraphMethods[] = {
  {"w_plug",  dynamicgraph::python::plug, METH_VARARGS,
   "plug an output signal into an input signal"},
  {"w_debugtrace",  dynamicgraph::python::enableTrace, METH_VARARGS,
   "Enable or disable tracing debug info in a file"},
  // Signals
  {"create_signal_base", dynamicgraph::python::signalBase::create, METH_VARARGS,
   "create a SignalBase C++ object"},
  {"signalbase_get_time", dynamicgraph::python::signalBase::getTime,
   METH_VARARGS, "Get time of  a SignalBase"},
  {"signal_base_get_value", dynamicgraph::python::signalBase::getValue,
   METH_VARARGS, "Read the value of a signal"},
  {"signal_base_set_value", dynamicgraph::python::signalBase::setValue,
   METH_VARARGS, "Set the value of a signal"},
  // Entity
  {"create_entity", dynamicgraph::python::entity::create, METH_VARARGS,
   "create an Entity C++ object"},
  {"entity_get_name", dynamicgraph::python::entity::getName, METH_VARARGS,
   "get the name of an Entity"},
  {"entity_get_signal", dynamicgraph::python::entity::getSignal, METH_VARARGS,
   "get signal by name from an Entity"},
  {"entity_display_signals", dynamicgraph::python::entity::displaySignals,
   METH_VARARGS,
   "Display the list of signals of an entity in standard output"},
  {"entity_execute_command",
   dynamicgraph::python::entity::executeCommand,
   METH_VARARGS,
   "execute a command"},
  {"factory_get_entity_class_list",
   dynamicgraph::python::factory::getEntityClassList,
   METH_VARARGS,
   "return the list of entity classes"},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initwrap(void)
{
    PyObject *m;

    m = Py_InitModule("wrap", dynamicGraphMethods);
    if (m == NULL)
        return;

    dynamicgraph::python::error =
      PyErr_NewException("dynamic_graph.wrap.error", NULL, NULL);
    Py_INCREF(dynamicgraph::python::error);
    PyModule_AddObject(m, "error", dynamicgraph::python::error);
}
