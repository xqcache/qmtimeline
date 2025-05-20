#pragma once

#include "nlohmann/json.hpp"
#include "qmflags.h"
#include "qmtimeline_global.h"
#include "qmtlserializable.h"
#include <QVariant>

class QMTIMELINE_EXPORT QmTLItemData : public QmTlSerializable {
public:
    enum Role : int {
        NoneRole = 0,
        OriginRole = 0x01,
        DurationRole = 0x02,
        AllRole = std::numeric_limits<int>::max()
    };

    inline constexpr static Role userRole(qint64 index)
    {
        assert(index < 32 && "The role must be less than 32.");
        return static_cast<Role>(1 << (index + 10));
    }

public:
    virtual ~QmTLItemData() noexcept = default;

    void setOrigin(qint64 origin);
    qint64 origin() const;
    qint64 destination() const;

    void setDuration(qint64 duration);
    qint64 duration() const;

    inline void setDirty(bool dirty = true);
    inline bool isDirty() const;
    inline void resetDirty();

public:
    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;
    virtual bool setProperty(const QVariant& value, int role);
    virtual std::optional<QVariant> property(int role) const;

protected:
    qint64 origin_ { 0 };
    qint64 duration_ { 0 };
    bool dirty_ { false };
};

inline void QmTLItemData::setDirty(bool dirty)
{
    dirty_ = dirty;
}

inline bool QmTLItemData::isDirty() const
{
    return dirty_;
}

inline void QmTLItemData::resetDirty()
{
    setDirty(false);
}

QM_DECLARE_FLAGS(QmTLItemDataRoles, QmTLItemData::Role, int);
