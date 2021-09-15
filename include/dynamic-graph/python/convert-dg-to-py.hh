// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/value.h>

#include <boost/python.hpp>

namespace dynamicgraph {
namespace python {
namespace convert {

command::Value toValue(boost::python::object o, const command::Value::Type& type);
boost::python::object fromValue(const command::Value& value);

}  // namespace convert
}  // namespace python
}  // namespace dynamicgraph
