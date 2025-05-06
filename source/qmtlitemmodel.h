#pragma once

#include "nlohmann/json.hpp"
#include "qmtlitemdata.h"
#include <QObject>


struct QmTLItemModelPrivate;

class QmTLItemModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLItemModel(std::unique_ptr<QmTLItemData> item_data, QObject* parent = nullptr);
    ~QmTLItemModel() noexcept override;

    virtual bool load(const nlohmann::json& json) = 0;
    virtual nlohmann::json save() const = 0;

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
        Type = 1,
        UserType = 100
    };

    virtual int type() const = 0;

protected:
    std::unique_ptr<QmTLItemData> data_;
};