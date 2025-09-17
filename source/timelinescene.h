#pragma once

#include "timeline_global.h"
#include <QGraphicsScene>

class TIMELINE_LIB_EXPORT TimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit TimelineScene(QObject *parent = nullptr);
};