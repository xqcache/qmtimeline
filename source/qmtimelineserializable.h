#pragma once

#include "nlohmann/json.hpp"

namespace qmtl {

class QmTimelineSerializable {
public:
    virtual ~QmTimelineSerializable() noexcept = default;

    virtual bool load(const nlohmann::json& j) = 0;
    virtual nlohmann::json save() const = 0;
};

} // namespace qmtl