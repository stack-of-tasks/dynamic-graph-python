// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>

#include <dynamic-graph/entity.h>
#include <dynamic-graph/factory.h>

#include <dynamic-graph/command.h>
#include <dynamic-graph/value.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/linear-algebra.h>

#include "dynamic-graph/python/dynamic-graph-py.hh"
#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/exception.hh"

// Ignore "dereferencing type-punned pointer will break strict-aliasing rules"
// warnings on gcc caused by Py_RETURN_TRUE and Py_RETURN_FALSE.
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

using dynamicgraph::Entity;
using dynamicgraph::Matrix;
using dynamicgraph::SignalBase;
using dynamicgraph::Vector;
using dynamicgraph::command::Command;
using dynamicgraph::command::Value;

namespace dynamicgraph {
namespace python {

using namespace convert;

namespace entity {

/**
   \brief Create an instance of Entity
*/
PyObject* create(PyObject* /*self*/, PyObject* args) {
  char* className = NULL;
  char* instanceName = NULL;

  if (!PyArg_ParseTuple(args, "ss", &className, &instanceName)) return NULL;

  Entity* obj = NULL;
  /* Try to find if the corresponding object already exists. */
  if (dynamicgraph::PoolStorage::getInstance()->existEntity(instanceName, obj)) {
    if (obj->getClassName() != className) {
      std::string msg("Found an object named " + std::string(instanceName) +
                      ",\n"
                      "but this object is of type " +
                      std::string(obj->getClassName()) + " and not " + std::string(className));
      PyErr_SetString(DGPYERROR, msg.c_str());
      return NULL;
    }
  } else /* If not, create a new object. */
  {
    try {
      obj = dynamicgraph::FactoryStorage::getInstance()->newEntity(std::string(className), std::string(instanceName));
    }
    CATCH_ALL_EXCEPTIONS();
  }

  // Return the pointer as a PyCapsule
  return PyCapsule_New((void*)obj, "dynamic_graph.Entity", NULL);
}

/**
   \brief Get name of entity
*/
PyObject* getName(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  void* pointer = NULL;
  std::string name;

  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "function takes a PyCapsule as argument");
    return NULL;
  }

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  try {
    name = entity->getName();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("s", name.c_str());
}

/**
   \brief Get class name of entity
*/
PyObject* getClassName(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  void* pointer = NULL;
  std::string name;

  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "function takes a PyCapsule as argument");
    return NULL;
  }

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  try {
    name = entity->getClassName();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("s", name.c_str());
}

/**
   \brief Check if the entity has a signal with the given name
*/
PyObject* hasSignal(PyObject* /*self*/, PyObject* args) {
  char* name = NULL;
  PyObject* object = NULL;
  void* pointer = NULL;

  if (!PyArg_ParseTuple(args, "Os", &object, &name)) Py_RETURN_FALSE;

  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "function takes a PyCapsule as argument");
    Py_RETURN_FALSE;
  }

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  bool hasSignal = false;
  try {
    hasSignal = entity->hasSignal(std::string(name));
  }
  CATCH_ALL_EXCEPTIONS();

  if (hasSignal)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/**
   \brief Get a signal by name
*/
PyObject* getSignal(PyObject* /*self*/, PyObject* args) {
  char* name = NULL;
  PyObject* object = NULL;
  void* pointer = NULL;

  if (!PyArg_ParseTuple(args, "Os", &object, &name)) return NULL;

  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "function takes a PyCapsule as argument");
    return NULL;
  }

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  SignalBase<int>* signal = NULL;
  try {
    signal = &(entity->getSignal(std::string(name)));
  }
  CATCH_ALL_EXCEPTIONS();

  // Return the pointer to the signal without destructor since the signal
  // is not owned by the calling object but by the Entity.
  return PyCapsule_New((void*)signal, "dynamic_graph.Signal", NULL);
}

PyObject* listSignals(PyObject* /*self*/, PyObject* args) {
  void* pointer = NULL;
  PyObject* object = NULL;

  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;

  if (!PyCapsule_CheckExact(object)) return NULL;

  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  try {
    Entity::SignalMap signalMap = entity->getSignalMap();
    // Create a tuple of same size as the command map
    PyObject* result = PyTuple_New(signalMap.size());
    unsigned int count = 0;

    for (Entity::SignalMap::iterator it = signalMap.begin(); it != signalMap.end(); it++) {
      SignalBase<int>* signal = it->second;
      PyObject* pySignal = PyCapsule_New((void*)signal, "dynamic_graph.Signal", NULL);
      PyTuple_SET_ITEM(result, count, pySignal);
      count++;
    }
    return result;
  }
  CATCH_ALL_EXCEPTIONS();
  return NULL;
}

