#pragma once

#include "itemdata/qmtlitemdata.h"
#include "nlohmann/json.hpp"
#include <QObject>

struct QmTLItemModelPrivate;

class QmTLItemModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLItemModel(QObject* parent = nullptr);
    ~QmTLItemModel() noexcept override;

    virtual bool load(const nlohmann::json& json) = 0;
    virtual nlohmann::json save() const = 0;

    virtual QmTLItemData& data();
    virtual const QmTLItemData& data() const;

    enum {
        Type = 1,
        UserType = 100
    };

    virtual int type() const = 0;

protected:
    virtual QmTLItemModelPrivate& d_ref() const = 0;
};