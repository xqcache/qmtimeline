#include "qmtlitemprimitive.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"

QmTLItemPrimitive::QmTLItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene)
    : scene_(scene)
    , item_id_(item_id)
{
    scene.addItem(this);
    setZValue(0);
}

QmTLGraphicsModel* QmTLItemPrimitive::graphModel() const
{
    return scene_.model();
}

void QmTLItemPrimitive::onDataChanged(QmTLItemDataRoles roles)
{
    if (roles.testFlag(QmTLItemData::OriginRole)) {
        auto* item_model = graphModel()->itemModel(item_id_);
        if (!item_model) {
            return;
        }
        auto new_x = scene_.mapToAxisX(item_model->data().origin());
        if (!qFuzzyCompare(new_x, x())) {
            setX(new_x);
        }
    }
    if (roles.testFlag(QmTLItemData::ToolTipRole)) {
        auto* item_model = graphModel()->itemModel(item_id_);
        if (!item_model) {
            return;
        }
        setToolTip(item_model->data().toolTip());
    }
}

void QmTLItemPrimitive::onItemOperate(QmTLItemDataRoles roles, const QVariant& param)
{
}

void QmTLItemPrimitive::fitInAxis()
{
}

void QmTLItemPrimitive::updatePosition()
{
}

int QmTLItemPrimitive::type() const
{
    return Type;
}
