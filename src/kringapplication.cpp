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

#include "kringapplication.h"

#include <KDBusService>
#include <KWindowSystem>

#include <callmodel.h>

#include "kringsettings.h"
#include "kringwindow.h"

KringApplication::KringApplication(int & argc, char ** argv)
  : QApplication(argc, argv)
{
  setOrganizationDomain(QStringLiteral("example.org"));

  auto * service = new KDBusService(KDBusService::Unique, this);
  connect(service,
          &KDBusService::activateRequested,
          this,
          [this]()
  {
    if (mainWindow) {
      KWindowSystem::forceActiveWindow(mainWindow->winId());
    }
    return;
  });
}

KringApplication::~KringApplication()
{
  if (mainWindow) {
    delete mainWindow;
  };

  delete &CallModel::instance();
}

void KringApplication::setMainWindow(KringWindow * window)
{
  if (mainWindow == window) {
    return;
  }

  if (mainWindow) {
    mainWindow->disconnect(this);
  }

  mainWindow = window;

  if (mainWindow) {
    connect(mainWindow,
            &QObject::destroyed,
            this,
            [this]()
    {
      mainWindow = nullptr;
      return;
    });
  }

  return;
}

void KringApplication::show()
{
  if (mainWindow) {
    if (KringSettings::windowHidingOnStart()
        && mainWindow->getSystemTrayIcon()) {
      mainWindow->hide();
    } else {
      mainWindow->show();
    }
  }
  return;
}
