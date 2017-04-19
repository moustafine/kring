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

#include "kringwindow.h"

KringApplication::KringApplication(int & argc, char ** argv)
  : QApplication(argc, argv),
    window(nullptr)
{
  setOrganizationDomain("example.org");

  KDBusService * service = new KDBusService(KDBusService::Unique, this);
  connect(service,
          &KDBusService::activateRequested,
          this,
          [this]()
  {
    KWindowSystem::forceActiveWindow(window->winId());
    return;
  });
}

KringApplication::~KringApplication()
{
  ;
}

void KringApplication::setMainWindow(KringWindow * window)
{
  if (window) {
    this->window = window;
  }
  return;
}
