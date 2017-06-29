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

#ifndef ACCOUNTASSISTANTDIALOG_H
#define ACCOUNTASSISTANTDIALOG_H

#include <KAssistantDialog>

#include <account.h>
#include <namedirectory.h>

class KPageWidgetItem;

class FinishPage;
class RingAccountCredentialsPage;

class AccountAssistantDialog : public KAssistantDialog
{
  Q_OBJECT

public:
  explicit AccountAssistantDialog(QWidget * parent = nullptr,
                                  Qt::WindowFlags flag = Qt::WindowFlags());
  ~AccountAssistantDialog();

public slots:
  void handleCurrentPageChange(KPageWidgetItem * currentPageItem,
                               KPageWidgetItem * previousPageItem);
  void setValidPage(QWidget * page, bool valid);

protected:
  void closeEvent(QCloseEvent * event) override;

private:
  void createRingAccount();

private slots:
  void handleAccountStateChange(Account::RegistrationState state);
  void handlePublicUserNameRegistrationEnd
  (NameDirectory::RegisterNameStatus status, const QString & name);

private:
  RingAccountCredentialsPage * ringAccountCredentialsPage = nullptr;
  FinishPage * finishPage = nullptr;

  KPageWidgetItem * ringAccountCredentialsPageItem = nullptr;
  KPageWidgetItem * progressPageItem = nullptr;
  KPageWidgetItem * finishPageItem = nullptr;

  Account * account = nullptr;
};

#endif // ACCOUNTASSISTANTDIALOG_H
