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


#### Build entity library ####
set(LIBRARY_NAME custom_entity)
ADD_LIBRARY(${LIBRARY_NAME} SHARED ${LIBRARY_NAME}.cpp)

#remove the "lib" prefix from the publig output name
SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES PREFIX "")

SET_TARGET_PROPERTIES(${LIBRARY_NAME}
  PROPERTIES
  SOVERSION ${PROJECT_VERSION}
  INSTALL_RPATH ${DYNAMIC_GRAPH_PLUGINDIR})

#add_dependencies(${LIBRARY_NAME} dynamic-graph)
target_link_libraries(${LIBRARY_NAME} dynamic-graph)

PKG_CONFIG_USE_DEPENDENCY(${LIBRARY_NAME} dynamic-graph)

STRING(REPLACE - _ PYTHON_LIBRARY_NAME ${LIBRARY_NAME})
MESSAGE(STATUS "dynamic_graph_plugindir: ${DYNAMIC_GRAPH_PLUGINDIR}")


# Generates a local module in unitTesting
FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dynamic_graph_tests/${LIBRARY_NAME}")
CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/cmake/dynamic_graph/submodule/__init__.py.cmake
  ${PROJECT_BINARY_DIR}/unitTesting/dynamic_graph_tests/${LIBRARY_NAME}/__init__.py
  )
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dynamic_graph_tests/__init__.py
  "")

# Generates a local wrap.so library

SET(SOURCE_PYTHON_MODULE "cmake/dynamic_graph/python-module-py.cc")

SET(PYTHON_MODULE   ${PYTHON_LIBRARY_NAME}-wrap)
CMAKE_POLICY(PUSH)
IF(POLICY CMP0037)
  CMAKE_POLICY(SET CMP0037 OLD)
ENDIF()

ADD_LIBRARY(${PYTHON_MODULE}
  MODULE
  ${PROJECT_SOURCE_DIR}/${SOURCE_PYTHON_MODULE})

SET_TARGET_PROPERTIES(${PYTHON_MODULE}
  PROPERTIES PREFIX ""
  OUTPUT_NAME dynamic_graph_tests/${LIBRARY_NAME}/wrap
  )

CMAKE_POLICY(POP)

TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PUBLIC_KEYWORD} "-Wl,--no-as-needed")
TARGET_LINK_LIBRARIES(${PYTHON_MODULE} ${PUBLIC_KEYWORD} ${LIBRARY_NAME} ${PYTHON_LIBRARY})

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

ADD_PYTHON_UNIT_TEST("test-custom-entity" "unitTesting/test_custom_entity.py" src unitTesting)
