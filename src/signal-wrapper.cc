// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include "signal-wrapper.hh"

#include <Python.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/command-bind.h>

namespace dynamicgraph {
namespace python {
namespace signalWrapper {
void convert(PyObject* o, bool& v) { v = (o == Py_True); }
void convert(PyObject* o, int& v) { v = (int)PyInt_AS_LONG(o); }
void convert(PyObject* o, float& v) { v = (float)PyFloat_AS_DOUBLE(o); }
void convert(PyObject* o, double& v) { v = PyFloat_AS_DOUBLE(o); }
void convert(PyObject* o, Vector& v) {
  v.resize(PyTuple_Size(o));
  for (int i = 0; i < v.size(); ++i) convert(PyTuple_GetItem(o, i), v[i]);
}
}  // namespace signalWrapper

PythonSignalContainer::PythonSignalContainer(const std::string& name) : Entity(name) {
  std::string docstring;

  docstring =
      "    \n"
      "    Remove a signal\n"
      "    \n"
      "      Input:\n"
      "        - name of the signal\n"
      "    \n";
  addCommand("rmSignal", command::makeCommandVoid1(*this, &PythonSignalContainer::rmSignal, docstring));
}

void PythonSignalContainer::signalRegistration(const SignalArray<int>& signals) {
  Entity::signalRegistration(signals);
}

void PythonSignalContainer::rmSignal(const std::string& name) { Entity::signalDeregistration(name); }

DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(PythonSignalContainer, "PythonSignalContainer");

template <class T, class Time>
bool SignalWrapper<T, Time>::checkCallable(PyObject* c, std::string& error) {
  if (PyCallable_Check(c) == 0) {
    PyObject* str = PyObject_Str(c);
    error = PyString_AsString(str);
    error += " is not callable";
    Py_DECREF(str);
    return false;
  }
  return true;
}

template class SignalWrapper<bool, int>;
template class SignalWrapper<int, int>;
template class SignalWrapper<float, int>;
template class SignalWrapper<double, int>;
template class SignalWrapper<Vector, int>;
}  // namespace python
}  // namespace dynamicgraph
