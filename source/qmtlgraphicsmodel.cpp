#include "qmtlgraphicsmodel.h"

struct QmTLGraphicsModelPrivate { };

QmTLGraphicsModel::QmTLGraphicsModel(QObject* parent)
    : QObject(parent)
    , d_(new QmTLGraphicsModelPrivate)
{
}

QmTLGraphicsModel::~QmTLGraphicsModel() noexcept
{
    delete d_;
}