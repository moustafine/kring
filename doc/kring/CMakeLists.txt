cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

include(KDEInstallDirs)

set(NAME
  ${PROJECT_NAME}
)

set(SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

kdoctools_create_handbook(${SOURCE_DIR}/en/index.docbook
  INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en
  SUBDIR ${NAME}
)
