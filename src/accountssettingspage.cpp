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

#include "accountssettingspage.h"

#include <QItemSelectionModel>
#include <QListView>
#include <QModelIndex>

#include <KLocalizedString>
#include <KMessageBox>

#include <account.h>
#include <accountmodel.h>

#include "ui_accountssettingspage.h"

#include "accountassistantdialog.h"

AccountsSettingsPage::AccountsSettingsPage(QWidget * parent)
  : QWidget(parent)
{
  ui = new Ui::AccountsSettingsPage();
  ui->setupUi(this);

  ui->accountListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->accountListView->setModel(&AccountModel::instance());

  connect(ui->accountListView->selectionModel(),
          &QItemSelectionModel::currentChanged,
          this,
          &AccountsSettingsPage::handleCurrentAccountIndexChange);

  auto currentIndex = ui->accountListView->currentIndex();
  handleCurrentAccountIndexChange(currentIndex, currentIndex);

  connect(&AccountModel::instance(),
          &AccountModel::accountStateChanged,
          this,
          [this](Account * account,  const Account::RegistrationState state)
  {
    Q_UNUSED(state);

    auto currentIndex = ui->accountListView->currentIndex();

    if (account
        == AccountModel::instance().getAccountByModelIndex(currentIndex)) {
      handleCurrentAccountIndexChange(currentIndex, currentIndex);
    }

    return;
  });
}

AccountsSettingsPage::~AccountsSettingsPage()
{
  delete ui;
}

void AccountsSettingsPage::on_addPushButton_clicked()
{
  accountAssistantDialog = new AccountAssistantDialog(this);
  accountAssistantDialog->setAttribute(Qt::WA_DeleteOnClose);
  accountAssistantDialog->show();

  return;
}

void AccountsSettingsPage::on_deletePushButton_clicked()
{
  auto account = AccountModel::instance()
      .getAccountByModelIndex(ui->accountListView->currentIndex());

  auto buttonCode
      = KMessageBox::warningContinueCancel(this,
                                           i18n("Do you want to delete"
                                                " account \"%1\"?",
                                                account->alias()),
                                           i18n("Account deletion"),
                                           KStandardGuiItem::del(),
                                           KStandardGuiItem::cancel());

  if (buttonCode == KMessageBox::Continue) {
    AccountModel::instance().remove(account);
    AccountModel::instance().save();

    ui->accountListView->setCurrentIndex(AccountModel::instance().index(0, 0));
  }

  return;
}

void AccountsSettingsPage::handleCurrentAccountIndexChange
(const QModelIndex & currentIndex, const QModelIndex & previousIndex)
{
  Q_UNUSED(previousIndex);

  if (currentIndex.isValid()) {
    auto registrationState = AccountModel::instance()
        .getAccountByModelIndex(currentIndex)->registrationState();

    switch (registrationState) {
      case Account::RegistrationState::INITIALIZING:
      case Account::RegistrationState::COUNT__:
      {
        ui->deletePushButton->setEnabled(false);
        break;
      }
      default:
      {
        ui->deletePushButton->setEnabled(true);
        break;
      }
    }
  } else {
    ui->deletePushButton->setEnabled(false);
  }

  return;
}
