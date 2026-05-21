//==================================================================================================
//  ArpeggioPreviewState.h
//  Project: ChordReference
//  Purpose: Lightweight UI-facing state holder for future arpeggio controls.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"

struct ArpeggioPreviewState
{
    bool enabled = true;
    bool smartMode = true;
    bool playing = false;
    int smartRecommendationIndex = 0;
    int selectedPatternIndex = 0;
    juce::String selectedPatternId;
    ArpeggioDensity density = ArpeggioDensity::Eighth;
    ArpeggioDirection direction = ArpeggioDirection::Smart;
    bool humanizeEnabled = true;
    int humanizeAmount = 50;
    bool includeUpperColourLayer = false;
    bool includeLeftRootBreakdown = false;
    bool includeBassDoubling = false;
    double previewPositionBeats = 0.0;

    void resetTransport()
    {
        playing = false;
        previewPositionBeats = 0.0;
    }
};
