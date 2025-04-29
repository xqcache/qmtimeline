#pragma once

#include "itemdata/qmtlitemdata.h"
#include <memory>

struct QmTLItemModelPrivate {
    std::unique_ptr<QmTLItemData> data;
};