#include "qmtldatetimeaxis.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimeZone>
#include <QWheelEvent>

struct QmTLDateTimeAxisPrivate {
    // 起始时间刻度值（单位：毫秒）
    qint64 tick_visual_min { 0 };
    // 时间刻度间隔
    qint64 tick_min { 0 };
    qint64 tick_max { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::days(1)).count() };
    // 时间刻度最小值（单位：毫秒）
    qint64 tick_unit { 100 };
    // 每隔10个刻度显示一个标签
    int tick_label_interval { 10 };
    // 每个刻度的像素宽度
    qreal tick_pixels { 20 };

    // 时间刻度标签格式
    QString tick_format { "HH:mm:ss.zz" };

    bool cursor_pressed = false;
    QSizeF cursor_size { 20, 40 };
    QPointF cursor_pos;
    qreal cursor_line_width { 2 };
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

void QmTLDateTimeAxis::setTickPixels(qreal tick_pixels)
{
    if (d_->tick_pixels == tick_pixels) {
        return;
    }
    d_->tick_pixels = tick_pixels;
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
    emit tickUnitChanged(tick_unit);
}

void QmTLDateTimeAxis::setTickLabelInterval(int tick_label_interval)
{
    if (d_->tick_label_interval == tick_label_interval) {
        return;
    }
    d_->tick_label_interval = tick_label_interval;
    updateTickArea();
}

qint64 QmTLDateTimeAxis::tickUnit() const
{
    return d_->tick_unit;
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

qint64 QmTLDateTimeAxis::timeKey() const
{
    return qRound(d_->cursor_pos.x() / d_->tick_pixels) * d_->tick_unit + d_->tick_visual_min;
}

int QmTLDateTimeAxis::visualTickCount() const
{
    return (width() - d_->cursor_size.width()) / d_->tick_pixels;
}

void QmTLDateTimeAxis::scaleUp()
{
    qint64 old_unit = d_->tick_unit;
    qint64 old_max = visualTickCount() * old_unit;
    qint64 new_max = visualTickCount() * (old_unit - 100);
    qint64 new_visual_min = qMax(0, d_->tick_visual_min + (new_max - old_max));
    if (new_visual_min == d_->tick_visual_min) {
        return;
    }
    d_->tick_visual_min = new_visual_min;
    setTickUnit(old_unit - 100);
    emit scaleChanged();
}

void QmTLDateTimeAxis::scaleDown()
{
    qint64 old_unit = d_->tick_unit;
    qint64 old_max = visualTickCount() * old_unit;
    qint64 new_max = visualTickCount() * (old_unit + 100);
    qint64 new_visual_min = qMax(0, d_->tick_visual_min + (new_max - old_max));
    if (new_visual_min == d_->tick_visual_min) {
        return;
    }

    auto old_tick_visual_min = d_->tick_visual_min;

    d_->tick_visual_min = new_visual_min;
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
            d_->cursor_pressed = true;
            setCursor(Qt::SizeAllCursor);
            return true;
        }
    }
    return false;
}

bool QmTLDateTimeAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!d_->cursor_pressed) {
        return false;
    }

    bool need_update = false;
    qreal x = event->position().x();
    int v_offset = qRound((x - d_->cursor_pos.x()) / d_->tick_pixels);
    int old_visual_min = d_->tick_visual_min;
    if (x > width() - cursorWidth()) {
        d_->tick_visual_min += v_offset;
        d_->tick_visual_min = qMin(d_->tick_visual_min, d_->tick_max);
        if (old_visual_min != d_->tick_visual_min) {
            need_update = true;
            emit visualChanged(d_->tick_visual_min);
        }
    } else if (x < 0) {
        d_->tick_visual_min += v_offset;
        d_->tick_visual_min = qMax(d_->tick_visual_min, d_->tick_min);
        if (old_visual_min != d_->tick_visual_min) {
            need_update = true;
            emit visualChanged(d_->tick_visual_min);
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
    if (d_->cursor_pressed) {
        d_->cursor_pressed = false;
        setCursor(Qt::ArrowCursor);
        return true;
    }
    return false;
}

void QmTLDateTimeAxis::updateTickArea()
{
    update(0, 0, width(), d_->cursor_size.height());
}

QPainterPath QmTLDateTimeAxis::cursorHeadShape() const
{
    QPainterPath shape;
    auto cursor_width = cursorWidth() * 0.8;
    auto cursor_height = cursorHeight();
    qreal top_margin = cursor_height * 0.2;
    qreal left_margin = (cursorWidth() - cursor_width) / 2;
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
    shape.moveTo(cursorWidth() / 2, cursorHeight());
    shape.lineTo(cursorWidth() / 2, height());
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
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // TODO: 绘制刻度区域背景色
    // painter.fillRect(QRect(0, 0, width(), d_->cursor_size.height()), painter.background());

    {
        // 绘制刻度线和刻度标签
        painter.save();
        auto pen = painter.pen();
        pen.setWidth(2);
        painter.setPen(pen);

        QDateTime dt_start = QDateTime::fromMSecsSinceEpoch(d_->tick_min, QTimeZone::utc()).addMSecs(d_->tick_visual_min);
        int tick_count = visualTickCount();
        for (int i = 0; i < tick_count; ++i) {
            if (i % d_->tick_label_interval == 0) {
                qreal x = i * d_->tick_pixels + d_->cursor_size.width() / 2;
                painter.drawLine(QPointF(x, d_->cursor_size.height()), QPointF(x, d_->cursor_size.height() * 0.9));
                // {
                //     painter.save();
                //     QPen grid_pen(Qt::gray, 1.0);
                //     painter.setPen(grid_pen);
                //     painter.setOpacity(0.8);
                //     painter.drawLine(QPointF(x, d_->cursor_size.height()), QPointF(x, height()));
                //     painter.restore();
                // }
                QDateTime dt = dt_start.addMSecs(i * d_->tick_unit);
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

        QString value_str = QDateTime::fromMSecsSinceEpoch(timeKey(), QTimeZone::utc()).toString(d_->tick_format);
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
    return mapToAxis(time_key) + d_->cursor_size.width() / 2;
}