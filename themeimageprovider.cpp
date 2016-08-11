#include "themeimageprovider.h"

#include <QIcon>
#include <QDebug>

ThemeImageProvider::ThemeImageProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{
}

QPixmap ThemeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QIcon icon = QIcon::fromTheme(id);
    *size = QSize(64, 64);
    QPixmap pix = icon.pixmap(*size);
    qDebug() << id << "isNull:" << icon.isNull() << "requested" << requestedSize << "pix size" << pix.size();
    return pix;
}
