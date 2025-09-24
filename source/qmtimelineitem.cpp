#include "qmtimelineitem.h"
#include "qmtimelineitemmodel.h"
#include "qmtimelinelog.h"
#include <QCoreApplication>

namespace qmtl {

QmTimelineItem::QmTimelineItem(QmItemID item_id, QmTimelineItemModel* model)
    : model_(model)
    , item_id_(item_id)
{
    palette_.setBrush(QPalette::Base, QColor("#006064"));
    palette_.setBrush(QPalette::Disabled, QPalette::Base, Qt::gray);
    palette_.setBrush(QPalette::AlternateBase, QColor("#006064"));
    palette_.setColor(QPalette::Text, Qt::white);
    palette_.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
}

void QmTimelineItem::setNumber(int number)
{
    if (number == number_) {
        return;
    }
    number_ = number;
    setDirty(true);
    notifyPropertyChanged(NumberRole);
}

void QmTimelineItem::setStart(qint64 frame_no)
{
    if (frame_no == start_) {
        return;
    }
    int old_start = start_;
    start_ = frame_no;
    setDirty(true);
    notifyPropertyChanged(StartRole | ToolTipRole, old_start);
}

void QmTimelineItem::setDuration(qint64 frame_count)
{
    if (frame_count == duration_) {
        return;
    }
    duration_ = frame_count;
    setDirty(true);
    notifyPropertyChanged(DurationRole | ToolTipRole);
}

void QmTimelineItem::setEnabled(bool enabled)
{
    if (enabled == enabled_) {
        return;
    }
    enabled_ = enabled;
    setDirty(true);
    notifyPropertyChanged(EnabledRole);
}

int QmTimelineItem::type() const
{
    return Type;
}

bool QmTimelineItem::isValid() const
{
    return true;
}

QString QmTimelineItem::toolTip() const
{
    return QCoreApplication::translate("QmTimelineItem", "Frame Start: %1").arg(start_);
}

bool QmTimelineItem::setProperty(int role, const QVariant& data)
{
    if (data.isNull()) {
        return false;
    }
    switch (role) {
    case StartRole: {
        qint64 start = data.value<qint64>();
        if (model_->isFrameRangeOccupied(QmTimelineItemModel::itemRowId(item_id_), start, duration_, item_id_)) {
            QMTL_LOG_ERROR("This time range already occupied! start:{}, duration:{}", start, duration_);
            return false;
        }
        if (!model_->isFrameInRange(start, duration_)) {
            QMTL_LOG_ERROR("This time range is out of range! start:{}, duration:{}", start, duration_);
            return false;
        }
        setStart(start);
    } break;
    case DurationRole: {
        qint64 duration = data.value<qint64>();
        if (model_->isFrameRangeOccupied(QmTimelineItemModel::itemRowId(item_id_), start_, duration, item_id_)) {
            QMTL_LOG_ERROR("This time range already occupied! start:{}, duration:{}", start_, duration);
            return false;
        }
        if (!model_->isFrameInRange(start_, duration)) {
            QMTL_LOG_ERROR("This time range is out of range! start:{}, duration:{}", start_, duration);
            return false;
        }
        setDuration(duration);
    } break;
    case EnabledRole: {
        setEnabled(data.toBool());
    } break;
    default:
        break;
    }
    return true;
}

std::optional<QVariant> QmTimelineItem::property(int role) const
{
    switch (role) {
    case StartRole:
        return start_;
    case DurationRole:
        return duration_;
    case NumberRole:
        return number_;
    case EnabledRole:
        return enabled_;
    default:
        break;
    }
    return std::nullopt;
}

bool QmTimelineItem::operate(int op_role, const QVariant& param)
{
    switch (op_role) {
    case OperationRole::OpIncreaseNumberRole:
        setNumber(number_ + param.toInt());
        return true;
    case OperationRole::OpDecreaseNumberRole:
        setNumber(number_ - param.toInt());
        return true;
    case OperationRole::OpUpdateAsHead:
    case OperationRole::OpUpdateAsTail:
        model_->notifyItemOperateFinished(item_id_, op_role);
        return true;
    default:
        break;
    }
    return false;
}

const QPalette& QmTimelineItem::palette() const
{
    return palette_;
}

QList<QmTimelineItem::PropertyElement> QmTimelineItem::editableProperties() const
{
    QList<QmTimelineItem::PropertyElement> elements;
    {
        QmTimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("QmTimelineItem", "Enabled:");
        elmt.readonly = false;
        elmt.role = EnabledRole;
        elmt.editor_type = "CheckBox";
        elements.append(elmt);
    }
    return elements;
}

void QmTimelineItem::blockBuddyUpdate(int role)
{
    buddy_block_bitmap_ |= role;
}

void QmTimelineItem::unblockBuddyUpdate(int role)
{
    buddy_block_bitmap_ &= ~role;
}

void QmTimelineItem::insertBuddyUpdater(int role, const PropertyBuddy& buddy)
{
    buddy_updators_[role].push_back(buddy);
}

bool QmTimelineItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        QMTL_LOG_ERROR("Failed to load item. Exception: {}", except.what());
    }
    return false;
}

nlohmann::json QmTimelineItem::save() const
{
    nlohmann::json j;
    j["number"] = number_;
    j["start"] = start_;
    j["duration"] = duration_;
    return j;
}

void QmTimelineItem::notifyPropertyChanged(int role, const QVariant& old_value)
{
    model_->notifyItemPropertyChanged(item_id_, role, old_value);
}

void QmTimelineItem::updateBuddyProperty(int role, const QVariant& param)
{
    if (buddy_block_bitmap_ & role) {
        return;
    }

    auto it = buddy_updators_.find(role);
    if (it == buddy_updators_.end()) {
        return;
    }
    for (const auto& buddy : it->second) {
        blockBuddyUpdate(buddy.role);
        setProperty(buddy.role, buddy.recalc_func(this, param));
        unblockBuddyUpdate(buddy.role);
    }
}

void from_json(const nlohmann::json& j, QmTimelineItem& item)
{
    j["number"].get_to(item.number_);
    j["start"].get_to<qint64>(item.start_);
    j["duration"].get_to<qint64>(item.duration_);
}

} // namespace qmtl
