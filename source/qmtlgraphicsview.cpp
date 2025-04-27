#include "qmtlgraphicsview.h"

struct QmTLGraphicsViewPrivate { };

QmTLGraphicsView::QmTLGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new QmTLGraphicsViewPrivate)
{
}

QmTLGraphicsView::~QmTLGraphicsView() noexcept
{
    delete d_;
}