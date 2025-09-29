#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QWidget>

namespace qmtl {

class QmTimelineView;
struct QmTimelineAxisPrivate;

class QMTIMELINE_LIB_EXPORT QmTimelineAxis : public QWidget {
    Q_OBJECT
public:
    enum Feature {
        // 当范围发生变化后恢复播头的位置
        KeepPlayheadPos = 0x01,
        // 播头位置可超出屏幕
        PlayheadCanOverflow = 0x02,
    };
    Q_ENUM(Feature)
    Q_DECLARE_FLAGS(Features, Feature)

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

    void setFeature(Feature feature, bool on = true);
    void setFeatures(Features features);

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

Q_DECLARE_OPERATORS_FOR_FLAGS(QmTimelineAxis::Features)

} // namespace qmtl
