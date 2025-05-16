#pragma once

#include "qmtimeline_global.h"
#include <QDateTime>
#include <QWidget>

struct QmTLDateTimeAxisPrivate;

class QMTIMELINE_EXPORT QmTLDateTimeAxis : public QWidget {
    Q_OBJECT
public:
    explicit QmTLDateTimeAxis(QWidget* parent = nullptr);
    ~QmTLDateTimeAxis() noexcept override;

    void setFormat(const QString& format = "HH:mm:ss");
    void setRange(qint64 min, qint64 max);
    void setMin(qint64 min);
    void setMax(qint64 max);
    void setTickPixels(qreal tick_pixels);
    void setTickUnit(qint64 tick_unit);
    void setTickLabelInterval(int tick_label_interval);

    void setCursorHeight(qreal height);

    qreal cursorHeight() const;
    qreal cursorWidth() const;
    qint64 tickOffset() const;
    qint64 tickUnit() const;
    qreal tickPixels() const;
    int visualTickCount() const;
    // 最大和最小间隔
    qint64 rangeInterval() const;

    void scaleUp();
    void scaleDown();

    qreal mapToAxis(qint64 time_key) const;
    qreal mapToAxisX(qint64 time_key) const;
    // 当前光标所在的time_key
    qint64 value() const;

signals:
    void tickUnitChanged(qint64 tick_unit);
    void rangeChanged(qint64 min, qint64 max);
    void visualRangeChanged(qint64 visual_min);
    void scaleChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;

private:
    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);

    QPainterPath cursorShape() const;
    QPainterPath cursorHeadShape() const;
    QPainterPath cursorTailShape() const;

    void updateTickArea();
    void updateCursorArea();

private:
    QmTLDateTimeAxisPrivate* d_ { nullptr };
    Q_DISABLE_COPY(QmTLDateTimeAxis)
};