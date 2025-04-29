#include "qmtlgraphicsmodel.h"
#include "itemmodel/qmtlframeitemmodel.h"
#include "itemmodel/qmtlitemmodel.h"

struct QmTLGraphicsModelPrivate {
    std::map<QmTLItemID, std::unique_ptr<QmTLItemModel>> item_models;
    QmTLItemID next_id { 0 };
};

QmTLGraphicsModel::QmTLGraphicsModel(QObject* parent)
    : QObject(parent)
    , d_(new QmTLGraphicsModelPrivate)
{
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

QmTLItemID QmTLGraphicsModel::createItem(int type)
{
    QmTLItemModel* item_model = nullptr;
    switch (type) {
    case QmTLFrameItemModel::Type:
        item_model = new QmTLFrameItemModel();
        break;
    };
    if (!item_model) {
        return kQmTLInvalidItemID;
    }
    QmTLItemID item_id = d_->next_id++;
    d_->item_models[item_id].reset(item_model);
    emit itemCreated(item_id);
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
        emit itemChanged(item_id, QmTLItemDataRole::TimeKey);
    } break;
    default:
        break;
    }
}

void QmTLGraphicsModel::requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles)
{
    emit itemChanged(item_id, roles);
}