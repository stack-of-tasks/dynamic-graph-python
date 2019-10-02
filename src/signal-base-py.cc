// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <Python.h>
#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-caster.h>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/factory.h>

#include "convert-dg-to-py.hh"
#include "exception.hh"
#include "signal-wrapper.hh"

using dynamicgraph::SignalBase;

namespace dynamicgraph {
namespace python {

extern PyObject* dgpyError;
using namespace convert;

namespace signalBase {

static void destroy(PyObject* self);

/**
   \brief Create an instance of SignalBase
*/
PyObject* create(PyObject* /*self*/, PyObject* args) {
  char* name = NULL;

  if (!PyArg_ParseTuple(args, "s", &name)) return NULL;

  SignalBase<int>* obj = NULL;
  obj = new SignalBase<int>(std::string(name));

  // Return the pointer
  return PyCapsule_New((void*)obj, "dynamic_graph.Signal", destroy);
}

template <class T>
SignalWrapper<T, int>* createSignalWrapperTpl(const char* name, PyObject* o, std::string& error) {
  typedef SignalWrapper<T, int> SignalWrapper_t;
  if (!SignalWrapper_t::checkCallable(o, error)) {
    return NULL;
  }

  SignalWrapper_t* obj = new SignalWrapper_t(name, o);
  return obj;
}

PythonSignalContainer* getPythonSignalContainer() {
  const std::string instanceName = "python_signals";
  const std::string className = "PythonSignalContainer";
  Entity* obj;
  if (PoolStorage::getInstance()->existEntity(instanceName, obj)) {
    if (obj->getClassName() != className) {
      std::string msg("Found an object named " + std::string(instanceName) +
                      ",\n"
                      "but this object is of type " +
                      std::string(obj->getClassName()) + " and not " + std::string(className));
      PyErr_SetString(dgpyError, msg.c_str());
      return NULL;
    }
  } else {
    try {
      obj = FactoryStorage::getInstance()->newEntity(std::string(className), std::string(instanceName));
    }
    CATCH_ALL_EXCEPTIONS();
  }
  return dynamic_cast<PythonSignalContainer*>(obj);
}

#define SIGNAL_WRAPPER_TYPE(IF, Enum, Type)                               \
  IF(command::Value::typeName(command::Value::Enum).compare(type) == 0) { \
    obj = createSignalWrapperTpl<Type>(name, object, error);              \
  }

/**
   \brief Create an instance of SignalWrapper
*/
PyObject* createSignalWrapper(PyObject* /*self*/, PyObject* args) {
  PythonSignalContainer* psc = getPythonSignalContainer();
  if (psc == NULL) return NULL;

  char* name = NULL;
  char* type = NULL;
  PyObject* object = NULL;

  if (!PyArg_ParseTuple(args, "ssO", &name, &type, &object)) return NULL;

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

  if (obj == NULL) {
    PyErr_SetString(dgpyError, error.c_str());
    return NULL;
  }
  // Register signal into the python signal container
  psc->signalRegistration(*obj);

  // Return the pointer
  return PyCapsule_New((void*)obj, "dynamic_graph.SignalWrapper", destroy);
}

/**
   \brief Destroy an instance of InvertedPendulum
*/
static void destroy(PyObject* self) {
  SignalBase<int>* obj = (SignalBase<int>*)self;
  delete obj;
}

PyObject* getTime(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* obj = (SignalBase<int>*)pointer;

  int time = obj->getTime();
  return Py_BuildValue("i", time);
}

PyObject* setTime(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  int time;
  if (!PyArg_ParseTuple(args, "Oi", &object, &time)) return NULL;
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(dgpyError, "object should be a C object");
    return NULL;
  }

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* obj = (SignalBase<int>*)pointer;

