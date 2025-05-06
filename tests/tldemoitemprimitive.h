#pragma once

#include "qmtlitemprimitive.h"

struct TLDemoItemPrimitivePrivate;

class TLDemoItemPrimitive : public QmTLItemPrimitive {
    Q_OBJECT
public:
    explicit TLDemoItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene);
    ~TLDemoItemPrimitive() noexcept override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void fitInAxis() override;

private:
    void drawDelay(QPainter* painter);

    QRectF calcBoundingRect() const;

private:
    TLDemoItemPrimitivePrivate* d_ { nullptr };
};
