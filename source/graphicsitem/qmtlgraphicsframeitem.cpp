#include "qmtlgraphicsframeitem.h"
#include "itemmodel/qmtlframeitemmodel.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"
#include <QPainter>

struct QmTLGraphicsFrameItemPrivate { };

QmTLGraphicsFrameItem::QmTLGraphicsFrameItem(QmTLGraphicsScene& scene, QmTLItemID item_id)
    : QmTLGraphicsItem(scene, item_id)
    , d_(new QmTLGraphicsFrameItemPrivate)
{
}

QmTLGraphicsFrameItem::~QmTLGraphicsFrameItem() noexcept
{
    delete d_;
}

QRectF QmTLGraphicsFrameItem::boundingRect() const
{
    if (!graphModel()) [[unlikely]] {
        return {};
    }
    auto* item_model = static_cast<QmTLFrameItemModel*>(graphModel()->itemModel(item_id_));
    if (!item_model) [[unlikely]] {
        return {};
    }
    auto delay = static_cast<QmTLFrameItemData&>(item_model->data()).delay();

    return QRectF(0, 0, delay.has_value() ? graphScene().mapToAxis(*delay) : 40, 100);
}

void QmTLGraphicsFrameItem::fitInAxis()
{
    if (!graphModel()) [[unlikely]] {
        return;
    }
    auto* item_model = static_cast<QmTLFrameItemModel*>(graphModel()->itemModel(item_id_));
    if (!item_model) [[unlikely]] {
        return;
    }
    setX(graphScene().mapToAxisX(item_model->data().timeKey()));
}

void QmTLGraphicsFrameItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (!graphModel()) [[unlikely]] {
        return;
    }
    auto* item_model = static_cast<QmTLFrameItemModel*>(graphModel()->itemModel(item_id_));
    if (!item_model) [[unlikely]] {
        return;
    }

    painter->fillRect(boundingRect(), QBrush(Qt::blue));
}