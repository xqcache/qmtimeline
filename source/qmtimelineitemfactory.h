#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <memory>

namespace qmtl {

class QmTimelineItem;
class QmTimelineItemView;
class QmTimelineItemModel;
class QmTimelineScene;

struct QmTimelineItemCreateor {
    bool with_connection = false;
    std::function<std::unique_ptr<QmTimelineItem>(QmItemID, QmTimelineItemModel*)> item_creator;
    std::function<std::unique_ptr<QmTimelineItemView>(QmItemID, QmTimelineScene*)> item_view_creator;
};

struct QmTimelineItemFactoryPrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineItemFactory {
public:
    static QmTimelineItemFactory& instance();
    ~QmTimelineItemFactory() noexcept;

    std::unique_ptr<QmTimelineItem> createItem(QmItemID item_id, QmTimelineItemModel* model) const;
    std::unique_ptr<QmTimelineItemView> createItemView(QmItemID item_id, QmTimelineScene* scene) const;

    bool itemHasConnection(QmItemID item_id) const;

    bool registerItemType(int type, std::unique_ptr<QmTimelineItemCreateor>&& creator);
    bool unRegisterItemType(int type);
    bool hasItemType(int type) const;

private:
    Q_DISABLE_COPY_MOVE(QmTimelineItemFactory);
    QmTimelineItemFactory();

    QmTimelineItemCreateor* itemTypeCreator(QmItemID item_id) const;

private:
    QmTimelineItemFactoryPrivate* d_ { nullptr };
};

} // namespace qmtl
