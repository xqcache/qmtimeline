#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QWidget>

class QmTimelineView;
struct QmTimelineAxisPrivate;

class QMTIMELINE_LIB_EXPORT QmTimelineAxis : public QWidget {
    Q_OBJECT
public:
    explicit QmTimelineAxis(QmTimelineView* view);
    ~QmTimelineAxis() noexcept override;

    void setFrameFormat(QmFrameFormat frame_fmt);
    QmFrameFormat frameFormat() const;

    void setFps(qint64 fps);
    void setMaximum(qint64 value);
    void setMinimum(qint64 value);

    qint64 minimum() const;
    qint64 maximum() const;

    qint64 frame() const;
    qreal frameWidth() const;

    void setPlayheadHeight(qreal height);

    qreal mapFrameToAxis(qint64 frame_count) const;
    qreal mapFrameToAxisX(qint64 frame_no) const;
    void movePlayhead(qint64 frame_no);

signals:
    void playheadPressed(qint64 frame_no);
    void playheadReleased(qint64 frame_no);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);

    void drawPlayhead(QPainter& painter);
    void drawRuler(QPainter& painter);

    void updatePlayheadX(qreal x, bool force = false);
    void updateRulerArea();

    qreal innerWidth() const;
    qreal maxTickLabelWidth() const;
    qreal tickUnit() const;
    qreal tickWidth() const;
    qreal tickCount() const;
    qint64 frameCount() const;

    void updateTickWidth();

    QString valueToText(qint64 value) const;

private:
    QmTimelineAxisPrivate* d_ { nullptr };
};
