#pragma once

#include <QGraphicsView>

struct QmTLGraphicsViewPrivate;

class QmTLGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit QmTLGraphicsView(QWidget* parent = nullptr);
    ~QmTLGraphicsView() noexcept override;

private:
    QmTLGraphicsViewPrivate* d_ { nullptr };
};