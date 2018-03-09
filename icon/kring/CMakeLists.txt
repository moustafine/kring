cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

include(KDEInstallDirs)
include(ECMInstallIcons)

set(NAME
  ${PROJECT_NAME}
)

set(SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SOURCE
  ${SOURCE_DIR}/sc-apps-${NAME}.svgz
)

ecm_install_icons(ICONS
  ${SOURCE}
  DESTINATION ${ICON_INSTALL_DIR}
  THEME hicolor
)
