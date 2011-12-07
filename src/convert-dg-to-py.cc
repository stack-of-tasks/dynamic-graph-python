// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph-python is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// dynamic-graph-python is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Lesser Public License for more details.  You should
// have received a copy of the GNU Lesser General Public License along
// with dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#include "../src/convert-dg-to-py.hh"
#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-caster.h>



namespace dynamicgraph {

  using ::dynamicgraph::SignalBase;

  namespace python {
    namespace convert {


      void fillMatrixRow(Matrix& m, unsigned iRow, PyObject* tuple)
      {
	if (PyTuple_Size(tuple) != (int)m.nbCols()) {
	  throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
				 "lines of matrix have different sizes.");
	}
	for (unsigned int iCol=0; iCol < m.nbCols(); iCol++) {
	  PyObject* pyDouble = PyTuple_GetItem(tuple, iCol);
	  if (PyFloat_Check(pyDouble))
	    m(iRow, iCol) = PyFloat_AsDouble(pyDouble);
	  else if(PyInt_Check(pyDouble))
	    m(iRow, iCol) = (int)PyInt_AS_LONG(pyDouble)+0.0;
	  else
	    throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
				   "element of matrix should be "
				   "a floating point number.");
	}
      }

      command::Value pythonToValue(PyObject* pyObject,
				   const command::Value::Type& valueType)
      {
	using command::Value;
	bool bvalue;
	unsigned uvalue;
	int ivalue;
	float fvalue;
	double dvalue;
	std::string svalue;
	Vector v;
	Matrix m;
	unsigned int nCols;
	unsigned int size;
	PyObject* row;
	unsigned int nRows;

	switch (valueType) {
	case (Value::BOOL) :
	  if (!PyBool_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "bool");
	  }
	  bvalue = PyObject_IsTrue(pyObject);
	  return Value(bvalue);
	  break;
	case (Value::UNSIGNED) :
	  if (!PyInt_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "unsigned int");
	  }
	  uvalue = PyInt_AsUnsignedLongMask(pyObject);
	  return Value(uvalue);
	  break;
	case (Value::INT) :
	  if (!PyInt_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "int");
	  }
	  ivalue = (int)PyInt_AS_LONG(pyObject);
	  return Value(ivalue);
	  break;
	case (Value::FLOAT) :
	  if (PyFloat_Check(pyObject)) {
	    fvalue = (float)PyFloat_AsDouble(pyObject);
	    return Value(fvalue);
	  } else if (PyInt_Check(pyObject)) {
	    fvalue = (float)PyInt_AS_LONG(pyObject);
	    return Value(fvalue);
	  } else {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "float");
	  }
	  break;
	case (Value::DOUBLE) :
	  if (PyFloat_Check(pyObject)) {
	    dvalue = PyFloat_AsDouble(pyObject);
	    return Value(dvalue);
	  } else if (PyInt_Check(pyObject)) {
	    dvalue = 0.0+PyInt_AS_LONG(pyObject);
	    return Value(dvalue);
	  } else {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "double");
	  }
	  break;
	case (Value::STRING) :
	  if (!PyString_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "string");
	  }
	  svalue = PyString_AsString(pyObject);
	  return Value(svalue);
	  break;
	case (Value::VECTOR) :
	  // Check that argument is a tuple
	  if (!PyTuple_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "vector");
	  }
	  size = PyTuple_Size(pyObject);
	  v.resize(size);
	  for (unsigned int i=0; i<size; i++) {
	    PyObject* pyDouble = PyTuple_GetItem(pyObject, i);
	    if (PyFloat_Check(pyDouble))
	      v(i) = PyFloat_AsDouble(pyDouble);
	    else if(PyInt_Check(pyDouble))
	      v(i) = (int)PyInt_AS_LONG(pyDouble)+0.0;
	    else
	      throw ExceptionPython(ExceptionPython::VECTOR_PARSING,
				     "element of vector should be a floating "
				     "point number.");
	  }
	  return Value(v);
	  break;
	case (Value::MATRIX) :
	  // Check that argument is a tuple
	  if (!PyTuple_Check(pyObject)) {
	    throw ExceptionPython(ExceptionPython::VALUE_PARSING,
				   "matrix");
	  }
	  nRows = PyTuple_Size(pyObject);
	  if (nRows == 0) {
	    return Value(Matrix());
	  }
	  row = PyTuple_GetItem(pyObject, 0);
	  if (!PyTuple_Check(row)) {
	    throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
				   "matrix");
	  }
	  nCols = PyTuple_Size(row);

	  m.resize(nRows, nCols);
	  fillMatrixRow(m, 0, row);

	  for (unsigned int iRow=1; iRow<nRows; iRow++) {
	    row = PyTuple_GetItem(pyObject, iRow);
	    if (!PyTuple_Check(row)) {
	      throw ExceptionPython(ExceptionPython::MATRIX_PARSING,
				     "matrix");
	    }
	    fillMatrixRow(m, iRow, row);
	  }
	  return Value(m);
	  break;
	default:
	  std::cerr << "Only int, double and string are supported."
		    << std::endl;
	}
	return Value();
      }

      PyObject* vectorToPython(const Vector& vector)
      {
	PyObject* tuple = PyTuple_New(vector.size());
	for (unsigned int index = 0; index < vector.size(); index++) {
	  PyObject* pyDouble = PyFloat_FromDouble(vector(index));
	  PyTuple_SET_ITEM(tuple, index, pyDouble);
	}
	return tuple;
      }

      PyObject* matrixToPython(const Matrix& matrix)
      {
	PyObject* tuple = PyTuple_New(matrix.nbRows());
	for (unsigned int iRow = 0; iRow < matrix.nbRows(); iRow++) {
	  PyObject* row = PyTuple_New(matrix.nbCols());
	  for (unsigned iCol=0; iCol < matrix.nbCols(); iCol++) {
	    PyObject* pyDouble = PyFloat_FromDouble(matrix(iRow, iCol));
	    PyTuple_SET_ITEM(row, iCol, pyDouble);
	  }
	  PyTuple_SET_ITEM(tuple, iRow, row);
	}
	return tuple;
      }

      PyObject* valueToPython(const command::Value& value)
      {
	using command::Value;
	bool boolValue;
	unsigned unsignedValue;
	int intValue;
	float floatValue;
	double doubleValue;
	std::string stringValue;
	Vector vectorValue;
	Matrix matrixValue;

	switch(value.type()) {
	case (Value::BOOL) :
	  boolValue = value.value();
	  if (boolValue) {
	    return PyBool_FromLong(1);
	  }
	  return PyBool_FromLong(0);
	case (Value::UNSIGNED) :
	  unsignedValue = value.value();
	  return Py_BuildValue("I", unsignedValue);
	case (Value::INT) :
	  intValue = value.value();
	  return Py_BuildValue("i", intValue);
	case (Value::FLOAT) :
	  floatValue = value.value();
	  return Py_BuildValue("f", floatValue);
	case (Value::DOUBLE) :
	  doubleValue = value.value();
	  return Py_BuildValue("d", doubleValue);
	case (Value::STRING) :
	  stringValue = (std::string) value.value();
	  return Py_BuildValue("s", stringValue.c_str());
	case (Value::VECTOR) :
	  vectorValue = value.value();
	  return vectorToPython(vectorValue);
	case (Value::MATRIX) :
	  matrixValue = value.value();
	  return matrixToPython(matrixValue);
	default:
	  return Py_BuildValue("");
	}
	return Py_BuildValue("");
      }

    } // namespace dynamicgraph
  } // namespace python
} // namespace convert
