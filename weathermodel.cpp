#include "weathermodel.h"
#include "infoclimatparser.h"

#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QFile>

#include <iostream>

WeatherModel::WeatherModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    fetchData();
}

int WeatherModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return m_data.count();
}

int WeatherModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return 5; // see WeatherDataPrivate
}

QVariant WeatherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

void WeatherModel::slotDataAvailable(const QJsonDocument &doc)
{
    qDebug();
    QTextStream stream(stdout);
    QFile dump("dump.json");
    if (dump.open(QIODevice::WriteOnly)) {
        dump.write(doc.toJson(QJsonDocument::Indented));
    }
    InfoClimatParser parser(stream);
    m_data = parser.parse(doc);
}

void WeatherModel::slotError()
{
    qWarning() << "Error fetching weather data";
}

void WeatherModel::fetchData()
{
    qDebug();
    m_provider = DataProvider::createProvider();
    connect(m_provider.data(), &DataProvider::dataAvailable, this, &WeatherModel::slotDataAvailable);
    connect(m_provider.data(), &DataProvider::error, this, &WeatherModel::slotError);
    m_provider->ensureDataAvailable();
}
