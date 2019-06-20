// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph-python is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// dynamic-graph-python is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Lesser Public License for more details.  You should
// have received a copy of the GNU Lesser General Public License along
// with dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#include <Python.h>
#include <iostream>
#include <sstream>
#include <string>

#include <dynamic-graph/debug.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/signal-base.h>

#include "exception.hh"
#include "signal-wrapper.hh"

namespace dynamicgraph {
  namespace python {

    // Declare functions defined in other source files
    namespace signalBase {
      extern PyObject* create(PyObject* self, PyObject* args);
      extern PyObject* createSignalWrapper(PyObject* self, PyObject* args);
      extern PyObject* getTime(PyObject* self, PyObject* args);
      extern PyObject* setTime(PyObject* self, PyObject* args);
      extern PyObject* getName(PyObject* self, PyObject* args);
      extern PyObject* getClassName(PyObject* self, PyObject* args);
      extern PyObject* display(PyObject* self, PyObject* args);
      extern PyObject* displayDependencies(PyObject* self, PyObject* args);
      extern PyObject* getValue(PyObject* self, PyObject* args);
      extern PyObject* setValue(PyObject* self, PyObject* args);
      extern PyObject* recompute(PyObject* self, PyObject* args);
      extern PyObject* unplug(PyObject* self, PyObject* args);
      extern PyObject* isPlugged(PyObject* self, PyObject* args);
      extern PyObject* getPlugged(PyObject* self, PyObject* args);
    }
    namespace entity {
      extern PyObject* create(PyObject* self, PyObject* args);
      extern PyObject* display(PyObject* self, PyObject* args);
      extern PyObject* display(PyObject* self, PyObject* args);
      extern PyObject* getName(PyObject* self, PyObject* args);
      extern PyObject* getClassName(PyObject* self, PyObject* args);
      extern PyObject* hasSignal(PyObject* self, PyObject* args);
      extern PyObject* getSignal(PyObject* self, PyObject* args);
      extern PyObject* listSignals(PyObject* self, PyObject* args);
      extern PyObject* executeCommand(PyObject* self, PyObject* args);
      extern PyObject* listCommands(PyObject* self, PyObject* args);
      extern PyObject* getCommandDocstring(PyObject* self, PyObject* args);
      extern PyObject* getDocString(PyObject* self, PyObject* args);
      extern PyObject* setLoggerVerbosityLevel(PyObject*self, PyObject *args);
      extern PyObject* getLoggerVerbosityLevel(PyObject *self, PyObject *args);
      extern PyObject* setTimeSample(PyObject*self, PyObject *args);
      extern PyObject* getTimeSample(PyObject *self, PyObject *args);
      extern PyObject* setStreamPrintPeriod(PyObject*self, PyObject *args);
      extern PyObject* getStreamPrintPeriod(PyObject *self, PyObject *args);
    }

    namespace factory {
      extern PyObject* getEntityClassList(PyObject* self, PyObject* args);
    }
    namespace signalCaster {
      extern PyObject* getSignalTypeList(PyObject* self, PyObject* args);
    }
    namespace pool {
      extern PyObject* writeGraph (PyObject* self, PyObject* args);
      extern PyObject* getEntityList(PyObject* self, PyObject* args);
    }
    namespace debug {
      extern PyObject* addLoggerFileOutputStream(PyObject* self, PyObject* args);
      extern PyObject* addLoggerCoutOutputStream(PyObject* self, PyObject* args);
      extern PyObject* closeLoggerFileOutputStream(PyObject* self, PyObject* args);
      extern PyObject* realTimeLoggerSpinOnce(PyObject* self, PyObject* args);
      extern PyObject* realTimeLoggerDestroy(PyObject* self, PyObject* args);
      extern PyObject* realTimeLoggerInstance(PyObject* self, PyObject* args);
    }

    PyObject* dgpyError;

