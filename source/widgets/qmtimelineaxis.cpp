#include "qmtimelineaxis.h"
#include "qmtimelineutil.h"
#include "qmtimelineview.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>

namespace qmtl {

struct QmTimelineAxisPrivate {
    QmTimelineView* view { nullptr };

    struct Playhead {
        qreal height { 40 };
        qreal x { 0 };
    } playhead;

    struct Ruler {
        QMarginsF margins { 20, 0, 20, 0 };
        qreal frame_width { 0 };
        qreal tick_width { 0 };
        qint64 minimum { 0 };
        qint64 maximum { 1 };
        int sub_ticks { 5 };
    } ruler;

    QmTimelineAxis::Features features;

    QmFrameFormat frame_fmt { QmFrameFormat::TimeCode };
    double fps { 24.0 };

    bool pressed { false };
};

QmTimelineAxis::QmTimelineAxis(QmTimelineView* view)
    : QWidget(view)
    , d_(new QmTimelineAxisPrivate)
{
    d_->view = view;
    d_->ruler.tick_width = maxTickLabelWidth();
    d_->ruler.frame_width = innerWidth() / static_cast<qreal>(frameCount());
    setMouseTracking(true);
}

QmTimelineAxis::~QmTimelineAxis() noexcept
{
    delete d_;
}

bool QmTimelineAxis::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (handleMousePressEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (handleMouseReleaseEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseMove:
        if (handleMouseMoveEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void QmTimelineAxis::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateTickWidth();
}

void QmTimelineAxis::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPlayhead(painter);
    drawRuler(painter);
}

void QmTimelineAxis::leaveEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
}

void QmTimelineAxis::drawPlayhead(QPainter& painter)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.setOpacity(0.8);

    qreal x = d_->playhead.x + d_->ruler.margins.left();
    qreal w = qMax(frameWidth(), 2.0);

    painter.setPen(Qt::NoPen);
    painter.drawRect(x, 0, w, height());

    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(x, 2, x + w, 2);

    painter.restore();
    const QString label = valueToText(frame());
    auto label_rect = fontMetrics().boundingRect(label);
    label_rect.setWidth(label_rect.width() + 2);
    label_rect.moveBottom(d_->playhead.height - fontMetrics().height());
    label_rect.moveLeft(d_->playhead.x + d_->ruler.margins.left() + 2);

    if (label_rect.right() > width()) {
        label_rect.moveRight(x - 2);
    }

    painter.drawText(label_rect, label);
}

void QmTimelineAxis::drawRuler(QPainter& painter)
{
    painter.save();
    painter.drawLine(0, d_->playhead.height, width(), d_->playhead.height);

    QPen grid_pen = painter.pen();
    grid_pen.setStyle(Qt::DotLine);

    qreal tick_width = tickWidth();
    qint64 tick_count = tickCount() + 2;
    qint64 tick_unit = qRound64(tickUnit());

    qint64 frame_no = d_->ruler.minimum;
    for (qint64 i = 0; i < tick_count && frame_no <= d_->ruler.maximum; ++i, frame_no += tick_unit) {
        qreal x = d_->ruler.margins.left() + i * tick_width;
        painter.drawLine(x, d_->playhead.height, x, d_->playhead.height * 0.8);

        painter.save();
        painter.setPen(grid_pen);
        painter.setOpacity(0.5);
        painter.drawLine(x, d_->playhead.height, x, height());
        painter.restore();

        QString time_text = valueToText(frame_no);
        painter.drawText(x + 2, d_->playhead.height - 2, time_text);
    }
    painter.restore();
}

bool QmTimelineAxis::handleMousePressEvent(QMouseEvent* event)
{
    if (!isEnabled()) {
        return false;
    }

    if (QRectF(0, 0, width(), d_->playhead.height).contains(event->position())) {
        d_->pressed = true;
        updatePlayheadX(event->position().x() - d_->ruler.margins.left());
    }

    emit playheadPressed(frame());

    return false;
}

bool QmTimelineAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!isEnabled()) {
        return false;
    }
    if (!d_->pressed) {
        return false;
    }

    updatePlayheadX(event->position().x() - d_->ruler.margins.left());
    return true;
}

bool QmTimelineAxis::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (!isEnabled()) {
        return false;
    }

    if (d_->pressed) {
        d_->pressed = false;
        emit playheadReleased(frame());
        return true;
    }
    return false;
}

void QmTimelineAxis::updatePlayheadX(qreal x, bool force)
{
    x = qint64(x / frameWidth()) * frameWidth();
    x = qMin(qMax(0.0, x), (frameCount() - 1) * frameWidth());

    if (qFuzzyCompare(x, d_->playhead.x)) {
        if (force) {
            d_->playhead.x = x;
            update(x, 0, frameWidth(), height());
        }
        return;
    }
    d_->playhead.x = x;
    update();
}

void QmTimelineAxis::updateRulerArea()
{
    update(QRectF(0, 0, width(), d_->playhead.height).toRect());
}

