#pragma once

#include "itemmodel/qmtlitemmodel.h"
#include "nlohmann/json.hpp"
#include "qmtltypedef.h"
#include <QObject>
#include <QVariant>

struct QmTLGraphicsModelPrivate;
class QmTLGraphicsModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLGraphicsModel(QObject* parent = nullptr);
    ~QmTLGraphicsModel() noexcept override;

    bool load(const nlohmann::json& json);
    void save(nlohmann::json& json) const;

    QmTLItemID createItem(int type);
    QmTLItemModel* itemModel(QmTLItemID item_id) const;

    template <typename T>
        requires(std::is_base_of_v<QmTLItemModel, T>)
    T* itemModel(QmTLItemID item_id) const
    {
        return static_cast<T*>(itemModel(item_id));
    }

    void setItemData(QmTLItemID item_id, const QVariant& value, QmTLItemDataRole role);
    void requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles = QmTLItemDataRole::All);

signals:
    void itemCreated(QmTLItemID item_id);
    void itemRemoved(QmTLItemID item_id);
    void itemChanged(QmTLItemID item_id, QmTLItemDataRoles roles);

private:
    QmTLGraphicsModelPrivate* d_ { nullptr };
};
