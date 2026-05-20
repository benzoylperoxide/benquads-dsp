#include "doctest.h"
#include "core/GainCore.h"

TEST_CASE("GainCore - unity gain at 0 dB")
{
    benquads::GainCore<float> g;
    benquads::GainCore<float>::Config config{ 48000.f, 0.0f }; // zero ramp
    g.init(config);
    g.setCurrentAndTarget(0.0f); // 0 dB = linear 1.0

    CHECK(g.processSample(1.0f)  == doctest::Approx(1.0f));
    CHECK(g.processSample(-1.0f) == doctest::Approx(-1.0f));
    CHECK(g.processSample(0.5f)  == doctest::Approx(0.5f));
}

TEST_CASE("GainCore - silence at -infinity approx")
{
    benquads::GainCore<float> g;
    benquads::GainCore<float>::Config config{ 48000.f, 0.0f };
    g.init(config);
    g.setCurrentAndTarget(-60.0f); // -60 dB = linear ~0.001

    float out = g.processSample(1.0f);
    CHECK(out < 0.002f);
    CHECK(out > 0.0f);
}

TEST_CASE("GainCore - 6 dB doubles amplitude")
{
    benquads::GainCore<float> g;
    benquads::GainCore<float>::Config config{ 48000.f, 0.0f };
    g.init(config);
    g.setCurrentAndTarget(6.0f); // ~linear 2.0

    CHECK(g.processSample(1.0f) == doctest::Approx(1.995f).epsilon(0.01f));
}

TEST_CASE("GainCore - smoothing advances on processSample, holds on processSampleHold")
{
    benquads::GainCore<float> g;
    benquads::GainCore<float>::Config config{ 48000.f, 0.1f };
    g.init(config);
    g.setCurrentAndTarget(-60.0f);   // start at near-silence
    g.setTargetDecibels(0.0f);       // ramp toward 1.0 

    CHECK(g.isSmoothing());

    float advancing  = g.processSample(1.0f);      // advances smoother
    float held       = g.processSampleHold(1.0f);  // does not advance
    float advancing2 = g.processSample(1.0f);      // advances again

    CHECK(held       == doctest::Approx(advancing).epsilon(0.0001f));
    CHECK(advancing2 >  advancing);
}

TEST_CASE("GainCore - processBlock matches repeated processSample")
{
    const int N = 16;
    float bufA[N], bufB[N];
    for (int i = 0; i < N; ++i) bufA[i] = bufB[i] = 1.0f;

    benquads::GainCore<float>::Config config{ 48000.f, 0.1f };

    benquads::GainCore<float> gA;
    gA.init(config);
    gA.setCurrentAndTarget(-6.0f);
    gA.processBlock(bufA, N);

    benquads::GainCore<float> gB;
    gB.init(config);
    gB.setCurrentAndTarget(-6.0f);
    for (int i = 0; i < N; ++i)
        bufB[i] = gB.processSample(bufB[i]);

    for (int i = 0; i < N; ++i)
        CHECK(bufA[i] == doctest::Approx(bufB[i]).epsilon(0.0001f));
}