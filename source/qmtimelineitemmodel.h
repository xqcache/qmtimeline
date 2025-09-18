#pragma once

#include "qmtimeline_global.h"
#include <QObject>

class QMTIMELINE_LIB_EXPORT QmTimelineItemModel : public QObject {
    Q_OBJECT
public:
    explicit QmTimelineItemModel(QObject* parent = nullptr);
};