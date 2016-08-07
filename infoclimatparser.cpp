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

#include "infoclimatparser.h"
#include <KUnitConversion/Value>

#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

InfoClimatParser::InfoClimatParser(QTextStream &errorStream)
    : m_errorStream(errorStream)
{
}

InfoClimatParser::~InfoClimatParser()
{
    m_errorStream.flush();
}

bool InfoClimatParser::parse(const QJsonDocument &doc)
{
    const QJsonObject root = doc.object();
    const QStringList keys = root.keys(); // already sorted
    static QString request_state = QStringLiteral("request_state");
    if (!keys.contains(request_state))
        return false;
    const int stateValue = root.value(request_state).toInt();
    if (stateValue != 200) {
        qWarning() << "Got error: request_state=" << stateValue;
        return false;
    }
    foreach (const QString &key, keys) {
        const QDateTime date = QDateTime::fromString(key, Qt::ISODate); // result is in LocalTime, which is fine
        if (!date.isValid()) // filter out the other keys like request_state, request_key, message, model_run, source
            continue;
        const QJsonObject details = root.value(key).toObject();
        const QJsonObject temperature = details.value("temperature").toObject();
        const double kelvin = temperature.value("sol").toDouble();
        KUnitConversion::Value temp(kelvin, KUnitConversion::Kelvin);
        const int celsius = qRound(temp.convertTo(KUnitConversion::Celsius).number());
        qDebug() << date << celsius;
    }
    return true;
}
