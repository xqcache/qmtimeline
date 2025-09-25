#pragma once

#include "nlohmann/json.hpp"
#include "qmtimelinetype.h"
#include <QUndoCommand>

namespace qmtl {

class QmTimelineItemModel;

class QmTimelineItemCreateCommand : public QUndoCommand {
public:
    explicit QmTimelineItemCreateCommand(QmTimelineItemModel* model, QmItemID item_id, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QmTimelineItemModel* model_ { nullptr };
    QmItemID item_id_ { kInvalidItemID };
    nlohmann::json item_data_;
};

class QmTimelineItemDeleteCommand : public QUndoCommand {
public:
    explicit QmTimelineItemDeleteCommand(QmTimelineItemModel* model, QmItemID item_id, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QmTimelineItemModel* model_ { nullptr };
    QmItemID item_id_ { kInvalidItemID };
    nlohmann::json item_data_;
};

class QmTimelineItemMoveCommand : public QUndoCommand {
public:
    explicit QmTimelineItemMoveCommand(QmTimelineItemModel* model, QmItemID item_id, qint64 old_start, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QmTimelineItemModel* model_ { nullptr };
    QmItemID item_id_ { kInvalidItemID };
    qint64 old_start_ { -1 };
    qint64 new_start_ { -1 };
};

} // namespace qmtl