#include "weatherdata.h"

#include <QDateTime>

class WeatherDataEntryPrivate : public QSharedData
{
public:
    WeatherDataEntryPrivate()
        : dateTime(),
          temperature_celsius(0),
          average_wind(0),
          gust_wind(0),
          wind_direction(0),
          mm_rain(0) {}

    QDateTime dateTime;
    int temperature_celsius;
    int average_wind;
    int gust_wind;
    int wind_direction;
    double mm_rain;
    QString weather_icon;
};

WeatherDataEntry::WeatherDataEntry()
    : d(new WeatherDataEntryPrivate)
{
}

WeatherDataEntry::~WeatherDataEntry()
{

}

WeatherDataEntry::WeatherDataEntry(const WeatherDataEntry &other)
    : d(other.d)
{
}

WeatherDataEntry::WeatherDataEntry(WeatherDataEntry &&other) noexcept
    : d(other.d)
{
    other.d = Q_NULLPTR;
}

WeatherDataEntry &WeatherDataEntry::operator=(const WeatherDataEntry &other)
{
    // copy+swap idiom, exception safe
    WeatherDataEntry copy(other);
    using namespace std;
    swap(copy.d, d);
    return *this;
}

WeatherDataEntry &WeatherDataEntry::operator=(WeatherDataEntry &&other)
{
    d = other.d;
    other.d = Q_NULLPTR;
    return *this;
}

void WeatherDataEntry::setTemperatureWindRain(const QDateTime &dateTime, int celsius, double average_wind, double gust_wind, int wind_direction, double mm_rain)
{
    WeatherDataEntryPrivate *dp = d; // do the detach logic only once
    dp->dateTime = dateTime;
    dp->temperature_celsius = celsius;
    dp->average_wind = average_wind;
    dp->gust_wind = gust_wind;
    dp->wind_direction = wind_direction;
    dp->mm_rain = mm_rain;
    // TODO: missing weather_icon
}

void WeatherDataEntry::setDateTime(const QDateTime &dateTime)
{
    d->dateTime = dateTime;
}

void WeatherDataEntry::setTemperature(int temperature_celsius)
{
    d->temperature_celsius = temperature_celsius;
}

void WeatherDataEntry::setWeatherIcon(const QString &icon)
{
    d->weather_icon = icon;
}

QString WeatherDataEntry::toString() const
{
    const WeatherDataEntryPrivate *dp = d;
    QString str = dp->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz t"))
            + ", temp=" + QString::number(dp->temperature_celsius)
            + ", wind=(" + QString::number(dp->average_wind) + ", " + QString::number(dp->gust_wind)
            + ", " + QString::number(dp->wind_direction) + "), rain=" + QString::number(dp->mm_rain);
    if (!dp->weather_icon.isEmpty())
        str += ", icon=" + dp->weather_icon;
    return str;
}

QDateTime WeatherDataEntry::dateTime() const
{
    return d->dateTime;
}

int WeatherDataEntry::temperature_celsius() const
{
    return d->temperature_celsius;
}

int WeatherDataEntry::average_wind() const
{
    return d->average_wind;
}

int WeatherDataEntry::gust_wind() const
{
    return d->gust_wind;
}

int WeatherDataEntry::wind_direction() const
{
    return d->wind_direction;
}

double WeatherDataEntry::mm_rain() const
{
    return d->mm_rain;
}

QString WeatherDataEntry::weather_icon() const
{
    return d->weather_icon;
}

void WeatherData::merge(const QVector<WeatherDataEntry> &vec)
{
    if (vec.isEmpty()) {
        return;
    }
    if (m_data.isEmpty()) {
        m_data = vec;
    } else {
        // Assumption: m_data has older data than vec
        // Set insertIdx to where vec should go to.
        const QDateTime newStart = vec.at(0).dateTime();
        int insertIdx = m_data.count();
        for (int i = 0; i < m_data.count(); ++i) {
            const QDateTime existingStart = m_data.at(i).dateTime();
            if (existingStart >= newStart) {
                insertIdx = i;
                break;
            }
        }
        m_data.resize(insertIdx);
        m_data += vec;
    }
}
