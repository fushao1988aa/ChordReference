//==================================================================================================
//  ArpeggioEngine.cpp
//  Project: ChordReference
//  Purpose: Intelligent arpeggio generation facade implementation.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#include "ArpeggioEngine.h"
#include "ArpeggioPatternLibrary.h"
#include "ArpeggioMatcher.h"
#include "ArpeggioMidiRenderer.h"
#include "ProgressionEngine.h"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace
{
    int wrapPitchClass(int pitch)
    {
        return (pitch % 12 + 12) % 12;
    }

    int getParsedRootPitchClass(const ParsedChord& chord, PitchClass tonic, ScaleType scaleType)
    {
        if (chord.isSecondaryDominant)
        {
            const int targetPitch = static_cast<int>(tonic)
                + ProgressionEngine::getScaleDegreeInterval(chord.secondaryTarget, scaleType);
            return wrapPitchClass(targetPitch + 7);
        }

        return wrapPitchClass(static_cast<int>(tonic)
                              + ProgressionEngine::getScaleDegreeInterval(chord.scaleDegree, scaleType)
                              + chord.alteration);
    }

    std::vector<int> orderChordFromRoot(std::vector<int> chordPitches, int rootPitchClass)
    {
        std::sort(chordPitches.begin(), chordPitches.end());
        chordPitches.erase(std::unique(chordPitches.begin(), chordPitches.end()), chordPitches.end());

        std::sort(chordPitches.begin(), chordPitches.end(), [rootPitchClass](int a, int b) {
            const int da = wrapPitchClass(a - rootPitchClass);
            const int db = wrapPitchClass(b - rootPitchClass);
            return da < db;
        });

        return chordPitches;
    }

    int closestMidiForPitchClass(int pitchClass, int target, int lowMidi, int highMidi)
    {
        int best = lowMidi + wrapPitchClass(pitchClass - lowMidi);
        int bestDistance = std::abs(best - target);

        for (int note = best; note <= highMidi; note += 12)
        {
            const int distance = std::abs(note - target);
            if (distance < bestDistance)
            {
                best = note;
                bestDistance = distance;
            }
        }

        return juce::jlimit(lowMidi, highMidi, best);
    }
}

std::vector<std::vector<int>> ArpeggioEngine::buildMidiChords(const ArpeggioContext& context,
                                                              const ArpeggioGenerationSettings& settings)
{
    auto pitchClassChords = ProgressionEngine::getChordNotes(context.romanSequence, context.tonic, context.scaleType);
    std::vector<std::vector<int>> midiChords;
    midiChords.reserve(pitchClassChords.size());

    auto parsedChords = ProgressionEngine::parseProgression(context.romanSequence);
    std::vector<int> previousVoicing;

    for (size_t chordIndex = 0; chordIndex < pitchClassChords.size(); ++chordIndex)
    {
        auto chordPitches = pitchClassChords[chordIndex];
        const int rootPitchClass = chordIndex < static_cast<size_t>(parsedChords.size())
            ? getParsedRootPitchClass(parsedChords[static_cast<int>(chordIndex)], context.tonic, context.scaleType)
            : (!chordPitches.empty() ? chordPitches.front() : 0);
        chordPitches = orderChordFromRoot(chordPitches, rootPitchClass);

        std::vector<int> chord;
        chord.reserve(chordPitches.size() + 1);

        const int low = juce::jmax(settings.lowMidi, 52);
        const int high = juce::jmin(settings.highMidi, 79);

        for (size_t toneIndex = 0; toneIndex < chordPitches.size(); ++toneIndex)
        {
            const int pc = wrapPitchClass(chordPitches[toneIndex]);
            const int defaultTarget = 60 + static_cast<int>(toneIndex) * 4;
            const int target = toneIndex < previousVoicing.size()
                ? previousVoicing[toneIndex]
                : defaultTarget;

            int note = closestMidiForPitchClass(pc, target, low, high);
            if (!chord.empty() && note <= chord.back() - 7)
                note += 12;
            if (note > high)
                note -= 12;

            chord.push_back(juce::jlimit(settings.lowMidi, settings.highMidi, note));
        }

        if (chord.empty())
            continue;

        const int center = std::accumulate(chord.begin(), chord.end(), 0) / static_cast<int>(chord.size());
        if (!previousVoicing.empty())
        {
            const int previousCenter = std::accumulate(previousVoicing.begin(), previousVoicing.end(), 0)
                                     / static_cast<int>(previousVoicing.size());
            if (center - previousCenter > 8)
                for (auto& note : chord) note -= 12;
            else if (previousCenter - center > 8)
                for (auto& note : chord) note += 12;
        }

        for (auto& note : chord)
            note = juce::jlimit(settings.lowMidi, settings.highMidi, note);

        previousVoicing = chord;
        midiChords.push_back(chord);
    }

    return midiChords;
}

ArpeggioGenerationResult ArpeggioEngine::generate(const ArpeggioContext& context,
                                                  const ArpeggioGenerationSettings& settings)
{
    ArpeggioGenerationResult result;

    if (context.romanSequence.trim().isEmpty())
    {
        result.error = "Empty progression";
        return result;
    }

    auto patterns = ArpeggioPatternLibrary::getBuiltInPatterns();
    result.rankedPatterns = ArpeggioMatcher::rankPatterns(context, settings, patterns);
    if (result.rankedPatterns.empty())
    {
        result.error = "No arpeggio patterns available";
        return result;
    }

    result.selectedPattern = result.rankedPatterns.front().pattern;

    if (settings.preferredPatternId.isNotEmpty())
    {
        ArpeggioPattern preferred;
        if (ArpeggioPatternLibrary::findPatternById(settings.preferredPatternId, preferred))
            result.selectedPattern = preferred;
    }

    auto midiChords = buildMidiChords(context, settings);
    if (midiChords.empty())
    {
        result.error = "Could not resolve progression chords";
        return result;
    }

    result.sequence = ArpeggioMidiRenderer::renderToSequence(midiChords, result.selectedPattern, context, settings);
    result.ok = result.sequence.getNumEvents() > 0;
    if (!result.ok)
        result.error = "Generated sequence is empty";

    return result;
}

juce::File ArpeggioEngine::generateMidiFile(const ArpeggioContext& context,
                                            const ArpeggioGenerationSettings& settings)
{
    auto result = generate(context, settings);
    if (!result.ok)
        return {};

    return ArpeggioMidiRenderer::writeMidiFile(result.sequence, context);
}
