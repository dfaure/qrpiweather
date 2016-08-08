#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include <QTest>
#include <QSignalSpy>

#include "filedataprovider.h"
#include "infoclimatparser.h"

class TestFileData : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fileDataProviderShouldProvideData();
    void parserShouldFetchDatesAndData();

private:
    void fetchData(QJsonDocument *doc);
};

void TestFileData::fetchData(QJsonDocument *doc)
{
    DataProvider::Ptr provider(new FileDataProvider(QFINDTESTDATA("infoclimat.json")));
    provider->ensureDataAvailable();
    QSignalSpy spyError(provider.data(), SIGNAL(error()));
    QSignalSpy spyDataAvailable(provider.data(), SIGNAL(dataAvailable(QJsonDocument)));

    QVERIFY(spyDataAvailable.wait(1000));
    QVERIFY(spyError.isEmpty());
    *doc = spyDataAvailable.at(0).at(0).value<QJsonDocument> ();
    QCOMPARE(doc->object().size(), 69);
}

void TestFileData::fileDataProviderShouldProvideData()
{
    QJsonDocument doc;
    fetchData(&doc);
}

void TestFileData::parserShouldFetchDatesAndData()
{
    QJsonDocument doc;
    fetchData(&doc);

    QString output;
    QTextStream stream(&output);
    InfoClimatParser parser(stream);
    QVector<WeatherData> wdlist = parser.parse(doc);
    QCOMPARE(wdlist.count(), 64);
    QCOMPARE(wdlist.last().toString(), QStringLiteral("2016-08-12 08:00:00.000 CEST, temp=14, wind=(31.5, 58.3, 355), rain=0"));
    QVERIFY2(output.isEmpty(), qPrintable(output));
}

QTEST_MAIN(TestFileData)

#include "filedatatest.moc"
