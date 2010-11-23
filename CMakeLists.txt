#
#  Copyright 2010 CNRS
#  Author: Florent Lamiraux
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(cmake/base.cmake)
SET(PROJECT_NAME dynamic-graph-python)
project(${PROJECT_NAME})

SET(PROJECT_VERSION 1.0)

SET(CMAKE_VERBOSE_MAKEFILE true)

IF(WIN32)
  SET(LIBDIR_KW "/LIBPATH:")
  SET(LIBINCL_KW "")
  SET(LIB_EXT ".lib")
ENDIF(WIN32)

IF(UNIX)
  SET(LIBDIR_KW "-L")
  SET(LIBINCL_KW "-l")
  SET(LIB_EXT "")
ENDIF(UNIX)

#
# Look for dependencies
#

# Load Pkgconfig macros
INCLUDE(FindPkgConfig)
SET(PACKAGE_REQUIREMENTS "")

#
#  Dependency to package dynamic-graph
#
SET(DYNAMIC_GRAPH_FOUND 0)
SET(DYNAMIC_GRAPH_REQUIRED "dynamic-graph >= 1.0")
PKG_CHECK_MODULES(DYNAMIC_GRAPH REQUIRED ${DYNAMIC_GRAPH_REQUIRED})
EXEC_PROGRAM(${PKG_CONFIG_EXECUTABLE} ARGS
  --variable=doxygendocdir dynamic-graph
  OUTPUT_VARIABLE DYNAMIC_GRAPH_DOXYGENDOCDIR)
MESSAGE(STATUS "pkg-config: ${PKG_CONFIG_EXECUTABLE}")
MESSAGE(STATUS "DYNAMIC_GRAPH_DOCDIR=${DYNAMIC_GRAPH_DOCDIR}")
SET(DYNAMIC_GRAPH_FOUND 1)

IF(NOT ${DYNAMIC_GRAPH_FOUND})
  MESSAGE(FATAL_ERROR "Check that package dynamic-graph is installed in a directory pointed out by PKG_CONFIG_PATH.")
ENDIF(NOT ${DYNAMIC_GRAPH_FOUND})

IF(PACKAGE_REQUIREMENTS STREQUAL "")
  SET(PACKAGE_REQUIREMENTS ${DYNAMIC_GRAPH_REQUIRED})
ELSE(PACKAGE_REQUIREMENTS STREQUAL "")
  SET(PACKAGE_REQUIREMENTS "${PACKAGE_REQUIREMENTS}, ${DYNAMIC_GRAPH_REQUIRED}")
ENDIF(PACKAGE_REQUIREMENTS STREQUAL "")

#
# pkg-config file
#

SET(install_pkg_prefix "\${prefix}")
SET(install_pkg_exec_prefix "\${exec_prefix}")
SET(install_pkg_libdir "\${libdir}")
SET(install_pkg_datarootdir "\${datarootdir}")
SET(install_pkg_include_dir "\${includedir}")

# Install the file in the appropriate place.
SET(install_libdir_pkgconfig ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
  DESTINATION ${install_libdir_pkgconfig}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)

ADD_SUBDIRECTORY(src)

#
# Documentation produced by doxygen
#
SET(SPHINX_EXECUTABLE sphinx-build)
add_subdirectory(doc)
