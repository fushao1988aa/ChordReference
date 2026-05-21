//==================================================================================================
//  ArpeggioMidiRenderer.cpp
//  Project: ChordReference
//  Purpose: MIDI rendering implementation for intelligent arpeggios.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#include "ArpeggioMidiRenderer.h"
#include "ArpeggioHumanizer.h"
#include "ProgressionEngine.h"
#include <algorithm>
#include <cmath>

namespace
{
    enum class ChordFunctionRole
    {
        Tonic,
        Subdominant,
        Dominant,
        Passing
    };

    std::vector<ChordFunctionRole> getFunctionRoles(const juce::String& romanSequence)
    {
        std::vector<ChordFunctionRole> roles;
        auto parsed = ProgressionEngine::parseProgression(romanSequence);
        roles.reserve(static_cast<size_t>(parsed.size()));

        for (const auto& chord : parsed)
        {
            if (chord.isSecondaryDominant || chord.scaleDegree == 5 || chord.scaleDegree == 7)
                roles.push_back(ChordFunctionRole::Dominant);
            else if (chord.scaleDegree == 2 || chord.scaleDegree == 4)
                roles.push_back(ChordFunctionRole::Subdominant);
            else if (chord.scaleDegree == 1 || chord.scaleDegree == 6)
                roles.push_back(ChordFunctionRole::Tonic);
            else
                roles.push_back(ChordFunctionRole::Passing);
        }

        return roles;
    }

    ChordFunctionRole getRoleAt(const std::vector<ChordFunctionRole>& roles, size_t index)
    {
        return index < roles.size() ? roles[index] : ChordFunctionRole::Passing;
    }

    bool isLateGroovePattern(const ArpeggioPattern& pattern)
    {
        return pattern.grooveAffinity.equalsIgnoreCase("Late")
            || pattern.grooveAffinity.equalsIgnoreCase("Shuffle")
            || pattern.id.containsIgnoreCase("rnb")
            || pattern.id.containsIgnoreCase("lofi")
            || pattern.id.containsIgnoreCase("sway");
    }

    bool isTightGroovePattern(const ArpeggioPattern& pattern)
    {
        return pattern.grooveAffinity.equalsIgnoreCase("Tight")
            || pattern.id.containsIgnoreCase("future")
            || pattern.id.containsIgnoreCase("latin")
            || pattern.id.containsIgnoreCase("gate");
    }

    double grooveOffsetBeats(const ArpeggioPattern& pattern,
                             const ArpeggioGenerationSettings& settings,
                             double localBeat,
                             ChordFunctionRole role)
    {
        const double amount = juce::jlimit(0.0, 1.0, settings.humanize.amount / 100.0);
        const double fraction = localBeat - std::floor(localBeat);
        double offset = 0.0;

        if (isLateGroovePattern(pattern))
        {
            if (std::abs(fraction - 0.5) < 0.09 || std::abs(fraction - 0.75) < 0.09)
                offset += 0.035 * amount;
            if (role == ChordFunctionRole::Dominant && fraction > 0.55)
                offset -= 0.012 * amount;
        }
        else if (isTightGroovePattern(pattern))
        {
            if (std::abs(fraction - 0.25) < 0.08)
                offset -= 0.014 * amount;
            if (std::abs(fraction - 0.75) < 0.08)
                offset += 0.018 * amount;
        }
        else
        {
            if (std::abs(fraction - 0.5) < 0.08)
                offset += 0.014 * amount;
        }

        return offset;
    }

    int functionVelocityOffset(ChordFunctionRole role, bool isLastChord)
    {
        int offset = 0;
        switch (role)
        {
            case ChordFunctionRole::Dominant:     offset = 8; break;
            case ChordFunctionRole::Subdominant: offset = 3; break;
            case ChordFunctionRole::Tonic:       offset = -1; break;
            case ChordFunctionRole::Passing:     offset = 2; break;
        }

        return isLastChord ? offset - 5 : offset;
    }

    double functionDurationScale(ChordFunctionRole role, bool isLastChord, double localBeat, double beatsPerChord)
    {
        double scale = 1.0;
        if (role == ChordFunctionRole::Dominant)
            scale *= 0.92;
        else if (role == ChordFunctionRole::Tonic)
            scale *= 1.05;

        if (isLastChord && localBeat > beatsPerChord * 0.45)
            scale *= 1.28;

        return scale;
    }

