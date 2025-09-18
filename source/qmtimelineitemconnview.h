#pragma once

#include "qmtimelinetype.h"
#include <QFontMetricsF>
#include <QGraphicsObject>

namespace qmtl {

class QmTimelineScene;
class QmTimelineItemConnView : public QGraphicsObject {
    Q_OBJECT
public:
    explicit QmTimelineItemConnView(const QmItemConnID& conn_id, QmTimelineScene& scene);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QRectF boundingRect() const override;

    void fitInAxis();
    void updateX();
    void updateY();
    void updateGeometry();

public:
    enum {
        Type = UserType + 100
    };

    int type() const override;

private:
    QRectF calcBoundingRect() const;

private:
    QmItemConnID conn_id_;
    QmTimelineScene& scene_;
    QFontMetricsF font_metrics_;
};

} // namespace qmtl