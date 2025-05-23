#include "tldemoitemdata.h"

TLDemoItemData::~TLDemoItemData() noexcept
{
}

void TLDemoItemData::setDelay(int delay)
{
    delay_ = delay;
}

const std::optional<qint64>& TLDemoItemData::delay() const
{
    return delay_;
}

qint64 TLDemoItemData::delayValue(qint64 def_val) const
{
    return delay_.value_or(def_val);
}

bool TLDemoItemData::hasDelay() const
{
    return delay_.has_value();
}

void TLDemoItemData::clearDelay()
{
    delay_ = std::nullopt;
}
