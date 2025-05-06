#pragma once

#include "nlohmann/json.hpp"
#include "qmtlitemmodel.h"
#include "qmtltypedef.h"
#include <QObject>
#include <QVariant>

class QmTLItemRegistry;

struct QmTLGraphicsModelPrivate;
class QmTLGraphicsModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLGraphicsModel(std::unique_ptr<QmTLItemRegistry> item_registry, QObject* parent = nullptr);
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

    QmTLItemRegistry* itemRegistry() const;

signals:
    // 标记为内部信号，外部无法使用model对象来发送
    void itemCreated(QmTLItemID item_id, QPrivateSignal);
    void itemRemoved(QmTLItemID item_id, QPrivateSignal);
    void itemChanged(QmTLItemID item_id, QmTLItemDataRoles roles, QPrivateSignal);

private:
    QmTLGraphicsModelPrivate* d_ { nullptr };
};
