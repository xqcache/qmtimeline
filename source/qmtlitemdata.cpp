#include "qmtlitemdata.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlitemmodel.h"
#include "qmtltypedef.h"

QmTLItemData::QmTLItemData(QmTLItemModel* item_model)
    : item_model_(item_model)
{
}

void QmTLItemData::setOrigin(qint64 origin)
{
    if (origin == origin_) {
        return;
    }
    origin_ = origin;
    dirty_ = true;
    notifyPropertyChanged(QmTLItemData::OriginRole);
}

qint64 QmTLItemData::origin() const
{
    return origin_;
}

qint64 QmTLItemData::destination() const
{
    return origin_ + delay_;
}

void QmTLItemData::setDelay(qint64 delay)
{
    if (delay_ == delay) {
        return;
    }
    delay_ = delay;
    dirty_ = true;
    notifyPropertyChanged(QmTLItemData::DelayRole);
}

qint64 QmTLItemData::delay() const
{
    return delay_;
}

bool QmTLItemData::load(const nlohmann::json& json)
{
    resetDirty();
    try {
        origin_ = json.at("origin").get<qint64>();
        delay_ = json.at("delay").get<qint64>();
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
    json["delay"] = delay_;
    return json;
}

bool QmTLItemData::setProperty(const QVariant& value, int role)
{
    switch (role) {
    case OriginRole:
        setOrigin(value.value<qint64>());
        return true;
    case DelayRole:
        setDelay(value.value<qint64>());
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
    case DelayRole:
        return QVariant::fromValue<qint64>(delay_);
    default:
        break;
    }
    return std::nullopt;
}

void QmTLItemData::notifyPropertyChanged(int role)
{
    graphModel()->requestUpdate(item_model_->itemId(), role);
}

QString QmTLItemData::toolTip() const
{
    return QObject::tr("Origin: %1\nDelay: %2").arg(origin_).arg(delay_);
}

QmTLGraphicsModel* QmTLItemData::graphModel() const
{
    return item_model_->graphModel();
}