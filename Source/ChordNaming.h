//==================================================================================================
//  ChordNaming.h
//  项目：ChordReference (和弦调性速查器)
//  功能：集中处理和弦符号、中文名称、质量后缀等字符串生成
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include "MusicTypes.h"
#include <juce_core/juce_core.h>

class ChordNaming
{
public:
    // 根据根音、升/降号倾向、质量、音符数量和引申音后缀生成和弦符号（如 "Cm7b5", "G7#9"）
    static juce::String getSymbol(PitchClass root, bool isSharpKey, ChordQuality quality,
                                  int numNotes, const juce::String& extSuffix = {});

    // 根据和弦符号字符串生成中文名称（如 "Cmaj7" → "C大七"）
    static juce::String getChineseName(const juce::String& symbol);

    // 根据功能标识（如 "T", "S7", "D9"）和调式生成完整符号（根音 + 质量后缀）
    static juce::String getSymbolFromFunction(PitchClass root, bool isSharpKey,
                                              const juce::String& funcName, bool isMajor);
};