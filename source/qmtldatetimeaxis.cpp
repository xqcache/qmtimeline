#include "qmtldatetimeaxis.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimeZone>
#include <QWheelEvent>

struct QmTLDateTimeAxisPrivate {
    // 起始时间刻度值（单位：毫秒）
    qint64 visual_value_min { 0 };
    // 时间刻度间隔
    qint64 tick_min { 0 };
    qint64 tick_max { std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::days(1)).count() };
    // 时间刻度最小值（单位：毫秒）
    qint64 tick_unit { 100 };
    // 每隔10个刻度显示一个标签
    int tick_label_interval { 5 };
    // 每个刻度的像素宽度
    qreal tick_pixels { 10 };

    // 时间刻度标签格式
    QString tick_format { "HH:mm:ss.zz" };

    bool mouse_pressed = false;
    QPointF mouse_last_pos;
    QSizeF cursor_size { 10, 40 };
    QPointF cursor_pos;
    qreal cursor_line_width { 1 };
};

QmTLDateTimeAxis::QmTLDateTimeAxis(QWidget* parent)
    : QWidget(parent)
    , d_(new QmTLDateTimeAxisPrivate)
{
}

QmTLDateTimeAxis::~QmTLDateTimeAxis() noexcept
{
    delete d_;
}

void QmTLDateTimeAxis::setFormat(const QString& format)
{
    if (d_->tick_format == format) {
        return;
    }
    d_->tick_format = format;
    updateTickArea();
}

void QmTLDateTimeAxis::setRange(qint64 min, qint64 max)
{
    bool changed = false;
    if (d_->tick_min != min) {
        changed = true;
        d_->tick_min = min;
    }
    if (d_->tick_max != max) {
        changed = true;
        d_->tick_max = max;
    }
    if (changed) {
        emit rangeChanged(min, max);
    }
}

void QmTLDateTimeAxis::setMin(qint64 min)
{
    setRange(min, d_->tick_max);
}

void QmTLDateTimeAxis::setMax(qint64 max)
{
    setRange(d_->tick_min, max);
}

void QmTLDateTimeAxis::setTickPixels(qreal tick_pixels)
{
    if (d_->tick_pixels == tick_pixels) {
        return;
    }
    d_->tick_pixels = tick_pixels;
    d_->cursor_size.setWidth(tick_pixels);
    updateTickArea();
}

void QmTLDateTimeAxis::setTickUnit(qint64 tick_unit)
{
    tick_unit = qMax(static_cast<qint64>(d_->tick_pixels), tick_unit);
    if (d_->tick_unit == tick_unit) {
        return;
    }
    d_->tick_unit = tick_unit;
    updateTickArea();
}

void QmTLDateTimeAxis::setTickLabelInterval(int tick_label_interval)
{
    if (d_->tick_label_interval == tick_label_interval) {
        return;
    }
    d_->tick_label_interval = tick_label_interval;
    updateTickArea();
}

void QmTLDateTimeAxis::setCursorHeight(qreal height)
{
    d_->cursor_size.setHeight(height);
    updateTickArea();
    updateCursorArea();
}

qint64 QmTLDateTimeAxis::tickUnit() const
{
    return d_->tick_unit;
}

qint64 QmTLDateTimeAxis::rangeInterval() const
{
    return d_->tick_max - d_->tick_min;
}

qreal QmTLDateTimeAxis::tickPixels() const
{
    return d_->tick_pixels;
}

qreal QmTLDateTimeAxis::cursorHeight() const
{
    return d_->cursor_size.height();
}

qreal QmTLDateTimeAxis::cursorWidth() const
{
    return d_->cursor_size.width();
}

qint64 QmTLDateTimeAxis::calcValueByX(qreal x) const
{
    return qRound(x / d_->tick_pixels) * d_->tick_unit + d_->visual_value_min;
}

qint64 QmTLDateTimeAxis::value() const
{
    return calcValueByX(d_->cursor_pos.x());
}

int QmTLDateTimeAxis::visualTickCount() const
{
    return (width() - d_->cursor_size.width()) / d_->tick_pixels;
}

void QmTLDateTimeAxis::scaleByUnit(qreal ratio)
{
}

