//==================================================================================================
//  ProgressionRenderer.h
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦进行 MIDI 渲染器声明
//        - 定义渲染设置（和弦排列、节奏模式、人性化等）
//        - 提供静态方法将罗马序列生成 MIDI 文件或 MidiMessageSequence
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "MusicTypes.h"

struct RenderSettings
{
    enum class VoicingMode { Block, Drop2, Spread, Piano };
    enum class RhythmPattern { Block, ArpUp, ArpDown, ArpUpDown };

    VoicingMode voicing = VoicingMode::Block;
    RhythmPattern rhythm = RhythmPattern::Block;
    double bpm = 120.0;
    double noteLength = 4.0;          // 每个和弦持续拍数
    bool humanize = true;
    int humanizeVelocityRange = 5;
    int humanizeTimeRangeMs = 10;
    bool bassDouble = true;
};

class ProgressionRenderer
{
public:
    ProgressionRenderer() = default;

    static juce::File generateMidiFile(
        const juce::String& romanSequence,
        PitchClass tonic, ScaleType type,
        const RenderSettings& settings);

    static juce::MidiMessageSequence generateSequence(
        const juce::String& romanSequence,
        PitchClass tonic, ScaleType type,
        const RenderSettings& settings);

private:
    static std::vector<std::vector<int>> applyVoiceLeading(
        const std::vector<std::vector<int>>& rawChords,
        int lowestMidi = 48, int highestMidi = 84);

    static std::vector<std::vector<int>> applyVoicing(
        const std::vector<std::vector<int>>& chords,
        RenderSettings::VoicingMode mode);

    static void humanizeMessage(juce::MidiMessage& msg);
};