#include "qmtlgraphicsmodel.h"
#include "qmtlitemmodel.h"
#include "qmtlitemregistry.h"

struct QmTLGraphicsModelPrivate {
    std::pair<qint64, qint64> time_range;
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
    d_->time_range = std::make_pair<qint64, qint64>(0, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::hours(4)).count());
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

bool QmTLGraphicsModel::load(const nlohmann::json& root)
{
    clear();
    try {
        root["time-range"].get_to(d_->time_range);
        for (const auto& model_j : root["models"]) {
            auto item_id = model_j["id"].get<QmTLItemID>();
            int item_type = model_j.at("type").get<int>();
            auto item_model = d_->item_registry->createItemModel(item_id, item_type, this);
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
    root["time-range"] = d_->time_range;
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

void QmTLGraphicsModel::setTimeRange(qint64 minimum, qint64 maximum)
{
    d_->time_range = std::make_pair(minimum, maximum);
}

qint64 QmTLGraphicsModel::timeMinimum() const
{
    return d_->time_range.first;
}

qint64 QmTLGraphicsModel::timeMaximum() const
{
    return d_->time_range.second;
}

QmTLItemID QmTLGraphicsModel::createItemId(QmTLItemID index, [[maybe_unused]] const void* arg) const
{
    return index;
}

QmTLItemID QmTLGraphicsModel::parseItemIdIndex(QmTLItemID item_id) const
{
    return item_id;
}

QmTLItemID QmTLGraphicsModel::createItem(int type, const void* args)
{
    QmTLItemID item_id = createItemId(d_->next_id++, args);
    notifyItemAboutToBeCreated(item_id);
    auto item_model = d_->item_registry->createItemModel(type, item_id, this);
    if (!item_model) {
        return kQmTLInvalidItemID;
    }

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
}

std::optional<QVariant> QmTLGraphicsModel::itemProperty(QmTLItemID item_id, int role) const
{
    auto* item_model = itemModel(item_id);
    if (!item_model) {
        return std::nullopt;
    }
    return item_model->data().property(role);
}

void QmTLGraphicsModel::requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles)
{
    if (isItemBatchModified(item_id)) {
        d_->item_modifies[item_id].orFlags(roles);
    } else {
        emit itemChanged(item_id, roles, QPrivateSignal());
    }
}

void QmTLGraphicsModel::requestItemOperate(QmTLItemID item_id, QmTLItemDataRoles roles, const QVariant& param)
{
    emit itemOperate(item_id, roles, param, QPrivateSignal());
}

void QmTLGraphicsModel::beginBatchModify(QmTLItemID item_id)
{
    d_->item_modifies[item_id] = QmTLItemData::NoneRole;
}

void QmTLGraphicsModel::endBatchModify(QmTLItemID item_id)
{
    auto it = d_->item_modifies.find(item_id);
    if (it != d_->item_modifies.end() && it->second != QmTLItemData::NoneRole) {
        emit itemChanged(item_id, it->second, QPrivateSignal());
        d_->item_modifies.erase(it);
    }
}

bool QmTLGraphicsModel::isItemBatchModified(QmTLItemID item_id) const
{
    return d_->item_modifies.contains(item_id);
}

void QmTLGraphicsModel::notifyItemRemoved(QmTLItemID item_id)
{
    emit itemRemoved(item_id, QPrivateSignal());
}

void QmTLGraphicsModel::notifyItemAboutToBeRemoved(QmTLItemID item_id)
{
    emit itemAboutToBeRemoved(item_id, QPrivateSignal());
}

void QmTLGraphicsModel::notifyItemCreated(QmTLItemID item_id)
{
    emit itemCreated(item_id, QPrivateSignal());
}

void QmTLGraphicsModel::notifyItemAboutToBeCreated(QmTLItemID item_id)
{
    emit itemAboutToBeCreated(item_id, QPrivateSignal());
}