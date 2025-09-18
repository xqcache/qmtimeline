#include "qmtimelinescene.h"
#include "qmtimelinetype.h"
#include <QUndoStack>

struct QmTimelineScenePrivate {
    QmTimelineView* view { nullptr };
    QmTimelineItemModel* model { nullptr };
    QUndoStack* undo_stack { nullptr };
    // std::unordered_map<ItemID, std::unique_ptr<TimelineItemView>> item_views;
    // std::unordered_map<ItemConnID, std::unique_ptr<TimelineItemConnView>, ItemConnIDHash, ItemConnIDEqual> item_conn_views;
};

QmTimelineScene::QmTimelineScene(QObject* parent)
    : QGraphicsScene(parent)
    , d_(new QmTimelineScenePrivate)
{
}

QmTimelineScene::~QmTimelineScene() noexcept
{
    delete d_;
}

void QmTimelineScene::setView(QmTimelineView* view)
{
    d_->view = view;
}

QmTimelineItemModel* QmTimelineScene::model() const
{
    return d_->model;
}