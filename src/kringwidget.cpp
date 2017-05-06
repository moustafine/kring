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

#include "kringwidget.h"

#include "ui_kringwidget.h"

#include "kringsettings.h"

KringWidget::KringWidget(QWidget * parent)
  : QWidget(parent)
{
  ui = new Ui::KringWidget();
  ui->setupUi(this);
}

KringWidget::~KringWidget()
{
  delete ui;
}

void KringWidget::slotSwitchColors()
{
  // switch the foreground/background colors of the label
  QColor color = KringSettings::color_background();
  KringSettings::setColor_background(KringSettings::color_foreground());
  KringSettings::setColor_foreground(color);

  loadSettings();

  return;
}

void KringWidget::loadSettings()
{
  qCDebug(KRING) << "KringWidget::loadSettings()";
  QPalette palette;
  palette.setColor(QPalette::Window, KringSettings::color_background());
  palette.setColor(QPalette::WindowText, KringSettings::color_foreground());
  ui->templateLabel->setPalette(palette);

  // i18n : internationalization
  ui->templateLabel->setText(i18n("This project is %1 days old",
                                  KringSettings::val_time()));
  emit signalChangeStatusbar(i18n("Settings changed"));

  return;
}
