//==================================================================================================
//  MoodMapper.h
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦情绪与颜色映射声明
//        - 根据调性主音、功能符号（T/S/D/t/s）及调式返回情绪描述与氛围色
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include "MusicTypes.h"
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
// 情绪信息结构体
//   description : 情绪文字（中文）
//   colour      : 氛围色（用于卡片背景混合）
//==============================================================================
struct MoodInfo
{
    juce::String description;
    juce::Colour colour;
};

class MoodMapper
{
public:
    // 根据主音、功能名（T/S/D/t/s 等）、是否大调返回情绪信息
    static MoodInfo getMood(PitchClass tonic, const juce::String& function, bool isMajor);
};