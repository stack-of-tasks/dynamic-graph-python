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
LINK_DIRECTORIES(${DYNAMIC_GRAPH_PLUGINDIR})
DYNAMIC_GRAPH_PYTHON_MODULE("tracer" tracer tracer-wrap
  SOURCE_PYTHON_MODULE ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_graph/tracer/wrap.cc)
DYNAMIC_GRAPH_PYTHON_MODULE("tracer_real_time" tracer-real-time tracer_real_time-wrap
  SOURCE_PYTHON_MODULE ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_graph/tracer_real_time/wrap.cc)