    bool shouldLeavePhraseSpace(const ArpeggioPattern& pattern,
                                bool isLastChord,
                                const ArpeggioStep& step,
                                double localBeat,
                                double beatsPerChord,
                                int eventIndex)
    {
        if (!isLastChord || step.isBassAnchor || pattern.density == ArpeggioDensity::Whole)
            return false;

        if (localBeat < beatsPerChord * 0.62)
            return false;

        return pattern.density == ArpeggioDensity::Sixteenth || pattern.density == ArpeggioDensity::Mixed
            ? (eventIndex % 2 == 1)
            : (eventIndex % 3 == 1);
    }

    int clampToRangeByOctave(int note, int low, int high)
    {
        while (note < low) note += 12;
        while (note > high) note -= 12;
        return juce::jlimit(0, 127, note);
    }

    int getBassRoot(const std::vector<int>& chord)
    {
        if (chord.empty())
            return 36;

        return clampToRangeByOctave(chord.front(), 36, 48);
    }

    int getBassOctave(const std::vector<int>& chord)
    {
        return juce::jlimit(36, 55, getBassRoot(chord) + 12);
    }

        int getLeftHandRoot(const std::vector<int>& chord)
    {
        if (chord.empty())
            return 48;

        return clampToRangeByOctave(chord.front(), 45, 57);
    }

    int getLeftHandFifth(const std::vector<int>& chord)
    {
        if (chord.empty())
            return 55;

        return clampToRangeByOctave(chord.front() + 7, 47, 59);
    }

    int getUpperColourNote(const std::vector<int>& chord,
                           ChordFunctionRole role,
                           const ArpeggioPattern& pattern,
                           int colourIndex)
    {
        if (chord.empty())
            return 72;

        const int root = chord.front();
        juce::Array<int> intervals;

        if (role == ChordFunctionRole::Dominant)
            intervals.addArray({ 10, 14 });
        else if (pattern.familyId == "ChineseOpenFamily" || pattern.id.containsIgnoreCase("chinese"))
            intervals.addArray({ 2, 7, 9 });
        else if (pattern.familyId == "NeoSoulFamily" || pattern.id.containsIgnoreCase("rnb") || pattern.id.containsIgnoreCase("lofi"))
            intervals.addArray({ 14, 9, 10 });
        else
            intervals.addArray({ 14, 9, 7 });

        const int interval = intervals[colourIndex % intervals.size()];
        return clampToRangeByOctave(root + interval + 12, 67, 88);
    }

    bool shouldAddBassLayer(const ArpeggioPattern& pattern)
    {
        return pattern.density != ArpeggioDensity::Whole;
    }

    bool shouldAddColourLayer(const ArpeggioPattern& pattern)
    {
        return pattern.supportsExtensions && pattern.density != ArpeggioDensity::Whole;
    }

    void addLayeredNote(juce::MidiMessageSequence& sequence,
                        int note,
                        double start,
                        double duration,
                        int velocity,
                        double chordStart,
                        double beatsPerChord)
    {
        const double safeStart = juce::jlimit(chordStart, chordStart + beatsPerChord - 0.04, start);
        const double safeEnd = juce::jmin(chordStart + beatsPerChord - 0.01, safeStart + duration);
        if (safeEnd <= safeStart + 0.02)
            return;

        sequence.addEvent(juce::MidiMessage::noteOn(1, juce::jlimit(0, 127, note), static_cast<juce::uint8>(juce::jlimit(1, 127, velocity))), safeStart);
        sequence.addEvent(juce::MidiMessage::noteOff(1, juce::jlimit(0, 127, note)), safeEnd);
    }

