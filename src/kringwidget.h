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

#ifndef KRINGWIDGET_H
#define KRINGWIDGET_H

#include <QWidget>

namespace Ui {
class KringWidget;
}

class KringWidget : public QWidget
{
  Q_OBJECT

public:
  explicit KringWidget(QWidget* parent = nullptr);
  ~KringWidget();

public slots:
  void loadSettings();
  void slotSwitchColors();

signals:
  /**
   * Use this signal to change the content of the statusbar
   */
  void signalChangeStatusbar(const QString& text);

  /**
   * Use this signal to change the content of the caption
   */
  void signalChangeCaption(const QString& text);

private:
  Ui::KringWidget* ui = nullptr;
};

#endif // KRINGWIDGET_H
