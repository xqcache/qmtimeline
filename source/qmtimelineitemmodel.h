#pragma once

#include "qmtimeline_global.h"
#include "qmtimelineserializable.h"
#include "qmtimelinetype.h"
#include <QObject>
#include <QVariant>

namespace qmtl {

class QmTimelineItem;
class QmTimelineItemFactory;
struct QmTimelineItemModelPrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineItemModel : public QObject, public QmTimelineSerializable {
    Q_OBJECT
public:
    explicit QmTimelineItemModel(QObject* parent = nullptr);
    ~QmTimelineItemModel() noexcept override;

    template <typename T>
        requires std::is_base_of_v<QmTimelineItem, T>
    T* item(QmItemID item_id) const
    {
        return static_cast<T*>(item(item_id));
    }

    QmTimelineItem* item(QmItemID item_id) const;
    QmTimelineItem* itemByStart(int row, qint64 start) const;
    QmItemID itemIdByStart(int row, qint64 start) const;
    bool exists(QmItemID item_id) const;
    inline constexpr static int itemRow(QmItemID item_id);
    inline constexpr static int itemType(QmItemID item_id);
    inline constexpr static QmItemID makeItemID(int item_type, int item_row, QmItemID id_index);

    bool isFrameRangeOccupied(int row, qint64 start, qint64 duration, QmItemID except_item = kInvalidItemID) const;

    void removeItem(QmItemID item_id);
    QmItemID createItem(int item_type, int item_row, qint64 start, qint64 duration = 0, bool with_connection = false);
    QmItemConnID createFrameConnection(QmItemID from, QmItemID to);
    QmItemConnID previousConnection(QmItemID item_id) const;
    QmItemConnID nextConnection(QmItemID item_id) const;
    bool hasConnection(QmItemID item_id) const;
    void removeFrameNextConn(QmItemID item_id);
    void removeFramePrevConn(QmItemID item_id);
    void removeFrameConn(QmItemID item_id);

    bool setItemProperty(QmItemID item_id, int role, const QVariant& data);
    std::optional<QVariant> itemProperty(QmItemID item_id, int role) const;
    bool requestItemOperate(QmItemID item_id, int op_role, const QVariant& param = QVariant());
    void notifyLanguageChanged();

    QmTimelineItemFactory* itemFactory() const;

    void setTypeHidden(int row, int type, bool hidden);
    bool isTypeHidden(int type) const;
    void setTypeLocked(int type, bool locked);
    bool isTypeLocked(int type) const;
    void setTypeDisabled(int type, bool disabled);
    bool isTypeDisabled(int type) const;

    bool isItemHidden(QmItemID item_id) const;
    bool isItemLocked(QmItemID item_id) const;
    bool isItemDisabled(QmItemID item_id) const;

    void setRowCount(int row_count);
    int rowCount() const;
    int rowItemCount(int row) const;

    void setFrameMaximum(qint64 maximum);
    void setFrameMinimum(qint64 minimum);
    qint64 frameMinimum() const;
    qint64 frameMaximum() const;
    void setViewFrameMaximum(qint64 maximum);
    void setViewFrameMinimum(qint64 minimum);
    qint64 viewFrameMinimum() const;
    qint64 viewFrameMaximum() const;

    void setFps(double fps);
    double fps() const;

    virtual void clear();
    bool isDirty() const;
    void setDirty(bool dirty = true);
    void resetDirty();

    void setItemHeight(qreal height);
    qreal itemHeight() const;
    qreal itemY(QmItemID item_id) const;

    bool isFrameInRange(qint64 start, qint64 duration = 0) const;
    bool isItemInViewRange(QmItemID item_id) const;

    bool modifyItemStart(QmItemID item_id, qint64 start);

    QmItemID headItem(int row) const;
    QmItemID tailItem(int row) const;
    QmItemID previousItem(QmItemID item_id) const;
    QmItemID nextItem(QmItemID item_id) const;
    std::map<qint64, QmItemID> rowItems(int row) const;

    void notifyItemPropertyChanged(QmItemID item_id, int role);
    void notifyItemOperateFinished(QmItemID item_id, int op_role, const QVariant& param = QVariant());

    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

    qint64 frameToTime(qint64 frame_no) const;

    QString copyItem(QmItemID item_id) const;
    QmItemID pasteItem(const QString& data, qint64 frame_no);

signals:
    void itemCreated(QmItemID item_id);
    void itemAboutToBeRemoved(QmItemID item_id);
    void itemRemoved(QmItemID item_id);
    void itemChanged(QmItemID item_id, int role);
    void itemOperateFinished(QmItemID item_id, int op_role, const QVariant& param = QVariant());

    void itemConnCreated(const QmItemConnID& conn_id);
    void itemConnRemoved(const QmItemConnID& conn_id);

    void requestRefreshItemViewCache(QmItemID item_id);
    void requestRebuildItemViewCache(QmItemID item_id);

    void rowCountChanged(int row_count);
    void requestUpdateItemY(QmItemID item_id);

    void frameMaximumChanged(qint64 maximum);
    void frameMinimumChanged(qint64 minimum);
    void viewFrameMaximumChanged(qint64 maximum);
    void viewFrameMinimumChanged(qint64 minimum);
    void fpsChanged(double fps);

    void errorOccurred(const QString& error);

protected:
    friend void from_json(const nlohmann::json& j, QmTimelineItemModel& item);

private:
    QmItemID nextItemID() const;

    friend class TimelineItemCreateCommand;
    friend class TimelineItemDeleteCommand;
    virtual void loadItem(
        const nlohmann::json& j, const std::optional<QmItemID>& item_id_opt = std::nullopt, const std::optional<qint64>& start = std::nullopt);
    virtual nlohmann::json saveItem(QmItemID item_id) const;

private:
    QmTimelineItemModelPrivate* d_ { nullptr };
};

inline constexpr int QmTimelineItemModel::itemRow(QmItemID item_id)
{
    return (item_id >> 48) & 0xFF;
}

inline constexpr int QmTimelineItemModel::itemType(QmItemID item_id)
{
    return (item_id >> 56) & 0x7F;
}

inline constexpr QmItemID QmTimelineItemModel::makeItemID(int item_type, int item_row, QmItemID id_index)
{
    return ((static_cast<QmItemID>(item_type) & 0x7F) << 56) | ((static_cast<QmItemID>(item_row) & 0xFF) << 48) | ((id_index << 16) >> 16);
}

} // namespace qmtl
