//==================================================================================================
//  PianoSamplerVoice.h
//  项目：ChordReference (和弦调性速查器)
//  功能：内置钢琴合成器语音类（继承自 SamplerVoice）
//        - 复用于加载在内存中的钢琴采样
//        - 当前无额外逻辑，保留扩展空间
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class PianoSamplerVoice : public juce::SamplerVoice
{
public:
    // 继承自 SamplerVoice，无需额外代码
};