#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QGraphicsScene>

class QUndoCommand;
class QUndoStack;

namespace qmtl {

class QmTimelineView;
class QmTimelineItemModel;
class QmTimelineItemView;
class QmTimelineItemConnView;
struct QmTimelineScenePrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit QmTimelineScene(QmTimelineItemModel* model, QObject* parent = nullptr);
    ~QmTimelineScene() noexcept override;

    QmTimelineView* view() const;
    void setView(QmTimelineView* view);
    QmTimelineItemModel* model() const;

    QmTimelineItemView* itemView(QmItemID item_id) const;
    QmTimelineItemConnView* itemConnView(const QmItemConnID& conn_id) const;
    qreal itemConnViewWidth(const QmItemConnID& conn_id) const;

    qreal mapFrameToAxis(qint64 time) const;
    qreal mapFrameToAxisX(qint64 time) const;
    qreal axisToSceneX(qreal x) const;
    qreal axisTickWidth() const;
    qreal axisFramePixels() const;

    QList<QmItemID> selectedItems() const;

    void fitInAxis();

    void refreshCache();
    void undo();
    void redo();
    void recordUndo(QUndoCommand* command);
    QUndoStack* undoStack() const;
    void setUndoStack(QUndoStack* stack);

signals:
    void requestSceneContextMenu();
    void requestItemContextMenu(QmItemID item_id);
    void requestMoveItem(QmItemID item_id, qint64 start);
    void itemMoveFinished(QmItemID item_id, qint64 old_start);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
    void onItemCreated(QmItemID item_id);
    void onItemChanged(QmItemID item_id, int role);
    void onItemRemoved(QmItemID item_id);
    void onItemAboutToBeRemoved(QmItemID item_id);
    void onUpdateItemYRequested(QmItemID item_id);

    void onItemConnCreated(const QmItemConnID& conn_id);
    void onItemConnRemoved(const QmItemConnID& conn_id);

    void onRefreshItemViewCacheRequested(QmItemID item_id);
    void onRebuildItemViewCacheRequested(QmItemID item_id);

    void onItemOperateFinished(QmItemID item_id, int role, const QVariant& param);

private:
    QmTimelineScenePrivate* d_ { nullptr };
};
} // namespace qmtl