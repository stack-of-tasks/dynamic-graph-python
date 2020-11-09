#ifndef DYNAMIC_GRAPH_PY
#define DYNAMIC_GRAPH_PY

#include <iostream>
#include <sstream>

#include <boost/python.hpp>

#include <dynamic-graph/debug.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/signal-base.h>

#include "dynamic-graph/python/signal-wrapper.hh"

namespace bp = boost::python;

namespace dynamicgraph {
namespace python {

template <typename Iterator>
inline bp::list to_py_list(Iterator begin, Iterator end) {
  typedef typename Iterator::value_type T;
  bp::list lst;
  std::for_each(begin, end, [&](const T& t) { lst.append(t); });
  return lst;
}

template <typename Iterator>
inline bp::tuple to_py_tuple(Iterator begin, Iterator end) {
  return bp::tuple(to_py_list(begin, end));
}

template <typename T>
inline std::vector<T> to_std_vector(const bp::object& iterable) {
  return std::vector<T>(bp::stl_input_iterator<T>(iterable), bp::stl_input_iterator<T>());
}

void exposeSignals();

// Declare functions defined in other source files
namespace signalBase {
SignalBase<int>* createSignalWrapper(const char* name, const char* type, bp::object object);
}  // namespace signalBase
namespace entity {

/// \param obj an Entity object
void addCommands(boost::python::object obj);
void addSignals(boost::python::object obj);

Entity* create(const char* type, const char* name);
bp::object executeCmd(bp::tuple args, bp::dict);
}  // namespace entity

namespace factory {
bp::tuple getEntityClassList();
}
namespace pool {
void writeGraph(const char* filename);
bp::list getEntityList();
const std::map<std::string, Entity*>* getEntityMap();
}  // namespace pool
namespace debug {
void addLoggerFileOutputStream(const char* filename);
void addLoggerCoutOutputStream();
void closeLoggerFileOutputStream();
void realTimeLoggerSpinOnce();
void realTimeLoggerDestroy();
void realTimeLoggerInstance();
}  // namespace debug

}  // namespace python
}  // namespace dynamicgraph

#endif
