#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"
#include "qmtlgraphicsview.h"
#include "tldemoitemmodel.h"
#include "tldemoitemregistry.h"
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
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

    view.addAction("Save", QString("Ctrl+S"), &view, [model, scene, &view] {
        QString path = QFileDialog::getSaveFileName(&view, "Save model", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "(*.json)");
        if (path.isEmpty()) {
            return;
        }
        QFile file(path);
        if (!file.open(QFile::WriteOnly)) {
            return;
        }
        file.write(QString::fromStdString(model->save().dump(4)).toUtf8());
    });

    view.addAction("Open", QString("Ctrl+O"), &view, [model, scene, &view] {
        QString path = QFileDialog::getOpenFileName(&view, "Save model", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "(*.json)");
        if (path.isEmpty()) {
            return;
        }
        QFile file(path);
        if (!file.open(QFile::ReadOnly)) {
            return;
        }
        nlohmann::json json = nlohmann::json::parse(file.readAll().toStdString());
        if (model->load(json)) {
            qDebug() << "Model load success";
        } else {
            qDebug() << "Model load fail!";
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