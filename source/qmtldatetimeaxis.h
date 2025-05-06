#pragma once

#include <QDateTime>
#include <QWidget>

struct QmTLDateTimeAxisPrivate;

class QmTLDateTimeAxis : public QWidget {
    Q_OBJECT
public:
    explicit QmTLDateTimeAxis(QWidget* parent = nullptr);
    ~QmTLDateTimeAxis() noexcept override;

    void setFormat(const QString& format = "HH:mm:ss");
    void setRange(const QDateTime& min, const QDateTime& max);
    void setMin(const QDateTime& min);
    void setMax(const QDateTime& max);
    void setTickPixels(qreal tick_pixels);
    void setTickUnit(qint64 tick_unit);
    void setTickLabelInterval(int tick_label_interval);

    qreal cursorHeight() const;
    qreal cursorWidth() const;
    qint64 tickOffset() const;
    qint64 tickUnit() const;
    qreal tickPixels() const;
    int visualTickCount() const;

    void scaleUp();
    void scaleDown();

    qreal mapToAxis(qint64 time_key) const;
    qreal mapToAxisX(qint64 time_key) const;
    // 当前光标所在的time_key
    qint64 timeKey() const;

signals:
    void tickUnitChanged(qint64 tick_unit);
    void visualChanged(qint64 visual_min);
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
    void updateCursorArea(const QPointF& pos);

private:
    QmTLDateTimeAxisPrivate* d_ { nullptr };
    Q_DISABLE_COPY(QmTLDateTimeAxis)
};