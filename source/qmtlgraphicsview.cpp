#include "qmtlgraphicsview.h"
#include "qmtldatetimeaxis.h"
#include "qmtlgraphicsscene.h"
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>

struct QmTLGraphicsViewPrivate {
    QmTLDateTimeAxis* axis { nullptr };

    QMetaObject::Connection scene_signal;
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
    disconnect(d_->scene_signal);
    scene->setView(this);
    QGraphicsView::setScene(scene);
    d_->scene_signal = connect(scene, &QmTLGraphicsScene::requestScaleAxis, this, [this, scene](bool zoom_in) {
        d_->axis->setTickUnit(zoom_in ? d_->axis->tickUnit() + 100 : d_->axis->tickUnit() - 100);
        scene->fitInAxis();
    });
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
            QWheelEvent* new_evt = new QWheelEvent(viewport()->mapFromGlobal(old_evt->globalPosition()), old_evt->globalPosition(), old_evt->pixelDelta(), old_evt->angleDelta(),
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
