#include "weathermodel.h"
#include "infoclimatparser.h"

#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QDateTime>

#include <iostream>

WeatherModel::WeatherModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    fetchData();
}

int WeatherModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_data.count();
}

int WeatherModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 5; // see WeatherDataPrivate
}

QVariant WeatherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    if (row >= m_data.count())
        return QVariant();

    const WeatherData &wd = m_data.at(row);

    switch (role) {
    case Qt::DisplayRole:
        return wd.toString();
    case DayOfWeek: {
        const int weekday = wd.dateTime().date().dayOfWeek();
        return QDate::longDayName(weekday);
    }
    case Time:
        return wd.dateTime().time().hour();
    case Temperature:
        return wd.temperature_celsius();
    case AverageWind:
        return wd.average_wind();
    case GustWind:
        return wd.gust_wind();
    case WindDirection:
        return wd.wind_direction();
    case Rain:
        return wd.mm_rain();
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> WeatherModel::roleNames() const
{
    auto roles = QAbstractTableModel::roleNames();
    roles.insert(DayOfWeek, "dayOfWeek");
    roles.insert(Time, "time");
    roles.insert(Temperature, "temperature");
    roles.insert(AverageWind, "averageWind");
    roles.insert(GustWind, "gustWind");
    roles.insert(WindDirection, "windDirection");
    roles.insert(Rain, "rain");
    return roles;
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
    beginResetModel();
    m_data = parser.parse(doc);
    endResetModel();
    qDebug() << m_data.count() << "data" << rowCount() << "rows";
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
