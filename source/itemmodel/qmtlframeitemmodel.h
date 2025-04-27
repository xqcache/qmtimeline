#include "itemmodel/qmtlitemmodel.h"

class QmTLFrameItemModel : public QmTLItemModel {
    Q_OBJECT
public:
    explicit QmTLFrameItemModel(QObject* parent = nullptr);
    ~QmTLFrameItemModel() noexcept override;
};