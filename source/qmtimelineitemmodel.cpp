#include "qmtimelineitemmodel.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemfactory.h"
#include "qmtimelinelog.h"
#include "qmtimelineutil.h"
#include <set>

namespace nlohmann {
void from_json(const nlohmann::json& j, qmtl::QmItemConnID& conn_id)
{
    conn_id.from = j[0];
    conn_id.to = j[1];
}

void to_json(nlohmann::json& j, const qmtl::QmItemConnID& conn_id)
{
    j.emplace_back(conn_id.from);
    j.emplace_back(conn_id.to);
}
} // namespace nlohmann

namespace qmtl {

struct QmTimelineItemModelPrivate {
    std::map<QmItemID, std::unique_ptr<QmTimelineItem>> items;
    // {row: {start: item_id}}
    std::map<int, std::map<qint64, QmItemID>> item_table;
    // {row: {item_id: start}}
    std::map<int, std::unordered_map<QmItemID, qint64>> item_table_helper;
    std::set<int> hidden_types;
    std::set<int> locked_types;
    std::set<int> disabled_types;
    std::map<QmItemID, QmItemConnID> next_conns;
    std::map<QmItemID, QmItemConnID> prev_conns;
    int row_count { 1 };
    QmItemID id_index { 1 };
    std::array<qint64, 2> frame_range { 0, 1 };
    std::array<qint64, 2> view_frame_range { 0, 1 };
    double fps { 24.0 };

    bool dirty { false };
    qreal item_height { 40 };
};

QmTimelineItemModel::QmTimelineItemModel(QObject* parent)
    : QObject(parent)
    , d_(new QmTimelineItemModelPrivate)
{
}

QmTimelineItemModel::~QmTimelineItemModel() noexcept
{
    delete d_;
}

QmTimelineItem* QmTimelineItemModel::item(QmItemID item_id) const
{
    auto it = d_->items.find(item_id);
    if (it == d_->items.end()) {
        return nullptr;
    }
    return it->second.get();
}

QmTimelineItem* QmTimelineItemModel::itemByStart(int row, qint64 start) const
{
    auto item_id = itemIdByStart(row, start);
    if (item_id == kInvalidItemID) {
        return nullptr;
    }
    return item(item_id);
}

QmItemID QmTimelineItemModel::itemIdByStart(int row, qint64 start) const
{
    if (row < 0 || row >= d_->row_count) {
        return kInvalidItemID;
    }
    auto it = d_->item_table[row].lower_bound(start);
    if (it == d_->item_table[row].end()) {
        return kInvalidItemID;
    }
    return it->second;
}

bool QmTimelineItemModel::exists(QmItemID item_id) const
{
    return d_->items.contains(item_id);
}

bool QmTimelineItemModel::isFrameRangeOccupied(int row, qint64 start, qint64 duration, QmItemID except_item) const
{
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return false;
    }

    if (row_it->second.empty()) {
        return false;
    }

    auto frame_it = row_it->second.lower_bound(start);
    // 如果添加至末尾了
    if (frame_it == row_it->second.end()) {
        auto* tail_item = item(row_it->second.rbegin()->second);
        return tail_item && (start <= tail_item->start() + tail_item->duration());
    }

    if (frame_it->second != except_item) {
        if (start == frame_it->first || (start + duration >= frame_it->first)) {
            return true;
        }
    }

    if (auto next_frame_it = std::next(frame_it); next_frame_it != row_it->second.end()) {
        auto* next_item = item(next_frame_it->second);
        if (next_item != nullptr && start + duration >= next_item->start()) {
            return true;
        }
    }

    if (frame_it != row_it->second.begin()) {
        auto* prev_item = item(std::prev(frame_it)->second);
        if (prev_item != nullptr && start <= prev_item->start() + prev_item->duration()) {
            return true;
        }
    }

    return false;
}

