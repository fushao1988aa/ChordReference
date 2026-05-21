//==================================================================================================
//  ProgressionRenderer.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦进行 MIDI 渲染器实现
//        - 根据进行字符串与调式生成 MIDI 文件
//        - 支持四种节奏模式（柱式、上行琶音、下行琶音、上下行琶音）
//        - 声部导向与多种和声排列（Drop2, Spread, Piano）
//        - 人性化处理（力度与时间微调）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ProgressionRenderer.h"
#include "ProgressionEngine.h"
#include <algorithm>
#include <random>
#include <ctime>

//==============================================================================
// 生成 MIDI 文件
//==============================================================================
juce::File ProgressionRenderer::generateMidiFile(
    const juce::String& romanSequence,
    PitchClass tonic,
    ScaleType type,
    const RenderSettings& settings)
{
    const int ppq = 960;
    juce::MidiFile midi;
    midi.setTicksPerQuarterNote(ppq);

    juce::MidiMessageSequence track;

    // 1. 速度事件
    auto tempoEvent = juce::MidiMessage::tempoMetaEvent(
        static_cast<int>(60000000.0 / settings.bpm));
    tempoEvent.setTimeStamp(0.0);
    track.addEvent(tempoEvent);

    // 2. 拍号 (4/4)
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0.0);

    // 3. 生成音符序列
    auto sequence = generateSequence(romanSequence, tonic, type, settings);
    track.addSequence(sequence, 0.0);

    // 4. 结束标记
    auto endOfTrack = juce::MidiMessage::endOfTrack();
    endOfTrack.setTimeStamp(track.getEndTime() + 0.1);
    track.addEvent(endOfTrack);

    // 5. 时间刻度转换为 tick
    double ticksPerSecond = ppq * settings.bpm / 60.0;
    juce::MidiMessageSequence trackTicks;
    for (int i = 0; i < track.getNumEvents(); ++i)
    {
        double timeSec = track.getEventTime(i);
        double timeTicks = timeSec * ticksPerSecond;
        trackTicks.addEvent(track.getEventPointer(i)->message, timeTicks);
    }

    midi.addTrack(trackTicks);

    juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    juce::File midiFile = tempDir.getNonexistentChildFile("ChordProg", ".mid");
    juce::FileOutputStream outStream(midiFile);
    if (outStream.openedOk())
    {
        midi.writeTo(outStream);
        return midiFile;
    }

    return {};
}

