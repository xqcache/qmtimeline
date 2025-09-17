#pragma once

#include "timeline_global.h"
#include <QObject>

class TIMELINE_LIB_EXPORT TimelineItemModel : public QObject {
    Q_OBJECT
public:
    explicit TimelineItemModel(QObject* parent = nullptr);
};