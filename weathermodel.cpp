#include "weathermodel.h"
#include "infoclimatparser.h"
#include "wettercomparser.h"
#include "openweathermapparser.h"

#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QStandardPaths>
#include <qfileinfo.h>

#include <iostream>
#include <qdir.h>

WeatherModel::WeatherModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_parser(Q_NULLPTR),
      m_reloadTimer(new QTimer(this))
{
    m_backend = OpenWeatherMap;
    setBackend(m_backend);

    connect(m_reloadTimer, &QTimer::timeout, this, &WeatherModel::fetchData);
    m_reloadTimer->start(60 * 60 * 1000); // 1 hour
}

WeatherModel::~WeatherModel()
{
    delete m_parser;
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

    const WeatherDataEntry &wd = m_data.at(row);

    switch (role) {
    case Qt::DisplayRole:
        return wd.toString();
    case DayOfWeek: {
        const int weekday = wd.dateTime().date().dayOfWeek();
        return QDate::longDayName(weekday);
    }
    case Time:
        return wd.dateTime().time().hour();
    case DateTime:
        return wd.dateTime();
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
    case WeatherIcon:
        return wd.weather_icon();
    case WeatherDescription:
        return wd.weather_description();
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
    roles.insert(DateTime, "dateTime");
    roles.insert(Temperature, "temperature");
    roles.insert(AverageWind, "averageWind");
    roles.insert(GustWind, "gustWind");
    roles.insert(WindDirection, "windDirection");
    roles.insert(Rain, "rain");
    roles.insert(WeatherIcon, "weatherIcon");
    roles.insert(WeatherDescription, "weatherDescription");
    return roles;
}

void WeatherModel::toggleBackend()
{
    if (m_backend == LastBackend) {
        m_backend = InfoClimat;
    } else {
        m_backend = static_cast<Backend>(m_backend + 1);
    }
    setBackend(m_backend);
}

QString WeatherModel::backendName() const
{
    return m_parser->backendName();
}

QDateTime WeatherModel::currentDateTime() const
{
    QDateTime dt = QDateTime::currentDateTime();
    QTime time = dt.time();
    time.setHMS(time.hour(), 0, 0); // we could round rather than truncate, but does it matter?
    return QDateTime(dt.date(), time);
}

int WeatherModel::indexForCurrentDateTime() const
{
    const QDateTime dt = currentDateTime();
    const QVector<WeatherDataEntry> entries = m_data.entries();
    const auto it = std::find_if(entries.constBegin(), entries.constEnd(),
                           [dt](const WeatherDataEntry& entry) {
        return entry.dateTime() >= dt;
    });
    if (it != entries.constEnd()) {
        const int ret = std::distance(entries.constBegin(), it);
        qDebug() << "Scrolling to" << ret;
        return ret;
    }
    return -1;
}

void WeatherModel::slotDataAvailable(const QByteArray &data)
{
    qDebug();
    beginResetModel();
    m_data.merge(m_parser->parse(data));
    endResetModel();
    qDebug() << m_data.count() << "data" << rowCount() << "rows";

    if (m_backend == OpenWeatherMap) { // we could save all, but then we'd need different filenames...
        // (and there are some round-trip issues with image paths etc.)
        QDir().mkpath(QFileInfo(autoSaveFileName()).absolutePath());
        OpenWeatherMapParser::save(m_data.entries(), autoSaveFileName());
    }

    emit currentDateTimeChanged();
}

void WeatherModel::slotError()
{
    qWarning() << "Error fetching weather data";
}

void WeatherModel::fetchData()
{
    qDebug();
    m_provider = DataProvider::createProvider(m_parser->url(), m_parser->cacheFileName());
    connect(m_provider.data(), &DataProvider::dataAvailable, this, &WeatherModel::slotDataAvailable);
    connect(m_provider.data(), &DataProvider::error, this, &WeatherModel::slotError);
    m_provider->ensureDataAvailable();
}

QString WeatherModel::autoSaveFileName() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/autosave.json";
}

void WeatherModel::setBackend(WeatherModel::Backend backend)
{
    m_data.clear();
    delete m_parser;
    switch (backend) {
    case InfoClimat:
        m_parser = new InfoClimatParser;
        break;
    case WetterCom:
        m_parser = new WetterComParser;
        break;
    case OpenWeatherMap:
    {
        m_parser = new OpenWeatherMapParser;
        beginResetModel();
        QFile file(autoSaveFileName());
        if (file.open(QIODevice::ReadOnly)) {
            m_data.merge(m_parser->parse(file.readAll()));
        }
        break;
    }
    }

    emit backendNameChanged();
    fetchData();
}
