#ifndef WETTERCOMPARSER_H
#define WETTERCOMPARSER_H

#include "weatherparserbase.h"

/**
 * The same backend as the plasma applet.
 *
 * Problem: no wind data supplied, even though the API docs say that is supported
 * (keys "wd" and "ws" are just not present).
 */
class WetterComParser : public WeatherParserBase
{
public:
    WetterComParser();
    ~WetterComParser();

    QUrl url() const Q_DECL_OVERRIDE;
    QString cacheFileName() const Q_DECL_OVERRIDE;
    QString backendName() const Q_DECL_OVERRIDE;
    QVector<WeatherData> parse(const QByteArray &data) Q_DECL_OVERRIDE;
};

#endif // WETTERCOMPARSER_H
