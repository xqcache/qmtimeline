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

const std::optional<qint64>& QmTLFrameItemData::delay() const
{
    return delay_;
}

qint64 QmTLFrameItemData::delayValue(qint64 def_val) const
{
    return delay_.value_or(def_val);
}

bool QmTLFrameItemData::hasDelay() const
{
    return delay_.has_value();
}

void QmTLFrameItemData::clearDelay()
{
    delay_ = std::nullopt;
}
