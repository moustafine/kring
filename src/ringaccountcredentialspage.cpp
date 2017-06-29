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

#include "ringaccountcredentialspage.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include <KColorScheme>
#include <KUser>

#include <account.h>

#include "ui_ringaccountcredentialspage.h"

RingAccountCredentialsPage::RingAccountCredentialsPage(QWidget * parent)
  : QWidget(parent)
{
  ui = new Ui::RingAccountCredentialsPage();
  ui->setupUi(this);

  KUser user;
  ui->fullNameLineEdit->setText(user.property(KUser::FullName).toString());

  QRegularExpression userNameRegularExpression
      (QStringLiteral("[A-Za-z0-9]+(-?[A-Za-z0-9]+)*"));
  QValidator * validUserNameator
      = new QRegularExpressionValidator(userNameRegularExpression, this);
  ui->userNameLineEdit->setValidator(validUserNameator);
  ui->userNameLineEdit->setText(qgetenv("USER"));

  validateUserName();

  connect(ui->userNameLineEdit,
          &QLineEdit::textEdited,
          this,
          &RingAccountCredentialsPage::validateUserName);

  ui->searchingStateLabel
      ->setVisible(ui->registerPublicUserNameCheckBox->isChecked());

  connect(ui->registerPublicUserNameCheckBox,
          &QCheckBox::stateChanged,
          this,
          &RingAccountCredentialsPage
          ::handleRegisterPublicUserNameCheckBoxStateChange);

  connect(ui->passwordLineEdit,
          &QLineEdit::textEdited,
          this,
          &RingAccountCredentialsPage::validatePassword);
  connect(ui->confirmPasswordLineEdit,
          &QLineEdit::textEdited,
          this,
          &RingAccountCredentialsPage::validatePassword);
}

RingAccountCredentialsPage::~RingAccountCredentialsPage()
{
  delete ui;
}

void RingAccountCredentialsPage::updateUi()
{
  KColorScheme colorScheme(QPalette::Active);
  auto warningColorName = colorScheme
      .background(KColorScheme::NegativeBackground)
      .color()
      .name(QColor::HexRgb);

  if (validUserName) {
    setStyleSheet(QStringLiteral());
  } else {
    setStyleSheet(QStringLiteral("QLineEdit#userNameLineEdit {"
                                 "background:")
                  + warningColorName
                  + QStringLiteral("}"));
  }

  if (!validPassword) {
    setStyleSheet(styleSheet()
                  + QStringLiteral("QLineEdit#passwordLineEdit {"
                                   "background:")
                  + warningColorName
                  + QStringLiteral("}"));
  }

  if (!validPassword || !validConfirmPassword) {
    setStyleSheet(styleSheet()
                  + QStringLiteral("QLineEdit#confirmPasswordLineEdit {"
                                   "background:")
                  + warningColorName
                  + QStringLiteral("}"));
  }

  if (!validUserName || !validPassword || !validConfirmPassword) {
    emit valid(this, false);

    return;
  }

  emit valid(this, true);

  return;
}

QString RingAccountCredentialsPage::getFullName() const
{
  return ui->fullNameLineEdit->text();
}

QString RingAccountCredentialsPage::getUserName() const
{
  return ui->userNameLineEdit->text();
}

QString RingAccountCredentialsPage::getPassword() const
{
  return ui->passwordLineEdit->text();
}

bool RingAccountCredentialsPage::isPublicUserNameRegistrationRequested() const
{
  return ui->registerPublicUserNameCheckBox->isChecked();
}

void RingAccountCredentialsPage::validateUserName()
{
  validUserName = true;

  if (ui->userNameLineEdit->text().isEmpty()) {
     validUserName = false;
  }

  if (ui->registerPublicUserNameCheckBox->isChecked()) {
     validUserName = false;

     ui->searchingStateLabel->setText(i18n("Searching..."));

     NameDirectory::instance().lookupName(nullptr,
                                          QStringLiteral(),
                                          ui->userNameLineEdit->text());
  } else {
     ui->searchingStateLabel->clear();
  }

  updateUi();

  return;
}

void RingAccountCredentialsPage::validatePassword()
{
  validPassword = true;
  if (ui->passwordLineEdit->text().isEmpty()) {
     validPassword = false;
  }

  validConfirmPassword = true;
  if (ui->passwordLineEdit->text() != ui->confirmPasswordLineEdit->text()) {
     validConfirmPassword = false;
  }

  updateUi();

  return;
}

void
RingAccountCredentialsPage::handleRegisterPublicUserNameCheckBoxStateChange
(int state)
{
  auto checkState = static_cast<Qt::CheckState>(state);

  switch (checkState) {
    case Qt::Checked:
    case Qt::PartiallyChecked:
    {
      connect(&NameDirectory::instance(),
              &NameDirectory::registeredNameFound,
              this,
              &RingAccountCredentialsPage::validatePublicUserName);
      break;
    }
    case Qt::Unchecked:
    {
      disconnect(&NameDirectory::instance(),
                 &NameDirectory::registeredNameFound,
                 this,
                 &RingAccountCredentialsPage::validatePublicUserName);
      break;
    }
    default:
    {
      break;
    }
  }

  validateUserName();

  return;
}

void RingAccountCredentialsPage::validatePublicUserName
(const Account * account,
 NameDirectory::LookupStatus status,
 const QString & address,
 const QString & name)
{
  Q_UNUSED(account)
  Q_UNUSED(address)

  if (!isActiveWindow()) {
    return;
  }

  validUserName = false;

  if (ui->userNameLineEdit->text() != name) {
    validateUserName();
    return;
  }

  switch (status) {
    case NameDirectory::LookupStatus::NOT_FOUND:
    {
      validUserName = true;
      ui->searchingStateLabel->setText(i18n("Name is available"));
      break;
    }
    case NameDirectory::LookupStatus::SUCCESS:
    {
      ui->searchingStateLabel->setText(i18n("Name not available"));
      break;
    }
    case NameDirectory::LookupStatus::INVALID_NAME:
    {
      ui->searchingStateLabel->setText(i18n("Name is invalid"));
      break;
    }
    case NameDirectory::LookupStatus::ERROR:
    {
      ui->searchingStateLabel->setText(i18n("Network error"));
      break;
    }
    default:
    {
      break;
    }
  }

  updateUi();

  return;
}
