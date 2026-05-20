#pragma once

#include <cmath>
#include "core/LinearSmoother.h"

namespace benquads {

// Converts decibels to a linear gain multiplier.
// -infinity dB (very negative) approaches 0.0, 0 dB = 1.0, 6 dB ~= 2.0.
// Kept outside the class so other modules can use it too.
template <typename SampleType>
inline SampleType decibelsToLinear(SampleType dB)
{
    return static_cast<SampleType>(std::pow(SampleType(10), dB / SampleType(20)));
}

template <typename SampleType>
class GainCore
{
public:
    struct Config
    {
        float sampleRate   = 48000.f;
        float rampSeconds  = 0.1f;
    };

    void init(const Config& config)
    {
        smoother_.init(config.sampleRate, config.rampSeconds);
    }

    // Call once at init time — jumps immediately, no ramp.
    void setCurrentAndTarget(SampleType dB)
    {
        smoother_.setCurrentAndTarget(decibelsToLinear<SampleType>(dB));
    }

    // Call each block from the audio thread — smoothly ramps to new value.
    void setTargetDecibels(SampleType dB)
    {
        smoother_.setTarget(decibelsToLinear<SampleType>(dB));
    }

    // Single-sample path — the portable primitive.
    // Only advances the smoother by one step; call once per sample across all channels.
    SampleType processSample(SampleType in)
    {
        return in * smoother_.getNextValue();
    }

    // Returns the current gain without advancing — use for channels after the first.
    SampleType processSampleHold(SampleType in)
    {
        return in * smoother_.getCurrentValue();
    }

    // Convenience block path for a single interleaved or mono buffer.
    void processBlock(SampleType* buffer, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
            buffer[i] = processSample(buffer[i]);
    }

    bool isSmoothing() const { return smoother_.isSmoothing(); }

private:
    LinearSmoother<SampleType> smoother_;
};

} // namespace benquads