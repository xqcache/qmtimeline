#include "tldemoitemregistry.h"
#include "tldemoitemmodel.h"
#include "tldemoitemprimitive.h"

std::unique_ptr<QmTLItemModel> TLDemoItemRegistry::createItemModel(int type) const
{
    switch (type) {
    case TLDemoItemModel::Type:
        return std::make_unique<TLDemoItemModel>();
    default:
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<QmTLItemPrimitive> TLDemoItemRegistry::createItemPrimitive(int type, QmTLItemID item_id, QmTLGraphicsScene& scene) const
{
    switch (type) {
    case TLDemoItemModel::Type:
        return std::make_unique<TLDemoItemPrimitive>(item_id, scene);
    default:
        return nullptr;
    }
    return nullptr;
}

qreal TLDemoItemRegistry::itemHeight(int type) const
{
    return 40;
}
