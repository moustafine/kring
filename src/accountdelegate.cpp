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

#include "accountdelegate.h"

#include <QAbstractProxyModel>

#include <account.h>
#include <accountmodel.h>

AccountDelegate::AccountDelegate(QObject * parent)
  : QStyledItemDelegate(parent)
{
  ;
}

AccountDelegate::~AccountDelegate()
{
  ;
}

bool AccountDelegate::editorEvent(QEvent * event,
                                  QAbstractItemModel * model,
                                  const QStyleOptionViewItem & option,
                                  const QModelIndex & index)
{
  auto eventHandled = QStyledItemDelegate::editorEvent(event,
                                                       model,
                                                       option,
                                                       index);

  if (eventHandled) {
    AccountModel * accountModel = nullptr;
    QModelIndex sourceIndex(index);

    while (!accountModel || model) {
      auto proxyModel = qobject_cast<QAbstractProxyModel *>(model);
      if (proxyModel) {
        model = proxyModel->sourceModel();
        sourceIndex = proxyModel->mapToSource(sourceIndex);
      } else {
        accountModel = qobject_cast<AccountModel *>(model);
        model = nullptr;
      }
    }

    if (accountModel) {
      auto account = accountModel->getAccountByModelIndex(sourceIndex);
      if (account) {
        if (!((account->editState() == Account::EditState::MODIFIED_COMPLETE)
              && account->performAction(Account::EditAction::SAVE))) {
          account->performAction(Account::EditAction::CANCEL);
        }
      }
    }
  }

  return eventHandled;
}
