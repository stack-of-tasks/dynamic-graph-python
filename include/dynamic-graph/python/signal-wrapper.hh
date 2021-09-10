// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#ifndef DGPY_SIGNAL_WRAPPER
#define DGPY_SIGNAL_WRAPPER

#include <dynamic-graph/entity.h>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/signal.h>

#include <boost/bind.hpp>
#include <boost/python.hpp>

#include "dynamic-graph/python/python-compat.hh"

namespace dynamicgraph {
namespace python {

class PythonSignalContainer : public Entity {
  DYNAMIC_GRAPH_ENTITY_DECL();

 public:
  using Entity::Entity;

  void signalRegistration(const SignalArray<int>& signals);

  void rmSignal(const std::string& name);
};

template <class T, class Time>
class SignalWrapper : public Signal<T, Time> {
 public:
  typedef Signal<T, Time> parent_t;
  typedef boost::python::object pyobject;

  static bool checkCallable(pyobject c, std::string& error);

  SignalWrapper(std::string name, pyobject callable) : parent_t(name), callable(callable) {
    typedef boost::function2<T&, T&, Time> function_t;
    function_t f = boost::bind(&SignalWrapper::call, this, _1, _2);
    this->setFunction(f);
  }

  virtual ~SignalWrapper(){};

 private:
  T& call(T& value, Time t) {
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    if (PyGILState_GetThisThreadState() == NULL) {
      dgDEBUG(10) << "python thread not initialized" << std::endl;
    }
    pyobject obj = callable(t);
    value = boost::python::extract<T>(obj);
    PyGILState_Release(gstate);
    return value;
  }
  pyobject callable;
};

}  // namespace python
}  // namespace dynamicgraph
#endif
