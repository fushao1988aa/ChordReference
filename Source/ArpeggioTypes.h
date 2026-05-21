//==================================================================================================
//  ArpeggioTypes.h
//  Project: ChordReference
//  Purpose: Shared data structures for the intelligent arpeggio system.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "MusicTypes.h"

enum class ArpeggioDirection
{
    Smart,
    Up,
    Down,
    UpDown,
    DownUp,
    Alternating,
    RandomLike,
    BlockPulse
};

enum class ArpeggioDensity
{
    Whole,
    Half,
    Quarter,
    Eighth,
    Sixteenth,
    Mixed
};

enum class ArpeggioPedalMode
{
    Clean,
    Connected,
    Ambient
};

struct ArpeggioStep
{
    double beatOffset = 0.0;
    double durationBeats = 0.5;
    int chordToneIndex = 0;       // 0=root/lowest, 1=next tone; wraps when needed.
    int octaveOffset = 0;
    int velocityOffset = 0;
    bool isBassAnchor = false;
    bool tieToNext = false;
};

struct ArpeggioPattern
{
    juce::String id;
    juce::String nameCN;
    juce::String nameEN;
    juce::String description;

    ArpeggioDirection direction = ArpeggioDirection::Smart;
    ArpeggioDensity density = ArpeggioDensity::Eighth;

    juce::StringArray styleTags;
    juce::StringArray moodTags;
    juce::StringArray regionTags;
    juce::StringArray functionFit;
    juce::StringArray formFit;
    juce::String familyId;
    juce::String grooveAffinity; // Straight / Late / Tight / Shuffle

    int complexity = 2;           // 1-5
    int energy = 3;               // 1-5
    bool pedalFriendly = true;
    bool bassAnchored = false;
    bool supportsExtensions = true;

    double cycleBeats = 1.0;
    std::vector<ArpeggioStep> steps;
};

struct ArpeggioContext
{
    juce::String presetName;
    juce::String presetDescription;
    juce::String progressionName;
    juce::String romanSequence;
    PitchClass tonic = PitchClass::C;
    ScaleType scaleType = ScaleType::Major;

    juce::StringArray styleTags;
    juce::StringArray moodTags;
    juce::StringArray regionTags;
    juce::StringArray formTags;
    juce::StringArray functionTags;
    juce::StringArray complexityTags;
    juce::StringArray textureTags;

    double bpm = 120.0;
    int beatsPerBar = 4;
    double beatsPerChord = 4.0;
};

struct ArpeggioHumanizeSettings
{
    bool enabled = true;
    int amount = 50;              // 0-100
    int seed = 0;
    int velocityRange = 8;
    double timingRangeMs = 10.0;
    double durationRange = 0.08;
    double swing = 0.0;           // 0-1, future use.
    ArpeggioPedalMode pedalMode = ArpeggioPedalMode::Connected;
};

struct ArpeggioGenerationSettings
{
    bool smartMode = true;
    juce::String preferredPatternId;
    ArpeggioDirection manualDirection = ArpeggioDirection::Up;
    ArpeggioDensity density = ArpeggioDensity::Eighth;
    ArpeggioHumanizeSettings humanize;
    bool includePedal = true;
    bool includeBassAnchor = false;
    bool includeUpperColourLayer = false;
    bool includeLeftRootBreakdown = false;
    bool includeBassDoubling = false;
    int lowMidi = 48;
    int highMidi = 84;
};

struct ArpeggioRankedPattern
{
    ArpeggioPattern pattern;
    int score = 0;
    juce::String reason;
};

struct ArpeggioGenerationResult
{
    bool ok = false;
    juce::String error;
    ArpeggioPattern selectedPattern;
    std::vector<ArpeggioRankedPattern> rankedPatterns;
    juce::MidiMessageSequence sequence; // timestamps are in beats until written to MIDI.
};
