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
  if (currentPage() == progressPageItem) {
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

      auto summaryLabel = new QLabel(finishPage);
      summaryLabel->setText(i18n("The Ring account was created."));

      finishPage->addWidget(summaryLabel);

      auto idGroupBox = new QGroupBox(finishPage);
      idGroupBox->setTitle(i18n("ID"));

      auto idLabel = new QLabel(idGroupBox);
      idLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
      idLabel->setAlignment(Qt::AlignCenter);
      idLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
      idLabel->setText(account->username());

      auto idLayout = new QVBoxLayout(idGroupBox);
      idLayout->addWidget(idLabel);

      finishPage->addWidget(idGroupBox);

      if (ringAccountCredentialsPage
          ->isPublicUserNameRegistrationRequested()) {
        bool publicUserNameRegistrationStarted
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
          auto messageWidget = new KMessageWidget(finishPage);
          messageWidget->setText(i18n("Could not start"
                                      " public user name"
                                      " registration process."));
          messageWidget->setWordWrap(true);
          messageWidget->setMessageType(KMessageWidget::Error);
          finishPage->insertWidget(0, messageWidget);
        }
      }

      finishPage->addItem(new QSpacerItem(20,
                                          40,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding));

      next();

      break;
    }
    case Account::RegistrationState::ERROR:
    {
      disconnect(account,
                 &Account::stateChanged,
                 this,
                 &AccountAssistantDialog::handleAccountStateChange);

      auto summaryLabel = new QLabel(finishPage);
      summaryLabel->setText(i18n("Ring account creation failed."));
      finishPage->addWidget(summaryLabel);

      finishPage->addItem(new QSpacerItem(20,
                                          40,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding));

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
    auto publicUserNameGroupBox = new QGroupBox(finishPage);
    publicUserNameGroupBox->setTitle(i18n("Public user name"));

    auto publicUserNameLabel = new QLabel(publicUserNameGroupBox);
    publicUserNameLabel
        ->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    publicUserNameLabel->setAlignment(Qt::AlignCenter);
    publicUserNameLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    publicUserNameLabel->setText(QStringLiteral("ring:")
                                 + account->registeredName());

    auto publicUserNameLayout = new QVBoxLayout(publicUserNameGroupBox);
    publicUserNameLayout->addWidget(publicUserNameLabel);

    finishPage->addWidget(publicUserNameGroupBox);
  } else {
    auto messageWidget = new KMessageWidget(finishPage);
    messageWidget->setWordWrap(true);
    messageWidget->setMessageType(KMessageWidget::Warning);
    finishPage->insertWidget(0, messageWidget);

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
  }

  finishPage->addItem(new QSpacerItem(20,
                                      40,
                                      QSizePolicy::Minimum,
                                      QSizePolicy::Expanding));

  next();

  return;
}
