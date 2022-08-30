# Python bindings

add_subdirectory(dynamic_graph)

set(PYTHON_SOURCES __init__.py attrpath.py entity.py signal_base.py
                   script_shortcuts.py tools.py)

foreach(source ${PYTHON_SOURCES})
  python_install_on_site(dynamic_graph ${source})
endforeach(source)

# --- ADD the wrap on the dg modules
link_directories(${DYNAMIC_GRAPH_PLUGINDIR})
dynamic_graph_python_module(
  "tracer" dynamic-graph::tracer tracer-wrap SOURCE_PYTHON_MODULE
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_graph/tracer/wrap.cc)
dynamic_graph_python_module(
  "tracer_real_time" dynamic-graph::tracer-real-time tracer_real_time-wrap
  SOURCE_PYTHON_MODULE
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_graph/tracer_real_time/wrap.cc)
