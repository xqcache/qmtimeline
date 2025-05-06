#pragma once

#include "qmtlitemprimitive.h"

struct TLFrameItemPrimitivePrivate;

class TLFrameItemPrimitive : public QmTLItemPrimitive {
    Q_OBJECT
public:
    explicit TLFrameItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene);
    ~TLFrameItemPrimitive() noexcept override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void fitInAxis() override;

private:
    void drawDelay(QPainter* painter);

    QRectF calcBoundingRect() const;

private:
    TLFrameItemPrimitivePrivate* d_ { nullptr };
};
