#pragma once

#include "itemdata/tlframeitemdata.h"
#include "qmtlitemmodel.h"

struct TLFrameItemModelPrivate;
class TLFrameItemModel : public QmTLItemModel {
    Q_OBJECT
public:
    explicit TLFrameItemModel(QObject* parent = nullptr);
    ~TLFrameItemModel() noexcept override;

public:
    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;

    enum {
        Type = QmTLItemModel::UserType + 1
    };

    int type() const override;

private:
    TLFrameItemModelPrivate* d_ { nullptr };
};
