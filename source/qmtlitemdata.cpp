#include "qmtlitemdata.h"
#include "qmtltypedef.h"

QmTLItemData::QmTLItemData(QmTLItemModel* item_model)
    : item_model_(item_model)
{
}

void QmTLItemData::setOrigin(qint64 origin)
{
    if (origin != origin_) {
        origin_ = origin;
        dirty_ = true;
    }
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
    if (duration_ == duration) {
        return;
    }
    duration_ = duration;
    dirty_ = true;
}

qint64 QmTLItemData::duration() const
{
    return duration_;
}

bool QmTLItemData::load(const nlohmann::json& json)
{
    resetDirty();
    try {
        origin_ = json.at("origin").get<qint64>();
        duration_ = json.at("duration").get<qint64>();
        return true;
    } catch (const nlohmann::json::exception& excep) {
        QMLOG_ERROR("{}:{} Failed to load item data. Exception: {}", __func__, __LINE__, excep.what());
        return false;
    }
}

nlohmann::json QmTLItemData::save() const
{
    nlohmann::json json;
    json["origin"] = origin_;
    json["duration"] = duration_;
    return json;
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

QString QmTLItemData::toolTip() const
{
    return QObject::tr("Origin: %1\nDuration: %2").arg(origin_).arg(duration_);
}