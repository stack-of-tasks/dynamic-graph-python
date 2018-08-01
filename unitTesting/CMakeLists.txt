#
#  Copyright
#

INCLUDE(CTest)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${PYTHON_LIBRARY_DIRS})


ADD_DEFINITIONS(-DDEBUG=2)

# provide path to library libdynamic-graph.so
LINK_DIRECTORIES(${DYNAMIC_GRAPH_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
LINK_DIRECTORIES(${${PROJECT_NAME}_BINARY_DIR}/src)

# Add dynamic-graph compilation flags and link to library libdynamic-graph.so
ADD_DEFINITIONS(${DYNAMIC_GRAPH_CFLAGS})


SET(EXECUTABLE_NAME interpreter-test)
ADD_EXECUTABLE(${EXECUTABLE_NAME} interpreter-test.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} dynamic-graph-python)
ADD_TEST(${EXECUTABLE_NAME}  ${EXECUTABLE_NAME})


## Test runfile
SET(EXECUTABLE_NAME interpreter-test-runfile)
ADD_EXECUTABLE(${EXECUTABLE_NAME} interpreter-test-runfile.cc)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} dynamic-graph-python)
ADD_TEST(${EXECUTABLE_NAME}  ${EXECUTABLE_NAME})

ADD_CUSTOM_COMMAND(TARGET interpreter-test-runfile POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-ok.py
  ${CMAKE_BINARY_DIR}/unitTesting
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-name_error.py
  ${CMAKE_BINARY_DIR}/unitTesting
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/unitTesting/test_python-syntax_error.py
  ${CMAKE_BINARY_DIR}/unitTesting
)

