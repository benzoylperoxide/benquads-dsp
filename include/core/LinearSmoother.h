#pragma once

namespace benquads {

template <typename SampleType>
class LinearSmoother
{
public:
    void init(float sampleRate, float rampSeconds)
    {
        rampSamples_ = static_cast<int>(sampleRate * rampSeconds);
    }

    void setCurrentAndTarget(SampleType value)
    {
        current_   = value;
        target_    = value;
        remaining_ = 0;
    }

    void setTarget(SampleType target)
    {
        target_    = target;
        remaining_ = rampSamples_;
        increment_ = (target_ - current_) / static_cast<SampleType>(rampSamples_);
    }

    SampleType getNextValue()
    {
        if (remaining_ > 0)
        {
            current_ += increment_;
            --remaining_;
        }
        return current_;
    }

    SampleType getCurrentValue() const { return current_; }
    bool isSmoothing()           const { return remaining_ > 0; }

private:
    SampleType current_   = SampleType(1);
    SampleType target_    = SampleType(1);
    SampleType increment_ = SampleType(0);
    int        remaining_    = 0;
    int        rampSamples_  = 0;
};

} // namespace benquads