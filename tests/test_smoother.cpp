#include "doctest.h"
#include "core/LinearSmoother.h"

TEST_CASE("LinearSmoother - immediate set")
{
    benquads::LinearSmoother<float> s;
    s.init(48000.f, 0.1f);
    s.setCurrentAndTarget(0.5f);

    CHECK(s.getCurrentValue() == doctest::Approx(0.5f));
    CHECK_FALSE(s.isSmoothing());

    // Should stay at 0.5 indefinitely
    CHECK(s.getNextValue() == doctest::Approx(0.5f));
    CHECK(s.getNextValue() == doctest::Approx(0.5f));
}

TEST_CASE("LinearSmoother - ramps toward target")
{
    benquads::LinearSmoother<float> s;
    s.init(48000.f, 0.1f);
    s.setCurrentAndTarget(0.0f);
    s.setTarget(1.0f);

    CHECK(s.isSmoothing());

    // Drain the ramp (0.1s * 48000 = 4800 samples)
    const int rampSamples = static_cast<int>(48000.f * 0.1f);
    float last = 0.0f;
    for (int i = 0; i < rampSamples; ++i)
    {
        float v = s.getNextValue();
        CHECK(v >= last);   // monotonically increasing
        last = v;
    }

    CHECK_FALSE(s.isSmoothing());
    CHECK(s.getCurrentValue() == doctest::Approx(1.0f).epsilon(0.01f));
}

TEST_CASE("LinearSmoother - retarget mid-ramp")
{
    benquads::LinearSmoother<float> s;
    s.init(48000.f, 0.1f);
    s.setCurrentAndTarget(0.0f);
    s.setTarget(1.0f);

    // Advance halfway
    const int half = static_cast<int>(48000.f * 0.1f) / 2;
    for (int i = 0; i < half; ++i)
        s.getNextValue();

    // Retarget back to 0 mid-ramp — should start moving back down
    float midpoint = s.getCurrentValue();
    s.setTarget(0.0f);

    CHECK(s.isSmoothing());
    float next = s.getNextValue();
    CHECK(next < midpoint);
}