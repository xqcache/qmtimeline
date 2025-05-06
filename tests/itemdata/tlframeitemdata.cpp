#include "tlframeitemdata.h"

TLFrameItemData::~TLFrameItemData() noexcept { }

bool TLFrameItemData::load(const nlohmann::json& json)
{
    return true;
}

nlohmann::json TLFrameItemData::save() const
{
    return nlohmann::json();
}

void TLFrameItemData::setDelay(int delay)
{
    delay_ = delay;
}

const std::optional<qint64>& TLFrameItemData::delay() const
{
    return delay_;
}

qint64 TLFrameItemData::delayValue(qint64 def_val) const
{
    return delay_.value_or(def_val);
}

bool TLFrameItemData::hasDelay() const
{
    return delay_.has_value();
}

void TLFrameItemData::clearDelay()
{
    delay_ = std::nullopt;
}
