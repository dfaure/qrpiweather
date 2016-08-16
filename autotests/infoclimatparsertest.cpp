#include <QObject>
#include <QDebug>

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

    InfoClimatParser parser;
    QVector<WeatherData> wdlist = parser.parse(data);
    QCOMPARE(wdlist.count(), 64);
    QCOMPARE(wdlist.last().toString(), QStringLiteral("2016-08-12 08:00:00.000 CEST, temp=14, wind=(32, 58, 355), rain=0"));
}

QTEST_MAIN(InfoClimatParserTest)

#include "infoclimatparsertest.moc"
