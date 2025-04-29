#include "itemdata/qmtlitemdata.h"

void QmTLItemData::setTimeKey(qint64 time_key)
{
    time_key_ = time_key;
}

qint64 QmTLItemData::timeKey() const
{
    return time_key_;
}