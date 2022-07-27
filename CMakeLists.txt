# Copyright 2010-2020, Florent Lamiraux, Thomas Moulard, Olivier Stasse, Guilhem
# Saurel, JRL, CNRS/AIST, LAAS-CNRS

cmake_minimum_required(VERSION 3.1)

# Project properties
set(PROJECT_ORG stack-of-tasks)
set(PROJECT_NAME dynamic-graph-python)
set(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
set(PROJECT_URL "https://github.com/${PROJECT_ORG}/${PROJECT_NAME}")

# Project options
option(SUFFIX_SO_VERSION "Suffix library name with its version" ON)

# Project configuration
set(PROJECT_USE_CMAKE_EXPORT TRUE)
set(CUSTOM_HEADER_DIR "dynamic-graph/python")
set(CXX_DISABLE_WERROR TRUE)
set(DOXYGEN_USE_MATHJAX YES)

# JRL-cmakemodule setup
include(cmake/base.cmake)
include(cmake/boost.cmake)

# Project definition
compute_project_args(PROJECT_ARGS LANGUAGES CXX)
project(${PROJECT_NAME} ${PROJECT_ARGS})
check_minimal_cxx_standard(14 ENFORCE)

# Project dependencies
add_project_dependency(dynamic-graph 4.4.0 REQUIRED)
add_project_dependency(eigenpy 2.7.10 REQUIRED)
include(cmake/python.cmake) # TODO: overwriting eigenpy/python.cmake
if(BUILD_TESTING)
  find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif(BUILD_TESTING)

if(Boost_VERSION GREATER 107299 OR Boost_VERSION_MACRO GREATER 107299)
  # Silence a warning about a deprecated use of boost bind by boost >= 1.73
  # without dropping support for boost < 1.73
  add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
endif()

# Main Library
set(${PROJECT_NAME}_HEADERS
    include/${CUSTOM_HEADER_DIR}/api.hh
    include/${CUSTOM_HEADER_DIR}/convert-dg-to-py.hh
    include/${CUSTOM_HEADER_DIR}/dynamic-graph-py.hh
    include/${CUSTOM_HEADER_DIR}/fwd.hh
    include/${CUSTOM_HEADER_DIR}/interpreter.hh
    include/${CUSTOM_HEADER_DIR}/module.hh
    include/${CUSTOM_HEADER_DIR}/python-compat.hh
    include/${CUSTOM_HEADER_DIR}/signal.hh
    include/${CUSTOM_HEADER_DIR}/signal-wrapper.hh)

set(${PROJECT_NAME}_SOURCES
    src/interpreter.cc src/dynamic_graph/python-compat.cc
    src/dynamic_graph/entity-py.cc src/dynamic_graph/convert-dg-to-py.cc)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES}
                                   ${${PROJECT_NAME}_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PUBLIC dynamic-graph::dynamic-graph)
target_link_boost_python(${PROJECT_NAME} PRIVATE)

if(SUFFIX_SO_VERSION)
  set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
endif(SUFFIX_SO_VERSION)

target_compile_definitions(${PROJECT_NAME}
                           PRIVATE PYTHON_LIBRARY="${PYTHON_LIBRARY}")

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION lib)

add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

install(FILES package.xml DESTINATION share/${PROJECT_NAME})
