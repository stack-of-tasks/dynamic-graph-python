// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-caster.h>

#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/python-compat.hh"

namespace dynamicgraph {

using ::dynamicgraph::SignalBase;

namespace python {
namespace convert {

void fillMatrixRow(Matrix& m, unsigned iRow, PyObject* sequence) {
  if (PySequence_Size(sequence) != (int)m.cols()) {
    throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "lines of matrix have different sizes.");
  }
  for (int iCol = 0; iCol < m.cols(); iCol++) {
    PyObject* pyDouble = PySequence_GetItem(sequence, iCol);
    if (PyFloat_Check(pyDouble))
      m(iRow, iCol) = PyFloat_AsDouble(pyDouble);
    else if (PyLong_Check(pyDouble))
      m(iRow, iCol) = (int)PyLong_AsLong(pyDouble) + 0.0;
    else
      throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
                            "element of matrix should be "
                            "a floating point number.");
  }
}
void fillMatrixRow(Eigen::Matrix4d& m, unsigned iRow, PyObject* sequence) {
  if (PySequence_Size(sequence) != (int)m.cols()) {
    throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "lines of matrix have different sizes.");
  }
  for (int iCol = 0; iCol < m.cols(); iCol++) {
    PyObject* pyDouble = PySequence_GetItem(sequence, iCol);
    if (PyFloat_Check(pyDouble))
      m(iRow, iCol) = PyFloat_AsDouble(pyDouble);
    else if (PyLong_Check(pyDouble))
      m(iRow, iCol) = (int)PyLong_AsLong(pyDouble) + 0.0;
    else
      throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
                            "element of matrix should be "
                            "a floating point number.");
  }
}

command::Value pythonToValue(PyObject* pyObject, const command::Value::Type& valueType) {
  using command::Value;
  bool bvalue;
  unsigned uvalue;
  int ivalue;
  float fvalue;
  double dvalue;
  std::string svalue;
  Vector v;
  Matrix m;
  Eigen::Matrix4d m4;
  Py_ssize_t nCols;
  Py_ssize_t size;
  PyObject* row;
  Py_ssize_t nRows;

  switch (valueType) {
    case (Value::BOOL):
      if (!PyBool_Check(pyObject)) {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "bool");
      }
      bvalue = PyObject_IsTrue(pyObject);
      return Value(bvalue);
      break;
    case (Value::UNSIGNED):
      if (PyLong_Check(pyObject)) {
        uvalue = (unsigned int)PyLong_AsUnsignedLongMask(pyObject);
#if PY_MAJOR_VERSION == 2
      } else if (PyInt_Check(pyObject)) {
        uvalue = (unsigned int)PyInt_AsUnsignedLongMask(pyObject);
#endif
      } else {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "unsigned int");
      }
      return Value(uvalue);
      break;
    case (Value::INT):
      if (PyLong_Check(pyObject)) {
        ivalue = (int)PyLong_AsLong(pyObject);
#if PY_MAJOR_VERSION == 2
      } else if (PyInt_Check(pyObject)) {
        ivalue = (int)PyInt_AsLong(pyObject);
#endif
      } else {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "int");
      }
      return Value(ivalue);
      break;
    case (Value::FLOAT):
      if (PyFloat_Check(pyObject)) {
        fvalue = (float)PyFloat_AsDouble(pyObject);
      } else if (PyLong_Check(pyObject)) {
        fvalue = (float)PyLong_AsLong(pyObject);
#if PY_MAJOR_VERSION == 2
      } else if (PyInt_Check(pyObject)) {
        fvalue = (float)PyInt_AsLong(pyObject);
#endif
      } else {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "float");
      }
      return Value(fvalue);
      break;
    case (Value::DOUBLE):
      if (PyFloat_Check(pyObject)) {
        dvalue = PyFloat_AsDouble(pyObject);
      } else if (PyLong_Check(pyObject)) {
        dvalue = (double)PyLong_AsLong(pyObject);
#if PY_MAJOR_VERSION == 2
      } else if (PyInt_Check(pyObject)) {
        dvalue = (double)PyInt_AsLong(pyObject);
#endif
      } else {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "double");
      }
      return Value(dvalue);
      break;
    case (Value::STRING):
      if (!PyUnicode_Check(pyObject)
#if PY_MAJOR_VERSION == 2
          && !PyString_Check(pyObject)
#endif
      ) {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "string");
      }
      svalue = obj_to_str(pyObject);
      return Value(svalue);
      break;
    case (Value::VECTOR):
      // Check that argument is a tuple
      if (!PySequence_Check(pyObject)) {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "vector");
      }
      size = PySequence_Size(pyObject);
      v.resize(size);
      for (Py_ssize_t i = 0; i < size; i++) {
        PyObject* pyDouble = PySequence_GetItem(pyObject, i);
        if (PyFloat_Check(pyDouble))
          v(i) = PyFloat_AsDouble(pyDouble);
        else if (PyLong_Check(pyDouble))
          v(i) = (int)PyLong_AsLong(pyDouble) + 0.0;
#if PY_MAJOR_VERSION == 2
        else if (PyInt_Check(pyDouble))
          v(i) = (int)PyInt_AsLong(pyDouble) + 0.0;
#endif
        else
          throw ExceptionPython(ExceptionPython::VECTOR_PARSING,
                                "element of vector should be a floating "
                                "point number.");
      }
      return Value(v);
      break;
    case (Value::MATRIX):
      // Check that argument is a tuple
      if (!PySequence_Check(pyObject)) {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "matrix");
      }
      nRows = PySequence_Size(pyObject);
      if (nRows == 0) {
        return Value(Matrix());
      }
      row = PySequence_GetItem(pyObject, 0);
      if (!PySequence_Check(row)) {
        throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "matrix");
      }
      nCols = PySequence_Size(row);

      m.resize((unsigned int)nRows, (unsigned int)nCols);
      fillMatrixRow(m, 0, row);

      for (Py_ssize_t iRow = 1; iRow < nRows; iRow++) {
        row = PySequence_GetItem(pyObject, iRow);
        if (!PySequence_Check(row)) {
          throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "matrix");
        }
        fillMatrixRow(m, static_cast<unsigned>(iRow), row);
      }
      return Value(m);
      break;
    case (Value::MATRIX4D):
      // Check that argument is a tuple
      if (!PySequence_Check(pyObject)) {
        throw ExceptionPython(ExceptionPython::VALUE_PARSING, "matrix4d");
      }
      nRows = PySequence_Size(pyObject);
      if (nRows == 0) {
        return Value(Eigen::Matrix4d());
      }
      row = PySequence_GetItem(pyObject, 0);
      if (!PySequence_Check(row)) {
        throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "matrix4d");
      }
      nCols = PySequence_Size(row);

      m4.resize(nRows, nCols);
      fillMatrixRow(m4, 0, row);

      for (Py_ssize_t iRow = 1; iRow < nRows; iRow++) {
        row = PySequence_GetItem(pyObject, iRow);
        if (!PySequence_Check(row)) {
          throw ExceptionPython(ExceptionPython::MATRIX_PARSING, "matrix");
        }
        fillMatrixRow(m4, static_cast<unsigned>(iRow), row);
      }
      return Value(m4);
      break;
    default:
      std::cerr << "Only int, double and string are supported." << std::endl;
  }
  return Value();
}

