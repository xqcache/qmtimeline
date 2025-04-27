#pragma once

#include <QObject>

class QmTLItemModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLItemModel(QObject* parent = nullptr);
    ~QmTLItemModel() noexcept override;
};