#include "qmtlitemmodel.h"

QmTLItemModel::QmTLItemModel(std::unique_ptr<QmTLItemData> item_data, QObject* parent)
    : QObject(parent)
    , data_(std::move(item_data))
{
}

QmTLItemModel::~QmTLItemModel() noexcept
{
}

QmTLItemData& QmTLItemModel::data()
{
    if (!data_) {
        assert(0 && "The Item data has not been initialized");
    }
    return *data_.get();
}

const QmTLItemData& QmTLItemModel::data() const
{
    if (!data_) {
        assert(0 && "The Item data has not been initialized");
    }
    return *data_.get();
}