QmItemID QmTimelineItemModel::createItem(int item_type, int row, qint64 start, qint64 duration, bool with_connection)
{
    if (row < 0 || row >= d_->row_count) {
        QMTL_LOG_ERROR("Failed to create frame item. Invalid row[{}], it must between 0 and {}", row, d_->row_count);
        return kInvalidItemID;
    }

    if (isFrameRangeOccupied(row, start, duration)) {
        QMTL_LOG_ERROR("The time period has been occupied.");
        return kInvalidItemID;
    }

    // 构造item_id
    QmItemID item_id = makeItemID(item_type, row, d_->id_index);
    auto item = QmTimelineItemFactory::instance().createItem(item_id, this);
    if (!item) {
        return kInvalidItemID;
    }

    // 获取插入位置的item序号，同时修改插入位置之后的item序号
    std::optional<int> number_opt;
    if (d_->item_table.contains(row)) {
        // 插入位置之后的item对应编号加一
        for (auto it = d_->item_table[row].upper_bound(start); it != d_->item_table[row].end(); ++it) {
            if (!number_opt) {
                auto item_number_opt = itemProperty(it->second, QmTimelineItem::NumberRole);
                if (item_number_opt.has_value()) {
                    number_opt = item_number_opt->value<int>();
                }
            }
            requestItemOperate(it->second, QmTimelineItem::OperationRole::OpIncreaseNumberRole, 1);
        }
    }

    QmItemID old_head = kInvalidItemID;
    QmItemID old_tail = kInvalidItemID;
    if (number_opt.has_value()) {
        if (*number_opt == 1) {
            old_head = headItem(row);
        }
    } else {
        old_tail = tailItem(row);
    }

    // 设置新item属性
    item->setNumber(number_opt.value_or(d_->item_table[row].size() + 1));
    item->setStart(start);
    item->setDuration(duration);

    // 登记item
    d_->id_index++;
    d_->items[item_id] = std::move(item);
    d_->dirty = true;
    d_->item_table[row][start] = item_id;
    d_->item_table_helper[row][item_id] = start;
    emit itemCreated(item_id);

    if (headItem(row) == item_id) {
        requestItemOperate(item_id, QmTimelineItem::OperationRole::OpUpdateAsHead);
    } else if (tailItem(row) == item_id) {
        requestItemOperate(item_id, QmTimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (old_head != kInvalidItemID) {
        requestItemOperate(old_head, QmTimelineItem::OperationRole::OpUpdateAsHead);
    } else if (old_tail != kInvalidItemID) {
        requestItemOperate(old_tail, QmTimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (with_connection) {
        // 增加Connection
        auto prev_item_id = previousItem(item_id);
        if (prev_item_id != kInvalidItemID) {
            removeFrameNextConn(prev_item_id);
            createFrameConnection(prev_item_id, item_id);
        }
        auto next_item_id = nextItem(item_id);
        if (next_item_id != kInvalidItemID) {
            removeFramePrevConn(next_item_id);
            createFrameConnection(item_id, next_item_id);
        }
    }

    return item_id;
}

void QmTimelineItemModel::removeItem(QmItemID item_id)
{
    auto item_it = d_->items.find(item_id);
    if (item_it == d_->items.end()) {
        return;
    }
    // 从item_sorts中删除item_id
    int row = itemRow(item_id);
    if (row < 0) {
        return;
    }

    emit itemAboutToBeRemoved(item_id);

    QmItemID new_head = kInvalidItemID;
    QmItemID new_tail = kInvalidItemID;

    QmItemID prev_item = previousItem(item_id);
    QmItemID next_item = nextItem(item_id);

    if (item_id == tailItem(row)) {
        new_tail = prev_item;
    } else if (item_id == headItem(row)) {
        new_head = next_item;
    }

    // 删除旧连接
    removeFrameConn(item_id);
    // 绑定新连接
    if (prev_item != kInvalidItemID && next_item != kInvalidItemID) {
        createFrameConnection(prev_item, next_item);
    }

    if (auto helper_row_it = d_->item_table_helper.find(row); helper_row_it != d_->item_table_helper.end()) {
        if (auto item_origin_it = helper_row_it->second.find(item_id); item_origin_it != helper_row_it->second.end()) {
            // 后续受影响的item序号需要重新设置
            for (auto it = d_->item_table[row].upper_bound(item_origin_it->second); it != d_->item_table[row].end(); ++it) {
                requestItemOperate(it->second, QmTimelineItem::OperationRole::OpDecreaseNumberRole, 1);
            }
            if (auto row_it = d_->item_table.find(row); row_it != d_->item_table.end()) {
                if (auto origin_item_it = row_it->second.find(item_origin_it->second); origin_item_it != row_it->second.end()) {
                    row_it->second.erase(origin_item_it);
                    if (row_it->second.empty()) {
                        d_->item_table.erase(row_it);
                    }
                }
            }
            helper_row_it->second.erase(item_origin_it);
            if (helper_row_it->second.empty()) {
                d_->item_table_helper.erase(helper_row_it);
            }
        }
    }
    d_->items.erase(item_it);
    if (new_head != kInvalidItemID) {
        requestItemOperate(new_head, QmTimelineItem::OperationRole::OpUpdateAsHead);
    } else if (new_tail != kInvalidItemID) {
        requestItemOperate(new_head, QmTimelineItem::OperationRole::OpUpdateAsTail);
    }
    emit itemRemoved(item_id);
    setDirty();
}

QmItemConnID QmTimelineItemModel::previousConnection(QmItemID item_id) const
{
    auto it = d_->prev_conns.find(item_id);
    if (it == d_->prev_conns.end()) {
        return {};
    }
    return it->second;
}

QmItemConnID QmTimelineItemModel::nextConnection(QmItemID item_id) const
{
    auto it = d_->next_conns.find(item_id);
    if (it == d_->next_conns.end()) {
        return {};
    }
    return it->second;
}

bool QmTimelineItemModel::hasConnection(QmItemID item_id) const
{
    return QmTimelineItemFactory::instance().itemHasConnection(item_id);
}

QmItemConnID QmTimelineItemModel::createFrameConnection(QmItemID from, QmItemID to)
{
    if (!exists(from) || !exists(to)) {
        return {};
    }

    QmItemConnID conn_id { .from = from, .to = to };
    d_->next_conns[from] = conn_id;
    d_->prev_conns[to] = conn_id;
    emit itemConnCreated(conn_id);
    return conn_id;
}

void QmTimelineItemModel::removeFrameConn(QmItemID item_id)
{
    removeFramePrevConn(item_id);
    removeFrameNextConn(item_id);
}

void QmTimelineItemModel::removeFrameNextConn(QmItemID item_id)
{
    auto it = d_->next_conns.find(item_id);
    if (it == d_->next_conns.end()) {
        return;
    }
    auto conn_id = it->second;
    d_->next_conns.erase(it);
    auto prev_it = d_->prev_conns.find(conn_id.to);
    if (prev_it != d_->prev_conns.end()) {
        d_->prev_conns.erase(prev_it);
    }
    emit itemConnRemoved(conn_id);
}

void QmTimelineItemModel::removeFramePrevConn(QmItemID item_id)
{
    auto it = d_->prev_conns.find(item_id);
    if (it == d_->prev_conns.end()) {
        return;
    }
    auto conn_id = it->second;
    d_->prev_conns.erase(it);
    auto prev_it = d_->next_conns.find(conn_id.from);
    if (prev_it != d_->next_conns.end()) {
        d_->next_conns.erase(prev_it);
    }
    emit itemConnRemoved(conn_id);
}

bool QmTimelineItemModel::setItemProperty(QmItemID item_id, int role, const QVariant& data)
{
    auto* item = this->item(item_id);
    if (!item) {
        return false;
    }
    return item->setProperty(role, data);
}

std::optional<QVariant> QmTimelineItemModel::itemProperty(QmItemID item_id, int role) const
{
    auto* item = this->item(item_id);
    if (!item) {
        return std::nullopt;
    }
    return item->property(role);
}

bool QmTimelineItemModel::requestItemOperate(QmItemID item_id, int op_role, const QVariant& param)
{
    auto* item = this->item(item_id);
    if (!item) {
        return {};
    }
    return item->operate(op_role, param);
}

void QmTimelineItemModel::setTypeHidden(int row, int type, bool hidden)
{
    if (hidden) {
        if (d_->hidden_types.contains(type)) {
            return;
        }
        d_->hidden_types.emplace(type);
    } else {
        if (!d_->hidden_types.contains(type)) {
            return;
        }
        d_->hidden_types.erase(type);
    }

    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end()) {
        it = d_->item_table.upper_bound(row);
    }
    for (; it != d_->item_table.end(); ++it) {
        for (const auto& [_, item_id] : it->second) {
            emit requestUpdateItemY(item_id);
        }
    }
    setDirty();
}

bool QmTimelineItemModel::isDirty() const
{
    return d_->dirty || std::any_of(d_->items.cbegin(), d_->items.cend(), [](const auto& pair) { return pair.second->isDirty(); });
}

void QmTimelineItemModel::setDirty(bool dirty)
{
    d_->dirty = dirty;
}

void QmTimelineItemModel::resetDirty()
{
    d_->dirty = false;
    std::for_each(d_->items.begin(), d_->items.end(), [](const auto& pair) { pair.second->resetDirty(); });
}

bool QmTimelineItemModel::isTypeHidden(int type) const
{
    return d_->hidden_types.contains(type);
}

bool QmTimelineItemModel::isItemHidden(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return true;
    }
    return d_->hidden_types.contains(itemType(item_id));
}

void QmTimelineItemModel::setTypeLocked(int type, bool locked)
{
    if (locked) {
        d_->locked_types.emplace(type);
    } else {
        d_->locked_types.erase(type);
    }
    setDirty();
}

bool QmTimelineItemModel::isTypeLocked(int type) const
{
    return d_->locked_types.contains(type);
}

bool QmTimelineItemModel::isItemLocked(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return true;
    }
    return isTypeLocked(itemType(item_id));
}

bool QmTimelineItemModel::isTypeDisabled(int type) const
{
    return d_->disabled_types.contains(type);
}

bool QmTimelineItemModel::isItemDisabled(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return true;
    }
    return isTypeDisabled(itemType(item_id));
}

