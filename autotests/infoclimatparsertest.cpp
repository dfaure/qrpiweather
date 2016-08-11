#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include <QTest>
#include <QSignalSpy>

#include "filedataprovider.h"
#include "infoclimatparser.h"

class InfoClimatParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fileDataProviderShouldProvideData();
    void parserShouldFetchDatesAndData();

private:
    void fetchData(QByteArray *data);
};

void InfoClimatParserTest::fetchData(QByteArray *data)
{
    DataProvider::Ptr provider(new FileDataProvider(QFINDTESTDATA("infoclimat.json")));
    provider->ensureDataAvailable();
    QSignalSpy spyError(provider.data(), &DataProvider::error);
    QSignalSpy spyDataAvailable(provider.data(), &DataProvider::dataAvailable);

    QVERIFY(spyDataAvailable.wait(1000));
    QVERIFY(spyError.isEmpty());
    *data = spyDataAvailable.at(0).at(0).value<QByteArray>();
}

void InfoClimatParserTest::fileDataProviderShouldProvideData()
{
    QByteArray data;
    fetchData(&data);
}

void InfoClimatParserTest::parserShouldFetchDatesAndData()
{
    QByteArray data;
    fetchData(&data);

    QString output;
    QTextStream stream(&output);
    InfoClimatParser parser(stream);
    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    QVERIFY2(!doc.isNull(), qPrintable("Error parsing JSON document: " + jsonError.errorString() + " at offset " + jsonError.offset));

    QVector<WeatherData> wdlist = parser.parse(doc);
    QCOMPARE(wdlist.count(), 64);
    QCOMPARE(wdlist.last().toString(), QStringLiteral("2016-08-12 08:00:00.000 CEST, temp=14, wind=(32, 58, 355), rain=0"));
    QVERIFY2(output.isEmpty(), qPrintable(output));
}

QTEST_MAIN(InfoClimatParserTest)

#include "infoclimatparsertest.moc"
