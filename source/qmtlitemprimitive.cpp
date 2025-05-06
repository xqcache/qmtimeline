#include "qmtlitemprimitive.h"
#include "qmtlgraphicsscene.h"

QmTLItemPrimitive::QmTLItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene)
    : scene_(scene)
    , item_id_(item_id)
{
    scene.addItem(this);
}

QmTLGraphicsModel* QmTLItemPrimitive::graphModel() const
{
    return scene_.model();
}

void QmTLItemPrimitive::fitInAxis()
{
}