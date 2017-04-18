set(kring_SRCS
    main.cpp
    kring.cpp
    kringview.cpp
)

kconfig_add_kcfg_files(kring_SRCS kringsettings.kcfgc)
ki18n_wrap_ui(kring_SRCS kringviewbase.ui settingsbase.ui)

add_executable(kring ${kring_SRCS})

target_link_libraries(kring
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    KF5::ConfigWidgets
)

install(TARGETS kring ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.example.kring.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES org.example.kring.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES kringui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kring)
