set(kring_SRCS
  abstractsettingspage.cpp
  accountassistantdialog.cpp
  accountdelegate.cpp
  accountproxymodel.cpp
  accountsettings.cpp
  accountssettingspage.cpp
  authenticationdialog.cpp
  finishpage.cpp
  generalsettingspage.cpp
  kringapplication.cpp
  kringwidget.cpp
  kringwindow.cpp
  main.cpp
  progresspage.cpp
  ringaccountcredentialspage.cpp
  ringaccountgeneralsettingspage.cpp
  ringaccountsettings.cpp
)

kconfig_add_kcfg_files(kring_SRCS kringsettings.kcfgc)
ki18n_wrap_ui(kring_SRCS
  accountssettingspage.ui
  authenticationdialog.ui
  finishpage.ui
  generalsettingspage.ui
  kringwidget.ui
  progresspage.ui
  ringaccountcredentialspage.ui
  ringaccountgeneralsettingspage.ui
)

include_directories(${LIB_RING_CLIENT_INCLUDE_DIR})

add_executable(kring ${kring_SRCS})

target_link_libraries(kring
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

install(TARGETS kring ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS
  org.example.kring.desktop
  DESTINATION ${XDG_APPS_INSTALL_DIR}
)
install(FILES
  org.example.kring.appdata.xml
  DESTINATION ${KDE_INSTALL_METAINFODIR}
)
install(FILES kringui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kring)
