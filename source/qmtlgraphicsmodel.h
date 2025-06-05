#pragma once

#include "nlohmann/json.hpp"
#include "qmtimeline_global.h"
#include "qmtlitemmodel.h"
#include "qmtlitemregistry.h"
#include "qmtlserializable.h"
#include "qmtltypedef.h"
#include <QObject>
#include <QVariant>

struct QmTLGraphicsModelPrivate;
class QMTIMELINE_EXPORT QmTLGraphicsModel : public QObject, public QmTlSerializable {
    Q_OBJECT
public:
    explicit QmTLGraphicsModel(std::unique_ptr<QmTLItemRegistry> item_registry, QObject* parent = nullptr);
    ~QmTLGraphicsModel() noexcept override;

    virtual void clear();
    virtual void removeItem(QmTLItemID item_id);

    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;

    QmTLItemID createItem(int type, const void* args = nullptr);
    QmTLItemModel* itemModel(QmTLItemID item_id) const;
    bool itemExists(QmTLItemID item_id) const;

    template <typename T>
        requires(std::is_base_of_v<QmTLItemModel, T>)
    T* itemModel(QmTLItemID item_id) const
    {
        return static_cast<T*>(itemModel(item_id));
    }

    void beginBatchModify(QmTLItemID item_id);
    void endBatchModify(QmTLItemID item_id);

    std::optional<QVariant> itemProperty(QmTLItemID item_id, int role) const;
    void setItemProperty(QmTLItemID item_id, const QVariant& value, int role);
    void requestUpdate(QmTLItemID item_id, QmTLItemDataRoles roles = QmTLItemData::AllRole, const QVariant& param = QVariant());

    QmTLItemRegistry* itemRegistry() const;

    void setTimeRange(qint64 minimum, qint64 maximum);
    qint64 timeMinimum() const;
    qint64 timeMaximum() const;

signals:
    // 标记为内部信号，外部无法使用model对象来发送
    void itemCreated(QmTLItemID item_id, QPrivateSignal);
    void itemRemoved(QmTLItemID item_id, QPrivateSignal);
    void itemAboutToBeRemoved(QmTLItemID item_id, QPrivateSignal);
    void itemChanged(QmTLItemID item_id, QmTLItemDataRoles roles, const QVariant& param, QPrivateSignal);

protected:
    virtual QmTLItemID createItemId(QmTLItemID index, const void* arg = nullptr) const;
    virtual QmTLItemID parseItemIdIndex(QmTLItemID item_id) const;

private:
    bool isItemBatchModified(QmTLItemID item_id) const;

private:
    QmTLGraphicsModelPrivate* d_ { nullptr };
};