PyObject* executeCommand(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  PyObject* argTuple = NULL;
  char* commandName = NULL;
  void* pointer = NULL;
  if (!PyArg_ParseTuple(args, "OsO", &object, &commandName, &argTuple)) {
    return NULL;
  }
  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "first argument is not an object");
    return NULL;
  }
  pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;
  // Retrieve the argument tuple
  if (!PyTuple_Check(argTuple)) {
    PyErr_SetString(PyExc_TypeError, "third argument is not a tuple");
    return NULL;
  }
  Py_ssize_t size = PyTuple_Size(argTuple);
  std::map<const std::string, Command*> commandMap = entity->getNewStyleCommandMap();
  if (commandMap.count(std::string(commandName)) != 1) {
    std::ostringstream oss;
    oss << "'" << entity->getName() << "' entity has no command '" << commandName << "'.";
    PyErr_SetString(PyExc_AttributeError, oss.str().c_str());
    return NULL;
  }
  Command* command = commandMap[std::string(commandName)];
  // Check that tuple size is equal to command number of arguments
  const std::vector<Value::Type> typeVector = command->valueTypes();
  if ((unsigned)size != typeVector.size()) {
    std::stringstream ss;
    ss << "command takes " << typeVector.size() << " parameters, " << size << " given.";
    PyErr_SetString(DGPYERROR, ss.str().c_str());
    return NULL;
  }
  std::vector<Value> valueVector;
  for (Py_ssize_t iParam = 0; iParam < size; iParam++) {
    PyObject* PyValue = PyTuple_GetItem(argTuple, iParam);
    Value::Type valueType = typeVector[iParam];
    try {
      Value value = pythonToValue(PyValue, valueType);
      valueVector.push_back(value);
    } catch (const std::exception& exc) {
      std::stringstream ss;
      ss << "while parsing argument " << iParam + 1 << ": expecting " << exc.what() << ".";
      PyErr_SetString(DGPYERROR, ss.str().c_str());
      return NULL;
    } catch (...) {
      PyErr_SetString(DGPYERROR, "Unknown exception");
      return NULL;
    }
  }
  command->setParameterValues(valueVector);
  try {
    Value result = command->execute();
    return valueToPython(result);
  }
  CATCH_ALL_EXCEPTIONS();
  return NULL;
}

PyObject* listCommands(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) {
    return NULL;
  }

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "function takes a PyCapsule as argument");
    return NULL;
  }
  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;
  typedef std::map<const std::string, command::Command*> CommandMap;
  CommandMap map = entity->getNewStyleCommandMap();
  Py_ssize_t nbCommands = (Py_ssize_t)map.size();
  // Create a tuple of same size as the command map
  PyObject* result = PyTuple_New(nbCommands);
  unsigned int count = 0;
  for (CommandMap::iterator it = map.begin(); it != map.end(); it++) {
    std::string commandName = it->first;
    PyObject* pyName = Py_BuildValue("s", commandName.c_str());
    PyTuple_SET_ITEM(result, count, pyName);
    count++;
  }
  return result;
}
PyObject* getCommandDocstring(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  char* commandName;
  if (!PyArg_ParseTuple(args, "Os", &object, &commandName)) {
    return NULL;
  }

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(DGPYERROR, "first argument is not an object");
    return NULL;
  }
  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;
  typedef std::map<const std::string, command::Command*> commandMap_t;
  typedef std::map<const std::string, command::Command*>::iterator iterator_t;
  commandMap_t map = entity->getNewStyleCommandMap();
  command::Command* command = NULL;
  iterator_t it = map.find(commandName);
  if (it == map.end()) {
    std::ostringstream oss;
    oss << "'" << entity->getName() << "' entity has no command '" << commandName << "'.";
    PyErr_SetString(PyExc_AttributeError, oss.str().c_str());
    return NULL;
  }
  command = it->second;
  std::string docstring = command->getDocstring();
  return Py_BuildValue("s", docstring.c_str());
}

PyObject* getDocString(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) {
    return NULL;
  }

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(DGPYERROR, "first argument is not an object");
    return NULL;
  }
  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;
  try {
    return Py_BuildValue("s", entity->getDocString().c_str());
  } catch (const std::exception& exc) {
    PyErr_SetString(DGPYERROR, exc.what());
    return NULL;
  } catch (...) {
    PyErr_SetString(DGPYERROR, "Unknown exception");
    return NULL;
  }
  return NULL;
}