    /**
       \brief plug a signal into another one.
    */
    PyObject*
    plug(PyObject* /*self*/, PyObject* args)
    {
      PyObject* objOut = NULL;
      PyObject* objIn = NULL;
      void* pObjOut;
      void* pObjIn;

      if (!PyArg_ParseTuple(args,"OO", &objOut, &objIn))
	return NULL;

      if (!PyCObject_Check(objOut)) {
	PyErr_SetString(PyExc_TypeError,
			"first argument should be a pointer to"
			" signalBase<int>.");
	return NULL;
      }
      if (!PyCObject_Check(objIn)) {
	PyErr_SetString(PyExc_TypeError,
			"second argument should be a pointer to"
			" signalBase<int>.");
	return NULL;
      }

      pObjIn = PyCObject_AsVoidPtr(objIn);
	SignalBase<int>* signalIn = (SignalBase<int>*)pObjIn;
      pObjOut = PyCObject_AsVoidPtr(objOut);
	SignalBase<int>* signalOut = (SignalBase<int>*)pObjOut;
      std::ostringstream os;

      try {
	signalIn->plug(signalOut);
      } CATCH_ALL_EXCEPTIONS();
      return Py_BuildValue("");
    }

    PyObject*
    enableTrace(PyObject* /*self*/, PyObject* args)
    {
      PyObject* boolean;
      char* filename = NULL;

      if (PyArg_ParseTuple(args,"Os", &boolean, &filename)) {
	if (!PyBool_Check(boolean)) {
	  PyErr_SetString(PyExc_TypeError, "enableTrace takes as first "
			  "argument True or False,\n""           and as "
			  "second argument a filename.");
	  return NULL;
	}
	if (PyObject_IsTrue(boolean)) {
	  try {
	    DebugTrace::openFile(filename);
	  } CATCH_ALL_EXCEPTIONS();
	} else {
	  try {
	    DebugTrace::closeFile(filename);
	  } CATCH_ALL_EXCEPTIONS();
	}
      } else {
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
  {"enableTrace",  dynamicgraph::python::enableTrace, METH_VARARGS,
   "Enable or disable tracing debug info in a file"},
  // Signals
  {"create_signal_base", dynamicgraph::python::signalBase::create, METH_VARARGS,
   "create a SignalBase C++ object"},
  {"create_signal_wrapper", dynamicgraph::python::signalBase::createSignalWrapper, METH_VARARGS,
   "create a SignalWrapper C++ object"},
  {"signal_base_get_time", dynamicgraph::python::signalBase::getTime,
   METH_VARARGS, "Get time of  a SignalBase"},
  {"signal_base_set_time", dynamicgraph::python::signalBase::setTime,
   METH_VARARGS, "Set time of  a SignalBase"},
  {"signal_base_get_name", dynamicgraph::python::signalBase::getName,
   METH_VARARGS, "Get the name of a signal"},
  {"signal_base_get_class_name", dynamicgraph::python::signalBase::getClassName,
   METH_VARARGS, "Get the class name of a signal"},
  {"signal_base_display", dynamicgraph::python::signalBase::display,
   METH_VARARGS, "Print the signal in a string"},
  {"signal_base_display_dependencies", dynamicgraph::python::signalBase::displayDependencies,
   METH_VARARGS, "Print the signal dependencies in a string"},
  {"signal_base_get_value", dynamicgraph::python::signalBase::getValue,
   METH_VARARGS, "Read the value of a signal"},
  {"signal_base_set_value", dynamicgraph::python::signalBase::setValue,
   METH_VARARGS, "Set the value of a signal"},
  {"signal_base_recompute", dynamicgraph::python::signalBase::recompute,
   METH_VARARGS, "Recompute the signal at given time"},
  {"signal_base_unplug", dynamicgraph::python::signalBase::unplug,
   METH_VARARGS, "Unplug the signal"},
  {"signal_base_isPlugged", dynamicgraph::python::signalBase::isPlugged,
   METH_VARARGS, "Whether the signal is plugged"},
  {"signal_base_getPlugged", dynamicgraph::python::signalBase::getPlugged,
   METH_VARARGS, "To which signal the signal is plugged"},
  // Entity
  {"create_entity", dynamicgraph::python::entity::create, METH_VARARGS,
   "create an Entity C++ object"},
  {"display_entity", dynamicgraph::python::entity::display, METH_VARARGS,
   "print an Entity C++ object"},
  {"entity_get_name", dynamicgraph::python::entity::getName, METH_VARARGS,
   "get the name of an Entity"},
  {"entity_get_class_name", dynamicgraph::python::entity::getClassName, METH_VARARGS,
   "get the class name of an Entity"},
  {"entity_has_signal", dynamicgraph::python::entity::hasSignal, METH_VARARGS,
   "return True if the entity has a signal with the given name"},
  {"entity_get_signal", dynamicgraph::python::entity::getSignal, METH_VARARGS,
   "get signal by name from an Entity"},
  {"entity_list_signals", dynamicgraph::python::entity::listSignals,
   METH_VARARGS,
   "Return the list of signals of an entity."},
  {"entity_execute_command",
   dynamicgraph::python::entity::executeCommand,
   METH_VARARGS,
   "execute a command"},
  {"entity_list_commands",
   dynamicgraph::python::entity::listCommands,
   METH_VARARGS,
   "list the commands of an entity"},
  {"entity_get_command_docstring",
   dynamicgraph::python::entity::getCommandDocstring,
   METH_VARARGS,
   "get the docstring of an entity command"},
  {"entity_get_docstring",
   dynamicgraph::python::entity::getDocString,
   METH_VARARGS,
   "get the doc string of an entity type"},
  {"factory_get_entity_class_list",
   dynamicgraph::python::factory::getEntityClassList,
   METH_VARARGS,
   "return the list of entity classes"},
  {"signal_caster_get_type_list",
   dynamicgraph::python::signalCaster::getSignalTypeList,
   METH_VARARGS,
   "return the list of signal type names"},
  {"writeGraph",
   dynamicgraph::python::pool::writeGraph,
   METH_VARARGS,
   "Write the graph of entities in a filename."},
  {"get_entity_list",
   dynamicgraph::python::pool::getEntityList,
   METH_VARARGS,
   "return the list of instanciated entities"},
  {"entity_set_logger_verbosity",
   dynamicgraph::python::entity::setLoggerVerbosityLevel,
   METH_VARARGS,
   "set the verbosity level of the entity"},
  {"entity_get_logger_verbosity",
   dynamicgraph::python::entity::getLoggerVerbosityLevel,
   METH_VARARGS,
   "get the verbosity level of the entity"},
  {"addLoggerFileOutputStream",
   dynamicgraph::python::debug::addLoggerFileOutputStream,
   METH_VARARGS,
   "add a output file stream to the logger by filename"},
  {"addLoggerCoutOutputStream",
   dynamicgraph::python::debug::addLoggerCoutOutputStream,
   METH_VARARGS,
   "add std::cout as output stream to the logger"},
  {"closeLoggerFileOutputStream",
   dynamicgraph::python::debug::closeLoggerFileOutputStream,
   METH_VARARGS,
   "close all the loggers file output streams."},
  {"entity_set_time_sample",
   dynamicgraph::python::entity::setTimeSample,
   METH_VARARGS,
   "set the time sample for printing debugging information"},
  {"entity_get_time_sample",
   dynamicgraph::python::entity::getTimeSample,
   METH_VARARGS,
   "get the time sample for printing debugging information"},
  {"entity_set_stream_print_period",
   dynamicgraph::python::entity::setStreamPrintPeriod,
   METH_VARARGS,
   "set the period at which debugging information are printed"},
  {"entity_get_stream_print_period",
   dynamicgraph::python::entity::getStreamPrintPeriod,
   METH_VARARGS,
   "get the period at which debugging information are printed"},
  {"real_time_logger_destroy",
   dynamicgraph::python::debug::realTimeLoggerDestroy,
   METH_VARARGS,
   "Destroy the real time logger."},
  {"real_time_logger_spin_once",
   dynamicgraph::python::debug::realTimeLoggerSpinOnce,
   METH_VARARGS,
   "Destroy the real time logger."},
  {"real_time_logger_instance",
   dynamicgraph::python::debug::realTimeLoggerInstance,
   METH_VARARGS,
   "Starts the real time logger."},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initwrap(void)
{
    PyObject *m;

    m = Py_InitModule("wrap", dynamicGraphMethods);
    if (m == NULL)
        return;

    std::string msg("dynamic_graph.error");

    dynamicgraph::python::dgpyError =
      PyErr_NewException(const_cast<char*>(msg.c_str()), NULL, NULL);
    Py_INCREF(dynamicgraph::python::dgpyError);
    PyModule_AddObject(m, "error", dynamicgraph::python::dgpyError);
}
