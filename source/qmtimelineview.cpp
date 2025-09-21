#include "qmtimelineview.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelinescene.h"
#include "widgets/qmtimelineaxis.h"
#include "widgets/qmtimelineranger.h"
#include "widgets/qmtimelinerangeslider.h"
#include <QMouseEvent>

namespace qmtl {

struct QmTimelineViewPrivate {
    QWidget* vbar_filler { nullptr };
    QmTimelineAxis* axis { nullptr };
    QmTimelineScene* scene { nullptr };
    QmTimelineRanger* ranger { nullptr };
    QList<QMetaObject::Connection> model_connections;
};

QmTimelineView::QmTimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new QmTimelineViewPrivate)
{

    d_->ranger = new QmTimelineRanger(this);
    initUi();
    setupSignals();
}

QmTimelineView::~QmTimelineView() noexcept
{
    delete d_;
}

void QmTimelineView::initUi()
{
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(NoAnchor);
    setResizeAnchor(NoAnchor);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->axis = new QmTimelineAxis(this);
    d_->vbar_filler = new QWidget(this);
    d_->vbar_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    addScrollBarWidget(d_->vbar_filler, Qt::AlignTop);

    setSceneSize(1000000, 200);
    setAxisPlayheadHeight(40);

    setStyleSheet(R"(
    QLineEdit {
        background-color: #2b2b2b;
        border: 1px solid #333333;
        border-radius: 3px;
        padding: 2px 4px;
        color: #ffffff;
    }
    QmTimelineRangeSlider {
        background-color: #2b2b2b;
    }
    )");
}

bool QmTimelineView::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ToolTip:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick: {
        return viewportEvent(event);
    } break;
    case QEvent::ContextMenu: {
        QContextMenuEvent* old_event = static_cast<QContextMenuEvent*>(event);
        QContextMenuEvent new_event(old_event->reason(), viewport()->mapFromGlobal(old_event->globalPos()), old_event->globalPos(), old_event->modifiers());
        return viewportEvent(&new_event);
    } break;
    default:
        break;
    }
    return QGraphicsView::event(event);
}

void QmTimelineView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    int ranger_height = d_->ranger->sizeHint().height();
    d_->axis->setGeometry(0, 0, viewport()->width(), height() - ranger_height);

    auto viewport_margins = viewportMargins();
    viewport_margins.setBottom(ranger_height);
    setViewportMargins(viewport_margins);
    d_->ranger->setGeometry(0, height() - ranger_height, width(), ranger_height);

    if (d_->scene) {
        d_->scene->fitInAxis();
    }
}

void QmTimelineView::setScene(QmTimelineScene* scene)
{
    if (!scene) {
        return;
    }
    if (scene == this->scene()) {
        return;
    }
    d_->scene = scene;
    scene->setView(this);
    QGraphicsView::setScene(scene);

    connect(d_->ranger->slider(), &QmTimelineRangeSlider::sliderReleased, scene, &QmTimelineScene::refreshCache);

    for (auto& connection : d_->model_connections) {
        disconnect(connection);
    }
    d_->model_connections.clear();

    auto* model = d_->scene->model();

    d_->model_connections.emplace_back(connect(model, &QmTimelineItemModel::viewFrameMaximumChanged, this, &QmTimelineView::onViewFrameMaximumChanged));
    d_->model_connections.emplace_back(connect(model, &QmTimelineItemModel::viewFrameMinimumChanged, this, &QmTimelineView::onViewFrameMinimumChanged));
    d_->model_connections.emplace_back(connect(model, &QmTimelineItemModel::frameMaximumChanged, this, &QmTimelineView::onFrameMaximumChanged));
    d_->model_connections.emplace_back(connect(model, &QmTimelineItemModel::frameMinimumChanged, this, &QmTimelineView::onFrameMinimumChanged));
    d_->model_connections.emplace_back(connect(model, &QmTimelineItemModel::fpsChanged, this, &QmTimelineView::onFpsChanged));

    d_->model_connections.emplace_back(
        connect(d_->ranger->slider(), &QmTimelineRangeSlider::frameRangeChanged, model, [this, model](qint64 minimum, qint64 maximum) {
            QSignalBlocker blocker(model);
            model->setFrameMaximum(maximum);
            model->setFrameMinimum(minimum);
        }));
    d_->model_connections.emplace_back(
        connect(d_->ranger->slider(), &QmTimelineRangeSlider::viewMinimumChanged, model, &QmTimelineItemModel::setViewFrameMinimum));
    d_->model_connections.emplace_back(
        connect(d_->ranger->slider(), &QmTimelineRangeSlider::viewMaximumChanged, model, &QmTimelineItemModel::setViewFrameMaximum));
    d_->model_connections.emplace_back(connect(d_->ranger, &QmTimelineRanger::fpsChanged, model, &QmTimelineItemModel::setFps));
}

