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

CONFIG_FILES(link-to-python.hh)
