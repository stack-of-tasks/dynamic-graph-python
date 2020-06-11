// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>
#include <sstream>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-caster.h>

#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/python-compat.hh"

namespace dynamicgraph {

using ::dynamicgraph::SignalBase;

namespace python {
namespace convert {

namespace bp = boost::python;

command::Value toValue(bp::object o, const command::Value::Type& valueType) {
  using command::Value;
  switch (valueType) {
    case (Value::BOOL):
      return Value(bp::extract<bool>(o));
    case (Value::UNSIGNED):
      return Value(bp::extract<unsigned>(o));
    case (Value::INT):
      return Value(bp::extract<int>(o));
    case (Value::FLOAT):
      return Value(bp::extract<float>(o));
    case (Value::DOUBLE):
      return Value(bp::extract<double>(o));
    case (Value::STRING):
      return Value(bp::extract<std::string>(o));
    case (Value::VECTOR):
      // TODO for backward compatibility, support tuple or list ?
      // I don't think so
      return Value(bp::extract<Vector>(o));
    case (Value::MATRIX):
      // TODO for backward compatibility, support tuple or list ?
      // I don't think so
      return Value(bp::extract<Matrix>(o));
    case (Value::MATRIX4D):
      return Value(bp::extract<Eigen::Matrix4d>(o));
    case (Value::VALUES):
      // TODO the vector of values cannot be built since
      // - the value type inside the vector are not know
      // - inferring the value type from the Python type is not implemented.
      throw std::invalid_argument("not implemented: cannot create a vector of values");
      break;
    default:
      std::cerr << "Only int, double and string are supported." << std::endl;
  }
  return Value();
}

bp::object fromValue(const command::Value& value) {
  using command::Value;
  switch (value.type()) {
    case (Value::BOOL):
      return bp::object(value.boolValue());
    default:
      return bp::object();
  }
  return bp::object();
}

}  // namespace convert
}  // namespace python
}  // namespace dynamicgraph
