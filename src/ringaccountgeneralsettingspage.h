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

#ifndef RINGACCOUNTGENERALSETTINGSPAGE_H
#define RINGACCOUNTGENERALSETTINGSPAGE_H

#include <namedirectory.h>

#include "abstractsettingspage.h"

class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QVBoxLayout;

class KMessageWidget;

class Account;

namespace Ui {
class RingAccountGeneralSettingsPage;
}

class RingAccountGeneralSettingsPage : public AbstractSettingsPage
{
  Q_OBJECT

public:
  explicit RingAccountGeneralSettingsPage(Account& account,
                                          QWidget* parent = nullptr);
  ~RingAccountGeneralSettingsPage();

public:
  bool isValid() const override;
  void validate() override;

private:
  void updateUi();

  void validateUserName();

  void setRegisteredPublicUserNameLabel(const QString& name);
  void setWarningMessage(const NameDirectory::RegisterNameStatus status);

private slots:
  void findPublicUserName(const QString& name);
  void validatePublicUserName(const Account* account,
                              NameDirectory::LookupStatus status,
                              const QString& address,
                              const QString& name);
  void handlePublicUserNameRegistrationEnd(
    NameDirectory::RegisterNameStatus status,
    const QString& name);

private:
  Ui::RingAccountGeneralSettingsPage* ui = nullptr;

  QVBoxLayout* publicUserNameLayout = nullptr;

  QLineEdit* publicUserNameLineEdit = nullptr;
  QPushButton* registerPublicUserNamePushButton = nullptr;
  QProgressBar* registeringPublicUserNameProgressBar = nullptr;
  QLabel* publicUserNameStateLabel = nullptr;

  KMessageWidget* warningMessageWidget = nullptr;
  KMessageWidget* errorMessageWidget = nullptr;

  bool userNameValid = false;
  bool publicUserNameValid = false;
};

#endif // RINGACCOUNTGENERALSETTINGSPAGE_H
