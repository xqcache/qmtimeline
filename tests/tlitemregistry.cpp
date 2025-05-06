#include "tlitemregistry.h"
#include "itemmodel/tlframeitemmodel.h"
#include "itemprimitive/tlframeitemprimitive.h"

std::unique_ptr<QmTLItemModel> TLItemRegistry::createItemModel(int type) const
{
    switch (type) {
    case TLFrameItemModel::Type:
        return std::make_unique<TLFrameItemModel>();
    default:
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<QmTLItemPrimitive> TLItemRegistry::createItemPrimitive(
    int type, QmTLItemID item_id, QmTLGraphicsScene& scene) const
{
    switch (type) {
    case TLFrameItemModel::Type:
        return std::make_unique<TLFrameItemPrimitive>(item_id, scene);
    default:
        return nullptr;
    }
    return nullptr;
}
