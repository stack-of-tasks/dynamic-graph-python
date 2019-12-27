#ifndef DGPY_PYTHON_COMPAT_H
#define DGPY_PYTHON_COMPAT_H

#include <string>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Get any PyObject and get its str() representation as an std::string
std::string obj_to_str(PyObject* o);

#endif
