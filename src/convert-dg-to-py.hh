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
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/value.h>
#include <dynamic-graph/python/exception-python.hh>

namespace dynamicgraph {
  namespace python {
    namespace convert {

      command::Value pythonToValue(PyObject* pyObject,
				   const command::Value::Type& valueType);
      PyObject* vectorToPython(const Vector& vector);
      PyObject* matrixToPython(const  ::dynamicgraph::Matrix& matrix);
      PyObject* valueToPython(const  ::dynamicgraph::command::Value& value);

    } // namespace dynamicgraph
  } // namespace python
}
