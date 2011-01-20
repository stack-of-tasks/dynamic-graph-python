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

#include <dynamic-graph/signal-caster.h>

namespace dynamicgraph {
  namespace python {

    extern PyObject* error;

    namespace signalCaster {
      PyObject* getSignalTypeList(PyObject* /*self*/, PyObject* args)
      {
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
