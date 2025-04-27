#pragma once

#include <QGraphicsScene>

class QmTLGraphicsModel;

struct QmTLGraphicsScenePrivate;

class QmTLGraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    Q_DISABLE_COPY(QmTLGraphicsScene);
    explicit QmTLGraphicsScene(QObject* parent = nullptr);
    ~QmTLGraphicsScene() noexcept override;

    void setModel(QmTLGraphicsModel* model);
    QmTLGraphicsModel* model() const;

private:
    QmTLGraphicsScenePrivate* d_ { nullptr };
};