void QmTimelineItemModel::setTypeDisabled(int type, bool disabled)
{
    if (disabled) {
        d_->disabled_types.emplace(type);
    } else {
        d_->disabled_types.erase(type);
    }
    setDirty();
}
void QmTimelineItemModel::setRowCount(int row_count)
{
    if (row_count == d_->row_count) {
        return;
    }
    d_->row_count = row_count;
    emit rowCountChanged(row_count);
}

int QmTimelineItemModel::rowCount() const
{
    return d_->row_count;
}

int QmTimelineItemModel::rowItemCount(int row) const
{
    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end()) {
        return 0;
    }
    return it->second.size();
}

void QmTimelineItemModel::setItemHeight(qreal height)
{
    d_->item_height = height;
}

qreal QmTimelineItemModel::itemHeight() const
{
    return d_->item_height;
}

qreal QmTimelineItemModel::itemY(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return -2 * d_->item_height;
    }
    int item_row = itemRow(item_id);
    if (item_row < 0 || item_row >= d_->row_count) {
        return -2 * d_->item_height;
    }
    if (isTypeHidden(itemType(item_id))) {
        return -2 * d_->item_height;
    }

    int hidden_count = std::distance(d_->hidden_types.begin(), d_->hidden_types.lower_bound(item_row));
    return (item_row - hidden_count) * d_->item_height;
}

