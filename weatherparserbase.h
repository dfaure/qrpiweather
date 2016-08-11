#ifndef WEATHERPARSERBASE_H
#define WEATHERPARSERBASE_H

#include "weatherdata.h"
#include <QUrl>
#include <QVector>

class WeatherParserBase
{
public:
    WeatherParserBase();
    virtual ~WeatherParserBase();

    virtual QUrl url() const = 0;
    virtual QString cacheFileName() const = 0;
    virtual QString backendName() const = 0;
    virtual QVector<WeatherData> parse(const QByteArray &data) = 0;
};

#endif // WEATHERPARSERBASE_H
