#include <dynamic-graph/tracer-real-time.h>

#include "dynamic-graph/python/module.hh"

BOOST_PYTHON_MODULE(wrap) {
  using dynamicgraph::Tracer;
  using dynamicgraph::TracerRealTime;

  bp::import("dynamic_graph.tracer");
  dynamicgraph::python::exposeEntity<TracerRealTime, bp::bases<Tracer> >();
}
