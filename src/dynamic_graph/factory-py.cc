// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include <iostream>
#include <dynamic-graph/factory.h>

#include "dynamic-graph/python/dynamic-graph-py.hh"

using dynamicgraph::Entity;
using dynamicgraph::ExceptionAbstract;

namespace dynamicgraph {
namespace python {

namespace factory {

/**
   \brief Get name of entity
*/
bp::tuple getEntityClassList() {
  std::vector<std::string> classNames;
  dynamicgraph::FactoryStorage::getInstance()->listEntities(classNames);
  return to_py_tuple(classNames.begin(), classNames.end());
}

}  // namespace factory
}  // namespace python
}  // namespace dynamicgraph
