#include "qmtlitemdata.h"
#include "qmtltypedef.h"

void QmTLItemData::setOrigin(qint64 origin)
{
    origin_ = origin;
}

qint64 QmTLItemData::origin() const
{
    return origin_;
}

qint64 QmTLItemData::destination() const
{
    return origin_ + duration_;
}

void QmTLItemData::setDuration(qint64 duration)
{
    duration_ = duration;
}

qint64 QmTLItemData::duration() const
{
    return duration_;
}

bool QmTLItemData::setProperty(const QVariant& value, int role)
{
    switch (role) {
    case OriginRole:
        setOrigin(value.value<qint64>());
        return true;
    case DurationRole:
        setDuration(value.value<qint64>());
        return true;
    default:
        break;
    }
    return false;
}

std::optional<QVariant> QmTLItemData::property(int role) const
{
    switch (role) {
    case OriginRole:
        return QVariant::fromValue<qint64>(origin_);
    case DurationRole:
        return QVariant::fromValue<qint64>(duration_);
    default:
        break;
    }
    return std::nullopt;
}
