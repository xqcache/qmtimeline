#pragma once

#include "timeline_global.h"
#include <QGraphicsView>

class TIMELINE_LIB_EXPORT TimelineView : public QGraphicsView {
    Q_OBJECT
public:
    explicit TimelineView(QWidget* parent);
};
