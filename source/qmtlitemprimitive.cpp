#include "qmtlitemprimitive.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"

QmTLItemPrimitive::QmTLItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene)
    : scene_(scene)
    , item_id_(item_id)
{
    scene.addItem(this);
    setY(10);
}

QmTLGraphicsModel* QmTLItemPrimitive::graphModel() const
{
    return scene_.model();
}

bool QmTLItemPrimitive::onDataChanged(QmTLItemDataRoles roles)
{
    if (roles.testFlag(QmTLItemDataRole::TimeKey)) {
        auto* item_model = graphModel()->itemModel(item_id_);
        if (!item_model) {
            return false;
        }
        auto new_x = scene_.mapToAxisX(item_model->data().timeKey());
        if (!qFuzzyCompare(new_x, x())) {
            setX(new_x);
        }
        return true;
    }
    return false;
}

void QmTLItemPrimitive::fitInAxis()
{
}