QmItemID QmTimelineItemModel::headItem(int row) const
{
    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end() || it->second.empty()) {
        return kInvalidItemID;
    }
    return it->second.begin()->second;
}

QmItemID QmTimelineItemModel::tailItem(int row) const
{
    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end() || it->second.empty()) {
        return kInvalidItemID;
    }
    return std::prev(it->second.end())->second;
}

QmItemID QmTimelineItemModel::previousItem(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return kInvalidItemID;
    }
    int row = itemRow(item_id);
    if (row < 0) {
        return kInvalidItemID;
    }

    auto helper_row_it = d_->item_table_helper.find(row);
    if (helper_row_it == d_->item_table_helper.end()) {
        return kInvalidItemID;
    }
    auto start_it = helper_row_it->second.find(item_id);
    if (start_it == helper_row_it->second.end()) {
        return kInvalidItemID;
    }
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return kInvalidItemID;
    }
    auto item_it = row_it->second.find(start_it->second);
    if (item_it == row_it->second.end() || item_it == row_it->second.begin()) {
        return kInvalidItemID;
    }
    auto prev_it = std::prev(item_it);
    return prev_it->second;
}

QmItemID QmTimelineItemModel::nextItem(QmItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return kInvalidItemID;
    }
    int row = itemRow(item_id);
    if (row < 0) {
        return kInvalidItemID;
    }

    auto helper_row_it = d_->item_table_helper.find(row);
    if (helper_row_it == d_->item_table_helper.end()) {
        return kInvalidItemID;
    }
    auto origin_it = helper_row_it->second.find(item_id);
    if (origin_it == helper_row_it->second.end()) {
        return kInvalidItemID;
    }
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return kInvalidItemID;
    }
    auto item_it = row_it->second.find(origin_it->second);
    if (item_it == row_it->second.end()) {
        return kInvalidItemID;
    }

    auto next_it = std::next(item_it);
    if (next_it == row_it->second.end()) {
        return kInvalidItemID;
    }
    return next_it->second;
}

