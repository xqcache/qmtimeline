
#include "qmtimelineitemfactory.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelineitemview.h"
#include "qmtimelinelog.h"
#include <unordered_map>

namespace qmtl {

struct QmTimelineItemFactoryPrivate {
    std::unordered_map<int, std::unique_ptr<QmTimelineItemCreateor>> creator_map;
};

QmTimelineItemFactory::QmTimelineItemFactory()
    : d_(new QmTimelineItemFactoryPrivate)
{
}

QmTimelineItemFactory::~QmTimelineItemFactory() noexcept
{
    delete d_;
}

QmTimelineItemFactory& QmTimelineItemFactory::instance()
{
    static QmTimelineItemFactory ins;
    return ins;
}

std::unique_ptr<QmTimelineItem> QmTimelineItemFactory::createItem(QmItemID item_id, QmTimelineItemModel* model) const
{
    auto* creator = itemTypeCreator(item_id);
    if (!creator) {
        return nullptr;
    }
    return creator->item_creator(item_id, model);
}

std::unique_ptr<QmTimelineItemView> QmTimelineItemFactory::createItemView(QmItemID item_id, QmTimelineScene* scene) const
{
    auto* creator = itemTypeCreator(item_id);
    if (!creator) {
        return nullptr;
    }
    return creator->item_view_creator(item_id, scene);
}

QmTimelineItemCreateor* QmTimelineItemFactory::itemTypeCreator(QmItemID item_id) const
{
    int item_type = QmTimelineItemModel::itemType(item_id);
    auto it = d_->creator_map.find(item_type);
    if (it == d_->creator_map.end()) {
        QMTL_LOG_CRITICAL("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        return nullptr;
    }
    return it->second.get();
}

bool QmTimelineItemFactory::itemHasConnection(QmItemID item_id) const
{
    auto* creator = itemTypeCreator(item_id);
    if (!creator) {
        return false;
    }
    return creator->with_connection;
}

bool QmTimelineItemFactory::registerItemType(int type, std::unique_ptr<QmTimelineItemCreateor>&& creator)
{
    if (!creator) {
        return false;
    }

    // 已经注册过了，就不能在注册了
    if (d_->creator_map.contains(type)) {
        return true;
    }

    d_->creator_map[type] = std::move(creator);
    return true;
}

bool QmTimelineItemFactory::unRegisterItemType(int type)
{
    return d_->creator_map.erase(type) > 0;
}

} // namespace qmtl
