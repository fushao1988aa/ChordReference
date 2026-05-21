//==================================================================================================
//  ArpeggioPatternLibrary.h
//  Project: ChordReference
//  Purpose: Built-in arpeggio pattern catalogue.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"

class ArpeggioPatternLibrary
{
public:
    static std::vector<ArpeggioPattern> getBuiltInPatterns();
    static bool findPatternById(const juce::String& id, ArpeggioPattern& outPattern);

private:
    static ArpeggioPattern makePattern(const juce::String& id,
                                       const juce::String& nameCN,
                                       const juce::String& nameEN,
                                       ArpeggioDirection direction,
                                       ArpeggioDensity density,
                                       double cycleBeats,
                                       std::initializer_list<ArpeggioStep> steps);
};