void QmTLDateTimeAxis::scaleUp()
{
    qint64 old_unit = d_->tick_unit;
    qint64 old_max = visualTickCount() * old_unit;
    qint64 new_max = visualTickCount() * (old_unit - 100);
    qint64 new_visual_min = qMax(0, d_->visual_value_min + (new_max - old_max));
    if (new_visual_min == d_->visual_value_min) {
        return;
    }
    d_->visual_value_min = new_visual_min;
    setTickUnit(old_unit - 100);
    emit scaleChanged();
}

void QmTLDateTimeAxis::scaleDown()
{
    qint64 old_unit = d_->tick_unit;
    qint64 old_max = visualTickCount() * old_unit;
    qint64 new_max = visualTickCount() * (old_unit + 100);
    qint64 new_visual_min = qMax(0, d_->visual_value_min + (new_max - old_max));
    if (new_visual_min == d_->visual_value_min) {
        return;
    }

    d_->visual_value_min = new_visual_min;
    setTickUnit(old_unit + 100);
    emit scaleChanged();
}

bool QmTLDateTimeAxis::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (!handleMousePressEvent(static_cast<QMouseEvent*>(event))) {
            event->ignore();
            return true;
        }
        break;
    case QEvent::MouseMove:
        if (!handleMouseMoveEvent(static_cast<QMouseEvent*>(event))) {
            event->ignore();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (!handleMouseReleaseEvent(static_cast<QMouseEvent*>(event))) {
            event->ignore();
            return true;
        }
        break;
    case QEvent::Wheel: {
        event->ignore();
        return true;
    } break;
    default:
        break;
    }
    return QWidget::event(event);
}

bool QmTLDateTimeAxis::handleMousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        if (cursorHeadShape().contains(event->position())) {
            d_->mouse_pressed = true;
            d_->mouse_last_pos = event->position();
            setCursor(Qt::SizeAllCursor);
            return true;
        }
    }
    return false;
}

bool QmTLDateTimeAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!d_->mouse_pressed) {
        return false;
    }

    bool need_update = false;
    qreal x = event->position().x();
    qint64 v_offset = qRound((x - d_->cursor_pos.x()) / d_->tick_pixels * d_->tick_unit);
    d_->mouse_last_pos = event->position();
    int old_visual_min = d_->visual_value_min;
    if (x > width() - cursorWidth()) {
        d_->visual_value_min += v_offset;
        d_->visual_value_min = qMin(d_->visual_value_min, d_->tick_max);
        if (old_visual_min != d_->visual_value_min) {
            need_update = true;
            emit visualRangeChanged(d_->visual_value_min);
        }
    } else if (x < 0) {
        d_->visual_value_min += v_offset;
        d_->visual_value_min = qMax(d_->visual_value_min, d_->tick_min);
        if (old_visual_min != d_->visual_value_min) {
            need_update = true;
            emit visualRangeChanged(d_->visual_value_min);
        }
    }

    x = qRound(x / d_->tick_pixels) * d_->tick_pixels;
    if (visualTickCount() * d_->tick_pixels > width()) {
        x = qMin<qreal>(x, (visualTickCount() - 1) * d_->tick_pixels);
    } else {
        x = qMin<qreal>(x, visualTickCount() * d_->tick_pixels);
    }
    x = qMax<qreal>(x, 0);
    int old_x = d_->cursor_pos.x();
    if (!qFuzzyCompare(d_->cursor_pos.x(), x)) {
        d_->cursor_pos.setX(x);
        need_update = true;
    }
    if (need_update) {
        updateTickArea();
        update(old_x, cursorHeight(), cursorWidth(), height() - cursorHeight());
        update(d_->cursor_pos.x(), cursorHeight(), cursorWidth(), height() - cursorHeight());
    }
    return true;
}

bool QmTLDateTimeAxis::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (d_->mouse_pressed) {
        d_->mouse_pressed = false;
        setCursor(Qt::ArrowCursor);
        return true;
    }
    return false;
}

void QmTLDateTimeAxis::updateTickArea()
{
    update(0, 0, width(), d_->cursor_size.height());
}

void QmTLDateTimeAxis::updateCursorArea()
{
    update(QRectF(d_->cursor_pos, d_->cursor_size).toRect());
}

