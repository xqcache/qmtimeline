#include "qmtlgraphicsview.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QmTLGraphicsView view;
    view.resize(1024, 360);
    view.show();

    return app.exec();
}