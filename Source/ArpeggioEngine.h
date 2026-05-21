//==================================================================================================
//  ArpeggioEngine.h
//  Project: ChordReference
//  Purpose: High-level intelligent arpeggio generation facade.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"

class ArpeggioEngine
{
public:
    static ArpeggioGenerationResult generate(const ArpeggioContext& context,
                                             const ArpeggioGenerationSettings& settings);

    static juce::File generateMidiFile(const ArpeggioContext& context,
                                       const ArpeggioGenerationSettings& settings);

private:
    static std::vector<std::vector<int>> buildMidiChords(const ArpeggioContext& context,
                                                         const ArpeggioGenerationSettings& settings);
};
