#pragma once

#include <QGraphicsObject>

class QmTLGraphicsScene;

class QmTLGraphicsItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit QmTLGraphicsItem(QmTLGraphicsScene& scene);
    virtual ~QmTLGraphicsItem() noexcept = default;

    inline QmTLGraphicsScene& graphScene();
    inline const QmTLGraphicsScene& graphScene() const;

protected:
    QmTLGraphicsScene& scene_;
};

inline QmTLGraphicsScene& QmTLGraphicsItem::graphScene()
{
    return scene_;
}

inline const QmTLGraphicsScene& QmTLGraphicsItem::graphScene() const
{
    return scene_;
}