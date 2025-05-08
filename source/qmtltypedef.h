#pragma once

#include <QFlags>
#include <qtypes.h>

using QmTLItemID = quint64;
constexpr QmTLItemID kQmTLInvalidItemID = std::numeric_limits<QmTLItemID>::max();

struct QmTLItemDataRole {
    enum Role : int {
        None = 0,
        TimeKey = 0x01,
        All = std::numeric_limits<int>::max(),
    };

    inline constexpr static QmTLItemDataRole::Role userRole(int index)
    {
        assert(index < 32 && "The role must be less than 32.");
        return static_cast<QmTLItemDataRole::Role>(1 << (index + 10));
    }
};

Q_DECLARE_FLAGS(QmTLItemDataRoles, QmTLItemDataRole::Role);
