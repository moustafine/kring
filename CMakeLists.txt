cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

project(kring)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
  ${ECM_MODULE_PATH}
  ${ECM_KDE_MODULE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/.cmake/modules
)

include(FeatureSummary)

set(QT_MIN_VERSION "5.7.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
  Core
  Gui
  Widgets
)

find_package(KF5 REQUIRED COMPONENTS
  ConfigWidgets
  CoreAddons
  DBusAddons
  DocTools
  I18n
  Notifications
  WindowSystem
  XmlGui
)

find_package(LibRingClient REQUIRED)

feature_summary(WHAT ALL
  INCLUDE_QUIET_PACKAGES
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)

add_subdirectory(app/${PROJECT_NAME})

add_subdirectory(doc/${PROJECT_NAME})

add_subdirectory(icon/${PROJECT_NAME})

add_subdirectory(meta/${PROJECT_NAME})
