#pragma once

#include "qmtlitemregistry.h"

class TLDemoItemRegistry : public QmTLItemRegistry {
public:
    std::unique_ptr<QmTLItemModel> createItemModel(int type) const override;
    std::unique_ptr<QmTLItemPrimitive> createItemPrimitive(int type, QmTLItemID item_id, QmTLGraphicsScene& scene) const override;

    qreal itemHeight(int type) const override;
};