#pragma once

#include "qmtlitemmodel.h"
#include "qmtlitemprimitive.h"

class QmTLGraphicsScene;

class QmTLItemRegistry {
public:
    virtual ~QmTLItemRegistry() noexcept = default;

    virtual std::unique_ptr<QmTLItemModel> createItemModel(int type) const = 0;
    virtual std::unique_ptr<QmTLItemPrimitive> createItemPrimitive(
        int type, QmTLItemID item_id, QmTLGraphicsScene& scene) const
        = 0;
};