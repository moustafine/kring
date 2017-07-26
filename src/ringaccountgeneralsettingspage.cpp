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

#include "ringaccountgeneralsettingspage.h"

#include <QFontDatabase>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QString>

#include <KColorScheme>

#include <account.h>

#include "ui_ringaccountgeneralsettingspage.h"

RingAccountGeneralSettingsPage::RingAccountGeneralSettingsPage
(Account & account, QWidget * parent)
  : AbstractSettingsPage(parent)
{
  ui = new Ui::RingAccountGeneralSettingsPage();
  ui->setupUi(this);

  ui->idLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  ui->idLabel->setAlignment(Qt::AlignCenter);
  ui->idLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  ui->idLabel->setText(account.username());

  QRegularExpression userNameRegularExpression
      (QStringLiteral("[A-Za-z0-9]+(-?[A-Za-z0-9]+)*"));
  QValidator * userNameValidator
      = new QRegularExpressionValidator(userNameRegularExpression, this);
  ui->kcfg_userName->setValidator(userNameValidator);
}

RingAccountGeneralSettingsPage::~RingAccountGeneralSettingsPage()
{
  delete ui;
}

bool RingAccountGeneralSettingsPage::isValid() const
{
  return userNameValid;
}

void RingAccountGeneralSettingsPage::validate()
{
  validateUserName();

  return;
}

void RingAccountGeneralSettingsPage::updateUi()
{
  KColorScheme colorScheme(QPalette::Active);
  auto warningColorName = colorScheme
      .background(KColorScheme::NegativeBackground)
      .color()
      .name(QColor::HexRgb);

  if (userNameValid) {
    setStyleSheet(QStringLiteral());
  } else {
    setStyleSheet(QStringLiteral("QLineEdit#kcfg_userName {"
                                 "background:")
                  + warningColorName
                  + QStringLiteral("}"));
  }

  return;
}

void RingAccountGeneralSettingsPage::validateUserName()
{
  userNameValid = true;

  if (ui->kcfg_userName->text().isEmpty()) {
    userNameValid = false;
  }

  updateUi();

  return;
}
