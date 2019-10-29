# Copyright 2010, 2011, Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST
#
# This file is part of dynamic-graph-python.
# dynamic-graph-python is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# dynamic-graph-python is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# dynamic-graph-python. If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(PROJECT_NAMESPACE stack-of-tasks)
SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/${PROJECT_NAMESPACE}/${PROJECT_NAME}")

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/pthread.cmake)
INCLUDE(cmake/cpack.cmake)
include(cmake/header.cmake)
include(cmake/python.cmake)


SET(DOXYGEN_USE_MATHJAX YES)

SET(CUSTOM_HEADER_DIR "dynamic-graph/python")
# Headers list.
SET(${PROJECT_NAME}_HEADERS
  include/dynamic-graph/python/interpreter.hh
  include/dynamic-graph/python/api.hh
  include/dynamic-graph/python/exception-python.hh

)

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

PROJECT(${PROJECT_NAME} CXX)

FINDPYTHON(2.7 EXACT REQUIRED)

# Trigger dependency to dynamic-graph.
ADD_REQUIRED_DEPENDENCY("dynamic-graph >= 3.0.0")

# Add dependency toward dynamic graph library in pkg-config file.
PKG_CONFIG_APPEND_LIBS("dynamic-graph-python")

# Search for Boost.
SET(BOOST_COMPONENTS python filesystem system thread program_options unit_test_framework)
SEARCH_FOR_BOOST()
SEARCH_FOR_EIGEN()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(unitTesting)
