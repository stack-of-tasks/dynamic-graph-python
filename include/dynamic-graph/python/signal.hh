// Copyright 2020, Joseph Mirabel, LAAS-CNRS.

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/signal.h>

#include <boost/python.hpp>
#include <sstream>

#include "dynamic-graph/python/signal-wrapper.hh"

namespace dynamicgraph {
namespace python {

template <typename T, typename Time>
auto exposeSignal(const std::string& name) {
  namespace bp = boost::python;

  typedef Signal<T, Time> S_t;
  bp::class_<S_t, bp::bases<SignalBase<Time> >, boost::noncopyable> obj(name.c_str(), bp::init<std::string>());
  obj.add_property("value", bp::make_function(&S_t::accessCopy, bp::return_value_policy<bp::copy_const_reference>()),
                   &S_t::setConstant,  // TODO check the setter
                   "the signal value.\n"
                   "warning: for Eigen objects, sig.value[0] = 1. may not work).");
  return obj;
}

template <typename T, typename Time>
auto exposeSignalWrapper(const std::string& name) {
  namespace bp = boost::python;

  typedef SignalWrapper<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(), bp::no_init);
  return obj;
}

template <typename T, typename Time>
auto exposeSignalPtr(const std::string& name) {
  namespace bp = boost::python;

  typedef SignalPtr<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(), bp::no_init);
  return obj;
}

template <typename T, typename Time>
auto exposeSignalTimeDependent(const std::string& name) {
  namespace bp = boost::python;

  typedef SignalTimeDependent<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(), bp::no_init);
  return obj;
}

template <typename T, typename Time>
void exposeSignalsOfType(const std::string& name) {
  exposeSignal<T, Time>("Signal" + name);
  exposeSignalPtr<T, Time>("SignalPtr" + name);
  exposeSignalWrapper<T, Time>("SignalWrapper" + name);
  exposeSignalTimeDependent<T, Time>("SignalTimeDependent" + name);
}

}  // namespace python
}  // namespace dynamicgraph
