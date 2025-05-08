#pragma once

#include "qmtimeline_global.h"
#include "qmtltypedef.h"
#include <QGraphicsObject>

class QmTLGraphicsModel;
class QmTLGraphicsScene;

/**
 * @brief Item图元
 */
class QMTIMELINE_EXPORT QmTLItemPrimitive : public QGraphicsObject {
    Q_OBJECT
public:
    explicit QmTLItemPrimitive(QmTLItemID item_id, QmTLGraphicsScene& scene);
    virtual ~QmTLItemPrimitive() noexcept = default;

    inline QmTLGraphicsScene& graphScene();
    inline const QmTLGraphicsScene& graphScene() const;

    virtual void fitInAxis();

protected:
    QmTLGraphicsModel* graphModel() const;

    friend class QmTLGraphicsScene;
    virtual bool onDataChanged(QmTLItemDataRoles roles);

protected:
    QmTLGraphicsScene& scene_;
    QmTLItemID item_id_ { kQmTLInvalidItemID };
};

inline QmTLGraphicsScene& QmTLItemPrimitive::graphScene()
{
    return scene_;
}

inline const QmTLGraphicsScene& QmTLItemPrimitive::graphScene() const
{
    return scene_;
}