  obj->setTime(time);
  return Py_BuildValue("");
}

PyObject* display(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* obj = (SignalBase<int>*)pointer;

  std::ostringstream oss;
  try {
    obj->display(oss);
  }
  CATCH_ALL_EXCEPTIONS();

  return Py_BuildValue("s", oss.str().c_str());
}

PyObject* displayDependencies(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  int time;
  if (!PyArg_ParseTuple(args, "OI", &object, &time)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* obj = (SignalBase<int>*)pointer;

  std::ostringstream oss;
  try {
    obj->displayDependencies(oss, time);
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("s", oss.str().c_str());
}

PyObject* getValue(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;

  try {
    {  // --- VECTOR SIGNALS -----------------
      // Two cases: the signal embeds directly a vector, or embeds
      // an object deriving from vector.In the first case,
      // the signal is directly cast into sig<vector>.
      // In the second case, the derived object can be access as a vector
      // using the signal-ptr<vector> type.
      Signal<dynamicgraph::Vector, int>* sigvec = dynamic_cast<Signal<dynamicgraph::Vector, int>*>(signal);
      if (NULL != sigvec) {
        return vectorToPython(sigvec->accessCopy());
      }

      // Extraction of object derinving from vector: plug signal into
      // a vector signal and get the value from the signal-ptr instead
      // of the original vector.
      SignalPtr<dynamicgraph::Vector, int> sigptr(NULL, "vector-caster");
      try {
        sigptr.plug(signal);
        return vectorToPython(sigptr.accessCopy());
      } catch (dynamicgraph::ExceptionSignal& ex) {
        if (ex.getCode() != dynamicgraph::ExceptionSignal::PLUG_IMPOSSIBLE) throw;
      }
    }

    {  // --- MATRIX SIGNALS --------------------
      // Two cases: the signal embeds directly a matrix, or embeds
      // an object deriving from matrix.In the first case,
      // the signal is directly cast into sig<matrix>.
      // In the second case, the derived object can be access as a matrix
      // using the signal-ptr<matrix> type.
      Signal<dynamicgraph::Matrix, int>* sigmat = dynamic_cast<Signal<dynamicgraph::Matrix, int>*>(signal);
      if (NULL != sigmat) {
        return matrixToPython(sigmat->accessCopy());
      }

      SignalPtr<dynamicgraph::Matrix, int> sigptr(NULL, "matrix-caster");
      try {
        sigptr.plug(signal);
        return matrixToPython(sigptr.accessCopy());
      } catch (dynamicgraph::ExceptionSignal& ex) {
        if (ex.getCode() != dynamicgraph::ExceptionSignal::PLUG_IMPOSSIBLE) throw;
      }
    }

    {  // --- HOMOGENEOUS MATRIX SIGNALS --------------------
      // Two cases: the signal embeds directly a matrix, or embeds
      // an object deriving from matrix.In the first case,
      // the signal is directly cast into sig<matrix>.
      // In the second case, the derived object can be access as a matrix
      // using the signal-ptr<matrix> type.

      // TODO: See if matrix homogeneous can be properly put in linear-algebra.h
      typedef Eigen::Transform<double, 3, Eigen::Affine> MatrixHomogeneous;
      Signal<MatrixHomogeneous, int>* sigmat = dynamic_cast<Signal<MatrixHomogeneous, int>*>(signal);
      if (NULL != sigmat) {
        return matrixToPython(sigmat->accessCopy().matrix());
      }

      SignalPtr<Eigen::Transform<double, 3, Eigen::Affine>, int> sigptr(NULL, "matrix-caster");
      try {
        sigptr.plug(signal);
        return matrixToPython(sigptr.accessCopy().matrix());
      } catch (dynamicgraph::ExceptionSignal& ex) {
        if (ex.getCode() != dynamicgraph::ExceptionSignal::PLUG_IMPOSSIBLE) throw;
      }
    }

    Signal<double, int>* sigdouble = dynamic_cast<Signal<double, int>*>(signal);
    if (NULL != sigdouble) {
      return Py_BuildValue("d", sigdouble->accessCopy());
    }
  }
  CATCH_ALL_EXCEPTIONS();

  /* Non specific signal: use a generic way. */
  std::ostringstream value;
  try {
    signal->get(value);
  }
  CATCH_ALL_EXCEPTIONS();

  std::string valueString = value.str();
  return Py_BuildValue("s", valueString.c_str());
}

PyObject* getName(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;

  std::string name;
  try {
    name = signal->getName();
  }
  CATCH_ALL_EXCEPTIONS();

  return Py_BuildValue("s", name.c_str());
}

PyObject* getClassName(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;

  std::string name;
  try {
    signal->getClassName(name);
  }
  CATCH_ALL_EXCEPTIONS();

  return Py_BuildValue("s", name.c_str());
}

PyObject* setValue(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  char* valueString = NULL;

  if (!PyArg_ParseTuple(args, "Os", &object, &valueString)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;
  std::ostringstream os;
  os << valueString;
  std::istringstream value(os.str());

  try {
    signal->set(value);
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* recompute(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  unsigned int time;
  if (!PyArg_ParseTuple(args, "OI", &object, &time)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;
  try {
    signal->recompute(time);
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* unplug(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;
  try {
    signal->unplug();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* isPlugged(PyObject*, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;
  bool plugged = false;
  try {
    plugged = signal->isPlugged();
  }
  CATCH_ALL_EXCEPTIONS();
  if (plugged)
    return PyBool_FromLong(1);
  else
    return PyBool_FromLong(0);
}

PyObject* getPlugged(PyObject*, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Signal");
  SignalBase<int>* signal = (SignalBase<int>*)pointer;
  SignalBase<int>* otherSignal = 0;
  try {
    bool plugged = signal->isPlugged();
    otherSignal = signal->getPluged();
    if (!plugged || otherSignal == 0) {
      std::string msg = std::string("Signal ") + signal->getName() + std::string(" is not plugged.");
      throw std::runtime_error(msg);
    }
  }
  CATCH_ALL_EXCEPTIONS();
  // Return the pointer to the signal without destructor since the signal
  // is not owned by the calling object.
  return PyCapsule_New((void*)otherSignal, "dynamic_graph.Signal", NULL);
}
}  // namespace signalBase
}  // namespace python
}  // namespace dynamicgraph
