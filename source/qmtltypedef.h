#pragma once

#include <limits>
#include <qtypes.h>

using QmTLItemID = quint64;
constexpr QmTLItemID kQmTLInvalidItemID = std::numeric_limits<QmTLItemID>::max();
