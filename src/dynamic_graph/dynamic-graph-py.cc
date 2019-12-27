// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>
#include <sstream>

#include <dynamic-graph/debug.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/signal-base.h>

#include "dynamic-graph/python/exception.hh"
#include "dynamic-graph/python/dynamic-graph-py.hh"
#include "dynamic-graph/python/signal-wrapper.hh"

namespace dynamicgraph {
namespace python {

#if PY_MAJOR_VERSION == 2
  PyObject* dgpyError;
# endif

/**
   \brief plug a signal into another one.
*/
PyObject* plug(
#if PY_MAJOR_VERSION >= 3
    PyObject* m, PyObject* args
#else
    PyObject*, PyObject* args
#endif
    ) {
  PyObject* objOut = NULL;
  PyObject* objIn = NULL;
  void* pObjOut;
  void* pObjIn;

  if (!PyArg_ParseTuple(args, "OO", &objOut, &objIn)) return NULL;

  if (!PyCapsule_CheckExact(objOut)) {
    PyErr_SetString(PyExc_TypeError,
                    "first argument should be a pointer to"
                    " signalBase<int>.");
    return NULL;
  }
  if (!PyCapsule_CheckExact(objIn)) {
    PyErr_SetString(PyExc_TypeError,
                    "second argument should be a pointer to"
                    " signalBase<int>.");
    return NULL;
  }

  pObjIn = PyCapsule_GetPointer(objIn, "dynamic_graph.Signal");
  SignalBase<int>* signalIn = (SignalBase<int>*)pObjIn;
  if (signalIn == NULL) {
      std::ostringstream oss;
      oss << "dynamic_graph.plug(a, b): Argument 'b' must be of type 'dynamic_graph.Signal', but got "
          << PyCapsule_GetName(objIn);
      PyErr_SetString(PyExc_TypeError, oss.str().c_str());
      return NULL;
  }
  pObjOut = PyCapsule_GetPointer(objOut, "dynamic_graph.Signal");
  SignalBase<int>* signalOut = (SignalBase<int>*)pObjOut;
  if (signalOut == NULL) {
      std::ostringstream oss;
      oss << "dynamic_graph.plug(a, b): Argument 'a' must be of type 'dynamic_graph.Signal', but got "
          << PyCapsule_GetName(objOut);
      PyErr_SetString(PyExc_TypeError, oss.str().c_str());
      return NULL;
  }
  std::ostringstream os;

  try {
    signalIn->plug(signalOut);
  }
  CATCH_ALL_EXCEPTIONS(m);
  return Py_BuildValue("");
}

PyObject* enableTrace(
#if PY_MAJOR_VERSION >= 3
    PyObject* m, PyObject* args
#else
    PyObject*, PyObject* args
#endif
    ) {
  PyObject* boolean;
  char* filename = NULL;

  if (PyArg_ParseTuple(args, "Os", &boolean, &filename)) {
    if (!PyBool_Check(boolean)) {
      PyErr_SetString(PyExc_TypeError,
                      "enableTrace takes as first "
                      "argument True or False,\n"
                      "           and as "
                      "second argument a filename.");
      return NULL;
    }
    if (PyObject_IsTrue(boolean)) {
      try {
        DebugTrace::openFile(filename);
      }
      CATCH_ALL_EXCEPTIONS(m);
    } else {
      try {
        DebugTrace::closeFile(filename);
      }
      CATCH_ALL_EXCEPTIONS(m);
    }
  } else {
    return NULL;
  }
  return Py_BuildValue("");
}

PyObject* error_out(
#if PY_MAJOR_VERSION >= 3
    PyObject* m, PyObject*
#else
    PyObject*, PyObject*
#endif
) {
  PyErr_SetString(DGPYERROR(m), "something bad happened");
  return NULL;
}

}  // namespace python
}  // namespace dynamicgraph

#ifdef __cplusplus
extern "C" {
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_wrap(void)
#else
void initwrap(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
  PyObject* m = PyModule_Create(&dynamicgraph::python::dynamicGraphModuleDef);
#else
  PyObject* m = Py_InitModule("wrap", dynamicgraph::python::dynamicGraphMethods);
#endif

  if (m == NULL) INITERROR;

  DGPYERROR(m) = PyErr_NewException(const_cast<char*>("dynamic_graph.dgpyError"), NULL, NULL);
  if (DGPYERROR(m) == NULL) {
    Py_DECREF(m);
    INITERROR;
  }

  Py_XINCREF(DGPYERROR(m));
  if (PyModule_AddObject(m, "dgpyError", DGPYERROR(m)) < 0) {
    Py_XDECREF(DGPYERROR(m));
    Py_CLEAR(DGPYERROR(m));
    Py_DECREF(m);
    INITERROR;
  }

#if PY_MAJOR_VERSION >= 3
  return m;
#endif
}

#ifdef __cplusplus
}  // extern "C"
#endif
