#pragma once

#include "qmtltypedef.h"
#include <QGraphicsObject>

class QmTLGraphicsModel;
class QmTLGraphicsScene;

class QmTLGraphicsItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit QmTLGraphicsItem(QmTLGraphicsScene& scene, QmTLItemID item_id);
    virtual ~QmTLGraphicsItem() noexcept = default;

    inline QmTLGraphicsScene& graphScene();
    inline const QmTLGraphicsScene& graphScene() const;

    virtual void fitInAxis();

protected:
    QmTLGraphicsModel* graphModel() const;

protected:
    QmTLGraphicsScene& scene_;
    QmTLItemID item_id_ { kQmTLInvalidItemID };
};

inline QmTLGraphicsScene& QmTLGraphicsItem::graphScene()
{
    return scene_;
}

inline const QmTLGraphicsScene& QmTLGraphicsItem::graphScene() const
{
    return scene_;
}