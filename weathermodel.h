#ifndef WEATHERMODEL_H
#define WEATHERMODEL_H

#include <QAbstractTableModel>
#include "dataprovider.h"
#include "weatherdata.h"
class QJsonDocument;

class WeatherModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit WeatherModel(QObject *parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private slots:
    void slotDataAvailable(const QJsonDocument &doc);
    void slotError();

private:
    void fetchData();

    QVector<WeatherData> m_data;

    DataProvider::Ptr m_provider;
};

#endif // WEATHERMODEL_H
