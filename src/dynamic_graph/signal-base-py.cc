// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>
#include <sstream>

#include <boost/python.hpp>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/signal-caster.h>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/factory.h>

#include "dynamic-graph/python/dynamic-graph-py.hh"
#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/signal-wrapper.hh"
#include "dynamic-graph/python/module.hh"

using dynamicgraph::SignalBase;

namespace bp = boost::python;

namespace dynamicgraph {
namespace python {

using namespace convert;

typedef int time_type;

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

template<typename Time> void exposeSignalBase (const char* name)
{
  typedef SignalBase<Time> S_t;
  bp::class_<S_t, boost::noncopyable>(name, bp::no_init)
    .add_property ("time",
        bp::make_function(&S_t::getTime, bp::return_value_policy<bp::copy_const_reference>()),
        &S_t::setTime)
    .add_property ("name",
        bp::make_function(&S_t::getName, bp::return_value_policy<bp::copy_const_reference>()))

    .def("getName", &S_t::getName, bp::return_value_policy<bp::copy_const_reference>())
    .def("getClassName", +[](const S_t& s) -> std::string {
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

    .def("__str__", +[](const S_t& s) -> std::string {
          std::ostringstream oss;
          s.display(oss);
          return oss.str();
        })
    .def("displayDependencies", +[](const S_t& s, int time) -> std::string {
          std::ostringstream oss;
          s.displayDependencies(oss, time);
          return oss.str();
        }, "Print the signal dependencies in a string")
    ;
}

template<typename T, typename Time> auto exposeSignal (const std::string& name)
{
  typedef Signal<T, Time> S_t;
  bp::class_<S_t, bp::bases<SignalBase<Time> >, boost::noncopyable> obj(name.c_str(), bp::init<std::string>());
  obj
    .add_property("value",
        bp::make_function(&S_t::accessCopy, bp::return_value_policy<bp::copy_const_reference>()),
        &S_t::setConstant, // TODO check the setter
        "the signal value.\n"
        "warning: for Eigen objects, sig.value[0] = 1. may not work).")
    ;
  return obj;
}

template<> auto exposeSignal<MatrixHomogeneous, time_type> (const std::string& name)
{
  typedef Signal<MatrixHomogeneous, time_type> S_t;
  bp::class_<S_t, bp::bases<SignalBase<time_type> >, boost::noncopyable> obj(name.c_str(), bp::init<std::string>());
  obj
    .add_property("value",
        +[](const S_t& signal) -> Matrix4 { return signal.accessCopy().matrix(); },
        +[](S_t& signal, const Matrix4& v) {
          // TODO it isn't hard to support pinocchio::SE3 type here.
          // However, this adds a dependency to pinocchio.
          signal.setConstant (MatrixHomogeneous(v));
        }, "the signal value.")
    ;
  return obj;
}

template<typename T, typename Time> auto exposeSignalWrapper (const std::string& name)
{
  typedef SignalWrapper<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(), bp::no_init);
  return obj;
}

template<typename T, typename Time> auto exposeSignalPtr (const std::string& name)
{
  typedef SignalPtr<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(), bp::no_init)
    ;
  return obj;
}

template<typename T, typename Time> auto exposeSignalTimeDependent (const std::string& name)
{
  typedef SignalTimeDependent<T, Time> S_t;
  bp::class_<S_t, bp::bases<Signal<T, Time> >, boost::noncopyable> obj(name.c_str(),
      bp::no_init)
    ;
  return obj;
}

template<typename T, typename Time>
void exposeSignalsOfType(const std::string& name)
{
  exposeSignal<T, Time>("Signal" + name);
  exposeSignalPtr<T, Time>("SignalPtr" + name);
  exposeSignalWrapper<T, Time>("SignalWrapper" + name);
  exposeSignalTimeDependent<T, Time>("SignalTimeDependent" + name);
}

void exposeSignals()
{
  exposeSignalBase<time_type>("SignalBase");

  exposeSignalsOfType<bool, time_type>("Bool");
  exposeSignalsOfType<int, time_type>("Int");
  exposeSignalsOfType<double, time_type>("Double");

  exposeSignalsOfType<Vector, time_type>("Vector");
  exposeSignalsOfType<Vector3, time_type>("Vector3");
  exposeSignalsOfType<Vector7, time_type>("Vector7");

  exposeSignalsOfType<Matrix, time_type>("Matrix");
  exposeSignalsOfType<MatrixRotation, time_type>("MatrixRotation");
  exposeSignalsOfType<MatrixHomogeneous, time_type>("MatrixHomogeneous");
  exposeSignalsOfType<MatrixTwist, time_type>("MatrixTwist");

  exposeSignalsOfType<Quaternion, time_type>("Quaternion");
  exposeSignalsOfType<VectorUTheta, time_type>("VectorUTheta");
}

namespace signalBase {

template <class T>
SignalWrapper<T, int>* createSignalWrapperTpl(const char* name, bp::object o, std::string& error) {
  typedef SignalWrapper<T, int> SignalWrapper_t;
  if (!SignalWrapper_t::checkCallable(o, error)) {
    return NULL;
  }

  SignalWrapper_t* obj = new SignalWrapper_t(name, o);
  return obj;
}

PythonSignalContainer* getPythonSignalContainer() {
  Entity* obj = entity::create ("PythonSignalContainer", "python_signals");
  return dynamic_cast<PythonSignalContainer*>(obj);
}

#define SIGNAL_WRAPPER_TYPE(IF, Enum, Type)                               \
  IF(command::Value::typeName(command::Value::Enum).compare(type) == 0) { \
    obj = createSignalWrapperTpl<Type>(name, object, error);              \
  }

/**
   \brief Create an instance of SignalWrapper
*/
SignalBase<int>* createSignalWrapper(const char* name, const char* type, bp::object object)
{
  PythonSignalContainer* psc = getPythonSignalContainer();
  if (psc == NULL) return NULL;

  SignalBase<int>* obj = NULL;
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

  if (obj == NULL)
    throw std::runtime_error(error);
  // Register signal into the python signal container
  psc->signalRegistration(*obj);

  // Return the pointer
  return obj;
}

}  // namespace signalBase
}  // namespace python
}  // namespace dynamicgraph
