// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)

#include <Python.h>

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/entity.h>

namespace dynamicgraph {
namespace python {
namespace signalWrapper {
void convert(PyObject* o, int& v);
void convert(PyObject* o, bool& v);
void convert(PyObject* o, float& v);
void convert(PyObject* o, double& v);
// void convert (PyObject* o, std::string& v);
void convert(PyObject* o, Vector& v);
// void convert (PyObject* o, Eigen::MatrixXd& v);
// void convert (PyObject* o, Eigen::Matrix4d& v);
}  // namespace signalWrapper

class PythonSignalContainer : public Entity {
  DYNAMIC_GRAPH_ENTITY_DECL();

 public:
  PythonSignalContainer(const std::string& name);

  void signalRegistration(const SignalArray<int>& signals);

  void rmSignal(const std::string& name);
};

template <class T, class Time>
class SignalWrapper : public Signal<T, Time> {
 public:
  typedef Signal<T, Time> parent_t;

  static bool checkCallable(PyObject* c, std::string& error);

  SignalWrapper(std::string name, PyObject* _callable) : parent_t(name), callable(_callable) {
    typedef boost::function2<T&, T&, Time> function_t;
    Py_INCREF(callable);
    function_t f = boost::bind(&SignalWrapper::call, this, _1, _2);
    this->setFunction(f);
  }

  virtual ~SignalWrapper() { Py_DECREF(callable); };

 private:
  T& call(T& value, Time t) {
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    if (PyGILState_GetThisThreadState() == NULL) {
      dgDEBUG(10) << "python thread not initialized" << std::endl;
    }
    char format[] = "i";
    PyObject* obj = PyObject_CallFunction(callable, format, t);
    if (obj == NULL) {
      dgERROR << "Could not call callable" << std::endl;
    } else {
      signalWrapper::convert(obj, value);
      Py_DECREF(obj);
    }
    PyGILState_Release(gstate);
    return value;
  }
  PyObject* callable;
};

}  // namespace python
}  // namespace dynamicgraph