PyObject* vectorToPython(const Vector& vector) {
  PyObject* tuple = PyTuple_New(vector.size());
  for (int index = 0; index < vector.size(); index++) {
    PyObject* pyDouble = PyFloat_FromDouble(vector(index));
    PyTuple_SET_ITEM(tuple, index, pyDouble);
  }
  return tuple;
}

PyObject* matrixToPython(const Matrix& matrix) {
  PyObject* tuple = PyTuple_New(matrix.rows());
  for (int iRow = 0; iRow < matrix.rows(); iRow++) {
    PyObject* row = PyTuple_New(matrix.cols());
    for (int iCol = 0; iCol < matrix.cols(); iCol++) {
      PyObject* pyDouble = PyFloat_FromDouble(matrix(iRow, iCol));
      PyTuple_SET_ITEM(row, iCol, pyDouble);
    }
    PyTuple_SET_ITEM(tuple, iRow, row);
  }
  return tuple;
}

PyObject* matrix4dToPython(const Eigen::Matrix4d& matrix) {
  PyObject* tuple = PyTuple_New(matrix.rows());
  for (int iRow = 0; iRow < matrix.rows(); iRow++) {
    PyObject* row = PyTuple_New(matrix.cols());
    for (int iCol = 0; iCol < matrix.cols(); iCol++) {
      PyObject* pyDouble = PyFloat_FromDouble(matrix(iRow, iCol));
      PyTuple_SET_ITEM(row, iCol, pyDouble);
    }
    PyTuple_SET_ITEM(tuple, iRow, row);
  }
  return tuple;
}

PyObject* valueToPython(const command::Value& value) {
  using command::Value;
  bool boolValue;
  unsigned unsignedValue;
  int intValue;
  float floatValue;
  double doubleValue;
  std::string stringValue;
  Vector vectorValue;
  Matrix matrixValue;
  Eigen::Matrix4d matrix4dValue;
  switch (value.type()) {
    case (Value::BOOL):
      boolValue = value.value();
      if (boolValue) {
        return PyBool_FromLong(1);
      }
      return PyBool_FromLong(0);
    case (Value::UNSIGNED):
      unsignedValue = value.value();
      return Py_BuildValue("I", unsignedValue);
    case (Value::INT):
      intValue = value.value();
      return Py_BuildValue("i", intValue);
    case (Value::FLOAT):
      floatValue = value.value();
      return Py_BuildValue("f", floatValue);
    case (Value::DOUBLE):
      doubleValue = value.value();
      return Py_BuildValue("d", doubleValue);
    case (Value::STRING):
      stringValue = (std::string)value.value();
      return Py_BuildValue("s", stringValue.c_str());
    case (Value::VECTOR):
      vectorValue = value.value();
      return vectorToPython(vectorValue);
    case (Value::MATRIX):
      matrixValue = value.value();
      return matrixToPython(matrixValue);
    case (Value::MATRIX4D):
      matrix4dValue = value.value();
      return matrix4dToPython(matrix4dValue);
    default:
      return Py_BuildValue("");
  }
  return Py_BuildValue("");
}

}  // namespace convert
}  // namespace python
}  // namespace dynamicgraph
