// Copyright 2011, 2012, Florent Lamiraux, LAAS-CNRS.
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
#include <dynamic-graph/pool.h>
#include <dynamic-graph/entity.h>
#include <vector>
#include "exception.hh"

namespace dynamicgraph {
  namespace python {

    extern PyObject* dgpyError;
    namespace pool {

      PyObject* writeGraph (PyObject* /*self*/, PyObject* args)
      {
	char* filename;
	if (!PyArg_ParseTuple(args, "s", &filename))
	  return NULL;
	try {
	  PoolStorage::getInstance()->writeGraph (filename);
	} CATCH_ALL_EXCEPTIONS();
	return Py_BuildValue ("");
      }

      /**
	 \brief Get list of entities
      */
      PyObject* getEntityList(PyObject* /*self*/, PyObject* args)
      {
	if (!PyArg_ParseTuple(args, ""))
	  return NULL;

	std::vector <std::string> entityNames;
	try {
	  const PoolStorage::Entities & listOfEntities=
	    dynamicgraph::PoolStorage::getInstance()
	    ->getEntityMap();

	  Py_ssize_t classNumber = listOfEntities.size();
	  // Build a tuple object
	  PyObject* classTuple = PyTuple_New(classNumber);

	  Py_ssize_t iEntity = 0;
	  for (PoolStorage::Entities::const_iterator entity_it =
		 listOfEntities.begin();
	       entity_it != listOfEntities.end();
	       ++entity_it)
	    {
	      const std::string & aname = entity_it->second->getName();

	      PyObject* className =
		Py_BuildValue("s", aname.c_str());
	      PyTuple_SetItem(classTuple, iEntity, className);
	      iEntity++;
	    }
	  return Py_BuildValue("O", classTuple);
	} CATCH_ALL_EXCEPTIONS();
	return NULL;
      }


    } // python
  } // namespace pool
} // dynamicgraph
