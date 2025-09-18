#pragma once

#include "qmtimeline_global.h"
#include <QGraphicsView>

class QmTimelineScene;
struct QmTimelineViewPrivate;

class QMTIMELINE_LIB_EXPORT QmTimelineView : public QGraphicsView {
    Q_OBJECT
public:
    explicit QmTimelineView(QWidget* parent);

    void setAxisPlayheadHeight(int height);
    void setScene(QmTimelineScene* scene);
    void setSceneSize(qreal width, qreal height);
    void setSceneWidth(qreal width);

    // protected:
    //     bool event(QEvent* event) override;
    //     void resizeEvent(QResizeEvent* event) override;

private:
    void initUi();

private:
    QmTimelineViewPrivate* d_ { nullptr };
};