PyObject* display(PyObject* /*self*/, PyObject* args) {
  /* Retrieve the entity instance. */
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object) || (!PyCapsule_CheckExact(object))) {
    PyErr_SetString(DGPYERROR, "first argument is not an object");
    return NULL;
  }
  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  /* Run the display function. */
  std::ostringstream oss;
  entity->display(oss);

  /* Return the resulting string. */
  return Py_BuildValue("s", oss.str().c_str());
}

/**
   \brief Set verbosity Level
*/
PyObject* setLoggerVerbosityLevel(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  PyObject* objectVerbosityLevel = NULL;
  if (!PyArg_ParseTuple(args, "OO", &object, &objectVerbosityLevel)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "First argument should be an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  // Retrieve object verbosity level
  PyObject* valueOfVerbosityLevel = PyObject_GetAttrString(objectVerbosityLevel, "value");
  long verbosityLevel = PyLong_AsLong(valueOfVerbosityLevel);

  try {
    switch (verbosityLevel) {
      case 0:
        entity->setLoggerVerbosityLevel(VERBOSITY_ALL);
        break;
      case 1:
        entity->setLoggerVerbosityLevel(VERBOSITY_INFO_WARNING_ERROR);
        break;
      case 2:
        entity->setLoggerVerbosityLevel(VERBOSITY_WARNING_ERROR);
        break;
      case 3:
        entity->setLoggerVerbosityLevel(VERBOSITY_ERROR);
        break;
      case 4:
        entity->setLoggerVerbosityLevel(VERBOSITY_NONE);
        break;
      default:
        entity->setLoggerVerbosityLevel(VERBOSITY_NONE);
        break;
    }
  } catch (const std::exception& exc) {
    PyErr_SetString(DGPYERROR, exc.what());
    return NULL;
  } catch (const char* s) {
    PyErr_SetString(DGPYERROR, s);
    return NULL;
  } catch (...) {
    PyErr_SetString(DGPYERROR, "Unknown exception");
    return NULL;
  }

  return Py_BuildValue("");
}

/**
   \brief Get verbosity Level
*/
PyObject* getLoggerVerbosityLevel(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "first argument is not an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  LoggerVerbosity alv;
  try {
    alv = entity->getLoggerVerbosityLevel();
  }
  CATCH_ALL_EXCEPTIONS();

  int ares = (int)alv;
  return Py_BuildValue("i", ares);
}

/**
   \brief Get stream print period
*/
PyObject* getStreamPrintPeriod(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "first argument is not an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  double r;
  try {
    r = entity->getStreamPrintPeriod();
  }
  CATCH_ALL_EXCEPTIONS();

  return Py_BuildValue("d", r);
}

/**
   \brief Set print period
*/
PyObject* setStreamPrintPeriod(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  double streamPrintPeriod = 0;
  if (!PyArg_ParseTuple(args, "Od", &object, &streamPrintPeriod)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "First argument should be an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  try {
    entity->setStreamPrintPeriod(streamPrintPeriod);

  } catch (const std::exception& exc) {
    PyErr_SetString(DGPYERROR, exc.what());
    return NULL;
  } catch (const char* s) {
    PyErr_SetString(DGPYERROR, s);
    return NULL;
  } catch (...) {
    PyErr_SetString(DGPYERROR, "Unknown exception");
    return NULL;
  }

  return Py_BuildValue("");
}

/**
   \brief Get stream print period
*/
PyObject* getTimeSample(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  if (!PyArg_ParseTuple(args, "O", &object)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "first argument is not an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  double r;
  try {
    r = entity->getTimeSample();
  }
  CATCH_ALL_EXCEPTIONS();

  return Py_BuildValue("d", r);
}

/**
   \brief Set time sample
*/
PyObject* setTimeSample(PyObject* /*self*/, PyObject* args) {
  PyObject* object = NULL;
  double timeSample;
  if (!PyArg_ParseTuple(args, "Od", &object, &timeSample)) return NULL;

  // Retrieve the entity instance
  if (!PyCapsule_CheckExact(object)) {
    PyErr_SetString(PyExc_TypeError, "First argument should be an object");
    return NULL;
  }

  void* pointer = PyCapsule_GetPointer(object, "dynamic_graph.Entity");
  Entity* entity = (Entity*)pointer;

  try {
    entity->setTimeSample(timeSample);

  } catch (const std::exception& exc) {
    PyErr_SetString(DGPYERROR, exc.what());
    return NULL;
  } catch (const char* s) {
    PyErr_SetString(DGPYERROR, s);
    return NULL;
  } catch (...) {
    PyErr_SetString(DGPYERROR, "Unknown exception");
    return NULL;
  }

  return Py_BuildValue("");
}

}  // namespace entity
}  // namespace python
}  // namespace dynamicgraph
