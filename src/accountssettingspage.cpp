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

#include <QByteArray>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLoggingCategory>
#include <QModelIndex>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QString>
#include <QTreeView>

#include <KConfigDialog>
#include <KConfigDialogManager>
#include <KCoreConfigSkeleton>
#include <KLocalizedString>
#include <KMessageBox>

#include <account.h>
#include <accountmodel.h>

#include "ui_accountssettingspage.h"

#include "accountassistantdialog.h"
#include "accountdelegate.h"
#include "accountproxymodel.h"
#include "ringaccountgeneralsettingspage.h"
#include "ringaccountsettings.h"

Q_DECLARE_LOGGING_CATEGORY(kring)

AccountsSettingsPage::AccountsSettingsPage(QWidget * parent)
  : QWidget(parent)
{
  ui = new Ui::AccountsSettingsPage();
  ui->setupUi(this);

  auto accountProxyModel = new AccountProxyModel(this);
  accountProxyModel->setSourceModel(&AccountModel::instance());

  sortFilterProxyModel = new QSortFilterProxyModel(this);
  sortFilterProxyModel->setSourceModel(accountProxyModel);

  delete ui->accountTreeView->itemDelegate();
  ui->accountTreeView
      ->setItemDelegate(new AccountDelegate(ui->accountTreeView));
  ui->accountTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->accountTreeView->setAlternatingRowColors(true);
  ui->accountTreeView->setRootIsDecorated(false);
  ui->accountTreeView->setModel(sortFilterProxyModel);
  ui->accountTreeView->setSortingEnabled(true);
  ui->accountTreeView->sortByColumn(0, Qt::AscendingOrder);

  ui->accountTreeView->header()
      ->setSectionResizeMode(QHeaderView::ResizeToContents);

  connect(ui->accountTreeView->selectionModel(),
          &QItemSelectionModel::currentChanged,
          this,
          &AccountsSettingsPage::handleCurrentAccountIndexChange);

  auto currentProxyIndex = ui->accountTreeView->currentIndex();
  handleCurrentAccountIndexChange(currentProxyIndex, currentProxyIndex);

  connect(&AccountModel::instance(),
          &AccountModel::accountStateChanged,
          this,
          [this](Account * account, const Account::RegistrationState state)
  {
    Q_UNUSED(state);

    auto currentProxyIndex = ui->accountTreeView->currentIndex();
    auto currentSourceIndex
        = sortFilterProxyModel->mapToSource(currentProxyIndex);

    if (account
        == AccountModel::instance()
        .getAccountByModelIndex(currentSourceIndex)) {
      handleCurrentAccountIndexChange(currentProxyIndex, currentProxyIndex);
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
  auto accountAssistantDialog = new AccountAssistantDialog();
  accountAssistantDialog->setAttribute(Qt::WA_DeleteOnClose);
  accountAssistantDialog->show();

  return;
}

void AccountsSettingsPage::on_modifyPushButton_clicked()
{
  auto currentSourceIndex
      = sortFilterProxyModel->mapToSource(ui->accountTreeView->currentIndex());

  auto account
      = AccountModel::instance().getAccountByModelIndex(currentSourceIndex);

  if (!account) {
    qCWarning(kring, "Failed to get account.");
    return;
  }

  auto accountSettingsDialog = new KPageDialog(this);
  accountSettingsDialog->setModal(true);
  accountSettingsDialog->setFaceType(KPageDialog::Tabbed);
  accountSettingsDialog->setStandardButtons(QDialogButtonBox::Ok
                                            | QDialogButtonBox::Apply
                                            | QDialogButtonBox::Cancel
                                            | QDialogButtonBox::Reset);

  AbstractSettingsPage * accountGeneralSettingsPage = nullptr;

  switch (account->protocol()) {
    case Account::Protocol::RING:
    {
      accountSettingsDialog->setWindowTitle(i18n("Ring account settings"));

      accountGeneralSettingsPage
          = new RingAccountGeneralSettingsPage(*account,
                                               accountSettingsDialog);

      break;
    }
    default:
    {
      break;
    }
  }

  if (!accountGeneralSettingsPage) {
    delete accountSettingsDialog;
    qCWarning(kring,
              "Failed to create a settings page for account \"%s\".",
              qUtf8Printable(account->alias()));
    return;
  }

  accountSettingsDialog->addPage(accountGeneralSettingsPage, i18n("General"));

  auto accountModifyingDialogManager
      = new KConfigDialogManager(accountSettingsDialog,
                                 new RingAccountSettings(*account));
  accountModifyingDialogManager->addWidget(accountGeneralSettingsPage);

  connect(accountSettingsDialog->button(QDialogButtonBox::Ok),
          &QAbstractButton::clicked,
          accountModifyingDialogManager,
          &KConfigDialogManager::updateSettings);
  connect(accountSettingsDialog->button(QDialogButtonBox::Apply),
          &QAbstractButton::clicked,
          accountModifyingDialogManager,
          &KConfigDialogManager::updateSettings);
  connect(accountSettingsDialog->button(QDialogButtonBox::Cancel),
          &QAbstractButton::clicked,
          accountModifyingDialogManager,
          &KConfigDialogManager::updateWidgets);
  connect(accountSettingsDialog->button(QDialogButtonBox::Reset),
          &QAbstractButton::clicked,
          accountModifyingDialogManager,
          &KConfigDialogManager::updateWidgets);

  auto validateAccountSettingsDialogHandler
      = [accountSettingsDialog,
      accountModifyingDialogManager,
      accountGeneralSettingsPage]()
  {
    accountGeneralSettingsPage->validate();

    if (accountModifyingDialogManager->hasChanged()) {
      if (accountGeneralSettingsPage->isValid()) {
        accountSettingsDialog->button(QDialogButtonBox::Ok)
            ->setEnabled(true);
        accountSettingsDialog->button(QDialogButtonBox::Apply)
            ->setEnabled(true);
      } else {
        accountSettingsDialog->button(QDialogButtonBox::Ok)
            ->setEnabled(false);
        accountSettingsDialog->button(QDialogButtonBox::Apply)
            ->setEnabled(false);
      }
      accountSettingsDialog->button(QDialogButtonBox::Reset)
          ->setEnabled(true);
    } else {
      accountSettingsDialog->button(QDialogButtonBox::Ok)
          ->setEnabled(true);
      accountSettingsDialog->button(QDialogButtonBox::Apply)
          ->setEnabled(false);
      accountSettingsDialog->button(QDialogButtonBox::Reset)
          ->setEnabled(false);
    }

    return;
  };

  connect(accountModifyingDialogManager,
          static_cast<void (KConfigDialogManager::*)()>(&KConfigDialogManager
                                                        ::settingsChanged),
          validateAccountSettingsDialogHandler);
  connect(accountModifyingDialogManager,
          &KConfigDialogManager::widgetModified,
          validateAccountSettingsDialogHandler);
  validateAccountSettingsDialogHandler();

  connect(&AccountModel::instance(),
          &AccountModel::accountRemoved,
          accountSettingsDialog,
          [account, accountSettingsDialog](Account * removedAccount)
  {
    if (account == removedAccount) {
      accountSettingsDialog->close();
    }
    return;
  });

  accountSettingsDialog->setAttribute(Qt::WA_DeleteOnClose);
  accountSettingsDialog->show();

  return;
}

void AccountsSettingsPage::on_deletePushButton_clicked()
{
  auto currentSourceIndex
      = sortFilterProxyModel->mapToSource(ui->accountTreeView->currentIndex());

  auto account
      = AccountModel::instance().getAccountByModelIndex(currentSourceIndex);

  if (!account) {
    qCWarning(kring, "Failed to get account.");
    return;
  }

  auto accountId = account->id();

  auto buttonCode
      = KMessageBox::warningContinueCancel(this,
                                           i18n("Do you want to delete"
                                                " account \"%1\"?",
                                                account->alias()),
                                           i18n("Account deletion"),
                                           KStandardGuiItem::del(),
                                           KStandardGuiItem::cancel());

  account = AccountModel::instance().getById(accountId);

  if ((buttonCode == KMessageBox::Continue) && account) {
    AccountModel::instance().remove(account);
    AccountModel::instance().save();

    ui->accountTreeView->setCurrentIndex(sortFilterProxyModel->index(0, 0));
  }

  return;
}

void AccountsSettingsPage::handleCurrentAccountIndexChange
(const QModelIndex & currentProxyIndex, const QModelIndex & previousProxyIndex)
{
  Q_UNUSED(previousProxyIndex);

  auto currentSourceIndex
      = sortFilterProxyModel->mapToSource(currentProxyIndex);

  auto account
      = AccountModel::instance().getAccountByModelIndex(currentSourceIndex);

  if (account) {
    switch (account->registrationState()) {
      case Account::RegistrationState::INITIALIZING:
      case Account::RegistrationState::COUNT__:
      {
        ui->modifyPushButton->setEnabled(false);
        ui->deletePushButton->setEnabled(false);
        break;
      }
      default:
      {
        ui->modifyPushButton->setEnabled(true);
        ui->deletePushButton->setEnabled(true);
        break;
      }
    }
  } else {
    ui->modifyPushButton->setEnabled(false);
    ui->deletePushButton->setEnabled(false);
  }

  return;
}
