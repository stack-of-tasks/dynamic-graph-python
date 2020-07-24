#  Python bindings

ADD_SUBDIRECTORY(dynamic_graph)

SET(PYTHON_SOURCES
  __init__.py
  attrpath.py
  entity.py
  signal_base.py
  script_shortcuts.py
  tools.py
  )

FOREACH(source ${PYTHON_SOURCES})
  PYTHON_INSTALL_ON_SITE(dynamic_graph ${source})
ENDFOREACH(source)

# --- ADD the wrap on the dg modules
DYNAMIC_GRAPH_PYTHON_MODULE("tracer" dynamic-graph::tracer tracer-wrap)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer_real_time" dynamic-graph::tracer-real-time
  tracer_real_time-wrap)