std::map<qint64, QmItemID> QmTimelineItemModel::rowItems(int row) const
{
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return {};
    }
    return row_it->second;
}

void QmTimelineItemModel::notifyItemPropertyChanged(QmItemID item_id, int role)
{
    if (!d_->items.contains(item_id)) {
        return;
    }
    emit itemChanged(item_id, role);
}

void QmTimelineItemModel::notifyItemOperateFinished(QmItemID item_id, int op_role, const QVariant& param)
{
    if (!d_->items.contains(item_id)) {
        return;
    }
    emit itemOperateFinished(item_id, op_role, param);
}

void QmTimelineItemModel::setFrameMaximum(qint64 maximum)
{
    if (maximum == d_->frame_range[1] || maximum < d_->frame_range[0] + 1) {
        return;
    }
    d_->frame_range[1] = maximum;
    setDirty(true);
    emit frameMaximumChanged(maximum);
}

void QmTimelineItemModel::setFrameMinimum(qint64 minimum)
{
    if (minimum == d_->frame_range[0] || minimum > d_->frame_range[1] - 1) {
        return;
    }
    d_->frame_range[0] = minimum;
    setDirty(true);
    emit frameMinimumChanged(minimum);
}

qint64 QmTimelineItemModel::frameMinimum() const
{
    return d_->frame_range[0];
}

qint64 QmTimelineItemModel::frameMaximum() const
{
    return d_->frame_range[1];
}

bool QmTimelineItemModel::isFrameInRange(qint64 start, qint64 duration) const
{
    return start >= d_->view_frame_range[0] && start + duration <= d_->view_frame_range[1];
}

bool QmTimelineItemModel::isItemInViewRange(QmItemID item_id) const
{
    auto* item = this->item(item_id);
    if (!item) {
        return false;
    }

    return (item->start() >= d_->view_frame_range[0] && item->start() <= d_->view_frame_range[1])
        || (item->start() + item->duration() >= d_->view_frame_range[0]);
}

