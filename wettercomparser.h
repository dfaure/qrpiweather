#ifndef WETTERCOMPARSER_H
#define WETTERCOMPARSER_H

#include "weatherparserbase.h"

class WetterComParser : public WeatherParserBase
{
public:
    WetterComParser();
    ~WetterComParser();

    QUrl url() const;
    QVector<WeatherData> parse(const QByteArray &data);
};

#endif // WETTERCOMPARSER_H
