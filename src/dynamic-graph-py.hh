#ifndef DYNAMIC_GRAPH_PY
#define DYNAMIC_GRAPH_PY

#include <Python.h>
#include <iostream>
#include <sstream>
#include <string>

#include <dynamic-graph/debug.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/signal-base.h>

#include "signal-wrapper.hh"

namespace dynamicgraph {
namespace python {

// Declare functions defined in other source files
namespace signalBase {
PyObject* create(PyObject* self, PyObject* args);
PyObject* createSignalWrapper(PyObject* self, PyObject* args);
PyObject* getTime(PyObject* self, PyObject* args);
PyObject* setTime(PyObject* self, PyObject* args);
PyObject* getName(PyObject* self, PyObject* args);
PyObject* getClassName(PyObject* self, PyObject* args);
PyObject* display(PyObject* self, PyObject* args);
PyObject* displayDependencies(PyObject* self, PyObject* args);
PyObject* getValue(PyObject* self, PyObject* args);
PyObject* setValue(PyObject* self, PyObject* args);
PyObject* recompute(PyObject* self, PyObject* args);
PyObject* unplug(PyObject* self, PyObject* args);
PyObject* isPlugged(PyObject* self, PyObject* args);
PyObject* getPlugged(PyObject* self, PyObject* args);
}  // namespace signalBase
namespace entity {
PyObject* create(PyObject* self, PyObject* args);
PyObject* display(PyObject* self, PyObject* args);
PyObject* display(PyObject* self, PyObject* args);
PyObject* getName(PyObject* self, PyObject* args);
PyObject* getClassName(PyObject* self, PyObject* args);
PyObject* hasSignal(PyObject* self, PyObject* args);
PyObject* getSignal(PyObject* self, PyObject* args);
PyObject* listSignals(PyObject* self, PyObject* args);
PyObject* executeCommand(PyObject* self, PyObject* args);
PyObject* listCommands(PyObject* self, PyObject* args);
PyObject* getCommandDocstring(PyObject* self, PyObject* args);
PyObject* getDocString(PyObject* self, PyObject* args);
PyObject* setLoggerVerbosityLevel(PyObject* self, PyObject* args);
PyObject* getLoggerVerbosityLevel(PyObject* self, PyObject* args);
PyObject* setTimeSample(PyObject* self, PyObject* args);
PyObject* getTimeSample(PyObject* self, PyObject* args);
PyObject* setStreamPrintPeriod(PyObject* self, PyObject* args);
PyObject* getStreamPrintPeriod(PyObject* self, PyObject* args);
}  // namespace entity

namespace factory {
PyObject* getEntityClassList(PyObject* self, PyObject* args);
}
namespace signalCaster {
PyObject* getSignalTypeList(PyObject* self, PyObject* args);
}
namespace pool {
PyObject* writeGraph(PyObject* self, PyObject* args);
PyObject* getEntityList(PyObject* self, PyObject* args);
}  // namespace pool
namespace debug {
PyObject* addLoggerFileOutputStream(PyObject* self, PyObject* args);
PyObject* addLoggerCoutOutputStream(PyObject* self, PyObject* args);
PyObject* closeLoggerFileOutputStream(PyObject* self, PyObject* args);
PyObject* realTimeLoggerSpinOnce(PyObject* self, PyObject* args);
PyObject* realTimeLoggerDestroy(PyObject* self, PyObject* args);
PyObject* realTimeLoggerInstance(PyObject* self, PyObject* args);
}  // namespace debug

struct module_state {
  PyObject* dgpyError;
};

PyObject* plug(PyObject* /*self*/, PyObject* args);

PyObject* enableTrace(PyObject* /*self*/, PyObject* args);

PyObject* error_out(
#if PY_MAJOR_VERSION >= 3
    PyObject* m, PyObject*
#else
    PyObject*, PyObject*
#endif
    );

/**
   \brief List of python functions
*/
__attribute__((unused))
static PyMethodDef dynamicGraphMethods[] = {
    {"w_plug", dynamicgraph::python::plug, METH_VARARGS, "plug an output signal into an input signal"},
    {"enableTrace", dynamicgraph::python::enableTrace, METH_VARARGS, "Enable or disable tracing debug info in a file"},
    // Signals
    {"create_signal_base", dynamicgraph::python::signalBase::create, METH_VARARGS, "create a SignalBase C++ object"},
    {"create_signal_wrapper", dynamicgraph::python::signalBase::createSignalWrapper, METH_VARARGS,
     "create a SignalWrapper C++ object"},
    {"signal_base_get_time", dynamicgraph::python::signalBase::getTime, METH_VARARGS, "Get time of  a SignalBase"},
    {"signal_base_set_time", dynamicgraph::python::signalBase::setTime, METH_VARARGS, "Set time of  a SignalBase"},
    {"signal_base_get_name", dynamicgraph::python::signalBase::getName, METH_VARARGS, "Get the name of a signal"},
    {"signal_base_get_class_name", dynamicgraph::python::signalBase::getClassName, METH_VARARGS,
     "Get the class name of a signal"},
    {"signal_base_display", dynamicgraph::python::signalBase::display, METH_VARARGS, "Print the signal in a string"},
    {"signal_base_display_dependencies", dynamicgraph::python::signalBase::displayDependencies, METH_VARARGS,
     "Print the signal dependencies in a string"},
    {"signal_base_get_value", dynamicgraph::python::signalBase::getValue, METH_VARARGS, "Read the value of a signal"},
    {"signal_base_set_value", dynamicgraph::python::signalBase::setValue, METH_VARARGS, "Set the value of a signal"},
    {"signal_base_recompute", dynamicgraph::python::signalBase::recompute, METH_VARARGS,
     "Recompute the signal at given time"},
    {"signal_base_unplug", dynamicgraph::python::signalBase::unplug, METH_VARARGS, "Unplug the signal"},
    {"signal_base_isPlugged", dynamicgraph::python::signalBase::isPlugged, METH_VARARGS,
     "Whether the signal is plugged"},
    {"signal_base_getPlugged", dynamicgraph::python::signalBase::getPlugged, METH_VARARGS,
     "To which signal the signal is plugged"},
    // Entity
    {"create_entity", dynamicgraph::python::entity::create, METH_VARARGS, "create an Entity C++ object"},
    {"display_entity", dynamicgraph::python::entity::display, METH_VARARGS, "print an Entity C++ object"},
    {"entity_get_name", dynamicgraph::python::entity::getName, METH_VARARGS, "get the name of an Entity"},
    {"entity_get_class_name", dynamicgraph::python::entity::getClassName, METH_VARARGS,
     "get the class name of an Entity"},
    {"entity_has_signal", dynamicgraph::python::entity::hasSignal, METH_VARARGS,
     "return True if the entity has a signal with the given name"},
    {"entity_get_signal", dynamicgraph::python::entity::getSignal, METH_VARARGS, "get signal by name from an Entity"},
    {"entity_list_signals", dynamicgraph::python::entity::listSignals, METH_VARARGS,
     "Return the list of signals of an entity."},
    {"entity_execute_command", dynamicgraph::python::entity::executeCommand, METH_VARARGS, "execute a command"},
    {"entity_list_commands", dynamicgraph::python::entity::listCommands, METH_VARARGS,
     "list the commands of an entity"},
    {"entity_get_command_docstring", dynamicgraph::python::entity::getCommandDocstring, METH_VARARGS,
     "get the docstring of an entity command"},
    {"entity_get_docstring", dynamicgraph::python::entity::getDocString, METH_VARARGS,
     "get the doc string of an entity type"},
    {"factory_get_entity_class_list", dynamicgraph::python::factory::getEntityClassList, METH_VARARGS,
     "return the list of entity classes"},
    {"signal_caster_get_type_list", dynamicgraph::python::signalCaster::getSignalTypeList, METH_VARARGS,
     "return the list of signal type names"},
    {"writeGraph", dynamicgraph::python::pool::writeGraph, METH_VARARGS, "Write the graph of entities in a filename."},
    {"get_entity_list", dynamicgraph::python::pool::getEntityList, METH_VARARGS,
     "return the list of instanciated entities"},
    {"entity_set_logger_verbosity", dynamicgraph::python::entity::setLoggerVerbosityLevel, METH_VARARGS,
     "set the verbosity level of the entity"},
    {"entity_get_logger_verbosity", dynamicgraph::python::entity::getLoggerVerbosityLevel, METH_VARARGS,
     "get the verbosity level of the entity"},
    {"addLoggerFileOutputStream", dynamicgraph::python::debug::addLoggerFileOutputStream, METH_VARARGS,
     "add a output file stream to the logger by filename"},
    {"addLoggerCoutOutputStream", dynamicgraph::python::debug::addLoggerCoutOutputStream, METH_VARARGS,
     "add std::cout as output stream to the logger"},
    {"closeLoggerFileOutputStream", dynamicgraph::python::debug::closeLoggerFileOutputStream, METH_VARARGS,
     "close all the loggers file output streams."},
    {"entity_set_time_sample", dynamicgraph::python::entity::setTimeSample, METH_VARARGS,
     "set the time sample for printing debugging information"},
    {"entity_get_time_sample", dynamicgraph::python::entity::getTimeSample, METH_VARARGS,
     "get the time sample for printing debugging information"},
    {"entity_set_stream_print_period", dynamicgraph::python::entity::setStreamPrintPeriod, METH_VARARGS,
     "set the period at which debugging information are printed"},
    {"entity_get_stream_print_period", dynamicgraph::python::entity::getStreamPrintPeriod, METH_VARARGS,
     "get the period at which debugging information are printed"},
    {"real_time_logger_destroy", dynamicgraph::python::debug::realTimeLoggerDestroy, METH_VARARGS,
     "Destroy the real time logger."},
    {"real_time_logger_spin_once", dynamicgraph::python::debug::realTimeLoggerSpinOnce, METH_VARARGS,
     "Destroy the real time logger."},
    {"real_time_logger_instance", dynamicgraph::python::debug::realTimeLoggerInstance, METH_VARARGS,
     "Starts the real time logger."},
    {"error_out", (PyCFunction)dynamicgraph::python::error_out, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
__attribute__((unused))
static struct PyModuleDef dynamicGraphModuleDef = {
  PyModuleDef_HEAD_INIT,
  "wrap",
  NULL,
  sizeof(struct dynamicgraph::python::module_state),
  dynamicGraphMethods,
  NULL,
  NULL,
  NULL,
  NULL
};
#define GETSTATE(m) ((struct dynamicgraph::python::module_state*)PyModule_GetState(m))
#define DGPYERROR GETSTATE(PyState_FindModule(&dynamicgraph::python::dynamicGraphModuleDef))->dgpyError
#define INITERROR return NULL
#else
__attribute__((unused))
  static struct module_state _state;
#define GETSTATE(m) (&dynamicgraph::python::_state)
#define DGPYERROR dynamicgraph::python::_state.dgpyError
#define INITERROR return
#endif


}  // namespace python
}  // namespace dynamicgraph

#endif
