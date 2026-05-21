//==================================================================================================
//  ArpeggioMatcher.h
//  Project: ChordReference
//  Purpose: Scores arpeggio patterns against musical context.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"

class ArpeggioMatcher
{
public:
    static std::vector<ArpeggioRankedPattern> rankPatterns(const ArpeggioContext& context,
                                                           const ArpeggioGenerationSettings& settings,
                                                           const std::vector<ArpeggioPattern>& patterns);

private:
    static int countMatches(const juce::StringArray& a, const juce::StringArray& b);
    static int scoreFamilyFit(const ArpeggioContext& context, const ArpeggioPattern& pattern);
    static int scoreDensity(ArpeggioDensity requested, ArpeggioDensity candidate);
    static int scoreTempo(double bpm, const ArpeggioPattern& pattern);
    static int scoreGrooveFit(const ArpeggioContext& context, double bpm, const ArpeggioPattern& pattern);
    static int scoreFormDynamics(const ArpeggioContext& context, const ArpeggioPattern& pattern);
    static int scoreComplexityFit(const ArpeggioContext& context, const ArpeggioPattern& pattern);
    static int scoreTextureFit(const ArpeggioContext& context, const ArpeggioPattern& pattern);
};
