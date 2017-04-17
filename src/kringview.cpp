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

// application headers
#include "kringview.h"
#include "kringSettings.h"

kringView::kringView(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    slotSettingsChanged();
}

kringView::~kringView()
{
}

void kringView::slotSwitchColors()
{
    // switch the foreground/background colors of the label
    QColor color = kringSettings::color_background();
    kringSettings::setColor_background(kringSettings::color_foreground());
    kringSettings::setColor_foreground(color);

    slotSettingsChanged();
}

void kringView::slotSettingsChanged()
{
    qCDebug(KRING) << "kringView::slotSettingsChanged()";
    QPalette palette;
    palette.setColor(QPalette::Window, kringSettings::color_background());
    palette.setColor(QPalette::WindowText, kringSettings::color_foreground());
    m_ui.templateLabel->setPalette(palette);

    // i18n : internationalization
    m_ui.templateLabel->setText(i18n("This project is %1 days old", kringSettings::val_time()));
    emit signalChangeStatusbar(i18n("Settings changed"));
}
