/*
 *  Copyright 2010 (C) CNRS
 *  Author: Florent Lamiraux
 */

#include <Python.h>
#include <iostream>
//#include <sstream>
//#include <string>

#include <dynamic-graph/entity.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/linear-algebra.h>

#include "dynamic-graph/command.h"
#include "dynamic-graph/value.h"

using dynamicgraph::Entity;
using dynamicgraph::SignalBase;
using dynamicgraph::ExceptionAbstract;
using dynamicgraph::command::Command;
using dynamicgraph::command::Value;
using dynamicgraph::Vector;
using dynamicgraph::Matrix;

namespace dynamicgraph {
  namespace python {

    extern PyObject* error;

    namespace entity {

      static void destroy (void* self);
      static void fillMatrixRow(Matrix& m, unsigned index,
				PyObject* tuple);

      static Value pythonToValue(PyObject* pyObject,
				 const Value::Type& valueType);
      static PyObject* vectorToPython(const Vector& vector);
      static PyObject* valueToPython(const Value& value);

      /**
	 \brief Create an instance of Entity
      */
      PyObject* create(PyObject* self, PyObject* args)
      {
	char *className = NULL;
	char *instanceName = NULL;

	if (!PyArg_ParseTuple(args, "ss", &className, &instanceName))
	  return NULL;

	Entity* obj = NULL;
	try {
	  obj = dynamicgraph::g_factory.newEntity(std::string(className),
						  std::string(instanceName));
	} catch (std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	  return NULL;
	}

	// Return the pointer as a PyCObject
	return PyCObject_FromVoidPtr((void*)obj, NULL);
      }

      /**
	 \brief Destroy an instance of Entity
      */
      static void destroy (void* self)
      {
	Entity* obj = (Entity*)self;
	delete obj;
      }

