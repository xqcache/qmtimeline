#include "qmtldatetimeaxis.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <QTimeZone>

struct QmTLDateTimeAxisPrivate {
    qint64 minimum = 0;
    qint64 maximum = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1)).count();

    bool mouse_pressed = false;

    struct Tick {
        QString label_format { "HH:mm:ss.zz" };
        // 每个刻度所表示的时间单位（单位：毫秒）
        qint64 unit { 100 };
        // 刻度表示的时间偏移
        qint64 offset { 0 };
        // 刻度像素单位
        qreal pixels { 40 };
        // 刻度标签间隔（每隔x个刻度显示一个刻度标签）
        int label_interval { 5 };
    } tick;

    struct Cusor {
        // 光标大小
        qreal width_ratio = 0.8;
        qreal height = 40;
        // 光标位置
        QPointF pos { 0, 0 };

        qreal tail_width = 2;

        // clang-format off
        inline qreal x() const { return pos.x(); }
        inline qreal y() const { return pos.y(); }
        // clang-format on
    } cursor;
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

void QmTLDateTimeAxis::scaleUp()
{
    scaleByTickUnitRatio(0.5);
}

void QmTLDateTimeAxis::scaleDown()
{
    scaleByTickUnitRatio(2.0);
}

void QmTLDateTimeAxis::setTickPixels(qreal pixels)
{
    if (qFuzzyCompare(d_->tick.pixels, pixels)) {
        return;
    }
    d_->tick.pixels = pixels;
    updateTickArea();
    emit scaleChanged();
}

qreal QmTLDateTimeAxis::tickPixels() const
{
    return d_->tick.pixels;
}

void QmTLDateTimeAxis::setMaximum(qint64 maximum)
{
    setRange(d_->minimum, maximum);
}

void QmTLDateTimeAxis::setMinimum(qint64 minimum)
{
    setRange(minimum, d_->maximum);
}

void QmTLDateTimeAxis::setRange(qint64 min, qint64 max)
{
    if (min >= max) {
        return;
    }
    bool range_changed = false;

    if (min != d_->minimum) {
        d_->minimum = min;
        range_changed = true;
    }

    if (max != d_->maximum) {
        d_->maximum = max;
        range_changed = true;
    }

    if (range_changed) {
        emit rangeChanged(min, max);
    }
}

qint64 QmTLDateTimeAxis::maximum() const
{
    return d_->maximum;
}

qint64 QmTLDateTimeAxis::minimum() const
{
    return d_->minimum;
}

qint64 QmTLDateTimeAxis::rangeInterval() const
{
    return d_->maximum - d_->minimum;
}

void QmTLDateTimeAxis::scrollByX(qreal global_x)
{
    qint64 new_value = qRound64(global_x / d_->tick.pixels) * d_->tick.unit;
    if (new_value == value()) {
        return;
    }
    qreal new_cursor_x = d_->cursor.x();
    qint64 new_tick_offset = d_->tick.offset;

    const qreal old_cursor_x = d_->cursor.x();
    const qint64 old_tick_offset = d_->tick.offset;
    if (new_value < visualMinValue()) {
        new_cursor_x = 0;
        new_tick_offset = qMax(d_->minimum, new_value);
    } else if (new_value > visualMaxValue()) {
        new_cursor_x = visualTickCount() * d_->tick.pixels;
        new_tick_offset = qMin(new_value - visualValue(), d_->maximum);
    } else {
        new_cursor_x = d_->tick.pixels * (new_value - visualMinValue()) / d_->tick.unit;
    }

    bool cursor_area_dirty = false;
    if (!qFuzzyCompare(new_cursor_x, old_cursor_x)) {
        d_->cursor.pos.setX(new_cursor_x);
        cursor_area_dirty = true;
    }

    bool tick_area_dirty = false;
    if (new_tick_offset != old_tick_offset) {
        d_->tick.offset = new_tick_offset;
        tick_area_dirty = true;
        updateTickArea();
        emit visualRangeChanged(d_->tick.offset, d_->tick.offset + visualTickCount() * d_->tick.unit);
    }

    if (cursor_area_dirty) {
        if (!tick_area_dirty) {
            updateTickArea();
        }
        qreal tail_x_offset = (cursorWidth() - d_->cursor.tail_width) / 2.0;
        update(new_cursor_x + tail_x_offset, d_->cursor.height, new_cursor_x + tail_x_offset + d_->cursor.tail_width, height());
        update(d_->cursor.x() + tail_x_offset, d_->cursor.height, d_->cursor.x() + tail_x_offset + d_->cursor.tail_width, height());
    }
}

qint64 QmTLDateTimeAxis::value() const
{
    return visualValue() + d_->tick.offset;
}

qint64 QmTLDateTimeAxis::visualValue() const
{
    return calcVisualValueByX(d_->cursor.x());
}

qint64 QmTLDateTimeAxis::visualMinValue() const
{
    return d_->tick.offset;
}

