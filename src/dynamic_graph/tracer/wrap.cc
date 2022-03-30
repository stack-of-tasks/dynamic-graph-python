#include <dynamic-graph/tracer.h>

#include "dynamic-graph/python/module.hh"

BOOST_PYTHON_MODULE(wrap) {
  using dynamicgraph::Tracer;

  bp::import("dynamic_graph");
  dynamicgraph::python::exposeEntity<Tracer>().def("addSignal",
                                                   &Tracer::addSignalToTrace);
}
