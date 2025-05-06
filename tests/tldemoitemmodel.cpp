#include "tldemoitemmodel.h"
#include "tldemoitemdata.h"

struct TLDemoItemModelPrivate { };

TLDemoItemModel::TLDemoItemModel(QObject* parent)
    : QmTLItemModel(std::make_unique<TLDemoItemData>(), parent)
    , d_(new TLDemoItemModelPrivate)
{
}

TLDemoItemModel::~TLDemoItemModel() noexcept
{
    delete d_;
}

int TLDemoItemModel::type() const
{
    return TLDemoItemModel::Type;
}

bool TLDemoItemModel::load(const nlohmann::json& json)
{
    return true;
}

nlohmann::json TLDemoItemModel::save() const
{
    nlohmann::json json;

    return json;
}
