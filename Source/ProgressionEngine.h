//==================================================================================================
//  ProgressionEngine.h
//  项目：ChordReference (和弦调性速查器)
//  功能：罗马数字解析器与和弦进行引擎声明
//        - 解析单个罗马数字标记（如 V7、bIIImaj7、V/IV）
//        - 解析完整的进行序列（空格分隔的罗马数字）
//        - 根据调性将级数转换为 MIDI 音高集合
//        - 提供和弦质量公式、音阶级数半音间隔
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_core/juce_core.h>
#include "MusicTypes.h"

//==============================================================================
// 解析后的单个和弦信息
//==============================================================================
struct ParsedChord
{
    int scaleDegree = 1;               // 1-7 音阶级数
    int alteration = 0;                // 临时升降号偏移：+1(#), -1(b), 0
    juce::String quality;              // 和弦质量：major, minor, dom7, maj7, min7, dim7 等
    int bassOverride = -1;             // 斜杠低音级数，-1 表示无
    bool isSecondaryDominant = false;  // 是否为副属和弦 (V/X)
    int secondaryTarget = 1;           // 副属和弦的目标级数（有效时）
};

//==============================================================================
// 罗马数字解析器 & 转调引擎
//==============================================================================
class ProgressionEngine
{
public:
    ProgressionEngine() = default;

    // 解析单个罗马数字标记（如 "V7", "bIIImaj7", "V/IV"）
    static ParsedChord parseRomanToken(const juce::String& token);

    // 解析整个进行字符串（空格分隔的罗马数字序列）
    static juce::Array<ParsedChord> parseProgression(const juce::String& romanSequence);

    // 将解析后的和弦序列转换为音符集合（每个和弦返回一组音高 0‑11）
    static std::vector<std::vector<int>> getChordNotes(
        const juce::String& romanSequence,
        PitchClass tonic,
        ScaleType type);

    // 获取音阶级数对应的半音间隔（相对于主音）
    static int getScaleDegreeInterval(int degree, ScaleType type);

    // 判断罗马数字序列是否合法
    static bool isValidProgression(const juce::String& romanSequence);

private:
    // 和弦质量到音程的映射
    struct ChordFormula
    {
        juce::String quality;
        std::vector<int> intervals;   // 相对于根音的半音数
    };

    static const ChordFormula chordFormulas[];

    // 获取调式的全部七级半音间隔
    static std::vector<int> getScaleIntervals(ScaleType type);

    // 将音阶级数转换为音高（0‑11）
    static int degreeToPitch(int degree, int alteration, PitchClass tonic, ScaleType type);
};