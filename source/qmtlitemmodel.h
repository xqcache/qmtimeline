#pragma once

#include "nlohmann/json.hpp"
#include "qmtimeline_global.h"
#include "qmtlitemdata.h"
#include "qmtlserializable.h"
#include "qmtltypedef.h"
#include <QObject>
#include <QPalette>
#include <QVariant>

class QmTLGraphicsModel;

struct QmTLItemModelPrivate;

class QMTIMELINE_EXPORT QmTLItemModel : public QObject, public QmTlSerializable {
    Q_OBJECT
public:
    struct DataElement {
        QString label;
        int role;
        bool readonly;
        QString widget_type;
        std::map<QString, QVariant> widget_properties;
    };

    explicit QmTLItemModel(QmTLItemID item_id, std::unique_ptr<QmTLItemData> item_data, QmTLGraphicsModel* graph_model, QObject* parent = nullptr);
    ~QmTLItemModel() noexcept override;
    Q_DISABLE_COPY_MOVE(QmTLItemModel);

    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;

    virtual QmTLItemData& data();
    virtual const QmTLItemData& data() const;

    template <typename T>
        requires(std::is_base_of_v<QmTLItemData, T>)
    T& data()
    {
        return static_cast<T&>(data());
    }

    template <typename T>
        requires(std::is_base_of_v<QmTLItemData, T>)
    const T& data() const
    {
        return static_cast<const T&>(data());
    }

    enum {
        None = 0,
        Type = 1,
        UserType = 10
    };

    virtual int type() const = 0;

    virtual QString name() const = 0;

    virtual QList<DataElement> editableElements() const;

    virtual const QPalette& palette() const;

    inline QmTLGraphicsModel* graphModel() const;

    inline QmTLItemID itemId() const;

protected:
    QmTLGraphicsModel* graph_model_ { nullptr };
    std::unique_ptr<QmTLItemData> data_;
    QmTLItemID item_id_ { kQmTLInvalidItemID };
    QPalette palette_;
};

inline QmTLGraphicsModel* QmTLItemModel::graphModel() const
{
    return graph_model_;
}

inline QmTLItemID QmTLItemModel::itemId() const
{
    return item_id_;
}
