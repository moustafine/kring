cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

set(NAME
  ${PROJECT_NAME}
)

set(SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

ki18n_wrap_ui(UI_INCLUDE
  ${SOURCE_DIR}/accountssettingspage.ui
  ${SOURCE_DIR}/authenticationdialog.ui
  ${SOURCE_DIR}/finishpage.ui
  ${SOURCE_DIR}/generalsettingspage.ui
  ${SOURCE_DIR}/kringwidget.ui
  ${SOURCE_DIR}/progresspage.ui
  ${SOURCE_DIR}/ringaccountcredentialspage.ui
  ${SOURCE_DIR}/ringaccountgeneralsettingspage.ui
)

kconfig_add_kcfg_files(KCFG_SOURCE
  ${SOURCE_DIR}/kringsettings.kcfgc
)

set(INCLUDE
  ${SOURCE_DIR}/abstractsettingspage.h
  ${SOURCE_DIR}/accountassistantdialog.h
  ${SOURCE_DIR}/accountdelegate.h
  ${SOURCE_DIR}/accountproxymodel.h
  ${SOURCE_DIR}/accountsettings.h
  ${SOURCE_DIR}/accountssettingspage.h
  ${SOURCE_DIR}/authenticationdialog.h
  ${SOURCE_DIR}/finishpage.h
  ${SOURCE_DIR}/generalsettingspage.h
  ${SOURCE_DIR}/kringapplication.h
  ${SOURCE_DIR}/kringwidget.h
  ${SOURCE_DIR}/kringwindow.h
  ${SOURCE_DIR}/progresspage.h
  ${SOURCE_DIR}/ringaccountcredentialspage.h
  ${SOURCE_DIR}/ringaccountgeneralsettingspage.h
  ${SOURCE_DIR}/ringaccountsettings.h
  ${UI_INCLUDE}
)
set(SOURCE
  ${SOURCE_DIR}/abstractsettingspage.cpp
  ${SOURCE_DIR}/accountassistantdialog.cpp
  ${SOURCE_DIR}/accountdelegate.cpp
  ${SOURCE_DIR}/accountproxymodel.cpp
  ${SOURCE_DIR}/accountsettings.cpp
  ${SOURCE_DIR}/accountssettingspage.cpp
  ${SOURCE_DIR}/authenticationdialog.cpp
  ${SOURCE_DIR}/finishpage.cpp
  ${SOURCE_DIR}/generalsettingspage.cpp
  ${SOURCE_DIR}/kringapplication.cpp
  ${SOURCE_DIR}/kringwidget.cpp
  ${SOURCE_DIR}/kringwindow.cpp
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/progresspage.cpp
  ${SOURCE_DIR}/ringaccountcredentialspage.cpp
  ${SOURCE_DIR}/ringaccountgeneralsettingspage.cpp
  ${SOURCE_DIR}/ringaccountsettings.cpp
  ${KCFG_SOURCE}
)

add_executable(${NAME}
  ${INCLUDE}
  ${SOURCE}
)

target_include_directories(${NAME} PRIVATE
  ${SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${LIB_RING_CLIENT_INCLUDE_DIR}
)

target_link_libraries(${NAME}
  Qt5::Widgets
  KF5::ConfigWidgets
  KF5::CoreAddons
  KF5::DBusAddons
  KF5::I18n
  KF5::Notifications
  KF5::WindowSystem
  KF5::XmlGui
  ${LIB_RING_CLIENT_LIBRARY}
)

install(TARGETS ${NAME} ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
  ${SOURCE_DIR}/${NAME}ui.rc
  DESTINATION ${KXMLGUI_INSTALL_DIR}/${NAME}
)
