#include "qmtlitemmodel.h"
#include "qmtlitemmodel_p.h"

QmTLItemModel::QmTLItemModel(QObject* parent)
    : QObject(parent)
{
}

QmTLItemModel::~QmTLItemModel() noexcept
{
}

QmTLItemData& QmTLItemModel::data()
{
    return *d_ref().data.get();
}

const QmTLItemData& QmTLItemModel::data() const
{
    return *d_ref().data.get();
}
