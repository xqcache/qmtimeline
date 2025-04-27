#pragma once

#include <QObject>

struct QmTLGraphicsModelPrivate;
class QmTLGraphicsModel : public QObject {
    Q_OBJECT
public:
    explicit QmTLGraphicsModel(QObject* parent = nullptr);
    ~QmTLGraphicsModel() noexcept override;

private:
    QmTLGraphicsModelPrivate* d_ { nullptr };
};