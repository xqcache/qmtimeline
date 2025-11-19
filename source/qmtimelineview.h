#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QGraphicsView>

namespace qmtl {

class QmTimelineScene;
class QmTimelineItemModel;
struct QmTimelineViewPrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineView : public QGraphicsView {
    Q_OBJECT

public:
    QmTimelineView(QWidget* parent = nullptr);
    ~QmTimelineView() noexcept override;

    void setAxisPlayheadHeight(int height);
    void setScene(QmTimelineScene* scene);
    void setSceneSize(qreal width, qreal height);
    void setSceneWidth(qreal width);

    int axisPlayheadHeight() const;
    qreal axisFrameWidth() const;
    qint64 axisFrameNo() const;
    QmTimelineItemModel* model() const;

    void setFrameFormat(QmFrameFormat frame_fmt);
    QmFrameFormat frameFormat() const;
    qreal mapFromSceneX(qreal x) const;
    qreal mapToSceneX(qreal x) const;
    qreal mapFrameToAxis(qint64 frame_no) const;
    qreal mapFrameToAxisX(qint64 frame_no) const;

    bool isInView(qreal x, qreal width) const;

    void setViewMinimumInterval(qint64 interval);
    void setKeepPlayheadPos(bool on);

    int viewportBottomMargin() const;

    void movePlayhead(qint64 frame_no);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void initUi();
    void setupSignals();

    void onViewFrameMaximumChanged(qint64 value);
    void onViewFrameMinimumChanged(qint64 value);
    void onFrameMaximumChanged(qint64 value);
    void onFrameMinimumChanged(qint64 value);
    void onFpsChanged(double fps);

private:
    QmTimelineViewPrivate* d_ { nullptr };
};

} // namespace qmtl
