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

#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <KCoreConfigSkeleton>

class KConfig;

class Account;

class AccountSettings : public KCoreConfigSkeleton
{
  Q_OBJECT

public:
  class StringSettingItem : public ItemString
  {
  public:
    enum class Setting
    {
      UserName,
      FullName
    };

  public:
    StringSettingItem(Account& account,
                      Setting setting,
                      const QString& group,
                      const QString& key,
                      QString& reference,
                      const QString& defaultValue = QStringLiteral(""),
                      Type type = Normal);
    ~StringSettingItem();

  public:
    void readConfig(KConfig* config) override;
    void writeConfig(KConfig* config) override;

  private:
    Account& account;
    Setting setting;
  };

  class BoolSettingItem : public ItemBool
  {
  public:
    enum class Setting
    {
      AccountEnabled
    };

  public:
    BoolSettingItem(Account& account,
                    Setting setting,
                    const QString& group,
                    const QString& key,
                    bool& reference,
                    bool defaultValue = true);
    ~BoolSettingItem();

  public:
    void readConfig(KConfig* config) override;
    void writeConfig(KConfig* config) override;

  private:
    Account& account;
    Setting setting;
  };

public:
  explicit AccountSettings(QObject* parent = nullptr);
  ~AccountSettings();
};

#endif // ACCOUNTSETTINGS_H
