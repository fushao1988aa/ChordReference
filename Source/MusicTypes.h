#pragma once

#include <vector>
#include <string>
#include <juce_core/juce_core.h>
#include "LocalisedStrings.h"

enum class PitchClass : int
{
    C = 0,
    C_Sharp = 1,
    D = 2,
    D_Sharp = 3,
    E = 4,
    F = 5,
    F_Sharp = 6,
    G = 7,
    G_Sharp = 8,
    A = 9,
    A_Sharp = 10,
    B = 11,

    D_Flat = 1,
    E_Flat = 3,
    G_Flat = 6,
    A_Flat = 8,
    B_Flat = 10,
    C_Flat = 11
};

enum class ScaleType
{
    Major,
    NaturalMinor,
    Dorian,
    Phrygian,
    Lydian,
    Mixolydian,
    Locrian
};

enum class SkinType
{
    FluidPurpleOrange = 0,
    FluidLightShadow  = 1,
    IvorySatin        = 2,
    RoyalBlackGold    = 3
};

enum class ChordQuality
{
    MajorTriad,
    MinorTriad,
    Dominant7,
    Diminished,
    Augmented,
    Sus2,
    Sus4,
    MinorMajor7,
    Minor7b5
};

enum class ChordExtension
{
    Triad      = 3,
    Seventh    = 7,
    Ninth      = 9,
    Eleventh   = 11,
    Thirteenth = 13
};

inline ChordExtension intToChordExtension(int numNotes)
{
    switch (numNotes)
    {
        case 3:  return ChordExtension::Triad;
        case 7:  return ChordExtension::Seventh;
        case 9:  return ChordExtension::Ninth;
        case 11: return ChordExtension::Eleventh;
        case 13: return ChordExtension::Thirteenth;
        default: return ChordExtension::Triad;
    }
}

struct Chord
{
    PitchClass root;
    std::vector<int> intervals;
    juce::String name;
    juce::String roman;
};

inline juce::String getScaleTypeName(ScaleType type)
{
    switch (type)
    {
        case ScaleType::Major:        return LocalisedStrings::translate("auto_major_major");
        case ScaleType::NaturalMinor: return LocalisedStrings::translate("auto_natural_minor_naturalminor");
        case ScaleType::Dorian:       return LocalisedStrings::translate("auto_dorian_dorian");
        case ScaleType::Phrygian:     return LocalisedStrings::translate("auto_phrygian_phrygian");
        case ScaleType::Lydian:       return LocalisedStrings::translate("auto_lydian_lydian");
        case ScaleType::Mixolydian:   return LocalisedStrings::translate("auto_mixolydian_mixolydian");
        case ScaleType::Locrian:      return LocalisedStrings::translate("auto_locrian_locrian");
        default:                      return {};
    }
}
