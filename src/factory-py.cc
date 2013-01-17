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
#include <dynamic-graph/factory.h>

using dynamicgraph::Entity;
using dynamicgraph::ExceptionAbstract;

namespace dynamicgraph {
  namespace python {

    namespace factory {

      /**
	 \brief Get name of entity
      */
      PyObject* getEntityClassList(PyObject* /*self*/, PyObject* args)
      {
	if (!PyArg_ParseTuple(args, ""))
	  return NULL;

	std::vector <std::string> classNames;
	dynamicgraph::FactoryStorage::getInstance()
	  ->listEntities(classNames);

	Py_ssize_t classNumber = classNames.size();
	// Build a tuple object
	PyObject* classTuple = PyTuple_New(classNumber);
	
	for (Py_ssize_t iEntity = 0;
	     iEntity < (Py_ssize_t)classNames.size(); ++iEntity)
	  {
	    PyObject* className = 
	      Py_BuildValue("s", classNames[iEntity].c_str());
	    PyTuple_SetItem(classTuple, iEntity, className);
	  }

	return Py_BuildValue("O", classTuple);
      }
      
    } // namespace factory
  } // namespace python
} // namespace dynamicgraph
