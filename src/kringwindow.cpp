/*
Copyright (C) 2017 by Marat Moustafine <moustafine@tuta.io>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kringwindow.h"

#include <KActionCollection>
#include <KConfigDialog>
#include <KStatusNotifierItem>

#include "kringview.h"

KringWindow::KringWindow()
  : KXmlGuiWindow()
{
  systemTrayIcon = new KStatusNotifierItem(this);
  systemTrayIcon->setCategory(KStatusNotifierItem::ApplicationStatus);
  systemTrayIcon->setIconByName("kring");
  systemTrayIcon->setToolTip("kring",
                             i18n("Kring"),
                             i18n("A client for Ring"));

  kringView = new KringView(this);
  setCentralWidget(kringView);
  switchAction
      = actionCollection()->addAction(QStringLiteral("switch_action"));
  switchAction->setText(i18n("Switch Colors"));
  switchAction->setIcon(QIcon::fromTheme(QStringLiteral("fill-color")));
  connect(switchAction,
          &QAction::triggered,
          kringView,
          &KringView::slotSwitchColors);
  KStandardAction::quit(qApp,
                        &QApplication::closeAllWindows,
                        actionCollection());
  KStandardAction::preferences(this,
                               &KringWindow::settingsConfigure,
                               actionCollection());
  setupGUI();
}

KringWindow::~KringWindow()
{
  ;
}

void KringWindow::settingsConfigure()
{
  qCDebug(KRING) << "KringWindow::settingsConfigure()";
  // The preference dialog is derived from prefs_base.ui
  //
  // compare the names of the widgets in the .ui file
  // to the names of the variables in the .kcfg file
  //avoid to have 2 dialogs shown
  if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
    return;
  }
  KConfigDialog * dialog = new KConfigDialog(this,
                                             QStringLiteral("settings"),
                                             KringSettings::self());
  QWidget * generalSettingsDialog = new QWidget;
  settingsBase.setupUi(generalSettingsDialog);
  dialog->addPage(generalSettingsDialog,
                  i18n("General"),
                  QStringLiteral("package_setting"));
  connect(dialog,
          &KConfigDialog::settingsChanged,
          kringView,
          &KringView::slotSettingsChanged);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();

  return;
}