QPainterPath QmTLDateTimeAxis::cursorHeadShape() const
{
    QPainterPath shape;
    auto cursor_width = cursorWidth() * 0.6;
    auto cursor_height = cursorHeight();
    qreal top_margin = cursor_height * 0.2;
    qreal left_margin = (qMax(d_->tick_pixels, cursorWidth()) - cursor_width) / 2;
    shape.moveTo(left_margin, top_margin);
    shape.lineTo(left_margin, cursor_height * 0.8);
    shape.lineTo(left_margin + cursor_width / 2, cursor_height);
    shape.lineTo(left_margin + cursor_width, cursor_height * 0.8);
    shape.lineTo(left_margin + cursor_width, top_margin);
    shape.lineTo(left_margin, top_margin);
    shape.translate(d_->cursor_pos);
    return shape;
}

QPainterPath QmTLDateTimeAxis::cursorTailShape() const
{
    QPainterPath shape;
    qreal left_margin = qMax(d_->tick_pixels, cursorWidth()) / 2;
    shape.moveTo(left_margin, cursorHeight());
    shape.lineTo(left_margin, height());
    shape.translate(d_->cursor_pos);
    return shape;
}

QPainterPath QmTLDateTimeAxis::cursorShape() const
{
    QPainterPath shape;
    shape.addPath(cursorHeadShape());
    shape.addPath(cursorTailShape());
    return shape;
}

void QmTLDateTimeAxis::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    initPainter(&painter);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    {
        painter.save();
        QPen pen = painter.pen();
        pen.setWidthF(0.5);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawLine(0, d_->cursor_size.height(), width(), d_->cursor_size.height());
        painter.restore();
    }

    painter.setRenderHint(QPainter::Antialiasing, true);

    {
        // 绘制刻度线和刻度标签
        painter.save();
        auto pen = painter.pen();
        pen.setWidth(2);
        painter.setPen(pen);

        QDateTime dt_start = QDateTime::fromMSecsSinceEpoch(d_->tick_min, QTimeZone::utc()).addMSecs(d_->visual_value_min);
        QDateTime dt_max = QDateTime::fromMSecsSinceEpoch(d_->tick_min, QTimeZone::utc());
        int tick_count = visualTickCount();
        for (int i = 0; i < tick_count; ++i) {
            if (i % d_->tick_label_interval == 0) {
                QDateTime dt = dt_start.addMSecs(i * d_->tick_unit);
                if (dt > dt_max) {
                    break;
                }
                qreal x = i * d_->tick_pixels + qMax(d_->tick_pixels / 2.0, d_->cursor_size.width() / 2);
                painter.drawLine(QPointF(x, d_->cursor_size.height()), QPointF(x, d_->cursor_size.height() * 0.9));
                QString label = dt.toString(d_->tick_format);
                qreal label_width = painter.fontMetrics().horizontalAdvance(label);
                qreal label_x = x - label_width / 2;
                label_x = qMax(label_x, 0.0);
                label_x = qMin(label_x, width() - label_width);
                painter.drawText(label_x, d_->cursor_size.height() * 0.8, label);
            }
        }
        painter.restore();
    }

    {
        // 绘制光标
        QColor color(Qt::black);
        color.setAlpha(100);
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(cursorHeadShape());
        painter.restore();

        painter.save();
        painter.setPen(QPen(Qt::black, d_->cursor_line_width));
        painter.drawPath(cursorTailShape());
        painter.restore();

        QString value_str = QDateTime::fromMSecsSinceEpoch(value(), QTimeZone::utc()).toString(d_->tick_format);
        qreal value_str_width = painter.fontMetrics().boundingRect(value_str).width();
        painter.setPen(Qt::black);
        if (d_->cursor_pos.x() + cursorWidth() + value_str_width > width()) {
            painter.drawText(d_->cursor_pos.x() - value_str_width - 2, d_->cursor_pos.y() + painter.fontMetrics().height(), value_str);
        } else {
            painter.drawText(d_->cursor_pos.x() + cursorWidth() + 2, d_->cursor_pos.y() + painter.fontMetrics().height(), value_str);
        }
    }
}

qreal QmTLDateTimeAxis::mapToAxis(qint64 time_key) const
{
    return static_cast<qreal>(time_key - d_->tick_min) / d_->tick_unit * d_->tick_pixels;
}

qreal QmTLDateTimeAxis::mapToAxisX(qint64 time_key) const
{
    return mapToAxis(time_key) + qMax(d_->tick_pixels / 2.0, d_->cursor_size.width() / 2);
}