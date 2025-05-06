#include "qmtlgraphicsscene.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsview.h"
#include "qmtlitemmodel.h"
#include "qmtlitemregistry.h"
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

struct QmTLGraphicsScenePrivate {
    qreal row_height = 30;
    qreal row_spacing = 5;
    qreal scale_factor = 1.0;
    QmTLGraphicsView* view { nullptr };
    std::pair<qreal, qreal> scale_factor_range { 0.0, 30.0 };
    QmTLGraphicsModel* model { nullptr };
    QList<QMetaObject::Connection> model_signals;
    std::map<QmTLItemID, std::unique_ptr<QmTLItemPrimitive>> items;
};

QmTLGraphicsScene::QmTLGraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
    , d_(new QmTLGraphicsScenePrivate)
{
    // TODO: 后期需要根据Axis Timekey的最大值来确定宽度
    setSceneRect(0, 0, 20000, 20000);
}

QmTLGraphicsScene::~QmTLGraphicsScene() noexcept
{
    delete d_;
}

void QmTLGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
    if (!event->isAccepted()) {
        event->accept();
    }
}

void QmTLGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if (!event->isAccepted()) {
        event->accept();
    }
}

void QmTLGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    QGraphicsScene::wheelEvent(event);
    qreal scale_factor = d_->scale_factor;
    if (event->delta() > 0) {
        scale_factor += 0.1;
    } else {
        scale_factor -= 0.1;
    }
    scale_factor = qMax(d_->scale_factor_range.first, qMin(scale_factor, d_->scale_factor_range.second));
    if (!qFuzzyCompare(d_->scale_factor, scale_factor)) {
        d_->scale_factor = scale_factor;
        emit requestScaleAxis(event->delta() > 0);
    }
    event->accept();
}

void QmTLGraphicsScene::setModel(QmTLGraphicsModel* model)
{
    if (d_->model == model) {
        return;
    }

    for (const auto& conn : d_->model_signals) {
        disconnect(conn);
    }
    d_->model_signals.clear();
    d_->model = model;

    d_->model_signals.append(
        connect(d_->model, &QmTLGraphicsModel::itemCreated, this, &QmTLGraphicsScene::onItemCreated));

    d_->model_signals.append(
        connect(d_->model, &QmTLGraphicsModel::itemChanged, this, &QmTLGraphicsScene::onItemChanged));
}

QmTLGraphicsModel* QmTLGraphicsScene::model() const
{
    return d_->model;
}

void QmTLGraphicsScene::setView(QmTLGraphicsView* view)
{
    d_->view = view;
}

qreal QmTLGraphicsScene::mapToAxis(qint64 time_key) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapToAxis(time_key);
}

void QmTLGraphicsScene::onItemCreated(QmTLItemID item_id)
{
    if (!d_->model) {
        return;
    }
    if (!d_->model->itemRegistry()) [[unlikely]] {
        assert(0 && "Item registry is nullptr!");
        return;
    }
    auto* item_model = d_->model->itemModel(item_id);
    if (!item_model) {
        return;
    }
    d_->items[item_id] = std::move(d_->model->itemRegistry()->createItemPrimitive(item_model->type(), item_id, *this));
}

void QmTLGraphicsScene::onItemChanged(QmTLItemID item_id, QmTLItemDataRoles roles)
{
    auto* item = graphItem(item_id);
    if (roles.testFlag(QmTLItemDataRole::TimeKey)) {
        auto* item_model = d_->model->itemModel(item_id);
        if (!item_model) {
            return;
        }
        item->setPos(mapToAxisX(item_model->data().timeKey()), 10);
    }
}

qreal QmTLGraphicsScene::mapToAxisX(qint64 time_key) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapToAxisX(time_key);
}

qreal QmTLGraphicsScene::axisTickPixels() const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->axisTickPixels();
}

void QmTLGraphicsScene::setScaleFactorRange(qreal min, qreal max)
{
    d_->scale_factor_range.first = min;
    d_->scale_factor_range.second = max;
}

void QmTLGraphicsScene::fitInAxis()
{
    for (const auto& [_, item] : d_->items) {
        item->fitInAxis();
    }
}

qreal QmTLGraphicsScene::itemHeight(QmTLItemID item_id) const
{
    // TODO: 待实现
    return 100;
}

void QmTLGraphicsScene::updateItem(QmTLItemID item_id)
{
    auto it = d_->items.find(item_id);
    if (it != d_->items.end()) {
        it->second->update();
    }
}

QmTLItemPrimitive* QmTLGraphicsScene::graphItem(QmTLItemID item_id) const
{
    auto it = d_->items.find(item_id);
    if (it != d_->items.end()) {
        return it->second.get();
    }
    return nullptr;
}