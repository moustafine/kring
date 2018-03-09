cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

include(KDEInstallDirs)

set(NAME
  ${PROJECT_NAME}
)

set(SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

install(PROGRAMS
  ${SOURCE_DIR}/org.example.${NAME}.desktop
  DESTINATION ${XDG_APPS_INSTALL_DIR}
)
install(FILES
  ${SOURCE_DIR}/org.example.${NAME}.appdata.xml
  DESTINATION ${KDE_INSTALL_METAINFODIR}
)
