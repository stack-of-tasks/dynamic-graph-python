# 
#  Copyright 
# 

SET(EXECUTABLE_NAME test${PROJECT_NAME})

ADD_DEFINITIONS(-DDEBUG=2)

# provide path to library libdynamic-graph.so
LINK_DIRECTORIES(${DYNAMIC_GRAPH_LIBRARY_DIRS})

ADD_EXECUTABLE(${EXECUTABLE_NAME}
  main.cc
  dynamic-graph-python-test.cc)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

LINK_DIRECTORIES(${${PROJECT_NAME}_BINARY_DIR}/src)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
  dynamic-graph-python)

# Add dynamic-graph compilation flags and link to library libdynamic-graph.so
ADD_DEFINITIONS(${DYNAMIC_GRAPH_CFLAGS})
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${DYNAMIC_GRAPH_LIBRARIES})