//==============================================================================
// 生成音符序列（应用声部导向、排列、节奏）
//==============================================================================
juce::MidiMessageSequence ProgressionRenderer::generateSequence(
    const juce::String& romanSequence,
    PitchClass tonic,
    ScaleType type,
    const RenderSettings& settings)
{
    juce::MidiMessageSequence track;
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

    // 获取原始和弦音名（0‑11）
    auto rawChords = ProgressionEngine::getChordNotes(romanSequence, tonic, type);
    if (rawChords.empty()) return track;

    // 声部导向 + 排列处理
    auto chords = applyVoiceLeading(rawChords, 48, 84);
    chords = applyVoicing(chords, settings.voicing);

    double beatDuration = 60.0 / settings.bpm;
    double chordDuration = beatDuration * settings.noteLength;
    double currentTime = 0.0;
    std::uniform_int_distribution<int> velocityDrift(-settings.humanizeVelocityRange, settings.humanizeVelocityRange);
    std::uniform_real_distribution<double> timingDrift(-settings.humanizeTimeRangeMs / 1000.0,
                                                       settings.humanizeTimeRangeMs / 1000.0);

    auto meterAccent = [&](double eventTime)
    {
        const int beatIndex = static_cast<int>((eventTime / beatDuration) + 0.0001) % 4;
        if (beatIndex == 0) return 12;
        if (beatIndex == 2) return 5;
        return -5;
    };

    auto noteVelocity = [&](double eventTime, size_t noteIndex, bool isBass)
    {
        const int base = isBass ? 88 : (noteIndex == 0 ? 100 : 86 - std::min<int>(static_cast<int>(noteIndex), 5) * 3);
        const int drift = settings.humanize ? velocityDrift(rng) : 0;
        const int accent = settings.humanize ? meterAccent(eventTime) : 0;
        return std::clamp(base + accent + drift, isBass ? 56 : 52, isBass ? 112 : 116);
    };

    auto noteStartTime = [&](double baseTime, double noteLength, size_t noteIndex, bool isBlockChord)
    {
        if (!settings.humanize)
            return baseTime;

        const double strum = isBlockChord ? std::min(0.012, static_cast<double>(noteIndex) * 0.003) : 0.0;
        const double shifted = baseTime + strum + timingDrift(rng) * 0.45;
        return std::clamp(shifted, baseTime, baseTime + noteLength * 0.20);
    };

    auto noteEndTime = [&](double startTime, double nominalEnd)
    {
        if (!settings.humanize)
            return nominalEnd;

        const double shifted = nominalEnd + timingDrift(rng) * 0.35;
        return std::max(startTime + 0.05, shifted);
    };

    // 根据节奏模式生成音符事件
    for (size_t ci = 0; ci < chords.size(); ++ci)
    {
        auto& chord = chords[ci];

        if (settings.rhythm == RenderSettings::RhythmPattern::Block)
        {
            // 柱式：所有音符同时发声
            for (size_t ni = 0; ni < chord.size(); ++ni)
            {
                const int note = chord[ni];
                const double start = noteStartTime(currentTime, chordDuration, ni, true);
                const double end = noteEndTime(start, currentTime + chordDuration * 0.92);
                auto noteOn = juce::MidiMessage::noteOn(
                    1, note, static_cast<juce::uint8>(noteVelocity(currentTime, ni, false)));
                noteOn.setTimeStamp(start);
                auto noteOff = juce::MidiMessage::noteOff(1, note);
                noteOff.setTimeStamp(end);

                track.addEvent(noteOn);
                track.addEvent(noteOff);
            }

            // 低音加倍
            if (settings.bassDouble && !chord.empty())
            {
                int bassNote = chord[0] - 12;
                if (bassNote >= 24)
                {
                    auto bassOn = juce::MidiMessage::noteOn(
                        1, bassNote, static_cast<juce::uint8>(noteVelocity(currentTime, 0, true)));
                    bassOn.setTimeStamp(currentTime);
                    auto bassOff = juce::MidiMessage::noteOff(1, bassNote);
                    bassOff.setTimeStamp(noteEndTime(currentTime, currentTime + chordDuration * 0.92));
                    track.addEvent(bassOn);
                    track.addEvent(bassOff);
                }
            }

            currentTime += chordDuration;
        }
        else if (settings.rhythm == RenderSettings::RhythmPattern::ArpUp)
        {
            double noteStep = beatDuration * 0.25;
            for (size_t ni = 0; ni < chord.size(); ++ni)
            {
                double noteStart = currentTime + ni * noteStep;
                noteStart = noteStartTime(noteStart, noteStep, ni, false);
                auto noteOn = juce::MidiMessage::noteOn(
                    1, chord[ni], static_cast<juce::uint8>(noteVelocity(currentTime + ni * noteStep, ni, false)));
                noteOn.setTimeStamp(noteStart);

                double sustain = (ni == chord.size() - 1)
                    ? chordDuration * 0.8
                    : noteStep * 1.5;
                auto noteOff = juce::MidiMessage::noteOff(1, chord[ni]);
                noteOff.setTimeStamp(noteEndTime(noteStart, noteStart + sustain));

                track.addEvent(noteOn);
                track.addEvent(noteOff);
            }
            currentTime += chordDuration;
        }
        else if (settings.rhythm == RenderSettings::RhythmPattern::ArpDown)
        {
            double noteStep = beatDuration * 0.25;
            for (size_t ni = 0; ni < chord.size(); ++ni)
            {
                size_t revIdx = chord.size() - 1 - ni;
                double noteStart = currentTime + ni * noteStep;
                noteStart = noteStartTime(noteStart, noteStep, ni, false);
                auto noteOn = juce::MidiMessage::noteOn(
                    1, chord[revIdx], static_cast<juce::uint8>(noteVelocity(currentTime + ni * noteStep, ni, false)));
                noteOn.setTimeStamp(noteStart);

                double sustain = (ni == chord.size() - 1)
                    ? chordDuration * 0.8
                    : noteStep * 1.5;
                auto noteOff = juce::MidiMessage::noteOff(1, chord[revIdx]);
                noteOff.setTimeStamp(noteEndTime(noteStart, noteStart + sustain));

                track.addEvent(noteOn);
                track.addEvent(noteOff);
            }
            currentTime += chordDuration;
        }
        else if (settings.rhythm == RenderSettings::RhythmPattern::ArpUpDown)
        {
            double noteStep = beatDuration * 0.25;
            size_t count = 0;
            // 上行
            for (size_t ni = 0; ni < chord.size(); ++ni, ++count)
            {
                double noteStart = currentTime + count * noteStep;
                noteStart = noteStartTime(noteStart, noteStep, count, false);
                auto noteOn = juce::MidiMessage::noteOn(
                    1, chord[ni], static_cast<juce::uint8>(noteVelocity(currentTime + count * noteStep, ni, false)));
                noteOn.setTimeStamp(noteStart);
                auto noteOff = juce::MidiMessage::noteOff(1, chord[ni]);
                noteOff.setTimeStamp(noteEndTime(noteStart, noteStart + noteStep * 2));
                track.addEvent(noteOn);
                track.addEvent(noteOff);
            }
            // 下行（跳过最高音）
            for (int ni = static_cast<int>(chord.size()) - 2; ni >= 0; --ni, ++count)
            {
                double noteStart = currentTime + count * noteStep;
                noteStart = noteStartTime(noteStart, noteStep, count, false);
                auto noteOn = juce::MidiMessage::noteOn(
                    1, chord[static_cast<size_t>(ni)], static_cast<juce::uint8>(noteVelocity(currentTime + count * noteStep, count, false)));
                noteOn.setTimeStamp(noteStart);
                auto noteOff = juce::MidiMessage::noteOff(1, chord[static_cast<size_t>(ni)]);
                noteOff.setTimeStamp(noteEndTime(noteStart, noteStart + noteStep * 2));
                track.addEvent(noteOn);
                track.addEvent(noteOff);
            }
            currentTime += chordDuration;
        }
    }

    return track;
}