void QmTimelineView::setAxisPlayheadHeight(int height)
{
    d_->vbar_filler->setFixedHeight(height);
    d_->axis->setPlayheadHeight(height);
    setViewportMargins(0, height, 0, 0);
}

QmTimelineItemModel* QmTimelineView::model() const
{
    if (!d_->scene) {
        return nullptr;
    }
    return d_->scene->model();
}

void QmTimelineView::setFrameFormat(QmFrameFormat frame_fmt)
{
    d_->ranger->setFrameFormat(frame_fmt);
    d_->axis->setFrameFormat(frame_fmt);
}

QmFrameFormat QmTimelineView::frameFormat() const
{
    return d_->axis->frameFormat();
}

void QmTimelineView::setSceneSize(qreal width, qreal height)
{
    setSceneRect(0, 0, width, height);
}

void QmTimelineView::setSceneWidth(qreal width)
{
    setSceneRect(0, 0, width, sceneRect().height());
}

void QmTimelineView::setupSignals()
{
}

void QmTimelineView::onViewFrameMaximumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    {
        QSignalBlocker blocker(d_->ranger->slider());
        d_->ranger->slider()->setViewFrameMaximum(value);
    }
    d_->axis->setMaximum(value);
    d_->scene->fitInAxis();
}

void QmTimelineView::onViewFrameMinimumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    {
        QSignalBlocker blocker(d_->ranger->slider());
        d_->ranger->slider()->setViewFrameMinimum(value);
    }
    d_->axis->setMinimum(value);
    d_->scene->fitInAxis();
}

void QmTimelineView::onFrameMaximumChanged(qint64 value)
{
    QSignalBlocker blocker(d_->ranger->slider());
    d_->ranger->setFrameMaximum(value);
}

void QmTimelineView::onFrameMinimumChanged(qint64 value)
{
    QSignalBlocker blocker(d_->ranger->slider());
    d_->ranger->setFrameMinimum(value);
}

void QmTimelineView::onFpsChanged(double fps)
{
    d_->axis->setFps(fps);
    d_->ranger->setFps(fps);
}

void QmTimelineView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->fillRect(rect, backgroundBrush());
}

qreal QmTimelineView::mapFromSceneX(qreal x) const
{
    return mapFromScene(QPointF(x, 0)).x();
}

qreal QmTimelineView::mapFrameToAxis(qint64 frame_no) const
{
    return d_->axis->mapFrameToAxis(frame_no);
}

qreal QmTimelineView::mapFrameToAxisX(qint64 frame_no) const
{
    return d_->axis->mapFrameToAxisX(frame_no);
}

qreal QmTimelineView::mapToSceneX(qreal x) const
{
    return mapToScene(QPointF(x, 0).toPoint()).x();
}

bool QmTimelineView::isInView(qreal x, qreal width) const
{
    qreal view_x = mapFromSceneX(x);
    return view_x >= 0 && view_x + width <= this->width();
}

qreal QmTimelineView::axisFrameWidth() const
{
    return d_->axis->frameWidth();
}

qint64 QmTimelineView::axisFrameNo() const
{
    return d_->axis->frame();
}

} // namespace qmtl
