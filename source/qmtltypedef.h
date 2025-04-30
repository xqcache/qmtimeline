#pragma once

#include <QFlags>
#include <qtypes.h>

using QmTLItemID = qint32;
constexpr QmTLItemID kQmTLInvalidItemID = -1;

enum class QmTLItemDataRole : int {
    TimeKey = 0x01,
    All = std::numeric_limits<int>::max(),
};

Q_DECLARE_FLAGS(QmTLItemDataRoles, QmTLItemDataRole);
