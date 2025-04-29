#pragma once

#include "qmtltypedef.h"
#include <QGraphicsScene>

class QmTLGraphicsModel;
class QmTLGraphicsView;
class QmTLGraphicsItem;

struct QmTLGraphicsScenePrivate;

class QmTLGraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit QmTLGraphicsScene(QObject* parent = nullptr);
    ~QmTLGraphicsScene() noexcept override;

    void setModel(QmTLGraphicsModel* model);
    QmTLGraphicsModel* model() const;

    void setView(QmTLGraphicsView* view);

    qreal mapToAxis(qint64 time_key) const;
    qreal mapToAxisX(qint64 time_key) const;
    qreal axisTickPixels() const;

    void setScaleFactorRange(qreal min, qreal max);

    void fitInAxis();
    void updateItem(QmTLItemID item_id);
    QmTLGraphicsItem* graphItem(QmTLItemID item_id) const;

signals:
    void requestScaleAxis(bool zoom_in);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private slots:
    void onItemCreated(QmTLItemID item_id);
    void onItemChanged(QmTLItemID item_id, QmTLItemDataRoles roles);

private:
    QmTLGraphicsScenePrivate* d_ { nullptr };
    Q_DISABLE_COPY(QmTLGraphicsScene)
};