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

#ifndef FINISHPAGE_H
#define FINISHPAGE_H

#include <QWidget>

class QLayoutItem;

namespace Ui {
class FinishPage;
}

class FinishPage : public QWidget
{
  Q_OBJECT

public:
  explicit FinishPage(QWidget * parent = nullptr);
  ~FinishPage();

public:
  void addWidget(QWidget * widget,
                 int stretch = 0,
                 Qt::Alignment alignment = Qt::Alignment());
  void addItem(QLayoutItem * layoutItem);

  void insertWidget(int index,
                    QWidget * widget,
                    int stretch = 0,
                    Qt::Alignment alignment = Qt::Alignment());
  void insertItem(int index, QLayoutItem * layoutItem);

  int getItemCount() const;
private:
  Ui::FinishPage * ui = nullptr;
};

#endif // FINISHPAGE_H
