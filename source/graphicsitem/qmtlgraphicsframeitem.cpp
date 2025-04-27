#include "qmtlgraphicsframeitem.h"

struct QmTLGraphicsFrameItemPrivate { };

QmTLGraphicsFrameItem::QmTLGraphicsFrameItem(QmTLGraphicsScene& scene)
    : QmTLGraphicsItem(scene)
    , d_(new QmTLGraphicsFrameItemPrivate)
{
}

QmTLGraphicsFrameItem::~QmTLGraphicsFrameItem() noexcept
{
    delete d_;
}

QRectF QmTLGraphicsFrameItem::boundingRect() const
{
    return QRectF();
}

void QmTLGraphicsFrameItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}