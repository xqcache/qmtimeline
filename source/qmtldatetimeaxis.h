#pragma once

#include "qmtimeline_global.h"
#include <QPainterPath>
#include <QWidget>

struct QmTLDateTimeAxisPrivate;

class QMTIMELINE_EXPORT QmTLDateTimeAxis : public QWidget {
    Q_OBJECT
public:
    explicit QmTLDateTimeAxis(QWidget* parent = nullptr);
    ~QmTLDateTimeAxis() noexcept override;

    void scaleUp();
    void scaleDown();

    void setTickPixels(qreal pixels);
    qreal tickPixels() const;

    void setMaximum(qint64 maximum);
    void setMinimum(qint64 minimum);
    void setRange(qint64 min, qint64 max);
    qint64 maximum() const;
    qint64 minimum() const;

    qint64 rangeInterval() const;
    qint64 value() const;
    qint64 visualValue() const;

    void setCursorHeight(qreal height);
    qreal cursorHeight() const;
    qreal cursorWidth() const;

    qreal mapToAxis(qint64 timestamp, const std::optional<qint64>& special_offset = std::nullopt) const;
    qreal mapToAxisX(qint64 timestamp) const;

signals:
    // 缩放发生变化
    void scaleChanged();
    // 范围发生变化
    void rangeChanged(qint64 min, qint64 max);
    // 可视区域范围发生变化
    void visualRangeChanged(qint64 visual_min, qint64 visual_max);

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void updateTickArea();

private:
    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);

    QPainterPath cursorHeadShape() const;
    QPainterPath cursorTailShape() const;
    QPainterPath cursorShape() const;

    qreal cursorPaintWidth() const;
    int visualTickCount() const;

    void scaleByTickUnitRatio(qreal ratio);
    void scaleByTickUnit(qint64 unit_offset);

    qint64 calcVisualValueByX(qreal x) const;

private:
    QmTLDateTimeAxisPrivate* d_ { nullptr };
};