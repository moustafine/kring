/*
Copyright (C) 2017-2018 by Marat Moustafine <moustafine@tuta.io>

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

#include "ringaccountgeneralsettingspage.h"

#include <QColor>
#include <QFontDatabase>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QLoggingCategory>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QString>
#include <QVBoxLayout>

#include <KColorScheme>
#include <KLocalizedString>
#include <KMessageWidget>

#include <account.h>

#include "ui_ringaccountgeneralsettingspage.h"

#include "authenticationdialog.h"

Q_DECLARE_LOGGING_CATEGORY(kring)

RingAccountGeneralSettingsPage::RingAccountGeneralSettingsPage(Account& account,
                                                               QWidget* parent)
  : AbstractSettingsPage(parent)
{
  ui = new Ui::RingAccountGeneralSettingsPage();
  ui->setupUi(this);

  ui->idLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui->idLabel->setAlignment(Qt::AlignCenter);
  ui->idLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  ui->idLabel->setText(account.username());

  QRegularExpression userNameRegularExpression(
    QStringLiteral("[A-Za-z0-9]+(-?[A-Za-z0-9]+)*"));
  auto userNameValidator =
    new QRegularExpressionValidator(userNameRegularExpression, this);
  ui->kcfg_userName->setValidator(userNameValidator);

  auto publicUserName = account.registeredName();

  auto publicUserNameGroupBox = new QGroupBox(i18n("Public user name"), this);
  publicUserNameGroupBox->setObjectName(
    QStringLiteral("publicUserNameGroupBox"));

  if (publicUserName.isEmpty()) {
    publicUserNameLayout = new QVBoxLayout(publicUserNameGroupBox);

    auto registerPublicUserNameLayout = new QHBoxLayout();

    publicUserNameLineEdit = new QLineEdit(account.alias());
    publicUserNameLineEdit->setObjectName(
      QStringLiteral("publicUserNameLineEdit"));
    publicUserNameLineEdit->setClearButtonEnabled(true);
    publicUserNameLineEdit->setValidator(userNameValidator);

    registerPublicUserNameLayout->addWidget(publicUserNameLineEdit);

    registerPublicUserNamePushButton =
      new QPushButton(QIcon::fromTheme(QStringLiteral("object-locked")),
                      i18n("Register (experimental)"));

    registerPublicUserNameLayout->addWidget(registerPublicUserNamePushButton);

    publicUserNameLayout->addLayout(registerPublicUserNameLayout);

    auto publicUserNameStateLayout = new QHBoxLayout();
    publicUserNameStateLayout->addStretch();

    publicUserNameStateLabel = new QLabel();

    publicUserNameStateLayout->addWidget(publicUserNameStateLabel);

    publicUserNameLayout->addLayout(publicUserNameStateLayout);

    connect(&NameDirectory::instance(),
            &NameDirectory::registeredNameFound,
            this,
            &RingAccountGeneralSettingsPage::validatePublicUserName);
    connect(publicUserNameLineEdit,
            &QLineEdit::textChanged,
            this,
            &RingAccountGeneralSettingsPage::findPublicUserName);
    findPublicUserName(publicUserNameLineEdit->text());

    connect(
      &account,
      &Account::nameRegistrationEnded,
      this,
      &RingAccountGeneralSettingsPage ::handlePublicUserNameRegistrationEnd);
    connect(registerPublicUserNamePushButton,
            &QAbstractButton::clicked,
            this,
            [this, &account]() {
              AuthenticationDialog authenticationDialog(this);

              if (publicUserNameLineEdit && !authenticationDialog.exec()) {
                return;
              }

              auto publicUserNameRegistrationStarted =
                account.registerName(authenticationDialog.getPassword(),
                                     publicUserNameLineEdit->text());

              if (publicUserNameRegistrationStarted) {
                if (publicUserNameLineEdit) {
                  publicUserNameLineEdit->hide();
                }
                if (registerPublicUserNamePushButton) {
                  registerPublicUserNamePushButton->hide();
                }

                if (registeringPublicUserNameProgressBar) {
                  registeringPublicUserNameProgressBar->show();
                } else {
                  registeringPublicUserNameProgressBar = new QProgressBar();
                  registeringPublicUserNameProgressBar->setMinimum(0);
                  registeringPublicUserNameProgressBar->setMaximum(0);

                  publicUserNameLayout->insertWidget(
                    0, registeringPublicUserNameProgressBar);
                }

                if (publicUserNameStateLabel) {
                  publicUserNameStateLabel->setText(i18n("Registering..."));
                }
              } else {
                if (errorMessageWidget) {
                  errorMessageWidget->animatedShow();
                } else {
                  errorMessageWidget =
                    new KMessageWidget(i18n("Could not start"
                                            " public user name"
                                            " registration process."));
                  errorMessageWidget->setWordWrap(true);
                  errorMessageWidget->setMessageType(KMessageWidget::Error);

                  ui->vboxLayout->insertWidget(0, errorMessageWidget);
                }
              }

              return;
            });
  } else {
    setRegisteredPublicUserNameLabel(publicUserName);
  }

  ui->vboxLayout->insertWidget(ui->vboxLayout->indexOf(ui->nameGroupBox) + 1,
                               publicUserNameGroupBox);
}

RingAccountGeneralSettingsPage::~RingAccountGeneralSettingsPage()
{
  delete ui;
}

bool
RingAccountGeneralSettingsPage::isValid() const
{
  return userNameValid;
}

void
RingAccountGeneralSettingsPage::validate()
{
  validateUserName();

  return;
}

void
RingAccountGeneralSettingsPage::updateUi()
{
  KColorScheme colorScheme(QPalette::Active);
  auto warningColorName =
    colorScheme.background(KColorScheme::NegativeBackground)
      .color()
      .name(QColor::HexRgb);

  if (userNameValid) {
    setStyleSheet(QStringLiteral());
  } else {
    setStyleSheet(QStringLiteral("QLineEdit#kcfg_userName {"
                                 "background:") +
                  warningColorName + QStringLiteral("}"));
  }

  if (publicUserNameLineEdit && registerPublicUserNamePushButton) {
    if (publicUserNameValid) {
      registerPublicUserNamePushButton->setEnabled(true);
    } else {
      setStyleSheet(styleSheet() +
                    QStringLiteral("QLineEdit#publicUserNameLineEdit {"
                                   "background:") +
                    warningColorName + QStringLiteral("}"));

      registerPublicUserNamePushButton->setEnabled(false);
    }
  }

  return;
}

void
RingAccountGeneralSettingsPage::validateUserName()
{
  userNameValid = true;

  if (ui->kcfg_userName->text().isEmpty()) {
    userNameValid = false;
  }

  updateUi();

  return;
}

void
RingAccountGeneralSettingsPage::setRegisteredPublicUserNameLabel(
  const QString& name)
{
  if (publicUserNameLayout) {
    delete publicUserNameLayout;
    publicUserNameLayout = nullptr;
  }

  const auto publicUserNameGroupBoxName =
    QStringLiteral("publicUserNameGroupBox");

  auto publicUserNameGroupBox = findChild<QGroupBox*>(
    publicUserNameGroupBoxName, Qt::FindDirectChildrenOnly);

  if (!publicUserNameGroupBox) {
    qCWarning(kring,
              "Failed to get object by name \"%s\".",
              qUtf8Printable(publicUserNameGroupBoxName));
    return;
  }

  publicUserNameLayout = new QVBoxLayout(publicUserNameGroupBox);

  auto publicUserNameLabel = new QLabel(QStringLiteral("ring:") + name);
  publicUserNameLabel->setFont(
    QFontDatabase::systemFont(QFontDatabase::FixedFont));
  publicUserNameLabel->setAlignment(Qt::AlignCenter);
  publicUserNameLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

  publicUserNameLayout->addWidget(publicUserNameLabel);

  return;
}

void
RingAccountGeneralSettingsPage::setWarningMessage(
  const NameDirectory::RegisterNameStatus status)
{
  if (!warningMessageWidget) {
    return;
  }

  const auto messagePart = i18n("Could not register public user name.");
  const auto space = QStringLiteral(" ");

  switch (status) {
    case NameDirectory::RegisterNameStatus::WRONG_PASSWORD: {
      warningMessageWidget->setText(messagePart + space +
                                    i18n("Password is wrong."));
      break;
    }
    case NameDirectory::RegisterNameStatus::INVALID_NAME: {
      warningMessageWidget->setText(messagePart + space +
                                    i18n("User name is invalid."));
      break;
    }
    case NameDirectory::RegisterNameStatus::ALREADY_TAKEN: {
      warningMessageWidget->setText(messagePart + space +
                                    i18n("User name is already taken."));
      break;
    }
    case NameDirectory::RegisterNameStatus::NETWORK_ERROR: {
      warningMessageWidget->setText(messagePart + space +
                                    i18n("Network error has occurred."));
      break;
    }
    default: {
      warningMessageWidget->setText(messagePart + space +
                                    i18n("Unknown error has occurred."));
      break;
    }
  }

  return;
}

void
RingAccountGeneralSettingsPage::findPublicUserName(const QString& name)
{
  publicUserNameValid = false;

  updateUi();

  if (registeringPublicUserNameProgressBar) {
    registeringPublicUserNameProgressBar->hide();
  }

  if (publicUserNameLineEdit) {
    publicUserNameLineEdit->show();
  }
  if (registerPublicUserNamePushButton) {
    registerPublicUserNamePushButton->show();
  }

  if (publicUserNameStateLabel) {
    publicUserNameStateLabel->setText(i18n("Searching..."));
  }

  NameDirectory::instance().lookupName(nullptr, QStringLiteral(), name);

  return;
}

void
RingAccountGeneralSettingsPage::validatePublicUserName(
  const Account* account,
  NameDirectory::LookupStatus status,
  const QString& address,
  const QString& name)
{
  Q_UNUSED(account)
  Q_UNUSED(address)

  if (publicUserNameLineEdit && (publicUserNameLineEdit->text() != name)) {
    return;
  }

  publicUserNameValid = false;

  switch (status) {
    case NameDirectory::LookupStatus::NOT_FOUND: {
      publicUserNameValid = true;
      publicUserNameStateLabel->setText(i18n("Name is available"));
      break;
    }
    case NameDirectory::LookupStatus::SUCCESS: {
      publicUserNameStateLabel->setText(i18n("Name is not available"));
      break;
    }
    case NameDirectory::LookupStatus::INVALID_NAME: {
      publicUserNameStateLabel->setText(i18n("Name is invalid"));
      break;
    }
    case NameDirectory::LookupStatus::ERROR: {
      publicUserNameStateLabel->setText(i18n("Network error"));
      break;
    }
    default: {
      break;
    }
  }

  updateUi();

  return;
}

void
RingAccountGeneralSettingsPage::handlePublicUserNameRegistrationEnd(
  NameDirectory::RegisterNameStatus status,
  const QString& name)
{
  if (status == NameDirectory::RegisterNameStatus::SUCCESS) {
    if (publicUserNameLineEdit) {
      delete publicUserNameLineEdit;
      publicUserNameLineEdit = nullptr;
    }
    if (registerPublicUserNamePushButton) {
      delete registerPublicUserNamePushButton;
      registerPublicUserNamePushButton = nullptr;
    }
    if (registeringPublicUserNameProgressBar) {
      delete registeringPublicUserNameProgressBar;
      registeringPublicUserNameProgressBar = nullptr;
    }
    if (publicUserNameStateLabel) {
      delete publicUserNameStateLabel;
      publicUserNameStateLabel = nullptr;
    }

    if (warningMessageWidget) {
      delete warningMessageWidget;
      warningMessageWidget = nullptr;
    }
    if (errorMessageWidget) {
      delete errorMessageWidget;
      errorMessageWidget = nullptr;
    }

    setRegisteredPublicUserNameLabel(name);
  } else {
    if (warningMessageWidget) {
      setWarningMessage(status);

      warningMessageWidget->animatedShow();
    } else {
      warningMessageWidget = new KMessageWidget();
      warningMessageWidget->setWordWrap(true);
      warningMessageWidget->setMessageType(KMessageWidget::Warning);

      setWarningMessage(status);

      ui->vboxLayout->insertWidget(0, warningMessageWidget);
    }

    findPublicUserName(name);
  }

  return;
}
