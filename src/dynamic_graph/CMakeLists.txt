# Copyright 2010-2021, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem
# Saurel, JRL, CNRS/AIST, LAAS-CNRS

set(PYTHON_MODULE wrap)

add_library(
  ${PYTHON_MODULE} MODULE debug-py.cc dynamic-graph-py.cc factory-py.cc
                          pool-py.cc signal-base-py.cc signal-wrapper.cc)

target_link_libraries(${PYTHON_MODULE} PUBLIC ${PROJECT_NAME} eigenpy::eigenpy)

# Remove prefix lib
set_target_properties(${PYTHON_MODULE} PROPERTIES PREFIX "")

if(UNIX AND NOT APPLE)
  set_target_properties(${PYTHON_MODULE} PROPERTIES INSTALL_RPATH
                                                    "\$ORIGIN/../../..")
endif()

install(
  TARGETS ${PYTHON_MODULE}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${PYTHON_SITELIB}/dynamic_graph)
