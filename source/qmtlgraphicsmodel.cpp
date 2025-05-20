#include "qmtlgraphicsmodel.h"
#include "qmtlitemmodel.h"
#include "qmtlitemregistry.h"

struct QmTLGraphicsModelPrivate {
    std::unique_ptr<QmTLItemRegistry> item_registry;
    std::map<QmTLItemID, std::unique_ptr<QmTLItemModel>> item_models;
    QmTLItemID next_id { 0 };
    std::unordered_map<QmTLItemID, QmTLItemDataRoles> item_modifies;
};

QmTLGraphicsModel::QmTLGraphicsModel(std::unique_ptr<QmTLItemRegistry> item_registry, QObject* parent)
    : QObject(parent)
    , d_(new QmTLGraphicsModelPrivate)
{
    d_->item_registry = std::move(item_registry);
}

QmTLGraphicsModel::~QmTLGraphicsModel() noexcept
{
    delete d_;
}

void QmTLGraphicsModel::removeItem(QmTLItemID item_id)
{
    auto it = d_->item_models.find(item_id);
    if (it == d_->item_models.end()) {
        return;
    }
    emit itemAboutToBeRemoved(item_id, QPrivateSignal());
    d_->item_models.erase(it);
    emit itemRemoved(item_id, QPrivateSignal());
}

void QmTLGraphicsModel::clear()
{
    for (const auto& [item_id, _] : d_->item_models) {
        emit itemAboutToBeRemoved(item_id, QPrivateSignal());
        emit itemRemoved(item_id, QPrivateSignal());
    }
    d_->item_modifies.clear();
    d_->item_models.clear();
    d_->next_id = 0;
}

bool QmTLGraphicsModel::load(const nlohmann::json& json)
{
    clear();
    try {
        for (const auto& model_j : json["models"]) {
            auto item_id = model_j["id"].get<QmTLItemID>();
            int item_type = model_j.at("type").get<int>();
            auto item_model = d_->item_registry->createItemModel(item_type);
            if (!item_model) {
                QMLOG_ERROR("{}:{} Failed to create item model for item type '{}'", __func__, __LINE__, item_type);
                return false;
            }
            if (!item_model->load(model_j)) {
                return false;
            }
            d_->next_id = qMax(d_->next_id, parseItemIdIndex(item_id));
            d_->item_models[item_id] = std::move(item_model);
            emit itemCreated(item_id, QPrivateSignal());
            requestUpdate(item_id);
        }
        return true;
    } catch (const nlohmann::json::exception& excep) {
        return false;
    }
}

nlohmann::json QmTLGraphicsModel::save() const
{
    nlohmann::json root;
    for (const auto& [item_id, item_model] : d_->item_models) {
        auto json = item_model->save();
        json["id"] = item_id;
        root["models"].emplace_back(json);
    }
    return root;
}

QmTLItemModel* QmTLGraphicsModel::itemModel(QmTLItemID item_id) const
{
    auto it = d_->item_models.find(item_id);
    if (it != d_->item_models.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool QmTLGraphicsModel::itemExists(QmTLItemID item_id) const
{
    return d_->item_models.contains(item_id);
}

QmTLItemRegistry* QmTLGraphicsModel::itemRegistry() const
{
    return d_->item_registry.get();
}

QmTLItemID QmTLGraphicsModel::createItemId(QmTLItemID index, [[maybe_unused]] const void* arg) const
{
    return index;
}

QmTLItemID QmTLGraphicsModel::parseItemIdIndex(QmTLItemID item_id) const
{
    return item_id;
}

QmTLItemID QmTLGraphicsModel::createItem(int type, const void* arg)
{
    auto item_model = d_->item_registry->createItemModel(type);
    if (!item_model) {
        return kQmTLInvalidItemID;
    }

    QmTLItemID item_id = createItemId(d_->next_id++, arg);
    d_->item_models[item_id] = std::move(item_model);
    emit itemCreated(item_id, QPrivateSignal());
    return item_id;
}

void QmTLGraphicsModel::setItemProperty(QmTLItemID item_id, const QVariant& data, int role)
{
    auto* item_model = itemModel(item_id);
    if (!item_model) {
        return;
    }
    item_model->data().setProperty(data, role);
    if (isItemBatchModified(item_id)) {
        d_->item_modifies[item_id].setFlag(role);
    } else {
        requestUpdate(item_id, role);
    }
}

std::optional<QVariant> QmTLGraphicsModel::itemProperty(QmTLItemID item_id, int role) const
{
    auto* item_model = itemModel(item_id);
    if (!item_model) {
        return std::nullopt;
    }
    return item_model->data().property(role);
}

void QmTLGraphicsModel::requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles, const QVariant& param)
{
    emit itemChanged(item_id, roles, param, QPrivateSignal());
}

void QmTLGraphicsModel::beginBatchModify(QmTLItemID item_id)
{
    d_->item_modifies[item_id] = QmTLItemData::NoneRole;
}

void QmTLGraphicsModel::endBatchModify(QmTLItemID item_id)
{
    auto it = d_->item_modifies.find(item_id);
    if (it != d_->item_modifies.end() && it->second != QmTLItemData::NoneRole) {
        requestUpdate(item_id, it->second);
        d_->item_modifies.erase(it);
    }
}

bool QmTLGraphicsModel::isItemBatchModified(QmTLItemID item_id) const
{
    return d_->item_modifies.contains(item_id);
}