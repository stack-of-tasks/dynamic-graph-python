// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <Python.h>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/value.h>
#include <dynamic-graph/python/exception-python.hh>

namespace dynamicgraph {
namespace python {
namespace convert {

command::Value pythonToValue(PyObject* pyObject, const command::Value::Type& valueType);
PyObject* vectorToPython(const Vector& vector);
PyObject* matrixToPython(const ::dynamicgraph::Matrix& matrix);
PyObject* matrix4dToPython(const Eigen::Matrix4d& matrix);
PyObject* valueToPython(const ::dynamicgraph::command::Value& value);

}  // namespace convert
}  // namespace python
}  // namespace dynamicgraph
