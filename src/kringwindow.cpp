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

#include <QEvent>

#include <KActionCollection>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KStatusNotifierItem>

#include <callmodel.h>

#include "accountssettingspage.h"
#include "generalsettingspage.h"
#include "kringsettings.h"
#include "kringwidget.h"

KringWindow::KringWindow()
  : KXmlGuiWindow()
{
  kringWidget = new KringWidget(this);
  setCentralWidget(kringWidget);
  switchAction
      = actionCollection()->addAction(QStringLiteral("switch_action"));
  switchAction->setText(i18n("Switch Colors"));
  switchAction->setIcon(QIcon::fromTheme(QStringLiteral("fill-color")));
  connect(switchAction,
          &QAction::triggered,
          kringWidget,
          &KringWidget::slotSwitchColors);
  KStandardAction::close(this,
                         &KringWindow::close,
                         actionCollection());
  KStandardAction::quit(qApp,
                        &QCoreApplication::quit,
                        actionCollection());
  KStandardAction::preferences(this,
                               &KringWindow::showSettingsDialog,
                               actionCollection());
  setupGUI();
  loadSettings();
}

KringWindow::~KringWindow()
{
  delete &CallModel::instance();
}

const KStatusNotifierItem * KringWindow::getSystemTrayIcon() const
{
  return systemTrayIcon;
}

bool KringWindow::event(QEvent * event)
{
  if (event->type() == QEvent::WindowStateChange
      && isMinimized()
      && KringSettings::systemTrayIconEnabled()
      && KringSettings::windowHidingOnMinimize()
      && systemTrayIcon) {
    hide();
    return true;
  }
  return KXmlGuiWindow::event(event);
}

bool KringWindow::queryClose()
{
  qCDebug(KRING) << "KringWindow::queryClose()";
  if (KringSettings::systemTrayIconEnabled()
      && KringSettings::windowHidingOnClose()
      && systemTrayIcon) {
    hide();
    return false;
  } else {
    return true;
  }
}

void KringWindow::showSettingsDialog()
{
  qCDebug(KRING) << "KringWindow::showSettingsDialog()";

  if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
    return;
  }

  KConfigDialog * settingsDialog
      = new KConfigDialog(this,
                          QStringLiteral("settings"),
                          KringSettings::self());
  settingsDialog->addPage(new GeneralSettingsPage(),
                          i18n("General"),
                          QStringLiteral("preferences-system-windows"));
  settingsDialog->addPage(new AccountsSettingsPage(),
                          i18n("Accounts"),
                          QStringLiteral("system-users"));

  connect(settingsDialog,
          &KConfigDialog::settingsChanged,
          this,
          &KringWindow::loadSettings);

  settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
  settingsDialog->show();

  return;
}

void KringWindow::loadSettings()
{
  if (KringSettings::systemTrayIconEnabled()) {
    if (!systemTrayIcon) {
      systemTrayIcon = new KStatusNotifierItem(this);
      systemTrayIcon->setCategory(KStatusNotifierItem::ApplicationStatus);
      systemTrayIcon->setIconByName(QStringLiteral("kring"));
      systemTrayIcon->setToolTip(QStringLiteral("kring"),
                                 i18n("Kring"),
                                 i18n("A client for Ring"));
    }
  } else {
    if (systemTrayIcon) {
      systemTrayIcon->setParent(nullptr);
      delete systemTrayIcon;
      systemTrayIcon = nullptr;
    }
  }

  kringWidget->loadSettings();

  return;
}
