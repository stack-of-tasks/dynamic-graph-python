#include "dynamic-graph/python/python-compat.hh"

// Get any PyObject and get its str() representation as an std::string
std::string obj_to_str(PyObject* o) {
  std::string ret;
  PyObject* os;
#if PY_MAJOR_VERSION >= 3
  os = PyObject_Str(o);
  assert(os != NULL);
  assert(PyUnicode_Check(os));
  ret = PyUnicode_AsUTF8(os);
#else
  os = PyObject_Unicode(o);
  assert(os != NULL);
  assert(PyUnicode_Check(os));
  PyObject* oss = PyUnicode_AsUTF8String(os);
  assert(oss != NULL);
  ret = PyString_AsString(oss);
  Py_DECREF(oss);
#endif
  Py_DECREF(os);
  return ret;
}
