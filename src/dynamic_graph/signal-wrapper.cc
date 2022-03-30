// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include "dynamic-graph/python/signal-wrapper.hh"

#include <dynamic-graph/command-bind.h>
#include <dynamic-graph/factory.h>

namespace dynamicgraph {
namespace python {
void PythonSignalContainer::signalRegistration(
    const SignalArray<int>& signals) {
  Entity::signalRegistration(signals);
}

void PythonSignalContainer::rmSignal(const std::string& name) {
  Entity::signalDeregistration(name);
}

DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(PythonSignalContainer,
                                   "PythonSignalContainer");

template <class T, class Time>
bool SignalWrapper<T, Time>::checkCallable(pyobject c, std::string& error) {
  if (PyCallable_Check(c.ptr()) == 0) {
    error = boost::python::extract<std::string>(c.attr("__str__")());
    error += " is not callable";
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
