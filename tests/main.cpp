#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"
#include "qmtlgraphicsview.h"
#include "tldemoitemmodel.h"
#include "tldemoitemregistry.h"
#include <QAction>
#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QmTLGraphicsView view;
    view.resize(1024, 360);
    view.show();

    auto* model = new QmTLGraphicsModel(std::make_unique<TLDemoItemRegistry>(), &view);
    auto* scene = new QmTLGraphicsScene(&view);
    scene->setModel(model);
    view.setScene(scene);
    view.setStyleSheet("QmTLDateTimeAxis { background-color: #2E3440; }");
    view.addAction("Add Frame", QString("Ctrl+N"), &view, [model, scene, &view] {
        auto item_id = model->createItem(TLDemoItemModel::Type);
        if (auto* item_model = model->itemModel(item_id); item_model) {
            auto& item_data = static_cast<TLDemoItemData&>(item_model->data());
            item_data.setOrigin(view.axisTickValue());
            model->requestUpdate(item_id);
        }
    });

    QTimer::singleShot(200, [model] {
        auto item_id = model->createItem(TLDemoItemModel::Type);
        if (auto* item_model = model->itemModel(item_id); item_model) {
            auto& item_data = static_cast<TLDemoItemData&>(item_model->data());
            item_data.setOrigin(2000);
            model->requestUpdate(item_id);
        }
    });

    return app.exec();
}