qint64 QmTLDateTimeAxis::visualMaxValue() const
{
    return visualMinValue() + visualTickCount() * d_->tick.unit;
}

void QmTLDateTimeAxis::setCursorHeight(qreal height)
{
    if (!qFuzzyCompare(height, d_->cursor.height)) {
        qreal dirty_height = qMax(height, d_->cursor.height);
        d_->cursor.height = height;
        update(0, 0, width(), dirty_height);
    }
}

qreal QmTLDateTimeAxis::cursorHeight() const
{
    return d_->cursor.height;
}

qreal QmTLDateTimeAxis::cursorWidth() const
{
    return d_->tick.pixels * d_->cursor.width_ratio;
}

qreal QmTLDateTimeAxis::mapToAxis(qint64 timestamp, const std::optional<qint64>& special_offset) const
{
    return static_cast<qreal>(timestamp - special_offset.value_or(d_->tick.offset)) / d_->tick.unit * d_->tick.pixels;
}

qreal QmTLDateTimeAxis::mapToAxisX(qint64 timestamp) const
{
    return mapToAxis(timestamp) + d_->tick.pixels / 2.0;
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
    case QEvent::MouseButtonRelease:
        if (!handleMouseReleaseEvent(static_cast<QMouseEvent*>(event))) {
            event->ignore();
            return true;
        }
        break;
    case QEvent::MouseMove:
        if (!handleMouseMoveEvent(static_cast<QMouseEvent*>(event))) {
            event->ignore();
            return true;
        }
    default:
        break;
    }

    return QWidget::event(event);
}

void QmTLDateTimeAxis::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    {
        // 1. 绘制刻度区域背景色
        painter.save();
        painter.setPen(Qt::black);
        painter.fillRect(QRectF(0, 0, width(), d_->cursor.height), QColor("#bdbdbd"));
        painter.drawLine(0, d_->cursor.height, width(), d_->cursor.height);
        painter.restore();
    }

    {
        // 2. 绘制刻度线和刻度标签
        painter.save();

        QPen pen = painter.pen();
        pen.setWidth(2);
        pen.setColor(Qt::black);
        painter.setPen(pen);

        qint64 msces_per_label = d_->tick.label_interval * d_->tick.unit;
        qreal label_per_pixles = d_->tick.pixels * d_->tick.label_interval;
        qreal label_left_spacing = d_->tick.pixels / 2.0;

        QDateTime label_dt = QDateTime::fromMSecsSinceEpoch(d_->tick.offset, QTimeZone::utc());
        QDateTime label_dt_max = QDateTime::fromMSecsSinceEpoch(d_->maximum, QTimeZone::utc());
        for (qreal x = label_left_spacing; x < width(); x += label_per_pixles) {
            if (label_dt > label_dt_max) {
                break;
            }

            // 绘制刻度线
            painter.drawLine(x, cursorHeight(), x, cursorHeight() * 0.9);

            // 绘制刻度标签
            QString label = label_dt.toString(d_->tick.label_format);
            qreal label_width = painter.fontMetrics().horizontalAdvance(label);
            qreal label_x = x - label_width / 2;
            label_x = qMax(label_x, 0.0);
            label_x = qMin(label_x, width() - label_width);
            painter.drawText(label_x, cursorHeight() * 0.8, label);
            label_dt = label_dt.addMSecs(msces_per_label);
        }

        painter.restore();
    }

    {
        // 3. 绘制光标

        // 绘制光标头部
        painter.save();
        painter.setPen(Qt::black);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::darkGray);
        painter.setOpacity(0.8);
        painter.drawPath(cursorHeadShape());
        painter.restore();

        // 绘制光标尾部
        painter.save();
        painter.setPen(QPen(Qt::black, d_->cursor.tail_width));
        painter.drawPath(cursorTailShape());
        painter.restore();

        // 绘制当前光标值
        qreal cursor_margin = (d_->tick.pixels - cursorPaintWidth()) / 2.0;
        qreal cursor_right = d_->cursor.x() + cursor_margin + cursorPaintWidth();
        QString value_str = QDateTime::fromMSecsSinceEpoch(value(), QTimeZone::utc()).toString(d_->tick.label_format);
        qreal value_str_width = painter.fontMetrics().boundingRect(value_str).width();
        painter.setPen(Qt::black);
        if (cursor_right + value_str_width + 2 > width()) {
            painter.drawText(d_->cursor.x() + cursor_margin - value_str_width - 4, d_->cursor.y() + painter.fontMetrics().height(), value_str);
        } else {
            painter.drawText(cursor_right + 2, d_->cursor.y() + painter.fontMetrics().height(), value_str);
        }
    }
}

void QmTLDateTimeAxis::updateTickArea()
{
    update(0, 0, width(), d_->cursor.height);
}

bool QmTLDateTimeAxis::handleMousePressEvent(QMouseEvent* event)
{
    if (!cursorHeadShape().contains(event->position())) {
        return false;
    }
    d_->mouse_pressed = true;
    return true;
}

