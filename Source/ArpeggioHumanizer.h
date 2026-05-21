//==================================================================================================
//  ArpeggioHumanizer.h
//  Project: ChordReference
//  Purpose: Applies timing, velocity, duration, and pedal feel.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"
#include <random>

class ArpeggioHumanizer
{
public:
    explicit ArpeggioHumanizer(const ArpeggioHumanizeSettings& settings);

    int velocityFor(int baseVelocity, int stepVelocityOffset, int beatIndex, bool isBass);
    double timingOffsetBeats(double bpm, int noteIndex);
    double durationScale(int noteIndex);

private:
    ArpeggioHumanizeSettings settings;
    std::mt19937 rng;
};
