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
	} catch (const std::exception& exc) {
	  PyErr_SetString(dgpyError, exc.what());
	  return NULL;
	}
	return Py_BuildValue ("");
      }
    } // python
  } // dynamicgraph
} // namespace pool
