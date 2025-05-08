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
    d_->item_modifies.clear();
    for (const auto& [item_id, _] : d_->item_models) {
        removeItem(item_id);
    }
    d_->next_id = 0;
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

void QmTLGraphicsModel::setItemData(QmTLItemID item_id, const QVariant& data, int role)
{
    auto* item_model = itemModel(item_id);
    if (!item_model) {
        return;
    }
    item_model->data().setData(data, role);
    if (isItemBatchModified(item_id)) {
        d_->item_modifies[item_id].setFlag(static_cast<QmTLItemDataRole::Role>(role));
    } else {
        requestUpdate(item_id);
    }
}

void QmTLGraphicsModel::requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles)
{
    emit itemChanged(item_id, roles, QPrivateSignal());
}

void QmTLGraphicsModel::beginBatchModify(QmTLItemID item_id)
{
    d_->item_modifies[item_id] = QmTLItemDataRole::None;
}

void QmTLGraphicsModel::endBatchModify(QmTLItemID item_id)
{
    auto it = d_->item_modifies.find(item_id);
    if (it != d_->item_modifies.end() && it->second != QmTLItemDataRole::None) {
        requestUpdate(item_id, it->second);
        d_->item_modifies.erase(it);
    }
}

bool QmTLGraphicsModel::isItemBatchModified(QmTLItemID item_id) const
{
    return d_->item_modifies.contains(item_id);
}