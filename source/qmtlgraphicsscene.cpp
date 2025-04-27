#include "qmtlgraphicsscene.h"
#include "qmtlgraphicsmodel.h"

struct QmTLGraphicsScenePrivate {
    QmTLGraphicsModel* model { nullptr };
};

QmTLGraphicsScene::QmTLGraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
    , d_(new QmTLGraphicsScenePrivate)
{
}

QmTLGraphicsScene::~QmTLGraphicsScene() noexcept
{
    delete d_;
}

void QmTLGraphicsScene::setModel(QmTLGraphicsModel* model)
{
    if (d_->model == model) {
        return;
    }
    d_->model = model;
}

QmTLGraphicsModel* QmTLGraphicsScene::model() const
{
    return d_->model;
}