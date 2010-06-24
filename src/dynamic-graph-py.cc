/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
#include <sstream>
#include <string>

#include <dynamic-graph/interpreter.h>

static PyObject* error;

static dynamicgraph::Interpreter interpreter;

static PyObject*
plug(PyObject* self, PyObject* args)
{
  char* out = NULL;
  char* in = NULL;
  if (!PyArg_ParseTuple(args,"ss", &out, &in))
    return NULL;

  std::stringstream ss;
  std::ostringstream os;
  ss << std::string(out) << " " << std::string(in);
  std::istringstream cmdArg(ss.str());
  try {
    interpreter.cmdPlug(std::string("plug"), cmdArg, os);
  } catch (dynamicgraph::ExceptionFactory& exc) {
    PyErr_SetString(error, exc.getStringMessage().c_str());
    return NULL;
  }

  return Py_BuildValue("");
}

/**
   \brief List of python functions
*/
static PyMethodDef sotTutorialMethods[] = {
  {"plug",  plug, METH_VARARGS,
     "plug an output signal into an input signal"},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initwrap(void)
{
    PyObject *m;

    m = Py_InitModule("wrap", sotTutorialMethods);
    if (m == NULL)
        return;

    error = PyErr_NewException("dynamic_graph.wrap.error", NULL, NULL);
    Py_INCREF(error);
    PyModule_AddObject(m, "error", error);
}
