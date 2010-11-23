#
#  Copyright 2010 CNRS
#  Author: Florent Lamiraux
#

CONFIG_FILES_CMAKE(
  sphinx/index.rst
  sphinx/conf.py
  )

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
  COMMAND ${SPHINX_EXECUTABLE} -b html \"${CMAKE_CURRENT_BINARY_DIR}/sphinx\" \"${CMAKE_CURRENT_BINARY_DIR}/sphinx-html\"
)

ADD_CUSTOM_TARGET(documentation ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)

#
# Install html documentation
#

# html files
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sphinx-html
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME})

# pictures
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pictures
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME})
