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

#include "accountassistantdialog.h"

#include <QCloseEvent>
#include <QFontDatabase>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KMessageWidget>

#include <accountmodel.h>

#include "finishpage.h"
#include "progresspage.h"
#include "ringaccountcredentialspage.h"

AccountAssistantDialog::AccountAssistantDialog(QWidget * parent,
                                               Qt::WindowFlags flag)
  : KAssistantDialog(parent, flag)
{
  setWindowTitle(i18n("Account assistant"));

  ringAccountCredentialsPage = new RingAccountCredentialsPage();
  ringAccountCredentialsPageItem = addPage(ringAccountCredentialsPage,
                                           i18n("Ring account credentials"));

  connect(ringAccountCredentialsPage,
          &RingAccountCredentialsPage::valid,
          this,
          &AccountAssistantDialog::setValidPage);
  ringAccountCredentialsPage->validate();

  progressPageItem = addPage(new ProgressPage(), i18n("Progress"));
  setValid(progressPageItem, false);

  finishPage = new FinishPage();
  finishPageItem = addPage(finishPage, i18n("Finish"));

  connect(this,
          &KPageDialog::currentPageChanged,
          this,
          &AccountAssistantDialog::handleCurrentPageChange);
  handleCurrentPageChange(ringAccountCredentialsPageItem, nullptr);
}

AccountAssistantDialog::~AccountAssistantDialog()
{
  ;
}

void AccountAssistantDialog::handleCurrentPageChange
(KPageWidgetItem * currentPageItem, KPageWidgetItem * previousPageItem)
{
  if (currentPageItem == ringAccountCredentialsPageItem) {
    setStandardButtons(QDialogButtonBox::Cancel);
    nextButton()->setText(i18n("Create"));
  } else if (currentPageItem == progressPageItem) {
    setAppropriate(progressPageItem, false);
    setAppropriate(ringAccountCredentialsPageItem, false);
    setStandardButtons(QDialogButtonBox::NoButton);
    nextButton()->setText(i18n("Next"));
    if (previousPageItem == ringAccountCredentialsPageItem) {
      createRingAccount();
    }
  } else if (currentPageItem == finishPageItem) {
    setStandardButtons(QDialogButtonBox::NoButton);
  }

  return;
}

void AccountAssistantDialog::setValidPage(QWidget * page, bool valid)
{
  if (currentPage()->widget() == page) {
    setValid(currentPage(), valid);
  }

  return;
}

void AccountAssistantDialog::closeEvent(QCloseEvent * event)
{
  if ((currentPage() == progressPageItem)
      && (account->editState() != Account::EditState::REMOVED)) {
    event->ignore();
  } else {
    event->accept();
  }

  return;
}

void AccountAssistantDialog::createRingAccount()
{
  account
      = AccountModel::instance().add(ringAccountCredentialsPage->getUserName(),
                                     Account::Protocol::RING);
  if (!ringAccountCredentialsPage->getFullName().isEmpty()) {
    account->setDisplayName(ringAccountCredentialsPage->getFullName());
  }
  account->setArchivePassword(ringAccountCredentialsPage->getPassword());
  account->setUpnpEnabled(true);

  connect(account,
          &Account::stateChanged,
          this,
          &AccountAssistantDialog::handleAccountStateChange);

  connect(&AccountModel::instance(),
          &AccountModel::accountRemoved,
          this,
          [this](Account * removedAccount)
  {
    if (account == removedAccount) {
      account->performAction(Account::EditAction::REMOVE);
      close();
    }
    return;
  });

  account->performAction(Account::EditAction::SAVE);

  return;
}

