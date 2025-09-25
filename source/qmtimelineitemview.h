#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QGraphicsObject>

namespace qmtl {

class QmTimelineScene;
class QmTimelineItemModel;
class QMTIMELINE_LIB_EXPORT QmTimelineItemView : public QGraphicsObject {
    Q_OBJECT
public:
    QmTimelineItemView(QmItemID item_id, QmTimelineScene* scene);

    enum {
        Type = UserType + 1,
    };
    int type() const override;

    QRectF boundingRect() const override;
    qreal itemMargin() const;

    inline QmItemID itemId() const;

    QmTimelineItemModel* model() const;
    QmTimelineScene& sceneRef();
    const QmTimelineScene& sceneRef() const;

    virtual void updateX();
    virtual void updateY();
    virtual bool isInView() const;

    virtual void fitInAxis();
    virtual void refreshCache();
    virtual void rebuildCache();

    virtual bool onItemChanged(int role);
    virtual bool onItemOperateFinished(int op_role, const QVariant& param);

signals:
    void requestMove(QmItemID item_id, qint64 new_start);
    void moveFinished(QmItemID item_id, qint64 old_start);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

protected:
    virtual QRectF calcBoundingRect() const;

protected:
    qint64 start_bak_ { -1 };
    QmItemID item_id_ { kInvalidItemID };
    mutable QRectF bounding_rect_;
};

inline QmItemID QmTimelineItemView::itemId() const
{
    return item_id_;
}

} // namespace qmtl
