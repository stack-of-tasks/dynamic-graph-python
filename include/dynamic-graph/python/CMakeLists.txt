# Copyright 2010-2019, Florent Lamiraux, Guilhem Saurel, LAAS-CNRS

SET(${PROJECT_NAME}_HEADERS
  api.hh
  convert-dg-to-py.hh
  dynamic-graph-py.hh
  exception.hh
  exception-python.hh
  interpreter.hh
  python-compat.hh
  signal-wrapper.hh
)

INSTALL(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/${CUSTOM_HEADER_DIR})

CONFIG_FILES(link-to-python.hh)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/link-to-python.hh DESTINATION include/${CUSTOM_HEADER_DIR})
