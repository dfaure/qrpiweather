/*  This file is part of qmeteofrance
 *  Copyright 2014  David Faure  <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "themeimageprovider.h"
#include "weathermodel.h"

#define SHOW_TABLEVIEW 0

#if SHOW_TABLEVIEW
#include <QApplication>
#include <QTableView>
#else
#include <QGuiApplication>
#endif

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

int main(int argc, char **argv)
{
#if SHOW_TABLEVIEW
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

    WeatherModel model;

    QQuickView view;

    QQmlContext *context = view.engine()->rootContext();
    context->setContextProperty("myModel", &model);

    ThemeImageProvider *imageProvider = new ThemeImageProvider;
    view.engine()->addImageProvider(QLatin1String("fromTheme"), imageProvider);

    view.setSource(QUrl("qrc:view.qml"));
    view.show();

#if SHOW_TABLEVIEW
    QTableView table;
    table.setModel(&model);
    table.show();
#endif

    return app.exec();
}

/*
 * Intégrer nouvelles données avec données existantes, pour garder historique sur 5 derniers jours
 *
 * Couleur de fond pour nuit/jour
 *
 * Vue jour
 * Historique par mois sur le passe avec courbe temperature, vent, pluie
*/
