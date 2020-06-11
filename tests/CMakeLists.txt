# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem Saurel, JRL, CNRS/AIST, LAAS-CNRS

# Test the interpreter
ADD_UNIT_TEST(interpreter-test interpreter-test.cc)
TARGET_LINK_LIBRARIES(interpreter-test ${PROJECT_NAME})

# Test runfile
ADD_UNIT_TEST(interpreter-test-runfile interpreter-test-runfile.cc)
TARGET_LINK_LIBRARIES(interpreter-test-runfile ${PYTHON_LIBRARY} ${Boost_LIBRARIES} ${PROJECT_NAME})
TARGET_COMPILE_DEFINITIONS(interpreter-test-runfile PRIVATE PATH="${CMAKE_CURRENT_LIST_DIR}/")

# Test the module generation
## Create an entity

SET(LIBRARY_NAME "custom_entity")
ADD_LIBRARY(${LIBRARY_NAME} SHARED "${LIBRARY_NAME}.cpp")
IF(SUFFIX_SO_VERSION)
  SET_TARGET_PROPERTIES(${LIBRARY_NAME}
    PROPERTIES SOVERSION ${PROJECT_VERSION})
ENDIF(SUFFIX_SO_VERSION)

target_link_libraries(${LIBRARY_NAME} dynamic-graph::dynamic-graph)

## Create its bindings
## This mimics DYNAMIC_GRAPH_PYTHON_MODULE(${LIBRARY_NAME} ${LIBRARY_NAME} "${LIBRARY_NAME}-wrap")

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/submodule/__init__.py.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}/__init__.py
  )
SET(PYTHON_MODULE "${LIBRARY_NAME}-wrap")
set(DYNAMICGRAPH_MODULE_HEADER "${CMAKE_SOURCE_DIR}/tests/custom_entity_module.h")
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/python-module-py.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/python-module-py.cc
  @ONLY
  )
ADD_LIBRARY(${PYTHON_MODULE} MODULE ${CMAKE_CURRENT_BINARY_DIR}/python-module-py.cc)
SET_TARGET_PROPERTIES(${PYTHON_MODULE} PROPERTIES
  PREFIX ""
  OUTPUT_NAME ${LIBRARY_NAME}/wrap)

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} "-Wl,--no-as-needed")
TARGET_LINK_LIBRARIES(${PYTHON_MODULE}
  ${LIBRARY_NAME} dynamic-graph-python
  ${PYTHON_LIBRARY} ${Boost_PYTHON_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(${PYTHON_MODULE} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIRS})

## Test it
ADD_PYTHON_UNIT_TEST("test-custom-entity" "tests/test_custom_entity.py" src tests)

# also test other bindings, using this custom entity
ADD_PYTHON_UNIT_TEST("test-bindings" "tests/test_bindings.py" src tests)
