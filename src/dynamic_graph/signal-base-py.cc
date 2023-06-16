// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/value.h>

#include <boost/python.hpp>
#include <iostream>
#include <sstream>

#include "dynamic-graph/python/dynamic-graph-py.hh"
#include "dynamic-graph/python/signal-wrapper.hh"
#include "dynamic-graph/python/signal.hh"

using dynamicgraph::SignalBase;

namespace bp = boost::python;

namespace dynamicgraph {
namespace python {

typedef Eigen::AngleAxis<double> VectorUTheta;
typedef Eigen::Quaternion<double> Quaternion;

typedef Eigen::VectorXd Vector;
typedef Eigen::Vector3d Vector3;
typedef Eigen::Matrix<double, 7, 1> Vector7;

typedef Eigen::MatrixXd Matrix;
typedef Eigen::Matrix<double, 3, 3> MatrixRotation;
typedef Eigen::Matrix<double, 4, 4> Matrix4;
typedef Eigen::Transform<double, 3, Eigen::Affine> MatrixHomogeneous;
typedef Eigen::Matrix<double, 6, 6> MatrixTwist;

template <typename Time>
void exposeSignalBase(const char* name) {
  typedef SignalBase<Time> S_t;
  bp::class_<S_t, boost::noncopyable>(name, bp::no_init)
      .add_property("time",
                    bp::make_function(
                        &S_t::getTime,
                        bp::return_value_policy<bp::copy_const_reference>()),
                    &S_t::setTime)
      .add_property("name",
                    bp::make_function(
                        &S_t::getName,
                        bp::return_value_policy<bp::copy_const_reference>()))

      .def("getName", &S_t::getName,
           bp::return_value_policy<bp::copy_const_reference>())
      .def(
          "getClassName",
          +[](const S_t& s) -> std::string {
            std::string ret;
            s.getClassName(ret);
            return ret;
          })

      .def("plug", &S_t::plug, "Plug the signal to another signal")
      .def("unplug", &S_t::unplug, "Unplug the signal")
      .def("isPlugged", &S_t::isPlugged, "Whether the signal is plugged")
      .def("getPlugged", &S_t::getPluged,
           bp::return_value_policy<bp::reference_existing_object>(),
           "To which signal the signal is plugged")

      .def("recompute", &S_t::recompute, "Recompute the signal at given time")

      .def(
          "__str__",
          +[](const S_t& s) -> std::string {
            std::ostringstream oss;
            s.display(oss);
            return oss.str();
          })
      .def(
          "displayDependencies",
          +[](const S_t& s, sigtime_t time) -> std::string {
            std::ostringstream oss;
            s.displayDependencies(oss, time);
            return oss.str();
          },
          "Print the signal dependencies in a string");
}

template <>
auto exposeSignal<MatrixHomogeneous, sigtime_t>(const std::string& name) {
  typedef Signal<MatrixHomogeneous, sigtime_t> S_t;
  bp::class_<S_t, bp::bases<SignalBase<sigtime_t> >, boost::noncopyable> obj(
      name.c_str(), bp::init<std::string>());
  obj.add_property(
      "value",
      +[](const S_t& signal) -> Matrix4 {
        return signal.accessCopy().matrix();
      },
      +[](S_t& signal, const Matrix4& v) {
        // TODO it isn't hard to support pinocchio::SE3 type here.
        // However, this adds a dependency to pinocchio.
        signal.setConstant(MatrixHomogeneous(v));
      },
      "the signal value.");
  return obj;
}

void exposeSignals() {
  exposeSignalBase<sigtime_t>("SignalBase");

  exposeSignalsOfType<bool, sigtime_t>("Bool");
  exposeSignalsOfType<int, sigtime_t>("Int");
  exposeSignalsOfType<double, sigtime_t>("Double");

  exposeSignalsOfType<Vector, sigtime_t>("Vector");
  exposeSignalsOfType<Vector3, sigtime_t>("Vector3");
  exposeSignalsOfType<Vector7, sigtime_t>("Vector7");

  exposeSignalsOfType<Matrix, sigtime_t>("Matrix");
  exposeSignalsOfType<MatrixRotation, sigtime_t>("MatrixRotation");
  exposeSignalsOfType<MatrixHomogeneous, sigtime_t>("MatrixHomogeneous");
  exposeSignalsOfType<MatrixTwist, sigtime_t>("MatrixTwist");

  exposeSignalsOfType<Quaternion, sigtime_t>("Quaternion");
  exposeSignalsOfType<VectorUTheta, sigtime_t>("VectorUTheta");
}

namespace signalBase {

template <class T>
SignalWrapper<T, sigtime_t>* createSignalWrapperTpl(const char* name, bp::object o,
                                              std::string& error) {
  typedef SignalWrapper<T, sigtime_t> SignalWrapper_t;
  if (!SignalWrapper_t::checkCallable(o, error)) {
    return NULL;
  }

  SignalWrapper_t* obj = new SignalWrapper_t(name, o);
  return obj;
}

PythonSignalContainer* getPythonSignalContainer() {
  Entity* obj = entity::create("PythonSignalContainer", "python_signals");
  return dynamic_cast<PythonSignalContainer*>(obj);
}

#define SIGNAL_WRAPPER_TYPE(IF, Enum, Type)                               \
  IF(command::Value::typeName(command::Value::Enum).compare(type) == 0) { \
    obj = createSignalWrapperTpl<Type>(name, object, error);              \
  }

/**
   \brief Create an instance of SignalWrapper
*/
SignalBase<sigtime_t>* createSignalWrapper(const char* name, const char* type,
                                     bp::object object) {
  PythonSignalContainer* psc = getPythonSignalContainer();
  if (psc == NULL) return NULL;

  SignalBase<sigtime_t>* obj = NULL;
  std::string error;
  SIGNAL_WRAPPER_TYPE(if, BOOL, bool)
  // SIGNAL_WRAPPER_TYPE(else if, UNSIGNED ,bool)
  SIGNAL_WRAPPER_TYPE(else if, INT, int)
  SIGNAL_WRAPPER_TYPE(else if, FLOAT, float)
  SIGNAL_WRAPPER_TYPE(else if, DOUBLE, double)
  // SIGNAL_WRAPPER_TYPE(else if, STRING   ,bool)
  SIGNAL_WRAPPER_TYPE(else if, VECTOR, Vector)
  // SIGNAL_WRAPPER_TYPE(else if, MATRIX   ,bool)
  // SIGNAL_WRAPPER_TYPE(else if, MATRIX4D ,bool)
  else {
    error = "Type not understood";
  }

  if (obj == NULL) throw std::runtime_error(error);
  // Register signal into the python signal container
  psc->signalRegistration(*obj);

  // Return the pointer
  return obj;
}

}  // namespace signalBase
}  // namespace python
}  // namespace dynamicgraph
