#pragma once

#include "qmtimeline_global.h"
#include "qmtimelinetype.h"
#include <QWidget>

namespace Ui {
class QmTimelineRanger;
}

class QmTimelineRangeSlider;

struct QmTimelineRangerPrivate;
class QMTIMELINE_LIB_EXPORT QmTimelineRanger : public QWidget {
    Q_OBJECT

public:
    QmTimelineRanger(QWidget* parent = nullptr);
    ~QmTimelineRanger() noexcept override;

    QmTimelineRangeSlider* slider() const;

    // 以帧模式控制范围
    void setFrameFormat(QmFrameFormat frame_fmt);
    void setFrameMinimum(qint64 minimum);
    void setFrameMaximum(qint64 maximum);
    void setFps(double fps);
    double fps() const;

signals:
    void fpsChanged(double fps);

private:
    void initUi();
    void setupSignals();

private:
    Ui::QmTimelineRanger* ui_ { nullptr };
    QmTimelineRangerPrivate* d_ { nullptr };
};
