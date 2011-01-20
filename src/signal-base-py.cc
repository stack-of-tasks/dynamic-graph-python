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

#include <Python.h>
#include <iostream>
#include <sstream>

#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal-caster.h>

using dynamicgraph::SignalBase;

namespace dynamicgraph {
  namespace python {

    extern PyObject* error;

    namespace signalBase {

      static void destroy (void* self);

      /**
	 \brief Create an instance of SignalBase
      */
      PyObject* create(PyObject* /*self*/, PyObject* args)
      {
	char *name = NULL;

	if (!PyArg_ParseTuple(args, "s", &name))
	  return NULL;

	SignalBase<int>* obj = NULL;
	obj = new SignalBase<int>(std::string(name));

	// Return the pointer
	return PyCObject_FromVoidPtr((void*)obj, destroy);
      }

      /**
	 \brief Destroy an instance of InvertedPendulum
      */
      static void destroy (void* self)
      {
	SignalBase<int>* obj = (SignalBase<int>*)self;
	delete obj;
      }

      PyObject* getTime(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args,"O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* obj = (SignalBase<int>*)pointer;

	int time = obj->getTime();
	return Py_BuildValue("i", time);
      }

      PyObject* getValue(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args,"O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* signal = (SignalBase<int>*)pointer;

	std::ostringstream value;
	try {
	  signal->get(value);
	} catch (const dynamicgraph::ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	} catch (...) {
	  PyErr_SetString(error, "Unknown exception");
	  return NULL;
	}
	std::string valueString = value.str();
	return Py_BuildValue("s", valueString.c_str());
      }

      PyObject* getName(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args,"O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* signal = (SignalBase<int>*)pointer;

	std::string name;
	try {
	  name = signal->getName();
	} catch (const dynamicgraph::ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	} catch (...) {
	  PyErr_SetString(error, "Unknown exception");
	  return NULL;
	}
	return Py_BuildValue("s", name.c_str());
      }

      PyObject* setValue(PyObject* /*self*/, PyObject* args)
      {
	void * pointer = NULL;
	PyObject* object = NULL;
	char* valueString = NULL;

	if (!PyArg_ParseTuple(args,"Os", &object, &valueString))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* signal = (SignalBase<int>*)pointer;
	std::ostringstream os;
	os << valueString;
	std::istringstream value(os.str());

	try {
	  signal->set(value);
	} catch (const dynamicgraph::ExceptionAbstract& exc) {
	  PyErr_SetString(error, exc.getStringMessage().c_str());
	  return NULL;
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	  return NULL;
	} catch (...) {
	  PyErr_SetString(error, "Unknown exception");
	  return NULL;
	}

	return Py_BuildValue("");
      }

      PyObject* recompute(PyObject* /*self*/, PyObject* args)
      {
	void * pointer = NULL;
	PyObject* object = NULL;
	unsigned int time;
	if (!PyArg_ParseTuple(args,"OI", &object, &time))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* signal = (SignalBase<int>*)pointer;
	try {
	  signal->recompute(time);
	} catch (const std::exception& exc) {
	  PyErr_SetString(error, exc.what());
	}
	return Py_BuildValue("");
      }
    }
  }
}
