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

#include "accountsettings.h"

#include <account.h>

AccountSettings::AccountSettings(QObject * parent)
  : KCoreConfigSkeleton(QString(), parent)
{
  ;
}

AccountSettings::~AccountSettings()
{
  ;
}

AccountSettings::StringSettingItem::StringSettingItem
(Account & account,
 Setting setting,
 const QString & group,
 const QString & key,
 QString & reference,
 const QString & defaultValue,
 KCoreConfigSkeleton::ItemString::Type type)
  : ItemString(group, key, reference, defaultValue, type),
    account(account),
    setting(setting)
{
  ;
}

AccountSettings::StringSettingItem::~StringSettingItem()
{
  ;
}

void AccountSettings::StringSettingItem::readConfig(KConfig * config)
{
  switch (setting) {
    case Setting::UserName:
    {
      mReference = account.alias();
      break;
    }
    case Setting::FullName:
    {
      mReference = account.displayName();
      break;
    }
    default:
    {
      break;
    }
  }

  mLoadedValue = mReference;

  if (config) {
    readImmutability(KConfigGroup(config, mGroup));
 }

  return;
}

void AccountSettings::StringSettingItem::writeConfig(KConfig * config)
{
  Q_UNUSED(config);

  switch (setting) {
    case Setting::UserName:
    {
      account.setAlias(mReference);
      break;
    }
    case Setting::FullName:
    {
      account.setDisplayName(mReference);
      break;
    }
    default:
    {
      break;
    }
  }

  account.performAction(Account::EditAction::SAVE);

  return;
}

AccountSettings::BoolSettingItem::BoolSettingItem(Account & account,
                                                  Setting setting,
                                                  const QString & group,
                                                  const QString & key,
                                                  bool & reference,
                                                  bool defaultValue)
  : ItemBool(group, key, reference, defaultValue),
    account(account),
    setting(setting)
{
  ;
}

AccountSettings::BoolSettingItem::~BoolSettingItem()
{
  ;
}

void AccountSettings::BoolSettingItem::readConfig(KConfig * config)
{
  switch (setting) {
    case Setting::AccountEnabled:
    {
      mReference = account.isEnabled();
      break;
    }
    default:
    {
      break;
    }
  }

  mLoadedValue = mReference;

  if (config) {
    readImmutability(KConfigGroup(config, mGroup));
 }

  return;
}

void AccountSettings::BoolSettingItem::writeConfig(KConfig * config)
{
  Q_UNUSED(config);

  switch (setting) {
    case Setting::AccountEnabled:
    {
      account.setEnabled(mReference);
      break;
    }
    default:
    {
      break;
    }
  }

  account.performAction(Account::EditAction::SAVE);

  return;
}
