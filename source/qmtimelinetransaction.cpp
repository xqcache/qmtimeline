#include "qmtimelinetransaction.h"
#include "qmtimelineitem.h"
#include "qmtimelineitemmodel.h"
#include <QCoreApplication>

namespace qmtl {
QmTimelineItemCreateCommand::QmTimelineItemCreateCommand(QmTimelineItemModel* model, QmItemID item_id, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , item_data_(model->saveItem(item_id))
{
    setText(QCoreApplication::translate("QmTimelineItemCreateCommand", "Create Item"));
}

void QmTimelineItemCreateCommand::undo()
{
    model_->removeItem(item_id_);
}

void QmTimelineItemCreateCommand::redo()
{
    model_->loadItem(item_data_);
}

QmTimelineItemDeleteCommand::QmTimelineItemDeleteCommand(QmTimelineItemModel* model, QmItemID item_id, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , item_data_(model_->saveItem(item_id_))
{
    setText(QCoreApplication::translate("QmTimelineItemDeleteCommand", "Delete Item"));
}

void QmTimelineItemDeleteCommand::undo()
{
    model_->loadItem(item_data_);
}

void QmTimelineItemDeleteCommand::redo()
{
    model_->removeItem(item_id_);
}

QmTimelineItemMoveCommand::QmTimelineItemMoveCommand(QmTimelineItemModel* model, QmItemID item_id, qint64 old_start, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , old_start_(old_start)
{
    if (auto* item = model_->item(item_id_); item) {
        new_start_ = item->start();
    }

    setText(QCoreApplication::translate("QmTimelineItemMoveCommand", "Move Item"));
}

void QmTimelineItemMoveCommand::undo()
{
    auto* item = model_->item(item_id_);
    if (old_start_ >= model_->viewFrameMinimum() && old_start_ <= model_->viewFrameMaximum() && item && old_start_ != item->start()) {
        model_->modifyItemStart(item_id_, old_start_);
    }
}

void QmTimelineItemMoveCommand::redo()
{
    auto* item = model_->item(item_id_);
    if (new_start_ >= model_->viewFrameMinimum() && new_start_ <= model_->viewFrameMaximum() && item && new_start_ != item->start()) {
        model_->modifyItemStart(item_id_, new_start_);
    }
}

} // namespace qmtl