/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
#include <dynamic-graph/factory.h>

using dynamicgraph::Entity;
using dynamicgraph::ExceptionAbstract;

namespace dynamicgraph {
  namespace python {
    
    extern PyObject* error;

    namespace factory {

      /**
	 \brief Get name of entity
      */
      PyObject* getEntityClassList(PyObject* self, PyObject* args)
      {
	if (!PyArg_ParseTuple(args, ""))
	  return NULL;

	std::vector <std::string> classNames;
	dynamicgraph::g_factory.listEntities(classNames);
	unsigned int classNumber = classNames.size();
	// Build a tuple object
	PyObject* classTuple = PyTuple_New(classNumber);
	
	for (unsigned int iEntity = 0; iEntity < classNames.size(); iEntity++) {
	  PyObject* className = Py_BuildValue("s", classNames[iEntity].c_str());
	  PyTuple_SetItem(classTuple, iEntity, className);
	}

	return Py_BuildValue("O", classTuple);
      }
      
    } // namespace factory
  } // namespace python
} // namespace dynamicgraph
