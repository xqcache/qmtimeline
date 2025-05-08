#include "qmtlitemdata.h"
#include "qmtltypedef.h"

void QmTLItemData::setTimeKey(qint64 time_key)
{
    time_key_ = time_key;
}

qint64 QmTLItemData::timeKey() const
{
    return time_key_;
}

bool QmTLItemData::setData(const QVariant& data, int role)
{
    if (role == QmTLItemData::TimeKeyRole) {
        setTimeKey(data.value<qint64>());
        return true;
    }
    return false;
}