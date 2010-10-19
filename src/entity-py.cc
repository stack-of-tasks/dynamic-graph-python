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

using dynamicgraph::Entity;
using dynamicgraph::SignalBase;
using dynamicgraph::ExceptionAbstract;

namespace dynamicgraph {
  namespace python {
    
    extern PyObject* error;

    namespace entity {
      
      static void destroy (void* self);
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
    }
  }
}
