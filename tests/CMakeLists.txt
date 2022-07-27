# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem
# Saurel, JRL, CNRS/AIST, LAAS-CNRS

# Test the interpreter
add_unit_test(interpreter-test interpreter-test.cc)
target_link_libraries(interpreter-test PRIVATE ${PROJECT_NAME})

# Test runfile
add_unit_test(interpreter-test-runfile interpreter-test-runfile.cc)
target_link_libraries(interpreter-test-runfile PRIVATE ${PROJECT_NAME})
target_include_directories(interpreter-test-runfile
                           PRIVATE Boost::unit_test_framework)
target_compile_definitions(interpreter-test-runfile
                           PRIVATE PATH="${CMAKE_CURRENT_LIST_DIR}/")

# Test the module generation Create an entity

set(LIBRARY_NAME "custom_entity")
add_library(${LIBRARY_NAME} SHARED "${LIBRARY_NAME}.cpp")
if(SUFFIX_SO_VERSION)
  set_target_properties(${LIBRARY_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
endif(SUFFIX_SO_VERSION)

target_link_libraries(${LIBRARY_NAME} PRIVATE dynamic-graph::dynamic-graph)

# Create its bindings This mimics DYNAMIC_GRAPH_PYTHON_MODULE(${LIBRARY_NAME}
# ${LIBRARY_NAME} "${LIBRARY_NAME}-wrap")

configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/submodule/__init__.py.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}/__init__.py)
set(PYTHON_MODULE "${LIBRARY_NAME}-wrap")
set(DYNAMICGRAPH_MODULE_HEADER
    "${CMAKE_SOURCE_DIR}/tests/custom_entity_module.h")
configure_file(${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/python-module-py.cc.in
               ${CMAKE_CURRENT_BINARY_DIR}/python-module-py.cc @ONLY)
add_library(${PYTHON_MODULE} MODULE
            ${CMAKE_CURRENT_BINARY_DIR}/python-module-py.cc)
set_target_properties(${PYTHON_MODULE}
                      PROPERTIES PREFIX "" OUTPUT_NAME ${LIBRARY_NAME}/wrap)

if(UNIX AND NOT APPLE)
  target_link_libraries(${PYTHON_MODULE} PRIVATE "-Wl,--no-as-needed")
endif(UNIX AND NOT APPLE)

target_link_libraries(${PYTHON_MODULE} PRIVATE ${LIBRARY_NAME} ${PROJECT_NAME})
target_link_boost_python(${PYTHON_MODULE} PRIVATE)
target_include_directories(${PYTHON_MODULE} SYSTEM
                           PRIVATE ${PYTHON_INCLUDE_DIRS})

# Test it
add_python_unit_test("test-custom-entity" "tests/test_custom_entity.py" src
                     tests)

# also test other bindings, using this custom entity
add_python_unit_test("test-bindings" "tests/test_bindings.py" src tests)
