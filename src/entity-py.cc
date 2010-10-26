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

#include "dynamic-graph/command.h"
#include "dynamic-graph/value.h"

using dynamicgraph::Entity;
using dynamicgraph::SignalBase;
using dynamicgraph::ExceptionAbstract;
using dynamicgraph::command::Command;
using dynamicgraph::command::Value;
using dynamicgraph::command::AnyType;

namespace dynamicgraph {
  namespace python {

    extern PyObject* error;

    namespace entity {

      static void destroy (void* self);
      static Value pythonToValue(PyObject* pyObject,
					const Value::Type& valueType);
      static PyObject* valueToPython(Value value);
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
	} catch (dynamicgraph::ExceptionFactory& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	}

	// Return the pointer as a PyCObject
	return PyCObject_FromVoidPtr((void*)obj, destroy);
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

      Value pythonToValue(PyObject* pyObject,
			  const Value::Type& valueType)
      {
	double dvalue;
	std::string svalue;
	int ivalue;

	switch (valueType) {
	case (Value::INT) :
	  if (!PyLong_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "float");
	  }
	  ivalue = (int)PyLong_AsLong(pyObject);
	  std::cout << "int param = " << ivalue << std::endl;
	  return Value(ivalue);
	  break;
	case (Value::DOUBLE) :
	  if (!PyFloat_Check(pyObject)) {
	    throw ExceptionFactory(ExceptionFactory::GENERIC,
				   "float");
	  }
	  dvalue = PyFloat_AsDouble(pyObject);
	  std::cout << "double param = " << dvalue << std::endl;
	  return Value(dvalue);
	  break;
	case (Value::STRING) :
	  svalue = PyString_AsString(pyObject);
	  std::cout << "string param = \"" << dvalue << "\"" << std::endl;
	  return Value(svalue);
	  break;
	default:
	  std::cerr << "Only int, double and string are supported."
		    << std::endl;
	}
	return Value();
      }

      PyObject* valueToPython(Value value)
      {
	int intValue;
	double doubleValue;
	std::string stringValue;

	switch(value.type()) {
	case (Value::INT) :
	  intValue = value.value();
	  return Py_BuildValue("i", intValue);
	case (Value::DOUBLE) :
	  doubleValue = value.value();
	  return Py_BuildValue("d", doubleValue);
	case (Value::STRING) :
	  stringValue = (std::string) value.value();
	  return Py_BuildValue("s", stringValue.c_str());
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
	std::cout << "executeCommand:"<< std::endl;
	for (unsigned int i=0; i<valueVector.size(); i++) {
	  std::cout << "  value[" << i << "]=(" 
		    << valueVector[i] << ")" << std::endl;
	}
	command->setParameterValues(valueVector);
	try {
	  Value result = command->execute();
	  return valueToPython(result);
	} catch (const ExceptionAbstract& exc) {
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
    }
  }
}
