#pragma once

#include "qmtimeline_global.h"
#include <QGraphicsView>

class QmTLGraphicsScene;

struct QmTLGraphicsViewPrivate;

class QMTIMELINE_EXPORT QmTLGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit QmTLGraphicsView(QWidget* parent = nullptr);
    ~QmTLGraphicsView() noexcept override;

    qreal mapToAxisX(qint64 time_key) const;
    qreal mapToAxis(qint64 time_key) const;
    qreal axisTickPixels() const;
    qint64 axisTickValue() const;
    qint64 axisRangeInterval() const;

    void setAxisRange(qint64 min, qint64 max);
    void setAxisTickPixels(qreal tick_pixels);
    void setAxisCursorHeight(int height);
    void setScene(QmTLGraphicsScene* scene);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void initUi();
    void setupSignals();

private slots:
    void onAxisScaleChanged();
    void onAxisRangeChanged(qint64 min, qint64 max);

private:
    QmTLGraphicsViewPrivate* d_ { nullptr };
};