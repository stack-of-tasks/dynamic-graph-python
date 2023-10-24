// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <dynamic-graph/command.h>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/value.h>

#include <iostream>

#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/dynamic-graph-py.hh"

// Ignore "dereferencing type-punned pointer will break strict-aliasing rules"
// warnings on gcc caused by Py_RETURN_TRUE and Py_RETURN_FALSE.
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

namespace bp = boost::python;

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

/// \param obj an Entity object
void addCommands(bp::object obj) {
  Entity& entity = bp::extract<Entity&>(obj);
  for (const auto& el : entity.getNewStyleCommandMap())
    obj.attr(el.first.c_str()) = bp::object(bp::ptr(el.second));
}

/// \param obj an Entity object
void addSignals(bp::object obj) {
  Entity& entity = bp::extract<Entity&>(obj);
  for (const auto& el : entity.getSignalMap())
    // obj.attr(el.first.c_str()) = bp::make_function(
    //+[&entity,el]() { return &entity.getSignal(el.first); },
    // bp::return_value_policy<bp::reference_existing_object>());
    obj.attr(el.first.c_str()) = bp::object(bp::ptr(el.second));
}

/**
   \brief Create an instance of Entity
*/
Entity* create(const char* className, const char* instanceName) {
  Entity* obj = NULL;
  /* Try to find if the corresponding object already exists. */
  if (dynamicgraph::PoolStorage::getInstance()->existEntity(instanceName,
                                                            obj)) {
    if (obj->getClassName() != className) {
      throw std::invalid_argument("Found an object named " +
                                  std::string(instanceName) +
                                  ",\n"
                                  "but this object is of type " +
                                  std::string(obj->getClassName()) +
                                  " and not " + std::string(className));
    }
  } else /* If not, create a new object. */
  {
    obj = dynamicgraph::FactoryStorage::getInstance()->newEntity(
        std::string(className), std::string(instanceName));
  }

  return obj;
}

bp::object executeCmd(bp::tuple args, bp::dict) {
  Command& command = bp::template extract<Command&>(args[0]);
  if (bp::len(args) != int(command.valueTypes().size() + 1))
    // TODO Put expected and given number of args
    throw std::out_of_range("Wrong number of arguments");
  std::vector<Value> values;
  values.reserve(command.valueTypes().size());
  for (bp::ssize_t i = 1; i < bp::len(args); ++i)
    values.push_back(convert::toValue(
        args[i],
        command
            .valueTypes()[static_cast<std::vector<Value>::size_type>(i - 1)]));
  command.setParameterValues(values);
  return convert::fromValue(command.execute());
}

}  // namespace entity
}  // namespace python
}  // namespace dynamicgraph