    void renderEnhancementLayers(juce::MidiMessageSequence& sequence,
                                 const std::vector<int>& chord,
                                 const ArpeggioPattern& pattern,
                                 const ArpeggioContext& context,
                                 const ArpeggioGenerationSettings& settings,
                                 ChordFunctionRole role,
                                 bool isLastChord,
                                 double chordStart,
                                 double beatsPerChord,
                                 int chordIndex)
    {
        const double amount = settings.humanize.enabled ? juce::jlimit(0.0, 1.0, settings.humanize.amount / 100.0) : 0.0;

        if (settings.includeBassDoubling && shouldAddBassLayer(pattern))
        {
            const int root = getBassRoot(chord);
            const int bassVelocity = juce::jlimit(42, 92, 60 + pattern.energy * 3 + functionVelocityOffset(role, isLastChord));
            const double late = isLateGroovePattern(pattern) ? 0.018 * amount : 0.0;
            addLayeredNote(sequence, root, chordStart + late, beatsPerChord * (isLastChord ? 0.72 : 0.46), bassVelocity + 5, chordStart, beatsPerChord);
        }

        if (settings.includeLeftRootBreakdown && shouldAddBassLayer(pattern))
        {
            const int root = getLeftHandRoot(chord);
            const int fifth = getLeftHandFifth(chord);
            const int octave = getBassOctave(chord);
            const int bassVelocity = juce::jlimit(42, 92, 58 + pattern.energy * 3 + functionVelocityOffset(role, isLastChord));
            const double late = isLateGroovePattern(pattern) ? 0.014 * amount : 0.0;
            const double push = isTightGroovePattern(pattern) ? -0.008 * amount : 0.0;
            addLayeredNote(sequence, root, chordStart + late, beatsPerChord * 0.28, bassVelocity + 1, chordStart, beatsPerChord);

        if (beatsPerChord >= 2.0 && pattern.id != "alberti")
                addLayeredNote(sequence, fifth, chordStart + beatsPerChord * 0.50 + push, beatsPerChord * 0.32, bassVelocity - 4, chordStart, beatsPerChord);


        if ((pattern.density == ArpeggioDensity::Mixed || pattern.density == ArpeggioDensity::Sixteenth || pattern.energy >= 4)
                && beatsPerChord >= 3.0
                && !isLastChord)
            {
                addLayeredNote(sequence, octave, chordStart + beatsPerChord * 0.74 + push, beatsPerChord * 0.20, bassVelocity - 9, chordStart, beatsPerChord);
            }
        }

        if (settings.includeUpperColourLayer && shouldAddColourLayer(pattern))
        {
            const bool sparsePattern = pattern.density == ArpeggioDensity::Half
                                    || pattern.familyId == "CinematicOstinatoFamily"
                                    || pattern.familyId == "ChineseOpenFamily"
                                    || pattern.id.containsIgnoreCase("cinematic")
                                    || pattern.id.containsIgnoreCase("chinese");
            const bool energeticPattern = pattern.density == ArpeggioDensity::Mixed || pattern.density == ArpeggioDensity::Sixteenth || pattern.energy >= 4;
            const int colourVelocity = juce::jlimit(36, 84, 52 + pattern.energy * 4 + functionVelocityOffset(role, isLastChord) / 2);
            const double startA = chordStart + beatsPerChord * (sparsePattern ? 0.56 : 0.68);
            const double durA = beatsPerChord * (sparsePattern ? 0.34 : 0.16);

            addLayeredNote(sequence,
                           getUpperColourNote(chord, role, pattern, chordIndex),
                           startA + (isLateGroovePattern(pattern) ? 0.022 * amount : 0.0),
                           durA,
                           colourVelocity,
                           chordStart,
                           beatsPerChord);

            if (energeticPattern && !isLastChord)
            {
                addLayeredNote(sequence,
                               getUpperColourNote(chord, role, pattern, chordIndex + 1),
                               chordStart + beatsPerChord * 0.88 + (isTightGroovePattern(pattern) ? -0.01 * amount : 0.0),
                               beatsPerChord * 0.10,
                               colourVelocity - 8,
                               chordStart,
                               beatsPerChord);
            }
        }
    }
}

int ArpeggioMidiRenderer::resolveStepNote(const std::vector<int>& chord,
                                          const ArpeggioStep& step,
                                          const ArpeggioPattern& pattern,
                                          int chordIndex,
                                          int cycleIndex)
{
    if (chord.empty())
        return 60;

    int index = 0;
    if (!step.isBassAnchor)
    {
        const int chordSize = static_cast<int>(chord.size());
        const int baseIndex = ((step.chordToneIndex % chordSize) + chordSize) % chordSize;
        const bool allowRotation = pattern.direction == ArpeggioDirection::Smart
                                || pattern.direction == ArpeggioDirection::RandomLike
                                || pattern.density == ArpeggioDensity::Mixed;
        const int rotation = allowRotation ? (chordIndex + cycleIndex) % chordSize : 0;
        index = (baseIndex + rotation) % chordSize;
    }

    return juce::jlimit(0, 127, chord[static_cast<size_t>(index)] + step.octaveOffset * 12);
}

