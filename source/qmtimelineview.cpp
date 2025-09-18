#include "qmtimelineview.h"
#include "qmtimelinescene.h"
#include "widgets/qmtimelineaxis.h"
#include "widgets/qmtimelineranger.h"
#include "widgets/qmtimelinerangeslider.h"

struct QmTimelineViewPrivate {
    QWidget* vbar_filler { nullptr };
    QmTimelineAxis* axis { nullptr };
    QmTimelineScene* scene { nullptr };
    QmTimelineRanger* ranger { nullptr };
    QList<QMetaObject::Connection> model_connections;
};

QmTimelineView::QmTimelineView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
}

void QmTimelineView::initUi()
{
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(NoAnchor);
    setResizeAnchor(NoAnchor);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->axis = new QmTimelineAxis(this);
    d_->vbar_filler = new QWidget(this);
    d_->vbar_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    addScrollBarWidget(d_->vbar_filler, Qt::AlignTop);

    setSceneSize(1000000, 200);
    setAxisPlayheadHeight(40);

    setStyleSheet(R"(
    QLineEdit {
        background-color: #2b2b2b;
        border: 1px solid #333333;
        border-radius: 3px;
        padding: 2px 4px;
        color: #ffffff;
    }
    QmTimelineRangeSlider {
        background-color: #2b2b2b;
    }
    )");
}

void QmTimelineView::setSceneSize(qreal width, qreal height)
{
    setSceneRect(0, 0, width, height);
}

void QmTimelineView::setAxisPlayheadHeight(int height)
{
    d_->vbar_filler->setFixedHeight(height);
    d_->axis->setPlayheadHeight(height);
    setViewportMargins(0, height, 0, 0);
}

void QmTimelineView::setScene(QmTimelineScene* scene)
{
    if (!scene) {
        return;
    }
    if (scene == this->scene()) {
        return;
    }
    d_->scene = scene;
    scene->setView(this);
    QGraphicsView::setScene(scene);

    // connect(d_->ranger->slider(), &QmTimelineRangeSlider::sliderReleased, scene, &QmTimelineScene::refreshCache);

    // for (auto& connection : d_->model_connections) {
    //     disconnect(connection);
    // }
    // d_->model_connections.clear();

    // auto* model = d_->scene->model();
    // d_->model_connections.emplace_back(connect(model, &QmTimelineModel::viewFrameMaximumChanged, this, &QmTimelineView::onViewFrameMaximumChanged));
    // d_->model_connections.emplace_back(connect(model, &QmTimelineModel::viewFrameMinimumChanged, this, &QmTimelineView::onViewFrameMinimumChanged));
    // d_->model_connections.emplace_back(connect(model, &QmTimelineModel::frameMaximumChanged, this, &QmTimelineView::onFrameMaximumChanged));
    // d_->model_connections.emplace_back(connect(model, &QmTimelineModel::frameMinimumChanged, this, &QmTimelineView::onFrameMinimumChanged));
    // d_->model_connections.emplace_back(connect(model, &QmTimelineModel::fpsChanged, this, &QmTimelineView::onFpsChanged));

    // d_->model_connections.emplace_back(
    //     connect(d_->ranger->slider(), &QmTimelineRangeSlider::frameRangeChanged, model, [this, model](qint64 minimum, qint64 maximum) {
    //         QSignalBlocker blocker(model);
    //         model->setFrameMaximum(maximum);
    //         model->setFrameMinimum(minimum);
    //     }));
    // d_->model_connections.emplace_back(connect(d_->ranger->slider(), &QmTimelineRangeSlider::viewMinimumChanged, model,
    // &QmTimelineModel::setViewFrameMinimum)); d_->model_connections.emplace_back(connect(d_->ranger->slider(), &QmTimelineRangeSlider::viewMaximumChanged,
    // model, &QmTimelineModel::setViewFrameMaximum)); d_->model_connections.emplace_back(connect(d_->ranger, &TimelineRanger::fpsChanged, model,
    // &TimelineModel::setFps));
}