# Test the interpreter
SET(EXECUTABLE_NAME interpreter-test)
ADD_EXECUTABLE(${EXECUTABLE_NAME} interpreter-test.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} dynamic-graph-python)
ADD_TEST(${EXECUTABLE_NAME}  ${EXECUTABLE_NAME})

# Test runfile
SET(EXECUTABLE_NAME interpreter-test-runfile)
ADD_EXECUTABLE(${EXECUTABLE_NAME} interpreter-test-runfile.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} dynamic-graph-python)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${PYTHON_LIBRARY})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
ADD_TEST(${EXECUTABLE_NAME}  ${EXECUTABLE_NAME})

ADD_CUSTOM_COMMAND(TARGET interpreter-test-runfile POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-ok.py
  ${CMAKE_BINARY_DIR}/unitTesting
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-name_error.py
  ${CMAKE_BINARY_DIR}/unitTesting
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-syntax_error.py
  ${CMAKE_BINARY_DIR}/unitTesting
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-restart_interpreter.py
  ${CMAKE_BINARY_DIR}/unitTesting
)


# Test the module generation
## Create an entity

SET(LIBRARY_NAME "custom_entity")
ADD_LIBRARY(${LIBRARY_NAME} SHARED ${LIBRARY_NAME}.cpp)
SET_TARGET_PROPERTIES(${LIBRARY_NAME}
  PROPERTIES
  PREFIX ""
  SOVERSION ${PROJECT_VERSION}
  INSTALL_RPATH ${DYNAMIC_GRAPH_PLUGINDIR})

#add_dependencies(${LIBRARY_NAME} dynamic-graph)
target_link_libraries(${LIBRARY_NAME} dynamic-graph::dynamic-graph)

#PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} dynamic-graph)

## Create its bindings
## This mimics DYNAMIC_GRAPH_PYTHON_MODULE(${LIBRARY_NAME} ${LIBRARY_NAME} "${LIBRARY_NAME}-wrap")

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/submodule/__init__.py.cmake
  ${PROJECT_BINARY_DIR}/unitTesting/${LIBRARY_NAME}/__init__.py
  )
SET(PYTHON_MODULE "${LIBRARY_NAME}-wrap")
SET(SOURCE_PYTHON_MODULE "cmake/dynamic_graph/python-module-py.cc")
ADD_LIBRARY(${PYTHON_MODULE} MODULE ${PROJECT_SOURCE_DIR}/${SOURCE_PYTHON_MODULE})
SET_TARGET_PROPERTIES(${PYTHON_MODULE}
  PROPERTIES PREFIX ""
  OUTPUT_NAME ${LIBRARY_NAME}/wrap
  )

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PUBLIC_KEYWORD} "-Wl,--no-as-needed")
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PUBLIC_KEYWORD} ${LIBRARY_NAME} ${PYTHON_LIBRARY})

## Test it
ADD_PYTHON_UNIT_TEST("test-custom-entity" "unitTesting/test_custom_entity.py" src unitTesting)

# also test other bindings, using this custom entity
ADD_PYTHON_UNIT_TEST("test-bindings" "unitTesting/test_bindings.py" src unitTesting)
