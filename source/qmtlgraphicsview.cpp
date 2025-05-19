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

    bool skip_hbar_signal = false;
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
    setSceneRect(0, 0, mapToAxis(d_->axis->rangeInterval()), 20000);

    d_->scene_signals.append(connect(scene, &QmTLGraphicsScene::requestScaleAxis, this, [this, scene](bool zoom_in) {
        if (zoom_in) {
            d_->axis->scaleUp();
        } else {
            d_->axis->scaleDown();
        }
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

void QmTLGraphicsView::setAxisCursorHeight(int height)
{
    d_->axis->setCursorHeight(height);
    setViewportMargins(0, height, 0, 0);
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
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameShape(QFrame::NoFrame);

    setBackgroundBrush(QColor("#BDBDBD"));
}

void QmTLGraphicsView::setupSignals()
{
    connect(d_->axis, &QmTLDateTimeAxis::visualRangeChanged, this, [this](qint64 visual_min, qint64 visual_max) {
        if (auto* tl_scene = qobject_cast<QmTLGraphicsScene*>(scene()); tl_scene) {
            d_->skip_hbar_signal = true;
            horizontalScrollBar()->setValue(d_->axis->mapToAxis(visual_min, 0));
        }
    });
    connect(d_->axis, &QmTLDateTimeAxis::scaleChanged, this, &QmTLGraphicsView::onAxisScaleChanged);
    connect(d_->axis, &QmTLDateTimeAxis::rangeChanged, this, &QmTLGraphicsView::onAxisRangeChanged);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int scene_x) {
        if (d_->skip_hbar_signal) {
            d_->skip_hbar_signal = false;
            return;
        }
        QSignalBlocker blocker(d_->axis);
        d_->axis->scrollByX(scene_x);
    });
}

void QmTLGraphicsView::onAxisScaleChanged()
{
    if (auto* tl_scene = qobject_cast<QmTLGraphicsScene*>(scene()); tl_scene) {
        auto rect = sceneRect();
        rect.setWidth(qMax<qreal>(d_->axis->mapToAxis(d_->axis->rangeInterval()), viewport()->width()));
        setSceneRect(rect);
        tl_scene->fitInAxis();
    }
}

void QmTLGraphicsView::onAxisRangeChanged(qint64 min, qint64 max)
{
    if (auto* tl_scene = qobject_cast<QmTLGraphicsScene*>(scene()); tl_scene) {
        auto rect = sceneRect();
        rect.setWidth(d_->axis->mapToAxis(max - min));
        setSceneRect(rect);
    }
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
    translate(0, 0);
}

void QmTLGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(rect, backgroundBrush());

    // TODO: 绘制一个虚拟的原点，后期删除
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    painter->drawEllipse(0, 0, 10, 10);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::green);
    painter->drawRect(sceneRect());
}