bool QmTLDateTimeAxis::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (!d_->mouse_pressed) {
        return false;
    }
    d_->mouse_pressed = false;
    return true;
}

bool QmTLDateTimeAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!d_->mouse_pressed) {
        return false;
    }
    qreal x = qRound64(event->position().x() / d_->tick.pixels) * d_->tick.pixels;
    qreal x_offset = event->position().x() - d_->cursor.x();
    qint64 v_offset = qRound64(x_offset / d_->tick.pixels) * d_->tick.unit;
    qint64 new_tick_offset = d_->tick.offset;

    if (x > (width() - d_->tick.pixels / 2.0)) {
        new_tick_offset += v_offset;
        new_tick_offset = qMin(new_tick_offset, d_->maximum - visualValue());
    } else if (x < 0) {
        new_tick_offset += v_offset;
        new_tick_offset = qMax(new_tick_offset, d_->minimum);
    }

    x = qMin(x, visualTickCount() * d_->tick.pixels);
    x = qMax(x, 0.0);

    qint64 new_value = calcVisualValueByX(x) + new_tick_offset;
    if ((new_value < d_->minimum || new_value > d_->maximum)) {
        return false;
    }

    const qint64 old_tick_offset = d_->tick.offset;
    const qreal old_cursor_x = d_->cursor.x();

    bool cursor_area_dirty = false;
    if (!qFuzzyCompare(x, old_cursor_x)) {
        d_->cursor.pos.setX(x);
        cursor_area_dirty = true;
    }

    bool tick_area_dirty = false;
    if (new_tick_offset != old_tick_offset) {
        d_->tick.offset = new_tick_offset;
        tick_area_dirty = true;
        updateTickArea();
        emit visualRangeChanged(d_->tick.offset, d_->tick.offset + visualTickCount() * d_->tick.unit);
    }

    if (cursor_area_dirty) {
        if (!tick_area_dirty) {
            updateTickArea();
        }
        qreal tail_x_offset = (cursorWidth() - d_->cursor.tail_width) / 2.0;
        update(old_cursor_x + tail_x_offset, d_->cursor.height, old_cursor_x + tail_x_offset + d_->cursor.tail_width, height());
        update(d_->cursor.x() + tail_x_offset, d_->cursor.height, d_->cursor.x() + tail_x_offset + d_->cursor.tail_width, height());
    }

    return true;
}

int QmTLDateTimeAxis::visualTickCount() const
{
    return (width() - cursorWidth()) / d_->tick.pixels;
}

void QmTLDateTimeAxis::setTickUnit(qint64 tick_unit)
{
    const qint64 max_tick_unit = (d_->maximum - d_->minimum) / visualTickCount();
    const qint64 min_tick_unit = 1;

    qint64 new_tick_unit = qMax(min_tick_unit, qMin(max_tick_unit, tick_unit));
    if (new_tick_unit == d_->tick.unit) {
        return;
    }
    d_->tick.unit = new_tick_unit;
    updateTickArea();
    emit scaleChanged();
}

void QmTLDateTimeAxis::scaleByTickUnitRatio(qreal ratio)
{
    setTickUnit(qRound64(d_->tick.unit * ratio));
}

QPainterPath QmTLDateTimeAxis::cursorHeadShape() const
{
    QPainterPath shape;
    auto cursor_paint_width = cursorPaintWidth();
    auto cursor_height = cursorHeight();
    qreal top_margin = cursor_height * 0.2;
    qreal left_margin = (d_->tick.pixels - cursor_paint_width) / 2.0;
    shape.moveTo(left_margin, top_margin);
    shape.lineTo(left_margin, cursor_height * 0.8);
    shape.lineTo(left_margin + cursor_paint_width / 2, cursor_height);
    shape.lineTo(left_margin + cursor_paint_width, cursor_height * 0.8);
    shape.lineTo(left_margin + cursor_paint_width, top_margin);
    shape.lineTo(left_margin, top_margin);
    shape.translate(d_->cursor.pos);
    return shape;
}

QPainterPath QmTLDateTimeAxis::cursorTailShape() const
{
    QPainterPath shape;
    qreal left_margin = d_->tick.pixels / 2;
    shape.moveTo(left_margin, cursorHeight());
    shape.lineTo(left_margin, height());
    shape.translate(d_->cursor.pos);
    return shape;
}

QPainterPath QmTLDateTimeAxis::cursorShape() const
{
    QPainterPath shape;
    shape.addPath(cursorHeadShape());
    shape.addPath(cursorTailShape());
    return shape;
}

qreal QmTLDateTimeAxis::cursorPaintWidth() const
{
    return cursorWidth() * 0.6;
}

qint64 QmTLDateTimeAxis::calcVisualValueByX(qreal x) const
{
    return qRound64(x / d_->tick.pixels) * d_->tick.unit;
}
