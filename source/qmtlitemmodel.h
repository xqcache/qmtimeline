#pragma once

#include "nlohmann/json.hpp"
#include "qmtimeline_global.h"
#include "qmtlitemdata.h"
#include "qmtlserializable.h"
#include <QObject>
#include <QVariant>

struct QmTLItemModelPrivate;

class QMTIMELINE_EXPORT QmTLItemModel : public QObject, public QmTlSerializable {
    Q_OBJECT
public:
    explicit QmTLItemModel(std::unique_ptr<QmTLItemData> item_data, QObject* parent = nullptr);
    ~QmTLItemModel() noexcept override;

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
        return static_cast<T&>(data());
    }

    enum {
        Type = 0,
        UserType = 10
    };

    virtual int type() const = 0;

protected:
    std::unique_ptr<QmTLItemData> data_;
};