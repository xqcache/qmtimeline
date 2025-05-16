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

void QmTLItemPrimitive::onDataChanged(QmTLItemDataRoles roles, const QVariant& param)
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
}

void QmTLItemPrimitive::fitInAxis()
{
}