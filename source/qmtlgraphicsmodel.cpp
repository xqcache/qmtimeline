#include "qmtlgraphicsmodel.h"
#include "qmtlitemmodel.h"
#include "qmtlitemregistry.h"

struct QmTLGraphicsModelPrivate {
    std::unique_ptr<QmTLItemRegistry> item_registry;
    std::map<QmTLItemID, std::unique_ptr<QmTLItemModel>> item_models;
    QmTLItemID next_id { 0 };
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

QmTLItemModel* QmTLGraphicsModel::itemModel(QmTLItemID item_id) const
{
    auto it = d_->item_models.find(item_id);
    if (it != d_->item_models.end()) {
        return it->second.get();
    }
    return nullptr;
}

QmTLItemRegistry* QmTLGraphicsModel::itemRegistry() const
{
    return d_->item_registry.get();
}

QmTLItemID QmTLGraphicsModel::createItem(int type)
{
    auto item_model = d_->item_registry->createItemModel(type);
    if (!item_model) {
        return kQmTLInvalidItemID;
    }

    QmTLItemID item_id = d_->next_id++;
    d_->item_models[item_id] = std::move(item_model);
    emit itemCreated(item_id, QPrivateSignal());
    return item_id;
}

void QmTLGraphicsModel::setItemData(QmTLItemID item_id, const QVariant& value, QmTLItemDataRole role)
{
    auto* item_model = itemModel(item_id);
    if (!item_model) {
        return;
    }
    switch (role) {
    case QmTLItemDataRole::TimeKey: {
        item_model->data().setTimeKey(value.value<qint64>());
        emit itemChanged(item_id, QmTLItemDataRole::TimeKey, QPrivateSignal());
    } break;
    default:
        break;
    }
}

void QmTLGraphicsModel::requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles)
{
    emit itemChanged(item_id, roles, QPrivateSignal());
}