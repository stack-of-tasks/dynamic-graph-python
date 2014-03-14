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

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/pthread.cmake)
INCLUDE(cmake/cpack.cmake)
include(cmake/header.cmake)

SET(PROJECT_NAME dynamic-graph-python)
SET(PROJECT_DESCRIPTION "Dynamic graph library Python bindings")
SET(PROJECT_URL "http://github.com/jrl-umi3218/dynamic-graph-python")

SET(CUSTOM_HEADER_DIR "dynamic-graph/python")
# Headers list.
SET(${PROJECT_NAME}_HEADERS
  include/dynamic-graph/python/interpreter.hh
  include/dynamic-graph/python/api.hh
  include/dynamic-graph/python/exception-python.hh

)

SET(PKG_CONFIG_ADDITIONAL_VARIABLES plugindir ${PKG_CONFIG_ADDITIONAL_VARIABLES})

SETUP_PROJECT()

# Trigger dependency to dynamic-graph.
ADD_REQUIRED_DEPENDENCY("dynamic-graph >= 2.5.5-6")

# Add dependency toward dynamic graph library in pkg-config file.
PKG_CONFIG_APPEND_LIBS("dynamic-graph-python")

# Search for Boost.
SEARCH_FOR_BOOST()

# Make sure Boost.Filesystem v2 is used.
ADD_DEFINITIONS(-DBOOST_FILESYSTEM_VERSION=2)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(unitTesting)

SETUP_PROJECT_FINALIZE()

