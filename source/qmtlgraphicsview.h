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
    qint64 axisTimeKey() const;

    void setAxisTickPixels(qreal tick_pixels);

    void setScene(QmTLGraphicsScene* scene);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void initUi();
    void setupSignals();

private:
    QmTLGraphicsViewPrivate* d_ { nullptr };
};