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

#include "accountproxymodel.h"

#include <QString>
#include <QVariant>

#include <KLocalizedString>

#include <accountmodel.h>

AccountProxyModel::AccountProxyModel(QObject * parent)
  : QSortFilterProxyModel(parent)
{
  ;
}

AccountProxyModel::~AccountProxyModel()
{
  ;
}

QVariant AccountProxyModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{
  auto accountModel = qobject_cast<AccountModel *>(sourceModel());
  if (accountModel) {
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
      switch (section) {
        case 0:
        {
          return i18n("User name");
        }
        default:
        {
          break;
        }
      }
    }
    return QVariant();
  }
  return QSortFilterProxyModel::headerData(section, orientation, role);
}
