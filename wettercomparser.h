#ifndef WETTERCOMPARSER_H
#define WETTERCOMPARSER_H

#include "weatherdata.h"

class QTextStream;

class WetterComParser
{
public:
    WetterComParser(QTextStream &stream);
    ~WetterComParser();

    QVector<WeatherData> parse(const QByteArray &data);

private:
    QTextStream &m_errorStream;
};

#endif // WETTERCOMPARSER_H
