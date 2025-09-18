#pragma once

#include "qmtimeline_global.h"
#include <QGraphicsScene>

class QmTimelineView;
class QmTimelineItemModel;
struct QmTimelineScenePrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit QmTimelineScene(QObject* parent = nullptr);
    ~QmTimelineScene() noexcept override;

    void setView(QmTimelineView* view);
    QmTimelineItemModel* model() const;

private:
    QmTimelineScenePrivate* d_ { nullptr };
};