//==================================================================================================
//  ArpeggioHumanizer.cpp
//  Project: ChordReference
//  Purpose: Humanization implementation for arpeggiated MIDI.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#include "ArpeggioHumanizer.h"
#include <algorithm>

ArpeggioHumanizer::ArpeggioHumanizer(const ArpeggioHumanizeSettings& s)
    : settings(s),
      rng(static_cast<unsigned int>(s.seed != 0 ? s.seed : juce::Time::getMillisecondCounter()))
{
}

int ArpeggioHumanizer::velocityFor(int baseVelocity, int stepVelocityOffset, int beatIndex, bool isBass)
{
    const int strength = juce::jlimit(0, 100, settings.amount);
    const int accent = beatIndex == 0 ? 10 : (beatIndex == 2 ? 4 : -3);
    const int bassLift = isBass ? 5 : 0;

    int drift = 0;
    if (settings.enabled && strength > 0)
    {
        std::uniform_int_distribution<int> velDist(-settings.velocityRange, settings.velocityRange);
        drift = velDist(rng) * strength / 100;
    }

    return juce::jlimit(1, 127, baseVelocity + stepVelocityOffset + accent + bassLift + drift);
}

double ArpeggioHumanizer::timingOffsetBeats(double bpm, int noteIndex)
{
    if (!settings.enabled || settings.amount <= 0)
        return 0.0;

    std::uniform_real_distribution<double> timeDist(-settings.timingRangeMs, settings.timingRangeMs);
    const double ms = timeDist(rng) * settings.amount / 100.0;
    const double seconds = ms / 1000.0;
    const double beatSeconds = 60.0 / juce::jmax(1.0, bpm);
    const bool keepDownbeatsStable = noteIndex % 4 == 0;
    return keepDownbeatsStable ? seconds * 0.35 / beatSeconds : seconds / beatSeconds;
}

double ArpeggioHumanizer::durationScale(int noteIndex)
{
    if (!settings.enabled || settings.amount <= 0)
        return 1.0;

    std::uniform_real_distribution<double> durDist(-settings.durationRange, settings.durationRange);
    return juce::jlimit(0.72, 1.18, 1.0 + durDist(rng) * settings.amount / 100.0 + (noteIndex % 4 == 3 ? 0.04 : 0.0));
}
