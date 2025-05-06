#include "tlframeitemmodel.h"
#include "itemdata/tlframeitemdata.h"

struct TLFrameItemModelPrivate { };

TLFrameItemModel::TLFrameItemModel(QObject* parent)
    : QmTLItemModel(std::make_unique<TLFrameItemData>(), parent)
    , d_(new TLFrameItemModelPrivate)
{
}

TLFrameItemModel::~TLFrameItemModel() noexcept
{
    delete d_;
}

int TLFrameItemModel::type() const
{
    return TLFrameItemModel::Type;
}

bool TLFrameItemModel::load(const nlohmann::json& json)
{
    return true;
}

nlohmann::json TLFrameItemModel::save() const
{
    nlohmann::json json;

    return json;
}
