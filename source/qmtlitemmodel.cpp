#include "qmtlitemmodel.h"

QmTLItemModel::QmTLItemModel(std::unique_ptr<QmTLItemData> item_data, QObject* parent)
    : QObject(parent)
    , data_(std::move(item_data))
{
}

QmTLItemModel::~QmTLItemModel() noexcept
{
}

bool QmTLItemModel::load(const nlohmann::json& json)
{
    try {
        return data_->load(json.at("internal-data"));
    } catch (const nlohmann::json::exception& excep) {
        QMLOG_ERROR("{}:{} Failed to load item model. Exception: {}", __func__, __LINE__, excep.what());
        return false;
    }
}

nlohmann::json QmTLItemModel::save() const
{
    nlohmann::json json;
    json["type"] = type();
    json["internal-data"] = data_->save();
    return json;
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
