#include "itemmodel/qmtlframeitemmodel.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"
#include "qmtlgraphicsview.h"
#include <QAction>
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QmTLGraphicsView view;
    view.resize(1024, 360);
    view.show();

    auto* model = new QmTLGraphicsModel(&view);
    auto* scene = new QmTLGraphicsScene(&view);
    scene->setModel(model);
    view.setScene(scene);
    view.setStyleSheet("QmTLDateTimeAxis { background-color: #2E3440; }");
    view.addAction("Add Frame", QString("Ctrl+N"), &view, [model, scene] {
        auto item_id = model->createItem(QmTLFrameItemModel::Type);
        if (auto* item_model = model->itemModel(item_id); item_model) {
            auto& item_data = static_cast<QmTLFrameItemData&>(item_model->data());
            item_data.setDelay(1000);
            item_data.setTimeKey(3000);
            model->requestUpdate(item_id);
        }
    });

    return app.exec();
}