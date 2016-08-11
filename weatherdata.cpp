#include "weatherdata.h"

#include <QDateTime>

class WeatherDataPrivate : public QSharedData
{
public:
    WeatherDataPrivate()
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

WeatherData::WeatherData()
    : d(new WeatherDataPrivate)
{
}

WeatherData::~WeatherData()
{

}

WeatherData::WeatherData(const WeatherData &other)
    : d(other.d)
{
}

WeatherData::WeatherData(WeatherData &&other) noexcept
    : d(other.d)
{
    other.d = Q_NULLPTR;
}

WeatherData &WeatherData::operator=(const WeatherData &other)
{
    // copy+swap idiom, exception safe
    WeatherData copy(other);
    using namespace std;
    swap(copy.d, d);
    return *this;
}

WeatherData &WeatherData::operator=(WeatherData &&other)
{
    d = other.d;
    other.d = Q_NULLPTR;
    return *this;
}

void WeatherData::setTemperatureWindRain(const QDateTime &dateTime, int celsius, double average_wind, double gust_wind, int wind_direction, double mm_rain)
{
    WeatherDataPrivate *dp = d; // do the detach logic only once
    dp->dateTime = dateTime;
    dp->temperature_celsius = celsius;
    dp->average_wind = average_wind;
    dp->gust_wind = gust_wind;
    dp->wind_direction = wind_direction;
    dp->mm_rain = mm_rain;
    // TODO: missing weather_icon
}

void WeatherData::setDateTime(const QDateTime &dateTime)
{
    d->dateTime = dateTime;
}

void WeatherData::setTemperature(int temperature_celsius)
{
    d->temperature_celsius = temperature_celsius;
}

void WeatherData::setWeatherIcon(const QString &icon)
{
    d->weather_icon = icon;
}

QString WeatherData::toString() const
{
    const WeatherDataPrivate *dp = d;
    QString str = dp->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz t"))
            + ", temp=" + QString::number(dp->temperature_celsius)
            + ", wind=(" + QString::number(dp->average_wind) + ", " + QString::number(dp->gust_wind)
            + ", " + QString::number(dp->wind_direction) + "), rain=" + QString::number(dp->mm_rain);
    if (!dp->weather_icon.isEmpty())
        str += ", icon=" + dp->weather_icon;
    return str;
}

QDateTime WeatherData::dateTime() const
{
    return d->dateTime;
}

int WeatherData::temperature_celsius() const
{
    return d->temperature_celsius;
}

int WeatherData::average_wind() const
{
    return d->average_wind;
}

int WeatherData::gust_wind() const
{
    return d->gust_wind;
}

int WeatherData::wind_direction() const
{
    return d->wind_direction;
}

double WeatherData::mm_rain() const
{
    return d->mm_rain;
}

QString WeatherData::weather_icon() const
{
    return d->weather_icon;
}
