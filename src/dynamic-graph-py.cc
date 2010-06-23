/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
#include <string>

static PyObject*
loadPlugin(PyObject* self, PyObject* args)
{
  char* plugin = NULL;
  char* path = NULL;

  if (!PyArg_ParseTuple(args,"ss", &plugin, &path))
    return NULL;

  std::cout << "loadPlugin " << std::string(plugin) << " "
	    << std::string(path) << std::endl;
  return Py_BuildValue("");
}

static PyObject*
plug(PyObject* self, PyObject* args)
{
  char* out = NULL;
  char* in = NULL;
  if (!PyArg_ParseTuple(args,"ss", &out, &in))
    return NULL;

  std::cout << "plug " << std::string(out) << " "
	    << std::string(in) << std::endl;
  return Py_BuildValue("");
}

/**
   \brief List of python functions
*/
static PyMethodDef sotTutorialMethods[] = {
  {"loadPlugin",  loadPlugin, METH_VARARGS,
     "load a plugin into the dynamic-graph"},
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
}
