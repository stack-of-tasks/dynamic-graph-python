/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-caster.h>

namespace dynamicgraph {
  namespace python {

    extern PyObject* error;

    namespace signalCaster {
      PyObject* getSignalTypeList(PyObject* self, PyObject* args)
      {
	void * pointer = NULL;
	PyObject* object = NULL;

	if (!PyArg_ParseTuple(args, ""))
	  return NULL;
	std::vector<std::string> typeList =
	  dynamicgraph::g_caster.listTypenames();
	unsigned int typeNumber = typeList.size();
	// Build a tuple object
	PyObject* typeTuple = PyTuple_New(typeNumber);

	for (unsigned int iType = 0; iType < typeNumber; iType++) {
	  PyObject* className = Py_BuildValue("s", typeList[iType].c_str());
	  PyTuple_SetItem(typeTuple, iType, className);
	}

	return Py_BuildValue("O", typeTuple);
      }
    } //namespace signalCaster
  } // namespace dynamicgraph
} // namespace python
