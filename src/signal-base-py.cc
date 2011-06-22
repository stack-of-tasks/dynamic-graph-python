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
#include <dynamic-graph/signal.h>
#include <dynamic-graph/signal-caster.h>
#include <dynamic-graph/linear-algebra.h>

#include "convert-dg-to-py.hh"
#include "exception.hh"

using dynamicgraph::SignalBase;

namespace dynamicgraph {
  namespace python {

    extern PyObject* dgpyError;
    using namespace convert;

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

      PyObject* setTime(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	int time;
	if (!PyArg_ParseTuple(args,"Oi", &object, &time))
	  return NULL;
	if (!PyCObject_Check(object)) {
	  PyErr_SetString(dgpyError, "object should be a C object");
	  return NULL;
	}

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* obj = (SignalBase<int>*)pointer;

	obj->setTime(time);
	return Py_BuildValue("");
      }

      PyObject* display(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args,"O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* obj = (SignalBase<int>*)pointer;

	std::ostringstream oss;
	try {
	  obj->display(oss);
	} CATCH_ALL_EXCEPTIONS ();

	return Py_BuildValue("s", oss.str().c_str());
      }

      PyObject* displayDependencies(PyObject* /*self*/, PyObject* args)
      {
	void* pointer = NULL;
	PyObject* object = NULL;
	int time;
	if (!PyArg_ParseTuple(args,"OI", &object,&time))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* obj = (SignalBase<int>*)pointer;

	std::ostringstream oss;
	try {
	  obj->displayDependencies(oss,time);
	} CATCH_ALL_EXCEPTIONS ();
	return Py_BuildValue("s", oss.str().c_str());
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

	try {
	  { // --- VECTOR SIGNALS -----------------
	    // Two cases: the signal embeds directly a vector, or embeds
	    // an object deriving from vector.In the first case,
	    // the signal is directly cast into sig<vector>.
	    // In the second case, the derived object can be access as a vector
	    // using the signal-ptr<vector> type.
	    Signal<dynamicgraph::Vector,int> * sigvec
	      = dynamic_cast< Signal<dynamicgraph::Vector,int>* >( signal );
	    if( NULL!= sigvec ) {
	      return vectorToPython( sigvec->accessCopy() );
	    }

	    // Extraction of object derinving from vector: plug signal into
	    // a vector signal and get the value from the signal-ptr instead
	    // of the original vector.
	    SignalPtr<dynamicgraph::Vector,int> sigptr(NULL,"vector-caster");
	    try {
	      sigptr.plug(signal);
	      return vectorToPython( sigptr.accessCopy() );
	    }
	    catch( dynamicgraph::ExceptionSignal& ex ) {
	      if( ex.getCode() !=
		  dynamicgraph::ExceptionSignal::PLUG_IMPOSSIBLE )
		throw;
	    }
	  }

	  { // --- MATRIX SIGNALS --------------------
	    // Two cases: the signal embeds directly a matrix, or embeds
	    // an object deriving from matrix.In the first case,
	    // the signal is directly cast into sig<matrix>.
	    // In the second case, the derived object can be access as a matrix
	    // using the signal-ptr<matrix> type.
	    Signal<dynamicgraph::Matrix,int> * sigmat
	      = dynamic_cast< Signal<dynamicgraph::Matrix,int>* >( signal );
	    if( NULL!= sigmat ) {
	      return matrixToPython( sigmat->accessCopy() );
	    }

	    SignalPtr<dynamicgraph::Matrix,int> sigptr(NULL,"matrix-caster");
	    try {
	      sigptr.plug(signal);
	      return matrixToPython( sigptr.accessCopy() );
	    }
	    catch( dynamicgraph::ExceptionSignal& ex ) {
	      if( ex.getCode() !=
		  dynamicgraph::ExceptionSignal::PLUG_IMPOSSIBLE )
		throw;
	    }
	  }

	  Signal<double,int> * sigdouble
	    = dynamic_cast< Signal<double,int>* >( signal );
	  if( NULL!= sigdouble )
	    {
	      return Py_BuildValue("d", sigdouble->accessCopy() );
	    }
	} CATCH_ALL_EXCEPTIONS ();

	/* Non specific signal: use a generic way. */
	std::ostringstream value;
	try {
	  signal->get(value);
	} CATCH_ALL_EXCEPTIONS ();

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
	} CATCH_ALL_EXCEPTIONS ();

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
	} CATCH_ALL_EXCEPTIONS ();
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
	} CATCH_ALL_EXCEPTIONS ();
	return Py_BuildValue("");
      }

      PyObject* unplug(PyObject* /*self*/, PyObject* args)
      {
	void * pointer = NULL;
	PyObject* object = NULL;
	if (!PyArg_ParseTuple(args,"O", &object))
	  return NULL;
	if (!PyCObject_Check(object))
	  return NULL;

	pointer = PyCObject_AsVoidPtr(object);
	SignalBase<int>* signal = (SignalBase<int>*)pointer;
	try {
	  signal->unplug();
	} CATCH_ALL_EXCEPTIONS ();
	return Py_BuildValue("");
      }
    }
  }
}
