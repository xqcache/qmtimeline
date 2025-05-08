#pragma once

#include "nlohmann/json.hpp"
#include "qmflags.h"
#include "qmtimeline_global.h"
#include <QVariant>

class QMTIMELINE_EXPORT QmTLItemData {
public:
    enum Role : int {
        NoneRole = 0,
        TimeKeyRole,
        AllRole = std::numeric_limits<int>::max()
    };

    inline constexpr static Role userRole(int index)
    {
        assert(index < 32 && "The role must be less than 32.");
        return static_cast<Role>(1 << (index + 10));
    }

public:
    virtual ~QmTLItemData() noexcept = default;

    virtual bool load(const nlohmann::json& json) = 0;
    virtual nlohmann::json save() const = 0;

    void setTimeKey(qint64 time_key);
    qint64 timeKey() const;

    virtual bool setData(const QVariant& data, int role);

protected:
    qint64 time_key_ { 10 };
};

QM_DECLARE_FLAGS(QmTLItemDataRoles, QmTLItemData::Role, int);