void QmTimelineAxis::setPlayheadHeight(qreal height)
{
    d_->playhead.height = height;
    updateRulerArea();
}

qint64 QmTimelineAxis::minimum() const
{
    return d_->ruler.minimum;
}

qint64 QmTimelineAxis::maximum() const
{
    return d_->ruler.maximum;
}

void QmTimelineAxis::setMaximum(qint64 value)
{
    if (d_->ruler.maximum == value) {
        return;
    }
    auto recover_value = qScopeGuard([this, old_frame = frame()] { movePlayhead(old_frame); });
    d_->ruler.maximum = value;
    updateTickWidth();
    update();
    if (!d_->features.testFlag(KeepPlayheadPos)) {
        recover_value.dismiss();
    }
}

void QmTimelineAxis::setMinimum(qint64 value)
{
    if (d_->ruler.minimum == value) {
        return;
    }
    auto recover_value = qScopeGuard([this, old_frame = frame()] { movePlayhead(old_frame); });
    d_->ruler.minimum = value;
    updateTickWidth();
    update();
    if (!d_->features.testFlag(KeepPlayheadPos)) {
        recover_value.dismiss();
    }
}

qreal QmTimelineAxis::innerWidth() const
{
    return width() - d_->ruler.margins.left() - d_->ruler.margins.right();
}

qreal QmTimelineAxis::maxTickLabelWidth() const
{
    const auto& font_metrics = fontMetrics();
    return qMax(font_metrics.horizontalAdvance(valueToText(d_->ruler.minimum)), font_metrics.horizontalAdvance(valueToText(d_->ruler.maximum)))
        * (d_->frame_fmt == QmFrameFormat::Frame ? 1.7 : 1.3);
}

qreal QmTimelineAxis::tickCount() const
{
    return innerWidth() / d_->ruler.tick_width;
}

qreal QmTimelineAxis::tickUnit() const
{
    return 1.0 * frameCount() / tickCount();
}

qreal QmTimelineAxis::tickWidth() const
{
    return d_->ruler.tick_width;
}

qreal QmTimelineAxis::frameWidth() const
{
    return d_->ruler.frame_width;
}

qint64 QmTimelineAxis::frameCount() const
{
    return d_->ruler.maximum - d_->ruler.minimum + 1;
}

QString QmTimelineAxis::valueToText(qint64 value) const
{
    switch (d_->frame_fmt) {
    case QmFrameFormat::Frame:
        return QString::number(value);
    case QmFrameFormat::TimeCode:
        return QmTimelineUtil::formatTimeCode(value, d_->fps);
    case QmFrameFormat::TimeString:
        return QmTimelineUtil::formatTimeString(value, d_->fps);
    default:
        assert(0 && "Invalid format");
        break;
    }
    return "";
}

void QmTimelineAxis::setFps(qint64 fps)
{
    d_->fps = fps;
    update();
}

void QmTimelineAxis::setFrameFormat(QmFrameFormat frame_fmt)
{
    d_->frame_fmt = frame_fmt;
    updateTickWidth();
    update();
}

QmFrameFormat QmTimelineAxis::frameFormat() const
{
    return d_->frame_fmt;
}

qint64 QmTimelineAxis::frame() const
{
    return qRound64(d_->playhead.x / tickWidth() * tickUnit()) + d_->ruler.minimum;
}

qreal QmTimelineAxis::mapFrameToAxis(qint64 frame_count) const
{
    return static_cast<qreal>(frame_count) * frameWidth();
}

qreal QmTimelineAxis::mapFrameToAxisX(qint64 frame_no) const
{
    return mapFrameToAxis(frame_no - d_->ruler.minimum) + d_->ruler.margins.left();
}

void QmTimelineAxis::updateTickWidth()
{
    d_->ruler.frame_width = innerWidth() / static_cast<double>(frameCount());
    d_->ruler.tick_width = d_->ruler.frame_width;
    if (d_->ruler.tick_width < maxTickLabelWidth()) {
        d_->ruler.tick_width *= 1 + qRound64(maxTickLabelWidth() / d_->ruler.tick_width);
    }
}

void QmTimelineAxis::movePlayhead(qint64 frame_no)
{
    qreal x = mapFrameToAxis(frame_no - d_->ruler.minimum);
    if (!(d_->features & PlayheadCanOverflow)) {
        x = qMin(qMax(0.0, x), (frameCount() - 1) * frameWidth());
    }
    if (qFuzzyCompare(x, d_->playhead.x)) {
        return;
    }
    d_->playhead.x = x;
    update();
}

void QmTimelineAxis::setFeature(Feature feature, bool on)
{
    if (on) {
        setFeatures(d_->features | feature);
    } else {
        setFeatures(d_->features & ~feature);
    }
}

void QmTimelineAxis::setFeatures(Features features)
{
    if (features == d_->features) {
        return;
    }
    d_->features = features;
}

} // namespace qmtl