bool QmTimelineItemModel::modifyItemStart(QmItemID item_id, qint64 start)
{
    auto* item = this->item(item_id);
    if (!item) {
        return false;
    }
    if (item->start() == start) {
        return false;
    }
    if (auto prev_item_id = previousItem(item_id); prev_item_id != kInvalidItemID) {
        auto* prev_item = this->item(prev_item_id);
        if (prev_item != nullptr && prev_item->start() + prev_item->duration() >= start) {
            start = prev_item->start() + prev_item->duration() + 1;
        }
    }

    if (auto next_item_id = nextItem(item_id); next_item_id != kInvalidItemID) {
        auto* next_item = this->item(next_item_id);
        if (next_item != nullptr && next_item->start() <= start + item->duration()) {
            start = next_item->start() - item->duration() - 1;
        }
    }

    if (item->start() == start) {
        return false;
    }

    int item_row = itemRow(item_id);
    auto row_it = d_->item_table.find(item_row);
    if (row_it == d_->item_table.end()) [[unlikely]] {
        return false;
    }

    auto helper_row_it = d_->item_table_helper.find(item_row);
    if (helper_row_it == d_->item_table_helper.end()) [[unlikely]] {
        return false;
    }

    auto start_it = row_it->second.find(item->start());
    if (start_it != row_it->second.end()) {
        row_it->second.erase(start_it);
    }
    row_it->second[start] = item_id;
    helper_row_it->second[item_id] = start;

    item->setStart(start);
    return true;
}

void QmTimelineItemModel::setFps(double fps)
{
    if (qFuzzyCompare(d_->fps, fps) && fps > 0) {
        return;
    }
    d_->fps = fps;
    setDirty(true);
    emit fpsChanged(fps);
}

double QmTimelineItemModel::fps() const
{
    return d_->fps;
}

void QmTimelineItemModel::setViewFrameMaximum(qint64 maximum)
{
    if (maximum == d_->view_frame_range[1] || maximum < d_->view_frame_range[0] + 1) {
        return;
    }
    d_->view_frame_range[1] = maximum;
    setDirty(true);
    emit viewFrameMaximumChanged(maximum);
}

void QmTimelineItemModel::setViewFrameMinimum(qint64 minimum)
{
    if (minimum == d_->view_frame_range[0] || minimum > d_->view_frame_range[1] - 1) {
        return;
    }
    d_->view_frame_range[0] = minimum;
    setDirty(true);
    emit viewFrameMinimumChanged(minimum);
}

qint64 QmTimelineItemModel::viewFrameMinimum() const
{
    return d_->view_frame_range[0];
}

qint64 QmTimelineItemModel::viewFrameMaximum() const
{
    return d_->view_frame_range[1];
}

void QmTimelineItemModel::clear()
{
    std::set<QmItemID> item_ids;
    std::transform(d_->items.cbegin(), d_->items.cend(), std::inserter(item_ids, item_ids.begin()), [](const auto& pair) { return pair.first; });
    for (const auto& item_id : item_ids) {
        removeItem(item_id);
    }
    d_->id_index = 0;
    d_->dirty = false;
    d_->hidden_types.clear();
    d_->locked_types.clear();
    d_->disabled_types.clear();
}

qint64 QmTimelineItemModel::frameToTime(qint64 frame_no) const
{
    return QmTimelineUtil::frameToTime(frame_no, d_->fps);
}

bool QmTimelineItemModel::load(const nlohmann::json& j)
{
    try {
        clear();
        from_json(j, *this);
        return true;
    } catch (const std::exception& excep) {
        QMTL_LOG_ERROR("Failed to load item. Exception: {}", excep.what());
    }
    return false;
}

nlohmann::json QmTimelineItemModel::save() const
{
    nlohmann::json j;

    j["id_index"] = d_->id_index;
    j["row_count"] = d_->row_count;
    j["item_table"] = d_->item_table;
    j["item_table_helper"] = d_->item_table_helper;
    j["hidden_rows"] = d_->hidden_types;
    j["locked_rows"] = d_->locked_types;
    j["disabled_rows"] = d_->disabled_types;
    j["frame_range"] = d_->frame_range;
    j["view_frame_range"] = d_->view_frame_range;

    nlohmann::json items_j;
    for (const auto& [item_id, item_ptr] : d_->items) {
        nlohmann::json item_j;
        item_j["id"] = item_id;
        item_j["data"] = item_ptr->save();
        items_j.emplace_back(item_j);
    }
    j["items"] = items_j;

    nlohmann::json prev_conns_j;
    for (const auto& [item_id, conn_id] : d_->prev_conns) {
        nlohmann::json conn_item_j;
        conn_item_j["item_id"] = item_id;
        conn_item_j["connection"] = conn_id;
        prev_conns_j.emplace_back(conn_item_j);
    }
    j["prev_conns"] = prev_conns_j;

    nlohmann::json next_conns_j;
    for (const auto& [item_id, conn_id] : d_->next_conns) {
        nlohmann::json conn_item_j;
        conn_item_j["item_id"] = item_id;
        conn_item_j["connection"] = conn_id;
        next_conns_j.emplace_back(conn_item_j);
    }
    j["next_conns"] = next_conns_j;
    return j;
}

