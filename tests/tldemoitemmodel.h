#pragma once

#include "qmtlitemmodel.h"
#include "tldemoitemdata.h"

struct TLDemoItemModelPrivate;
class TLDemoItemModel : public QmTLItemModel {
    Q_OBJECT
public:
    explicit TLDemoItemModel(QObject* parent = nullptr);
    ~TLDemoItemModel() noexcept override;

public:
    enum {
        Type = QmTLItemModel::UserType + 1
    };

    int type() const override;

private:
    TLDemoItemModelPrivate* d_ { nullptr };
};
