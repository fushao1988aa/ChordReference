//==================================================================================================
//  ArpeggioMidiRenderer.h
//  Project: ChordReference
//  Purpose: Renders selected arpeggio patterns to MIDI.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#pragma once

#include "ArpeggioTypes.h"

class ArpeggioMidiRenderer
{
public:
    static juce::MidiMessageSequence renderToSequence(const std::vector<std::vector<int>>& midiChords,
                                                      const ArpeggioPattern& pattern,
                                                      const ArpeggioContext& context,
                                                      const ArpeggioGenerationSettings& settings);

    static juce::File writeMidiFile(const juce::MidiMessageSequence& beatSequence,
                                    const ArpeggioContext& context,
                                    const juce::String& filePrefix = "ChordArp");

private:
    static int resolveStepNote(const std::vector<int>& chord,
                               const ArpeggioStep& step,
                               const ArpeggioPattern& pattern,
                               int chordIndex,
                               int cycleIndex);
};
