#pragma once

#include "itemdata/qmtlframeitemdata.h"
#include "itemmodel/qmtlitemmodel.h"

struct QmTLFrameItemModelPrivate;
class QmTLFrameItemModel : public QmTLItemModel {
    Q_OBJECT
public:
    explicit QmTLFrameItemModel(QObject* parent = nullptr);
    ~QmTLFrameItemModel() noexcept override;

public:
    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;

    enum {
        Type = QmTLItemModel::UserType + 1
    };

    int type() const override;

private:
    QmTLItemModelPrivate& d_ref() const override;

private:
    QmTLFrameItemModelPrivate* d_ { nullptr };
};
