#pragma once

#include "qmtimeline_global.h"

#include <QString>

class QMTIMELINE_LIB_EXPORT QmTimelineUtil {
public:
    static QString formatTimeCode(qint64 value, double fps);
    static QString formatTimeString(qint64 frame_no, double fps, bool keep_msecs = true);

    static qint64 parseTimeCode(const QString& text, double fps);
    static qint64 parseTimeString(const QString& text, double fps, bool keep_msecs = true);

    static qint64 frameToTime(qint64 frame_no, double fps);
};