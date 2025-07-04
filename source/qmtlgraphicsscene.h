#pragma once

#include "qmtimeline_global.h"
#include "qmtlitemdata.h"
#include "qmtltypedef.h"
#include <QGraphicsScene>

class QmTLGraphicsModel;
class QmTLGraphicsView;
class QmTLItemPrimitive;
class QmTLItemRegistry;

struct QmTLGraphicsScenePrivate;

class QMTIMELINE_EXPORT QmTLGraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit QmTLGraphicsScene(QObject* parent = nullptr);
    ~QmTLGraphicsScene() noexcept override;

    virtual void fitInAxis();

    virtual void setModel(QmTLGraphicsModel* model);
    QmTLGraphicsModel* model() const;

    void setView(QmTLGraphicsView* view);
    QmTLGraphicsView* view() const;

    qreal mapToAxis(qint64 time_key) const;
    qreal mapToAxisX(qint64 time_key) const;
    qreal axisTickPixels() const;

    void setScaleFactorRange(qreal min, qreal max);

    void updateItem(QmTLItemID item_id);
    QmTLItemPrimitive* graphItem(QmTLItemID item_id) const;

signals:
    void requestScaleAxis(bool zoom_in);
    void requestSceneContextMenu();
    void requestItemContextMenu(QmTLItemID item_id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private slots:
    void onItemCreated(QmTLItemID item_id);
    void onItemAboutToBeRemoved(QmTLItemID item_id);
    void onItemChanged(QmTLItemID item_id, QmTLItemDataRoles roles);
    void onItemOperate(QmTLItemID item_id, QmTLItemDataRoles roles, const QVariant& param);

private:
    QmTLGraphicsScenePrivate* d_ { nullptr };
    Q_DISABLE_COPY(QmTLGraphicsScene)
};