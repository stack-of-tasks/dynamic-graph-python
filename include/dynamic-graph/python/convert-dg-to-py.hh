// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <boost/python.hpp>

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/value.h>

namespace dynamicgraph {
namespace python {
namespace convert {

command::Value toValue(boost::python::object o, const command::Value::Type& type);
boost::python::object fromValue(const command::Value& value);

}  // namespace convert
}  // namespace python
}  // namespace dynamicgraph
