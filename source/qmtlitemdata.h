#pragma once

#include "nlohmann/json.hpp"
#include <qtypes.h>

class QmTLItemData {
public:
    virtual ~QmTLItemData() noexcept = default;

    virtual bool load(const nlohmann::json& json) = 0;
    virtual nlohmann::json save() const = 0;

    void setTimeKey(qint64 time_key);
    qint64 timeKey() const;

protected:
    qint64 time_key_ { 10 };
};