      /**
	 \brief Get name of entity
      */
      PyObject* getName(PyObject* self, PyObject* args)
      {
	PyObject* object = NULL;
	void* pointer = NULL;
	std::string name;

	if (!PyArg_ParseTuple(args, "O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;

	try {
	 name = entity->getName();
	} catch(ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	}
	return Py_BuildValue("s", name.c_str());
      }

      /**
	 \brief Get a signal by name
      */
      PyObject* getSignal(PyObject* self, PyObject* args)
      {
	char *name = NULL;
	PyObject* object = NULL;
	void* pointer = NULL;

	if (!PyArg_ParseTuple(args, "Os", &object, &name))
	  return NULL;

	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;

	SignalBase<int>* signal = NULL;
	try {
	  signal = &(entity->getSignal(std::string(name)));
	} catch(ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	}
	// Return the pointer to the signal without destructor since the signal
	// is not owned by the calling object but by the Entity.
	return PyCObject_FromVoidPtr((void*)signal, NULL);
      }

      PyObject* displaySignals(PyObject* self, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;

	if (!PyArg_ParseTuple(args, "O", &object))
	  return NULL;

	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;
	try {
	  entity->displaySignalList(std::cout);
	} catch(ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	}
	return Py_BuildValue("");
      }

      void fillMatrixRow(Matrix& m, unsigned iRow, PyObject* tuple)
      {
	if (PyTuple_Size(tuple) != m.nbCols()) {
	  throw ExceptionFactory(ExceptionFactory::GENERIC,
				 "lines of matrix have different sizes.");
	}
	for (unsigned int iCol=0; iCol < m.nbCols(); iCol++) {
	  PyObject* pyDouble = PyTuple_GetItem(tuple, iCol);
	  if (!PyFloat_Check(pyDouble)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "element of matrix should be "
				   "a floating point number.");
	  }
	  m(iRow, iCol) = PyFloat_AsDouble(pyDouble);
	}
      }

      Value pythonToValue(PyObject* pyObject,
			  const Value::Type& valueType)
      {
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
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "bool");
	  }
	  bvalue = (pyObject == Py_True);
	  return Value(bvalue);
	  break;
	case (Value::UNSIGNED) :
	  if (!PyInt_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "unsigned int");
	  }
	  uvalue = PyInt_AsUnsignedLongMask(pyObject);
	  return Value(uvalue);
	  break;
	case (Value::INT) :
	  if (!PyInt_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "int");
	  }
	  ivalue = (int)PyInt_AS_LONG(pyObject);
	  return Value(ivalue);
	  break;
	case (Value::FLOAT) :
	  if (!PyFloat_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "float");
	  }
	  fvalue = (float)PyFloat_AsDouble(pyObject);
	  return Value(fvalue);
	  break;
	case (Value::DOUBLE) :
	  if (!PyFloat_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "float");
	  }
	  dvalue = PyFloat_AsDouble(pyObject);
	  return Value(dvalue);
	  break;
	case (Value::STRING) :
	  if (!PyString_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "string");
	  }
	  svalue = PyString_AsString(pyObject);
	  return Value(svalue);
	  break;
	case (Value::VECTOR) :
	  // Check that argument is a tuple
	  if (!PyTuple_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "vector");
	  }
	  size = PyTuple_Size(pyObject);
	  v.resize(size);
	  for (unsigned int i=0; i<size; i++) {
	    PyObject* pyDouble = PyTuple_GetItem(pyObject, i);
	    if (!PyFloat_Check(pyDouble)) {
	      throw ExceptionFactory(ExceptionFactory::GENERIC,
				     "element of vector should be a floating "
				     "point number.");
	    }
	    v(i) = PyFloat_AsDouble(pyDouble);
	  }
	  return Value(v);
	  break;
	case (Value::MATRIX) :
	  // Check that argument is a tuple
	  if (!PyTuple_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "matrix");
	  }
	  nRows = PyTuple_Size(pyObject);
	  if (nRows == 0) {
	    return Value(Matrix());
	  }
	  row = PyTuple_GetItem(pyObject, 0);
	  if (!PyTuple_Check(row)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "matrix");
	  }
	  nCols = PyTuple_Size(row);

	  m.resize(nRows, nCols);
	  fillMatrixRow(m, 0, row);

	  for (unsigned int iRow=1; iRow<nRows; iRow++) {
	    row = PyTuple_GetItem(pyObject, iRow);
	    if (!PyTuple_Check(row)) {
	      throw ExceptionFactory(ExceptionFactory::GENERIC,
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

      PyObject* valueToPython(const Value& value)
      {
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

      PyObject* executeCommand(PyObject* self, PyObject* args)
      {
	PyObject* object = NULL;
	PyObject* argTuple = NULL;
	char* commandName = NULL;
	void* pointer = NULL;

	if (!PyArg_ParseTuple(args, "OsO", &object, &commandName, &argTuple)) {
	  return NULL;
	}

	// Retrieve the entity instance
	if (!PyCObject_Check(object)) {
	  PyErr_SetString(error, "first argument is not an object");
	  return NULL;
	}
	pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;

	// Retrieve the argument tuple
	if (!PyTuple_Check(argTuple)) {
	  PyErr_SetString(error, "third argument is not a tuple");
	  return NULL;
	}
	unsigned int size = PyTuple_Size(argTuple);

	std::map<const std::string, Command*> commandMap =
	  entity->getNewStyleCommandMap();

	if (commandMap.count(std::string(commandName)) != 1) {
	  std::string msg = "command " + std::string(commandName) +
	    " is not referenced in Entity " + entity->getName();
	  PyErr_SetString(error, msg.c_str());
	  return NULL;
	}
	Command* command = commandMap[std::string(commandName)];
	// Check that tuple size is equal to command number of arguments
	const std::vector<Value::Type> typeVector = command->valueTypes();
	if (size != typeVector.size()) {
	  std::stringstream ss;
	  ss << "command takes " <<  typeVector.size()
	     << " parameters, " << size << " given.";
	  PyErr_SetString(error, ss.str().c_str());
	  return NULL;
	}

	std::vector<Value> valueVector;
	for (unsigned int iParam=0; iParam<size; iParam++) {
	  PyObject* PyValue = PyTuple_GetItem(argTuple, iParam);
	  Value::Type valueType = typeVector[iParam];
	  try {
	    Value value = pythonToValue(PyValue, valueType);
	    valueVector.push_back(value);
	  } catch (ExceptionAbstract& exc) {
	    std::stringstream ss;
	    ss << "argument " << iParam+1 << " should be a "
	       << exc.what() << ".";
	    PyErr_SetString(error, ss.str().c_str()) ;
	    return NULL;
	  }
	}
	command->setParameterValues(valueVector);
	try {
	  Value result = command->execute();
	  return valueToPython(result);
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what()) ;
	  return NULL;
	}
	return NULL;
      }

      PyObject* listCommands(PyObject* self, PyObject* args)
      {
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args, "O", &object)) {
	  return NULL;
	}

	// Retrieve the entity instance
	if (!PyCObject_Check(object)) {
	  PyErr_SetString(error, "first argument is not an object");
	  return NULL;
	}
	void* pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;
	typedef	std::map<const std::string, command::Command*>  CommandMap;
	CommandMap map = entity->getNewStyleCommandMap();
	unsigned int nbCommands = map.size();
	// Create a tuple of same size as the command map
	PyObject* result = PyTuple_New(nbCommands);
	unsigned int count = 0;
	for (CommandMap::iterator it=map.begin();
	     it != map.end(); it++) {
	  std::string commandName = it->first;
	  PyObject* pyName = Py_BuildValue("s", commandName.c_str());
	  PyTuple_SET_ITEM(result, count, pyName);
	  count++;
	}
	return result;
      }
      PyObject* getCommandDocstring(PyObject* self, PyObject* args)
      {
	PyObject* object = NULL;
	char* commandName;
	if (!PyArg_ParseTuple(args, "Os", &object, &commandName)) {
	  return NULL;
	}

	// Retrieve the entity instance
	if (!PyCObject_Check(object)) {
	  PyErr_SetString(error, "first argument is not an object");
	  return NULL;
	}
	void* pointer = PyCObject_AsVoidPtr(object);
	Entity* entity = (Entity*)pointer;
	typedef	std::map<const std::string, command::Command*>  CommandMap;
	CommandMap map = entity->getNewStyleCommandMap();
	command::Command* command = NULL;
	try {
	  command = map[commandName];
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	}
	std::string docstring = command->getDocstring();
	return Py_BuildValue("s", docstring.c_str());
      }
    }
  }
}
