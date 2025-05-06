#pragma once

#include "qmtlitemdata.h"

class TLDemoItemData : public QmTLItemData {
public:
    ~TLDemoItemData() noexcept override;

    bool load(const nlohmann::json& json) override;
    nlohmann::json save() const override;

    void setDelay(int delay);
    const std::optional<qint64>& delay() const;
    qint64 delayValue(qint64 def_val) const;

    bool hasDelay() const;
    void clearDelay();

private:
    std::optional<qint64> delay_;
};