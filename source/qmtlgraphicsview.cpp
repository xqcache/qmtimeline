#include "qmtlgraphicsview.h"
#include "qmtldatetimeaxis.h"
#include "qmtlgraphicsscene.h"
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>

struct QmTLGraphicsViewPrivate {
    QmTLDateTimeAxis* axis { nullptr };
    QList<QMetaObject::Connection> scene_signals;
};

QmTLGraphicsView::QmTLGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new QmTLGraphicsViewPrivate)
{
    d_->axis = new QmTLDateTimeAxis(this);
    initUi();
    setupSignals();
}

QmTLGraphicsView::~QmTLGraphicsView() noexcept
{
    delete d_;
}

void QmTLGraphicsView::setScene(QmTLGraphicsScene* scene)
{
    if (!scene) {
        return;
    }
    if (scene == this->scene()) {
        return;
    }
    for (const auto& conn : d_->scene_signals) {
        disconnect(conn);
    }
    d_->scene_signals.clear();
    scene->setView(this);
    QGraphicsView::setScene(scene);
    setSceneRect(0, 0, mapToAxis(axisRangeInterval()), 20000);

    d_->scene_signals.append(connect(scene, &QmTLGraphicsScene::requestScaleAxis, this, [this, scene](bool zoom_in) {
        if (zoom_in) {
            d_->axis->scaleUp();
        } else {
            d_->axis->scaleDown();
        }
    }));
    d_->scene_signals.append(connect(d_->axis, &QmTLDateTimeAxis::scaleChanged, this, [this, scene] { scene->fitInAxis(); }));
    d_->scene_signals.append(connect(d_->axis, &QmTLDateTimeAxis::rangeChanged, this, [this, scene](qint64 min, qint64 max) {
        auto rect = sceneRect();
        rect.setWidth(scene->mapToAxis(max - min));
        setSceneRect(rect);
    }));
}

qreal QmTLGraphicsView::mapToAxisX(qint64 time_key) const
{
    return d_->axis->mapToAxisX(time_key);
}

qreal QmTLGraphicsView::mapToAxis(qint64 time_key) const
{
    return d_->axis->mapToAxis(time_key);
}

qreal QmTLGraphicsView::axisTickPixels() const
{
    return d_->axis->tickPixels();
}

qint64 QmTLGraphicsView::axisTickValue() const
{
    return d_->axis->value();
}

qint64 QmTLGraphicsView::axisRangeInterval() const
{
    return d_->axis->rangeInterval();
}

void QmTLGraphicsView::setAxisRange(qint64 min, qint64 max)
{
    d_->axis->setRange(min, max);
}

void QmTLGraphicsView::setAxisTickPixels(qreal tick_pixels)
{
    d_->axis->setTickPixels(tick_pixels);
}

void QmTLGraphicsView::initUi()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameShape(QFrame::NoFrame);

    setBackgroundBrush(QColor("#BDBDBD"));
}

void QmTLGraphicsView::setupSignals()
{
    connect(d_->axis, &QmTLDateTimeAxis::visualRangeChanged, this, [this](qint64 visual_min) { horizontalScrollBar()->setValue(mapToAxis(visual_min)); });
}

bool QmTLGraphicsView::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent* old_evt = static_cast<QMouseEvent*>(event);
        QMouseEvent* new_evt = new QMouseEvent(old_evt->type(), viewport()->mapFromGlobal(old_evt->globalPosition()), old_evt->globalPosition(),
            old_evt->button(), old_evt->buttons(), old_evt->modifiers(), old_evt->pointingDevice());
        qApp->sendEvent(viewport(), new_evt);
        return true;
    } break;
    case QEvent::Wheel: {
        QWheelEvent* old_evt = static_cast<QWheelEvent*>(event);
        if (old_evt->modifiers() == Qt::ControlModifier) {
            QWheelEvent* new_evt
                = new QWheelEvent(viewport()->mapFromGlobal(old_evt->globalPosition()), old_evt->globalPosition(), old_evt->pixelDelta(), old_evt->angleDelta(),
                    old_evt->buttons(), old_evt->modifiers(), old_evt->phase(), old_evt->inverted(), old_evt->source(), old_evt->pointingDevice());
            qApp->sendEvent(viewport(), new_evt);
            return true;
        }
        QAbstractScrollArea::wheelEvent(old_evt);
        return true;
    } break;
    default:
        break;
    }
    return QGraphicsView::event(event);
}

void QmTLGraphicsView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    d_->axis->resize(viewport()->width(), viewport()->geometry().bottom());
    setViewportMargins(0, d_->axis->cursorHeight(), 0, 0);
}

void QmTLGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(rect, backgroundBrush());

    // TODO: 绘制一个虚拟的原点，后期删除
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    painter->drawEllipse(0, 0, 10, 10);
}
