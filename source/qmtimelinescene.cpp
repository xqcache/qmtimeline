#include "qmtimelinescene.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemconnview.h"
#include "qmtimelineitemfactory.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelineitemview.h"
#include "qmtimelinetype.h"
#include "qmtimelineview.h"
#include <QGraphicsSceneContextMenuEvent>
#include <QUndoStack>

namespace qmtl {
struct QmTimelineScenePrivate {
    QmTimelineView* view { nullptr };
    QmTimelineItemModel* model { nullptr };
    QUndoStack* undo_stack { nullptr };
    std::unordered_map<QmItemID, std::unique_ptr<QmTimelineItemView>> item_views;
    std::unordered_map<QmItemConnID, std::unique_ptr<QmTimelineItemConnView>, QmItemConnIDHash, QmItemConnIDEqual> item_conn_views;
};

QmTimelineScene::QmTimelineScene(QmTimelineItemModel* model, QObject* parent)
    : QGraphicsScene(parent)
    , d_(new QmTimelineScenePrivate)
{
    d_->undo_stack = new QUndoStack(this);
    d_->model = model;
    connect(model, &QmTimelineItemModel::itemCreated, this, &QmTimelineScene::onItemCreated);
    connect(model, &QmTimelineItemModel::itemChanged, this, &QmTimelineScene::onItemChanged);
    connect(model, &QmTimelineItemModel::itemRemoved, this, &QmTimelineScene::onItemRemoved);
    connect(model, &QmTimelineItemModel::itemOperateFinished, this, &QmTimelineScene::onItemOperateFinished);
    connect(model, &QmTimelineItemModel::requestUpdateItemY, this, &QmTimelineScene::onUpdateItemYRequested);

    connect(model, &QmTimelineItemModel::itemConnCreated, this, &QmTimelineScene::onItemConnCreated);
    connect(model, &QmTimelineItemModel::itemConnRemoved, this, &QmTimelineScene::onItemConnRemoved);
    connect(model, &QmTimelineItemModel::requestRefreshItemViewCache, this, &QmTimelineScene::onRefreshItemViewCacheRequested);
    connect(model, &QmTimelineItemModel::requestRebuildItemViewCache, this, &QmTimelineScene::onRebuildItemViewCacheRequested);
}

QmTimelineScene::~QmTimelineScene() noexcept
{
    delete d_;
}

void QmTimelineScene::setView(QmTimelineView* view)
{
    d_->view = view;
}

QmTimelineItemModel* QmTimelineScene::model() const
{
    return d_->model;
}

QmTimelineItemView* QmTimelineScene::itemView(QmItemID item_id) const
{
    auto it = d_->item_views.find(item_id);
    if (it != d_->item_views.end()) {
        return it->second.get();
    }
    return nullptr;
}

QmTimelineItemConnView* QmTimelineScene::itemConnView(const QmItemConnID& conn_id) const
{
    auto it = d_->item_conn_views.find(conn_id);
    if (it != d_->item_conn_views.end()) {
        return it->second.get();
    }
    return nullptr;
}

qreal QmTimelineScene::itemConnViewWidth(const QmItemConnID& conn_id) const
{
    auto it = d_->item_conn_views.find(conn_id);
    if (it == d_->item_conn_views.end()) {
        return 0;
    }

    auto* from_item = model()->item(conn_id.from);
    auto* to_item = model()->item(conn_id.to);
    if (!from_item || !to_item) {
        return 0;
    }

    qint64 from_dest = from_item->destination();
    qint64 to_start = to_item->start();
    return qMax(0.0, mapFrameToAxis(to_start - from_dest) - axisTickWidth());
}

qreal QmTimelineScene::mapFrameToAxis(qint64 time) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapFrameToAxis(time);
}

qreal QmTimelineScene::mapFrameToAxisX(qint64 time) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapFrameToAxisX(time);
}

qreal QmTimelineScene::axisTickWidth() const
{
    if (!d_->view) {
        return 1.0;
    }
    return 40;
}

qreal QmTimelineScene::axisFramePixels() const
{
    if (!d_->view) {
        return 1.0;
    }
    return d_->view->axisFramePixels();
}

void QmTimelineScene::fitInAxis()
{
    for (const auto& [_, item] : d_->item_views) {
        item->fitInAxis();
    }

    for (const auto& [_, conn] : d_->item_conn_views) {
        conn->fitInAxis();
    }
}

void QmTimelineScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    // 将事件传输给GraphicsItem
    QPointF pos = event->scenePos();
    auto* item = itemAt(pos, QTransform());
    if (!item) {
        emit requestSceneContextMenu();
        return;
    }
    auto* object = item->toGraphicsObject();
    if (!object) {
        emit requestSceneContextMenu();
        return;
    }

    if (object->type() == QmTimelineItemView::Type) {
        auto* item_view = static_cast<QmTimelineItemView*>(object);
        clearSelection();
        item_view->setSelected(true);
        emit requestItemContextMenu(item_view->itemId());
        return;
    }
    emit requestSceneContextMenu();
}

void QmTimelineScene::onItemCreated(QmItemID item_id)
{
    auto item_view = QmTimelineItemFactory::instance().createItemView(item_id, this);
    connect(item_view.get(), &QmTimelineItemView::requestMove, this, &QmTimelineScene::requestMoveItem);
    connect(item_view.get(), &QmTimelineItemView::moveFinished, this, &QmTimelineScene::itemMoveFinished);
    d_->item_views[item_id] = std::move(item_view);
}

void QmTimelineScene::onItemChanged(QmItemID item_id, int role)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->onItemChanged(role);

    // 尝试更新item之间的连接线
    if (role & QmTimelineItem::StartRole) {
        auto prev_conn_id = model()->previousConnection(item_id);
        if (prev_conn_id.isValid()) {
            auto* prev_conn_view = itemConnView(prev_conn_id);
            if (prev_conn_view) {
                prev_conn_view->updateX();
            }
        }
    }
    if (role & (QmTimelineItem::DurationRole | QmTimelineItem::StartRole)) {
        auto next_conn_id = model()->nextConnection(item_id);
        if (next_conn_id.isValid()) {
            auto* next_conn_view = itemConnView(next_conn_id);
            if (next_conn_view) {
                next_conn_view->updateX();
            }
        }
    }
}

void QmTimelineScene::onItemRemoved(QmItemID item_id)
{
    auto item_it = d_->item_views.find(item_id);
    if (item_it == d_->item_views.end()) {
        return;
    }
    d_->item_views.erase(item_it);
}

void QmTimelineScene::onUpdateItemYRequested(QmItemID item_id)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->updateY();
}

void QmTimelineScene::onItemConnCreated(const QmItemConnID& conn_id)
{

    auto* item_view = itemView(conn_id.from);
    if (!item_view) {
        return;
    }
    auto conn_item = new QmTimelineItemConnView(conn_id, *this);
    connect(item_view, &QGraphicsObject::yChanged, conn_item, [conn_item, item_view] { conn_item->setY(item_view->y()); });
    d_->item_conn_views[conn_id].reset(conn_item);
}

void QmTimelineScene::onItemConnRemoved(const QmItemConnID& conn_id)
{
    auto it = d_->item_conn_views.find(conn_id);
    if (it != d_->item_conn_views.end()) {
        d_->item_conn_views.erase(it);
    }
}

void QmTimelineScene::onItemOperateFinished(QmItemID item_id, int role, const QVariant& param)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->onItemOperateFinished(role, param);
}

QmTimelineView* QmTimelineScene::view() const
{
    return d_->view;
}

QList<QmItemID> QmTimelineScene::selectedItems() const
{
    QList<QmItemID> ids;
    for (const auto& item : QGraphicsScene::selectedItems()) {
        if (item->type() == QmTimelineItemView::Type) {
            ids.append(static_cast<QmTimelineItemView*>(item)->itemId());
        }
    }
    return ids;
}

void QmTimelineScene::refreshCache()
{
    for (const auto& [_, item] : d_->item_views) {
        item->refreshCache();
    }
}

void QmTimelineScene::onRefreshItemViewCacheRequested(QmItemID item_id)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->refreshCache();
}

void QmTimelineScene::onRebuildItemViewCacheRequested(QmItemID item_id)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->rebuildCache();
}

void QmTimelineScene::recordUndo(QUndoCommand* command)
{
    d_->undo_stack->push(command);
}

void QmTimelineScene::undo()
{
    if (d_->undo_stack->canUndo()) {
        d_->undo_stack->undo();
    }
}

void QmTimelineScene::redo()
{
    if (d_->undo_stack->canRedo()) {
        d_->undo_stack->redo();
    }
}

QUndoStack* QmTimelineScene::undoStack() const
{
    return d_->undo_stack;
}
} // namespace qmtl
