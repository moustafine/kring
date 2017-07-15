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

#ifndef RINGACCOUNTCREDENTIALSPAGE_H
#define RINGACCOUNTCREDENTIALSPAGE_H

#include "abstractsettingspage.h"

#include <namedirectory.h>

class KPageWidgetItem;

class Account;

namespace Ui {
class RingAccountCredentialsPage;
}

class RingAccountCredentialsPage : public AbstractSettingsPage
{
  Q_OBJECT

public:
  explicit RingAccountCredentialsPage(QWidget * parent = nullptr);
  ~RingAccountCredentialsPage();

public:
  bool isValid() const override;
  void validate() override;

  QString getFullName() const;
  QString getUserName() const;
  QString getPassword() const;

  bool isPublicUserNameRegistrationRequested() const;

signals:
  void valid(QWidget * page, bool valid);

private:
  void updateUi();

private slots:
  void validateUserName();
  void validatePassword();
  void handleRegisterPublicUserNameCheckBoxStateChange(int state);
  void validatePublicUserName(const Account * account,
                              NameDirectory::LookupStatus status,
                              const QString & address,
                              const QString & name);

private:
  Ui::RingAccountCredentialsPage * ui = nullptr;

  bool validUserName = false;
  bool validPassword = false;
  bool validConfirmPassword = false;
};

#endif // RINGACCOUNTCREDENTIALSPAGE_H
