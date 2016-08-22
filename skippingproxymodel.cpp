#include "skippingproxymodel.h"
#include "weathermodel.h"
#include <QDebug>

SkippingProxyModel::SkippingProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool SkippingProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex source_idx = sourceModel()->index(source_row, 0, source_parent);
    const int hour = source_idx.data(WeatherModel::Time).toInt();
    return hour == 15; // assumption: there is data for 15h every day. true for OpenWeatherMap...
}

int SkippingProxyModel::indexForCurrentDateTime() const
{
    const QDateTime dt = WeatherModel::currentDateTime();
    const int proxyRowCount = rowCount();
    for (int proxyRow = 0; proxyRow < proxyRowCount; ++proxyRow) {
        const QModelIndex proxyIdx = index(proxyRow, 0);
        if (proxyIdx.data(WeatherModel::DateTime).toDateTime() >= dt) {
            qDebug() << "(proxy) scrolling to" << proxyRow;
            return proxyRow;
        }
    }
    qDebug() << "(proxy) indexForCurrentDateTime() not found !?";
    return -1;
}
