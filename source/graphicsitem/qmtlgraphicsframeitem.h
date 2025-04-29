#pragma once

#include "graphicsitem/qmtlgraphcisitem.h"

struct QmTLGraphicsFrameItemPrivate;

class QmTLGraphicsFrameItem : public QmTLGraphicsItem {
    Q_OBJECT
public:
    explicit QmTLGraphicsFrameItem(QmTLGraphicsScene& scene, QmTLItemID item_id);
    ~QmTLGraphicsFrameItem() noexcept override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void fitInAxis() override;

private:
    QmTLGraphicsFrameItemPrivate* d_ { nullptr };
};
