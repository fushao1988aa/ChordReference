//==================================================================================================
//  PresetInspectorPanel.h
//  项目：ChordReference (和弦调性速查器)
//  功能：预设详情面板组件声明
//        - 显示预设封面、名称、和弦进行及描述
//        - 提供拖拽到轨道按钮（外部处理拖拽逻辑）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ProgressionData.h"
#include "MusicTypes.h"

class PresetInspectorPanel : public juce::Component
{
public:
    PresetInspectorPanel();

    // 设置当前要显示的预设条目及调性
    void setEntry(const ProgressionEntry& entry, PitchClass tonic, ScaleType type);

    // 绘制和布局
    void paint(juce::Graphics&) override;
    void resized() override;

    // 拖拽按钮（由外部 ProgressionBrowser 添加鼠标监听实现拖拽）
    juce::TextButton dragToTrackButton;
    juce::TextButton dragArpeggioToTrackButton;

private:
    ProgressionEntry currentEntry;
    PitchClass tonic = PitchClass::C;
    ScaleType scaleType = ScaleType::Major;
    juce::Image cachedCoverImage;      // 封面解码后的图像

    // 根据当前调性生成和弦符号字符串
    juce::String getChordSymbols() const;
    juce::String getRoman() const;
};
