//==================================================================================================
//  PluginEditor.h
//  项目：ChordReference (和弦调性速查器)
//  功能：插件编辑器主界面声明
//        - 组合 TopBar 与 ScaledContentComponent
//        - 管理预设浏览器、注册覆盖层
//        - 定时从 Processor 获取 MIDI 识别结果并更新界面
//        - 窗口大小保存与恢复、皮肤语言状态同步
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "ScaledContentComponent.h"
#include "TopBar.h"
#include "RegistrationOverlay.h"
#include "ProgressionData.h"
#include "ProgressionBrowser.h"

class ChordReferenceAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    //==============================================================================
    // 构造与析构
    //==============================================================================
    ChordReferenceAudioProcessorEditor(ChordReferenceAudioProcessor&);
    ~ChordReferenceAudioProcessorEditor() override;

    //==============================================================================
    // 界面重写
    //==============================================================================
    void resized() override;
    void timerCallback() override;
    void visibilityChanged() override;
    bool keyPressed(const juce::KeyPress& key) override;

    //==============================================================================
    // 注册覆盖层控制
    //==============================================================================
    void showRegistrationOverlay();

    //==============================================================================
    // 状态恢复（从 Processor 同步皮肤、调性、锁等）
    //==============================================================================
    void restoreUIState();

private:
    //==============================================================================
    // 内部组件
    //==============================================================================
    TopBar topBar;
    std::unique_ptr<ScaledContentComponent> scaledContent;
    std::unique_ptr<RegistrationOverlay> registrationOverlay;

    //==============================================================================
    // 预设浏览器
    //==============================================================================
    ProgressionData progressionData;
    std::unique_ptr<ProgressionBrowser> progressionBrowser;

    void showPresetBrowser();
    void hidePresetBrowser();

    //==============================================================================
    // 帮助对话框
    //==============================================================================
    void showHelpDialog();

    //==============================================================================
    // 注册检查
    //==============================================================================
    void checkRegistrationAndShowOverlay();

    //==============================================================================
    // MIDI 结果标记
    //==============================================================================
    bool midiResultReceived = false;
    int midiDeviceRefreshTicks = 0;

    //==============================================================================
    // 避免定时器重复保存：缓存上一次的卡片状态
    struct CachedCardState
    {
        int inv[3] = { -1, -1, -1 };
        ChordExtension typ[3] = { ChordExtension::Triad, ChordExtension::Triad, ChordExtension::Triad };
        ChordQuality quals[3] = { ChordQuality::MajorTriad, ChordQuality::MajorTriad, ChordQuality::MajorTriad };
        juce::String suffixes[3];
        juce::String funcNames[3];
    };
    CachedCardState lastSavedCardState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordReferenceAudioProcessorEditor)
};