//==============================================================================
// 声部导向：将原始音高类映射到合理的 MIDI 音域，并保持相邻和弦连接平滑
//==============================================================================
std::vector<std::vector<int>> ProgressionRenderer::applyVoiceLeading(
    const std::vector<std::vector<int>>& rawChords,
    int lowestMidi, int highestMidi)
{
    if (rawChords.empty()) return rawChords;

    std::vector<std::vector<int>> result;
    std::vector<int> previousNotes;

    for (size_t ci = 0; ci < rawChords.size(); ++ci)
    {
        auto pitches = rawChords[ci];
        if (pitches.empty())
        {
            result.push_back(pitches);
            continue;
        }

        std::vector<int> expanded;

        if (ci == 0)  // 第一个和弦：基于根音定位到 C3 附近
        {
            int root = pitches[0];
            int rootMidi = 60 + (root - (60 % 12) + 12) % 12;
            if (rootMidi < 60) rootMidi += 12;
            int rootOctave = rootMidi / 12;

            for (int pc : pitches)
            {
                int midiNote = rootOctave * 12 + pc;
                while (midiNote < rootMidi) midiNote += 12;
                if (midiNote > 96) midiNote -= 12;
                expanded.push_back(midiNote);
            }
        }
        else
        {
            // 后续和弦：尽量保持声部连接（相同音名保持同一音符）
            for (int pc : pitches)
            {
                bool foundSame = false;
                // 尝试在之前和弦中找到相同音名的音符
                for (int prevNote : previousNotes)
                {
                    if ((prevNote % 12) == pc)
                    {
                        expanded.push_back(prevNote);
                        foundSame = true;
                        break;
                    }
                }

                // 若未找到相同音名，则在合理八度内找最近距离
                if (!foundSame)
                {
                    int bestNote = -1;
                    int bestDistance = 999;
                    for (int oct = 2; oct <= 6; ++oct)
                    {
                        int candidate = oct * 12 + pc;
                        if (candidate >= lowestMidi && candidate <= highestMidi)
                        {
                            // 计算与前一和弦中最接近音符的距离（简单取与全部前一音符的最小距离）
                            int distance = 999;
                            if (!previousNotes.empty())
                            {
                                for (int prev : previousNotes)
                                    distance = std::min(distance, std::abs(candidate - prev));
                            }
                            else
                            {
                                distance = 0;
                            }
                            if (distance < bestDistance)
                            {
                                bestNote = candidate;
                                bestDistance = distance;
                            }
                        }
                    }
                    expanded.push_back(bestNote >= 0 ? bestNote : (5 * 12 + pc));
                }
            }
        }

        std::sort(expanded.begin(), expanded.end());
        result.push_back(expanded);
        previousNotes = expanded;
    }

    return result;
}

//==============================================================================
// 排列方式处理（Drop2、Spread、Piano）
//==============================================================================
std::vector<std::vector<int>> ProgressionRenderer::applyVoicing(
    const std::vector<std::vector<int>>& chords,
    RenderSettings::VoicingMode mode)
{
    if (mode == RenderSettings::VoicingMode::Block)
        return chords;

    std::vector<std::vector<int>> result;

    for (auto& chord : chords)
    {
        if (chord.size() < 4)
        {
            result.push_back(chord);
            continue;
        }

        auto voiced = chord;

        if (mode == RenderSettings::VoicingMode::Drop2)
        {
            if (voiced.size() >= 3)
            {
                int dropIndex = std::max(0, static_cast<int>(voiced.size()) - 2);
                voiced[dropIndex] -= 12;
            }
        }
        else if (mode == RenderSettings::VoicingMode::Spread)
        {
            voiced[0] -= 12;
            voiced[voiced.size() - 1] += 12;
        }
        else if (mode == RenderSettings::VoicingMode::Piano)
        {
            voiced[0] -= 12;
        }

        std::sort(voiced.begin(), voiced.end());
        result.push_back(voiced);
    }

    return result;
}

//==============================================================================
// 人性化：随机力度偏移、微小时间偏移
//==============================================================================
void ProgressionRenderer::humanizeMessage(juce::MidiMessage& msg)
{
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)) +
                            reinterpret_cast<std::uintptr_t>(&msg));
    static std::uniform_int_distribution<int> velDist(-5, 5);
    static std::uniform_real_distribution<double> timeDist(-0.005, 0.005);

    if (msg.isNoteOn())
    {
        int vel = std::clamp(msg.getVelocity() + velDist(rng), 20, 127);
        msg.setVelocity(static_cast<juce::uint8>(vel));
    }

    double ts = msg.getTimeStamp();
    ts += timeDist(rng);
    if (ts < 0.0) ts = 0.0;
    msg.setTimeStamp(ts);
}
