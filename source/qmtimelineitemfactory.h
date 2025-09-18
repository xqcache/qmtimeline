#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <memory>

namespace qmtl {

class QmTimelineItem;
class QmTimelineItemView;
class QmTimelineItemModel;
class QmTimelineScene;

class QmTimelineItemFactory {
public:
    std::unique_ptr<QmTimelineItem> createItem(QmItemID item_id, QmTimelineItemModel* model) const;
    std::unique_ptr<QmTimelineItemView> createItemView(QmItemID item_id, QmTimelineScene* scene) const;

    bool itemHasConnection(QmItemID item_id) const;
};

} // namespace qmtl
