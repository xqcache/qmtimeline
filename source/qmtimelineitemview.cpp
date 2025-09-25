#include "qmtimelineitemview.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelinescene.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

namespace qmtl {

QmTimelineItemView::QmTimelineItemView(QmItemID item_id, QmTimelineScene* scene)
    : item_id_(item_id)
{
    scene->addItem(this);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(0);
    bounding_rect_ = calcBoundingRect();
    updateX();
    updateY();
    setToolTip(model()->item(item_id)->toolTip());
    {
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
        effect->setColor(Qt::black);
        effect->setBlurRadius(20);
        effect->setOffset(0);
        setGraphicsEffect(effect);
    }
}

void QmTimelineItemView::fitInAxis()
{
    auto* scene = qobject_cast<QmTimelineScene*>(this->scene());
    if (!scene->model()) [[unlikely]] {
        return;
    }
    auto* item = static_cast<QmTimelineItem*>(scene->model()->item(item_id_));
    if (!item) [[unlikely]] {
        return;
    }
    bounding_rect_ = calcBoundingRect();
    qreal x = scene->mapFrameToAxisX(item->start());

    if (isInView()) {
        prepareGeometryChange();
    }

    if (!qFuzzyCompare(x, this->x())) {
        setX(x);
    }
}

QRectF QmTimelineItemView::boundingRect() const
{
    return bounding_rect_;
}

qreal QmTimelineItemView::itemMargin() const
{
    return qMin(bounding_rect_.width(), bounding_rect_.height()) * 0.1;
}

QmTimelineItemModel* QmTimelineItemView::model() const
{
    return qobject_cast<QmTimelineScene*>(scene())->model();
}

QmTimelineScene& QmTimelineItemView::sceneRef()
{
    return *qobject_cast<QmTimelineScene*>(scene());
}

const QmTimelineScene& QmTimelineItemView::sceneRef() const
{
    return *qobject_cast<const QmTimelineScene*>(scene());
}

QRectF QmTimelineItemView::calcBoundingRect() const
{
    QRectF result;
    if (!model()) [[unlikely]] {
        return result;
    }
    auto* item = model()->item(item_id_);
    if (!item) [[unlikely]] {
        return result;
    }

    if (!isInView()) {
        return result;
    }

    auto duration = item->duration();
    qreal tick_width = sceneRef().axisTickWidth();
    qreal width = duration > 0 ? sceneRef().mapFrameToAxis(duration) + tick_width : tick_width;
    qreal height = model()->itemHeight();
    result = QRectF(-tick_width / 2.0, 0, width, height);
    return result;
}

void QmTimelineItemView::updateX()
{
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    auto new_x = sceneRef().mapFrameToAxisX(item->start());
    if (!qFuzzyCompare(new_x, x())) {
        setX(new_x);
    }
}

void QmTimelineItemView::updateY()
{
    qreal new_y = model()->itemY(item_id_);
    if (qFuzzyCompare(new_y, y())) {
        return;
    }
    prepareGeometryChange();
    setY(new_y);
}

bool QmTimelineItemView::onItemChanged(int role)
{
    bool processed = false;
    if (role & QmTimelineItem::StartRole) {
        updateX();
        processed = true;
    }

    if (role & QmTimelineItem::DurationRole) {
        bounding_rect_ = calcBoundingRect();
        prepareGeometryChange();
        update();
        processed = true;
    }

    if (role & QmTimelineItem::NumberRole) {
        update();
        processed = true;
    }

    if (role & QmTimelineItem::EnabledRole) {
        setEnabled(model()->item(item_id_)->isEnabled());
        processed = true;
    }

    if (role & QmTimelineItem::ToolTipRole) {
        setToolTip(model()->item(item_id_)->toolTip());
        processed = true;
    }

    return processed;
}

bool QmTimelineItemView::onItemOperateFinished(int op_role, const QVariant& param)
{
    return false;
}

int QmTimelineItemView::type() const
{
    return Type;
}

bool QmTimelineItemView::isInView() const
{
    return model()->isItemInViewRange(item_id_);
}

void QmTimelineItemView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mousePressEvent(event);
    start_bak_ = model()->item(item_id_)->start();
}

void QmTimelineItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    if (start_bak_ < 0) {
        return;
    }
    auto guard = qScopeGuard([this] { start_bak_ = -1; });
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    if (start_bak_ == item->start()) {
        return;
    }
    emit moveFinished(item_id_, start_bak_);
}

void QmTimelineItemView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    qint64 new_start = qRound64(event->pos().x() / sceneRef().axisFrameWidth() + item->start());
    if (new_start < model()->viewFrameMinimum()) {
        new_start = model()->viewFrameMinimum();
    }
    if (new_start > model()->viewFrameMaximum() - item->duration()) {
        new_start = model()->viewFrameMaximum() - item->duration();
    }
    if (new_start == item->start()) {
        return;
    }
    emit requestMove(item_id_, new_start);
}

void QmTimelineItemView::refreshCache()
{
}

void QmTimelineItemView::rebuildCache()
{
}

} // namespace qmtl