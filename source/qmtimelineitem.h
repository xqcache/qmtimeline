#pragma once

#include "qmtimeline_global.h"
#include "qmtimelineserializable.h"
#include "qmtimelinetype.h"
#include <QObject>
#include <QPalette>
#include <QVariant>

namespace qmtl {

class QmTimelineItemModel;

class QMTIMELINE_LIB_EXPORT QmTimelineItem : public QmTimelineSerializable {
public:
    enum PropertyRole : int {
        NoneRole = 0,
        StartRole = 0x01,
        DurationRole = 0x02,
        NumberRole = 0x04,
        ToolTipRole = 0x08,
        EnabledRole = 0x10,
        AllRole = std::numeric_limits<int>::max()
    };

    enum OperationRole : int {
        OpIncreaseNumberRole = 0x01,
        OpDecreaseNumberRole = 0x02,
        OpUpdateAsHead = 0x04,
        OpUpdateAsTail = 0x08,
    };

    enum Type : int {
        None = 0,
        Type = 1,
        UserType = 2,
    };

    struct PropertyElement {
        QString label;
        int role;
        bool readonly;
        QString editor_type;
        std::map<QString, QVariant> editor_properties;
        // 用于更新关联的属性（指定QWidget的数据属性名称
        std::map<int, QString> buddy_value_qproperty_names;
    };

    struct PropertyBuddy {
        int role;
        std::function<QVariant(QmTimelineItem* item, const QVariant&)> recalc_func;
    };

    QmTimelineItem(QmItemID item_id, QmTimelineItemModel* model);
    virtual ~QmTimelineItem() = default;

    inline QmTimelineItemModel* model() const;

    inline qint64 start() const;
    inline qint64 end() const;
    inline qint64 duration() const;
    inline qint64 destination() const;
    inline QmItemID itemId() const;

    virtual void setNumber(int number);
    virtual void setStart(qint64 frame_no);
    virtual void setDuration(qint64 frame_count);

    inline bool isDirty() const;
    inline void setDirty(bool dirty);
    inline void resetDirty();
    inline int number() const;

    virtual bool isValid() const;
    inline bool isEnabled() const;
    void setEnabled(bool enabled);

    const QPalette& palette() const;

    virtual int type() const;
    virtual const char* typeName() const = 0;
    virtual QString toolTip() const;

    virtual bool setProperty(int role, const QVariant& data);
    virtual std::optional<QVariant> property(int role) const;

    virtual bool operate(int op_role, const QVariant& param);

    virtual QList<PropertyElement> editableProperties() const;

    void insertBuddyUpdater(int role, const PropertyBuddy& buddy);

    inline const std::unordered_map<int, std::vector<PropertyBuddy>>& buddyUpdaters() const;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

protected:
    inline constexpr static PropertyRole userRole(qint64 index);

    virtual void updateBuddyProperty(int role, const QVariant& param);

    void notifyPropertyChanged(int role);
    void blockBuddyUpdate(int role);
    void unblockBuddyUpdate(int role);

protected:
    friend void from_json(const nlohmann::json& j, QmTimelineItem& item);
    QPalette palette_;
    // 数据部分
    int number_ { 0 };
    // 起始帧
    qint64 start_ { 0 };
    // 持续帧数
    qint64 duration_ { 0 };

    bool enabled_ { true };

    std::unordered_map<int, std::vector<PropertyBuddy>> buddy_updators_;

private:
    Q_DISABLE_COPY(QmTimelineItem)
    QmItemID item_id_ { kInvalidItemID };
    QmTimelineItemModel* model_ { nullptr };
    bool dirty_ { false };
    std::underlying_type_t<PropertyRole> buddy_block_bitmap_ { 0 };
};

inline QmTimelineItemModel* QmTimelineItem::model() const
{
    return model_;
}

inline qint64 QmTimelineItem::start() const
{
    return start_;
}

inline qint64 QmTimelineItem::end() const
{
    return start_ + duration_;
}

inline qint64 QmTimelineItem::duration() const
{
    return duration_;
}

inline qint64 QmTimelineItem::destination() const
{
    return start_ + duration_;
}

inline QmItemID QmTimelineItem::itemId() const
{
    return item_id_;
}

inline bool QmTimelineItem::isDirty() const
{
    return dirty_;
}

inline void QmTimelineItem::setDirty(bool dirty)
{
    dirty_ = true;
}

inline void QmTimelineItem::resetDirty()
{
    dirty_ = false;
}

inline bool QmTimelineItem::isEnabled() const
{
    return enabled_;
}

inline int QmTimelineItem::number() const
{
    return number_;
}

inline constexpr QmTimelineItem::PropertyRole QmTimelineItem::userRole(qint64 index)
{
    assert(index < 32 && "The role must be less than 32.");
    return static_cast<PropertyRole>(1 << (index + 6));
}

inline const std::unordered_map<int, std::vector<QmTimelineItem::PropertyBuddy>>& QmTimelineItem::buddyUpdaters() const
{
    return buddy_updators_;
}

} // namespace qmtl