QString QmTimelineItemModel::copyItem(QmItemID item_id) const
{
    nlohmann::json j = saveItem(item_id);
    return QString::fromStdString(j.dump());
}

QmItemID QmTimelineItemModel::pasteItem(const QString& data, qint64 frame_no)
{
    try {
        auto j = nlohmann::json::parse(data.toStdString());
        if (!j.contains("id")) {
            return kInvalidItemID;
        }

        auto old_item_id = j["id"].get<QmItemID>();
        int row = itemRow(old_item_id);
        int type = itemType(old_item_id);

        QmItemID item_id = makeItemID(type, row, d_->id_index);
        loadItem(j, item_id, frame_no);
        ++d_->id_index;
        return item_id;
    } catch (const std::exception& excep) {
        QMTL_LOG_ERROR("Failed to parse item data. Exception: {}", excep.what());
        return kInvalidItemID;
    }
    return kInvalidItemID;
}

void QmTimelineItemModel::loadItem(const nlohmann::json& j, const std::optional<QmItemID>& item_id_opt, const std::optional<qint64>& start)
{
    QmItemID item_id = item_id_opt.value_or(j["id"]);
    if (exists(item_id)) {
        return;
    }

    int row = itemRow(item_id);
    auto item = QmTimelineItemFactory::instance().createItem(item_id, this);
    if (!item) {
        throw std::exception(std::format("create item[{}] failed!", item_id).c_str());
    }
    if (!item->load(j["data"])) {
        throw std::exception(std::format("load item[{}] failed!", item_id).c_str());
    }

    if (start.has_value()) {
        item->setStart(*start);
    }

    if (isFrameRangeOccupied(row, item->start(), item->duration())) {
        emit errorOccurred(tr("Another frame already exists in the current location!"));
        throw std::exception(std::format("frame range is occupied!").c_str());
    }

    // 获取插入位置的item序号，同时修改插入位置之后的item序号
    std::optional<int> number_opt;
    if (d_->item_table.contains(row)) {
        // 插入位置之后的item对应编号加一
        for (auto it = d_->item_table[row].upper_bound(item->start()); it != d_->item_table[row].end(); ++it) {
            if (!number_opt) {
                auto item_number_opt = itemProperty(it->second, QmTimelineItem::NumberRole);
                if (item_number_opt.has_value()) {
                    number_opt = item_number_opt->value<int>();
                }
            }
            requestItemOperate(it->second, QmTimelineItem::OperationRole::OpIncreaseNumberRole, 1);
        }
    }

    QmItemID old_head = kInvalidItemID;
    QmItemID old_tail = kInvalidItemID;
    if (number_opt.has_value()) {
        if (*number_opt == 1) {
            old_head = headItem(row);
        }
    } else {
        old_tail = tailItem(row);
    }

    item->setNumber(number_opt.value_or(d_->item_table[row].size() + 1));
    // 登记item
    d_->dirty = true;
    d_->item_table[row][item->start()] = item_id;
    d_->item_table_helper[row][item_id] = item->start();
    d_->items[item_id] = std::move(item);
    emit itemCreated(item_id);

    if (headItem(row) == item_id) {
        requestItemOperate(item_id, QmTimelineItem::OperationRole::OpUpdateAsHead);
    } else if (tailItem(row) == item_id) {
        requestItemOperate(item_id, QmTimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (old_head != kInvalidItemID) {
        requestItemOperate(old_head, QmTimelineItem::OperationRole::OpUpdateAsHead);
    } else if (old_tail != kInvalidItemID) {
        requestItemOperate(old_tail, QmTimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (j.contains("with_connection") && j["with_connection"].get<bool>()) {
        // 增加Connection
        auto prev_item_id = previousItem(item_id);
        if (prev_item_id != kInvalidItemID) {
            removeFrameNextConn(prev_item_id);
            createFrameConnection(prev_item_id, item_id);
        }
        auto next_item_id = nextItem(item_id);
        if (next_item_id != kInvalidItemID) {
            removeFramePrevConn(next_item_id);
            createFrameConnection(item_id, next_item_id);
        }
    }
    emit requestRebuildItemViewCache(item_id);
}

nlohmann::json QmTimelineItemModel::saveItem(QmItemID item_id) const
{
    nlohmann::json item_j;
    auto it = d_->items.find(item_id);
    if (it == d_->items.end()) {
        return item_j;
    }
    item_j["id"] = item_id;
    item_j["data"] = it->second->save();
    item_j["with_connection"] = hasConnection(item_id);
    return item_j;
}

void from_json(const nlohmann::json& j, QmTimelineItemModel& model)
{
    j["id_index"].get_to(model.d_->id_index);
    j["row_count"].get_to(model.d_->row_count);
    j["item_table"].get_to(model.d_->item_table);
    j["item_table_helper"].get_to(model.d_->item_table_helper);
    j["hidden_rows"].get_to(model.d_->hidden_types);
    j["locked_rows"].get_to(model.d_->locked_types);
    if (j.contains("disabled_rows")) {
        j["disabled_rows"].get_to(model.d_->disabled_types);
    }
    j["frame_range"].get_to(model.d_->frame_range);
    j["view_frame_range"].get_to(model.d_->view_frame_range);

    nlohmann::json items_j = j["items"];
    for (const auto& item_j : items_j) {
        QmItemID item_id = item_j["id"];
        auto item = QmTimelineItemFactory::instance().createItem(item_id, &model);
        if (!item) {
            throw std::exception(std::format("create item[{}] failed!", item_id).c_str());
        }
        if (!item->load(item_j["data"])) {
            throw std::exception(std::format("load item[{}] failed!", item_id).c_str());
        }
        model.d_->items[item_id] = std::move(item);
        emit model.itemCreated(item_id);
        emit model.requestRebuildItemViewCache(item_id);
    }

    nlohmann::json prev_conns_j = j["prev_conns"];
    for (const auto& conn_item_j : prev_conns_j) {
        QmItemID item_id = conn_item_j["item_id"];
        QmItemConnID conn_id = conn_item_j["connection"];
        model.d_->prev_conns[item_id] = conn_id;
    }

    nlohmann::json next_conns_j = j["next_conns"];
    for (const auto& conn_item_j : next_conns_j) {
        QmItemID item_id = conn_item_j["item_id"];
        QmItemConnID conn_id = conn_item_j["connection"];
        model.d_->next_conns[item_id] = conn_id;
        emit model.itemConnCreated(conn_id);
    }

    // 刷新每一行的头尾节点
    for (const auto& [_, items] : model.d_->item_table) {
        size_t item_size = items.size();
        if (item_size == 0) {
            continue;
        }
        emit model.notifyItemOperateFinished(items.begin()->second, QmTimelineItem::OpUpdateAsHead);
        if (auto tail_it = std::prev(items.end()); tail_it != items.begin()) {
            emit model.notifyItemOperateFinished(tail_it->second, QmTimelineItem::OpUpdateAsTail);
        }
    }

    // 通知Frame Range改变
    emit model.frameMaximumChanged(model.d_->frame_range[1]);
    emit model.frameMinimumChanged(model.d_->frame_range[0]);
    emit model.viewFrameMaximumChanged(model.d_->view_frame_range[1]);
    emit model.viewFrameMinimumChanged(model.d_->view_frame_range[0]);
    emit model.fpsChanged(model.d_->fps);
}

void QmTimelineItemModel::notifyLanguageChanged()
{
    for (const auto& [item_id, item_ptr] : d_->items) {
        emit itemChanged(item_id, QmTimelineItem::ToolTipRole);
    }
}
} // namespace qmtl
