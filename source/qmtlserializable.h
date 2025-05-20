#pragma once

#include "nlohmann/json.hpp"
#include "qmtimeline_global.h"

class QMTIMELINE_EXPORT QmTlSerializable {
public:
    virtual bool load(const nlohmann::json& json) = 0;
    virtual nlohmann::json save() const = 0;
};