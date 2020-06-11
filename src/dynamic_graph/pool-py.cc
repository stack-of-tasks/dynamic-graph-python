// Copyright 2011, 2012, Florent Lamiraux, LAAS-CNRS.

#include <dynamic-graph/pool.h>
#include <dynamic-graph/entity.h>
#include <vector>

#include "dynamic-graph/python/dynamic-graph-py.hh"

namespace dynamicgraph {
namespace python {

namespace pool {

void writeGraph(const char* filename)
{
  PoolStorage::getInstance()->writeGraph(filename);
}

const std::map<std::string, Entity*>* getEntityMap()
{
  return &PoolStorage::getInstance()->getEntityMap();
}

/**
   \brief Get list of entities
*/
bp::list getEntityList()
{
  std::vector<std::string> entityNames;
  bp::list res;
  const PoolStorage::Entities& listOfEntities =
    PoolStorage::getInstance()->getEntityMap();

  for (const auto& el : listOfEntities)
    res.append(el.second->getName());
  return res;
}

}  // namespace pool
}  // namespace python
}  // namespace dynamicgraph
