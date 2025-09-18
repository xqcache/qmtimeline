
#include "qmtimelineitemfactory.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelineitemview.h"
#include "qmtimelinelog.h"

namespace qmtl {

std::unique_ptr<QmTimelineItem> QmTimelineItemFactory::createItem(QmItemID item_id, QmTimelineItemModel* model) const
{
    int item_type = QmTimelineItemModel::itemType(item_id);
    switch (item_type) {
    default:
        QMTL_LOG_CRITICAL("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}

std::unique_ptr<QmTimelineItemView> QmTimelineItemFactory::createItemView(QmItemID item_id, QmTimelineScene* scene) const
{
    int item_type = QmTimelineItemModel::itemType(item_id);
    switch (item_type) {
    default:
        QMTL_LOG_CRITICAL("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}

bool QmTimelineItemFactory::itemHasConnection(QmItemID item_id) const
{
    return true;
}

} // namespace qmtl
