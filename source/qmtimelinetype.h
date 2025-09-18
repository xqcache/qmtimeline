#pragma once

#include <functional>
#include <limits>
#include <qtypes.h>

using QmItemID = quint64;
constexpr QmItemID kInvalidItemID = std::numeric_limits<QmItemID>::max();

struct QmItemConnID {
    QmItemID from = kInvalidItemID;
    QmItemID to = kInvalidItemID;

    bool isValid() const
    {
        return from != kInvalidItemID && to != kInvalidItemID;
    }
};

struct QmItemConnIDHash {
    std::size_t operator()(const QmItemConnID& conn_id) const
    {
        return std::hash<QmItemID>()(conn_id.from) ^ std::hash<QmItemID>()(conn_id.to);
    };
};

struct QmItemConnIDEqual {
    bool operator()(const QmItemConnID& lhs, const QmItemConnID& rhs) const
    {
        return lhs.from == rhs.from && lhs.to == rhs.to;
    }
};

enum class QmFrameFormat {
    Frame = 0,
    TimeCode,
    TimeString,
};
