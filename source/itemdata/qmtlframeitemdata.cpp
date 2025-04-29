#include "qmtlframeitemdata.h"

QmTLFrameItemData::~QmTLFrameItemData() noexcept { }

bool QmTLFrameItemData::load(const nlohmann::json& json)
{
    return true;
}

nlohmann::json QmTLFrameItemData::save() const
{
    return nlohmann::json();
}

void QmTLFrameItemData::setDelay(int delay)
{
    delay_ = delay;
}

const std::optional<int>& QmTLFrameItemData::delay() const
{
    return delay_;
}

bool QmTLFrameItemData::hasDelay() const
{
    return delay_.has_value();
}

void QmTLFrameItemData::clearDelay()
{
    delay_ = std::nullopt;
}
