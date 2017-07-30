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

#include <QModelIndex>
#include <QString>
#include <QVariant>

#include <KLocalizedString>

#include <account.h>
#include <accountmodel.h>

AccountProxyModel::AccountProxyModel(QObject * parent)
  : QIdentityProxyModel(parent)
{
  ;
}

AccountProxyModel::~AccountProxyModel()
{
  ;
}

QModelIndex AccountProxyModel::index(int row,
                                     int column,
                                     const QModelIndex & parent) const
{
  if (row < 0 || column < 0) {
    return QModelIndex();
  }

  auto accountModel = qobject_cast<AccountModel *>(sourceModel());
  if (accountModel) {
    switch (column) {
      case 0:
      {
        break;
      }
      case 1:
      {
        return createIndex(row, column);
      }
      default:
      {
        return QModelIndex();
      }
    }
  }
  return QIdentityProxyModel::index(row, column, parent);
}

int AccountProxyModel::columnCount(const QModelIndex & parent) const
{
  auto accountModel = qobject_cast<AccountModel *>(sourceModel());
  if (accountModel) {
    return 2;
  }
  return QIdentityProxyModel::columnCount(parent);
}

QVariant AccountProxyModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  auto accountModel = qobject_cast<AccountModel *>(sourceModel());
  if (accountModel && (index.column() > 0)) {
    if (role == Qt::DisplayRole) {
      auto accountIndex = QIdentityProxyModel::index(index.row(), 0);
      auto account = accountModel->getAccountByModelIndex(accountIndex);
      if (account) {
        switch (index.column()) {
          case 1:
          {
            switch (account->protocol()) {
              case Account::Protocol::SIP:
              {
                return QStringLiteral("SIP");
              }
              case Account::Protocol::RING:
              {
                return QStringLiteral("Ring");
              }
              default:
              {
                break;
              }
            }
            break;
          }
          default:
          {
            break;
          }
        }
      }
    }
    return QVariant();
  }
  return QIdentityProxyModel::data(index, role);
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
        case 1:
        {
          return i18n("Protocol");
        }
        default:
        {
          break;
        }
      }
    }
    return QVariant();
  }
  return QIdentityProxyModel::headerData(section, orientation, role);
}
