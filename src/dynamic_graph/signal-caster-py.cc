// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <Python.h>
#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-caster.h>

#include "dynamic-graph/python/dynamic-graph-py.hh"

namespace dynamicgraph {
namespace python {

namespace signalCaster {
PyObject* getSignalTypeList(PyObject* /*self*/, PyObject* args) {
  if (!PyArg_ParseTuple(args, "")) return NULL;
  std::vector<std::string> typeList = dynamicgraph::SignalCaster::getInstance()->listTypenames();
  Py_ssize_t typeNumber = typeList.size();
  // Build a tuple object
  PyObject* typeTuple = PyTuple_New(typeNumber);

  for (Py_ssize_t iType = 0; iType < typeNumber; ++iType) {
    PyObject* className = Py_BuildValue("s", typeList[iType].c_str());
    PyTuple_SetItem(typeTuple, iType, className);
  }

  return Py_BuildValue("O", typeTuple);
}
}  // namespace signalCaster
}  // namespace python
}  // namespace dynamicgraph
