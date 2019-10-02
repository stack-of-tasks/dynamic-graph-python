// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <Python.h>
#include <iostream>
#include <dynamic-graph/factory.h>

#include "dynamic-graph-py.hh"

using dynamicgraph::Entity;
using dynamicgraph::ExceptionAbstract;

namespace dynamicgraph {
namespace python {

namespace factory {

/**
   \brief Get name of entity
*/
PyObject* getEntityClassList(PyObject* /*self*/, PyObject* args) {
  if (!PyArg_ParseTuple(args, "")) return NULL;

  std::vector<std::string> classNames;
  dynamicgraph::FactoryStorage::getInstance()->listEntities(classNames);

  Py_ssize_t classNumber = classNames.size();
  // Build a tuple object
  PyObject* classTuple = PyTuple_New(classNumber);

  for (Py_ssize_t iEntity = 0; iEntity < (Py_ssize_t)classNames.size(); ++iEntity) {
    PyObject* className = Py_BuildValue("s", classNames[iEntity].c_str());
    PyTuple_SetItem(classTuple, iEntity, className);
  }

  return Py_BuildValue("O", classTuple);
}

}  // namespace factory
}  // namespace python
}  // namespace dynamicgraph
