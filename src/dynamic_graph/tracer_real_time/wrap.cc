#include "dynamic-graph/python/module.hh"

#include <dynamic-graph/tracer-real-time.h>

BOOST_PYTHON_MODULE(wrap)
{
  using dynamicgraph::Tracer;
  using dynamicgraph::TracerRealTime;

  bp::import("dynamic_graph.tracer");
  dynamicgraph::python::exposeEntity<TracerRealTime,
    bp::bases<Tracer> >()
    ;
}
