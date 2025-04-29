#include "qmtlframeitemmodel.h"
#include "itemdata/qmtlframeitemdata.h"
#include "qmtlitemmodel_p.h"

struct QmTLFrameItemModelPrivate : public QmTLItemModelPrivate {
};

QmTLFrameItemModel::QmTLFrameItemModel(QObject* parent)
    : QmTLItemModel(parent)
    , d_(new QmTLFrameItemModelPrivate)
{
    d_->data = std::make_unique<QmTLFrameItemData>();
}

QmTLFrameItemModel::~QmTLFrameItemModel() noexcept
{
    delete d_;
}

int QmTLFrameItemModel::type() const
{
    return QmTLFrameItemModel::Type;
}

QmTLItemModelPrivate& QmTLFrameItemModel::d_ref() const
{
    return *d_;
}

bool QmTLFrameItemModel::load(const nlohmann::json& json)
{
    return true;
}

nlohmann::json QmTLFrameItemModel::save() const
{
    nlohmann::json json;

    return json;
}
