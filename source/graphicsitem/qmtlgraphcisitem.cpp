#include "qmtlgraphcisitem.h"
#include "qmtlgraphicsscene.h"

QmTLGraphicsItem::QmTLGraphicsItem(QmTLGraphicsScene& scene, QmTLItemID item_id)
    : scene_(scene)
    , item_id_(item_id)
{
    scene.addItem(this);
}

QmTLGraphicsModel* QmTLGraphicsItem::graphModel() const
{
    return scene_.model();
}

void QmTLGraphicsItem::fitInAxis()
{
}