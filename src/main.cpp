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

// application header
#include "kring.h"

// KDE headers
#include <KAboutData>
#include <KLocalizedString>

// Qt headers
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(KRING)
Q_LOGGING_CATEGORY(KRING, "kring")
int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("kring");
    KAboutData aboutData( QStringLiteral("kring"),
                          i18n("Simple App"),
                          QStringLiteral("0.1"),
                          i18n("A Simple Application written with KDE Frameworks"),
                          KAboutLicense::GPL,
                          i18n("(c) 2017, Marat Moustafine <moustafine@tuta.io>"));

    aboutData.addAuthor(i18n("Marat Moustafine"),i18n("Author"), QStringLiteral("moustafine@tuta.io"));
    application.setWindowIcon(QIcon::fromTheme("kring"));
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(application);
    aboutData.processCommandLine(&parser);
    KAboutData::setApplicationData(aboutData);

    kring *appwindow = new kring;
    appwindow->show();
    return application.exec();
}