juce::MidiMessageSequence ArpeggioMidiRenderer::renderToSequence(const std::vector<std::vector<int>>& midiChords,
                                                                 const ArpeggioPattern& pattern,
                                                                 const ArpeggioContext& context,
                                                                 const ArpeggioGenerationSettings& settings)
{
    juce::MidiMessageSequence sequence;
    ArpeggioHumanizer humanizer(settings.humanize);

    if (midiChords.empty())
        return sequence;

    const double beatsPerChord = juce::jmax(0.25, context.beatsPerChord);
    const double cycleBeats = juce::jmax(0.25, pattern.cycleBeats);
    const auto functionRoles = getFunctionRoles(context.romanSequence);
    int eventIndex = 0;

    for (size_t chordIndex = 0; chordIndex < midiChords.size(); ++chordIndex)
    {
        auto chord = midiChords[chordIndex];
        chord.erase(std::unique(chord.begin(), chord.end()), chord.end());

        const double chordStart = static_cast<double>(chordIndex) * beatsPerChord;
        const bool isLastChord = chordIndex + 1 == midiChords.size();
        const auto role = getRoleAt(functionRoles, chordIndex);

        if (pattern.steps.empty())
        {
            for (int note : chord)
            {
                const int velocity = juce::jlimit(1, 127, 82 + functionVelocityOffset(role, isLastChord));
                sequence.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(velocity)), chordStart);
                sequence.addEvent(juce::MidiMessage::noteOff(1, note), chordStart + beatsPerChord * (isLastChord ? 0.98 : 0.9));
            }
            continue;
        }

        if (settings.includePedal)
        {
            const double pedalOffLead = settings.humanize.pedalMode == ArpeggioPedalMode::Clean ? 0.08 : 0.02;
            sequence.addEvent(juce::MidiMessage::controllerEvent(1, 64, 127), chordStart);
            sequence.addEvent(juce::MidiMessage::controllerEvent(1, 64, 0), chordStart + beatsPerChord - pedalOffLead);
        }

        renderEnhancementLayers(sequence, chord, pattern, context, settings, role, isLastChord,
                                chordStart, beatsPerChord, static_cast<int>(chordIndex));

        int cycleIndex = 0;
        for (double cycleStart = 0.0; cycleStart < beatsPerChord - 0.0001; cycleStart += cycleBeats, ++cycleIndex)
        {
            for (const auto& step : pattern.steps)
            {
                const double localBeat = cycleStart + step.beatOffset;
                if (localBeat >= beatsPerChord)
                    continue;

                if (shouldLeavePhraseSpace(pattern, isLastChord, step, localBeat, beatsPerChord, eventIndex))
                    continue;

                const int note = resolveStepNote(chord, step, pattern, static_cast<int>(chordIndex), cycleIndex);
                const int beatIndex = static_cast<int>(std::floor(localBeat)) % juce::jmax(1, context.beatsPerBar);
                const int baseVelocity = juce::jlimit(62, 104, 74 + pattern.energy * 3 + functionVelocityOffset(role, isLastChord));
                const int velocity = humanizer.velocityFor(baseVelocity, step.velocityOffset, beatIndex, step.isBassAnchor);
                const double groove = grooveOffsetBeats(pattern, settings, localBeat, role);
                const double start = chordStart + localBeat + groove + humanizer.timingOffsetBeats(context.bpm, eventIndex);
                const double durationScale = humanizer.durationScale(eventIndex)
                                           * functionDurationScale(role, isLastChord, localBeat, beatsPerChord);
                const double duration = juce::jlimit(0.05, beatsPerChord, step.durationBeats * durationScale);
                const double end = juce::jmin(chordStart + beatsPerChord - 0.01, start + duration);

                sequence.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(velocity)), juce::jmax(0.0, start));
                sequence.addEvent(juce::MidiMessage::noteOff(1, note), juce::jmax(0.0, end));
                ++eventIndex;
            }
        }
    }

    sequence.updateMatchedPairs();
    return sequence;
}

juce::File ArpeggioMidiRenderer::writeMidiFile(const juce::MidiMessageSequence& beatSequence,
                                               const ArpeggioContext& context,
                                               const juce::String& filePrefix)
{
    constexpr int ppq = 960;
    juce::MidiFile midi;
    midi.setTicksPerQuarterNote(ppq);

    juce::MidiMessageSequence track;
    track.addEvent(juce::MidiMessage::tempoMetaEvent(static_cast<int>(60000000.0 / juce::jmax(1.0, context.bpm))), 0.0);
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(context.beatsPerBar, 4), 0.0);

    for (int i = 0; i < beatSequence.getNumEvents(); ++i)
    {
        const auto* event = beatSequence.getEventPointer(i);
        if (event != nullptr)
            track.addEvent(event->message, event->message.getTimeStamp() * ppq);
    }

    auto end = juce::MidiMessage::endOfTrack();
    end.setTimeStamp((beatSequence.getEndTime() + 0.25) * ppq);
    track.addEvent(end);
    midi.addTrack(track);

    auto midiFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getNonexistentChildFile(filePrefix, ".mid");
    juce::FileOutputStream out(midiFile);
    if (out.openedOk())
    {
        midi.writeTo(out);
        return midiFile;
    }

    return {};
}