void AccountAssistantDialog::handleAccountStateChange
(Account::RegistrationState state)
{
  switch (state) {
    case Account::RegistrationState::READY:
    case Account::RegistrationState::UNREGISTERED:
    case Account::RegistrationState::TRYING:
    {
      disconnect(account,
                 &Account::stateChanged,
                 this,
                 &AccountAssistantDialog::handleAccountStateChange);

      account->performAction(Account::EditAction::RELOAD);

      auto summaryLabel = new QLabel(i18n("The Ring account was created."));

      finishPage->insertWidget(0, summaryLabel);

      auto idGroupBox = new QGroupBox(i18n("ID"));

      auto idLayout = new QVBoxLayout(idGroupBox);

      auto idLabel = new QLabel(account->username());
      idLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
      idLabel->setAlignment(Qt::AlignCenter);
      idLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

      idLayout->addWidget(idLabel);

      finishPage->insertWidget(finishPage->getItemCount() - 1, idGroupBox);

      if (ringAccountCredentialsPage
          ->isPublicUserNameRegistrationRequested()) {
        auto publicUserNameRegistrationStarted
            = account->registerName(ringAccountCredentialsPage->getPassword(),
                                    ringAccountCredentialsPage->getUserName());
        if (publicUserNameRegistrationStarted) {
          connect(account,
                  &Account::nameRegistrationEnded,
                  this,
                  &AccountAssistantDialog
                  ::handlePublicUserNameRegistrationEnd);
          return;
        } else {
          auto messageWidget
              = new KMessageWidget(i18n("Could not start"
                                        " public user name"
                                        " registration process."));
          messageWidget->setWordWrap(true);
          messageWidget->setMessageType(KMessageWidget::Error);

          finishPage->insertWidget(0, messageWidget);
        }
      }

      next();

      break;
    }
    case Account::RegistrationState::ERROR:
    {
      disconnect(account,
                 &Account::stateChanged,
                 this,
                 &AccountAssistantDialog::handleAccountStateChange);

      auto summaryLabel = new QLabel(i18n("Ring account creation failed."));

      finishPage->insertWidget(0, summaryLabel);

      next();

      break;
    }
    case Account::RegistrationState::INITIALIZING:
    case Account::RegistrationState::COUNT__:
    default:
    {
      break;
    }
  }

  return;
}

void AccountAssistantDialog::handlePublicUserNameRegistrationEnd
(NameDirectory::RegisterNameStatus status, const QString & name)
{
  Q_UNUSED(name);

  disconnect(account,
             &Account::nameRegistrationEnded,
             this,
             &AccountAssistantDialog::handlePublicUserNameRegistrationEnd);

  if (status == NameDirectory::RegisterNameStatus::SUCCESS) {
    auto publicUserNameGroupBox = new QGroupBox(i18n("Public user name"));

    auto publicUserNameLayout = new QVBoxLayout(publicUserNameGroupBox);

    auto publicUserNameLabel = new QLabel(QStringLiteral("ring:")
                                          + account->registeredName());
    publicUserNameLabel
        ->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    publicUserNameLabel->setAlignment(Qt::AlignCenter);
    publicUserNameLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    publicUserNameLayout->addWidget(publicUserNameLabel);

    finishPage->insertWidget(finishPage->getItemCount() - 1,
                             publicUserNameGroupBox);
  } else {
    auto messageWidget = new KMessageWidget();
    messageWidget->setWordWrap(true);
    messageWidget->setMessageType(KMessageWidget::Warning);

    const auto messagePart = i18n("Could not register public user name.");
    const auto space = QStringLiteral(" ");

    switch (status) {
      case NameDirectory::RegisterNameStatus::WRONG_PASSWORD:
      {
        messageWidget->setText(messagePart
                               + space
                               + i18n("Password is wrong."));
        break;
      }
      case NameDirectory::RegisterNameStatus::INVALID_NAME:
      {
        messageWidget->setText(messagePart
                               + space
                               + i18n("User name is invalid."));
        break;
      }
      case NameDirectory::RegisterNameStatus::ALREADY_TAKEN:
      {
        messageWidget->setText(messagePart
                               + space
                               + i18n("User name is already taken."));
        break;
      }
      case NameDirectory::RegisterNameStatus::NETWORK_ERROR:
      {
        messageWidget->setText(messagePart
                               + space
                               + i18n("Network error has occurred."));
        break;
      }
      default:
      {
        messageWidget->setText(messagePart
                               + space
                               + i18n("Unknown error has occurred."));
        break;
      }
    }

    finishPage->insertWidget(0, messageWidget);
  }

  next();

  return;
}
