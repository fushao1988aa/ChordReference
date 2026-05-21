//==================================================================================================
//  ScaledContentComponent.h
//  项目：ChordReference (和弦调性速查器)
//  功能：可缩放内容区组件，整合五度圈、和弦卡片、迷你键盘及底部信息栏
//        - 管理调性设置与和弦更新（来自用户交互或 MIDI 识别）
//        - 处理卡片转位/类型/质量变化的级联更新
//        - 皮肤主题切换与布局缩放
//        - 智能生成功能应用与提示
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "CircleOfFifths.h"
#include "ChordCard.h"
#include "MiniKeyboard.h"
#include "MusicTypes.h"
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>

class ScaledContentComponent : public juce::Component,
                               private juce::Timer
{
public:
    //==============================================================================
    // 构造与基础重写
    //==============================================================================
    ScaledContentComponent();
    void resized() override;
    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void parentHierarchyChanged() override;

    //==============================================================================
    // 调性与和弦控制（外部通过 Editor 调用）
    //==============================================================================

    //---- 切换调性与调式 ----
    void setKey(PitchClass tonic, ScaleType type);

    //---- 切换调性并指定功能和弦（MIDI 识别结果） ----
    void setKeyAndChord(PitchClass tonic, ScaleType type, const juce::String& function);

    //---- 仅更新和弦功能（不改变调性，用于锁调模式） ----
    void setChordOnly(const juce::String& function);
    void setMidiExpression(const std::vector<int>& notes,
                           const std::vector<int>& velocities,
                           int averageVelocity,
                           bool sustainPedalDown);

    //---- 重置为默认三和弦 ----
    void resetToDefaultTriads();

    //---- 手动覆盖标志（用户交互后锁定，避免 MIDI 覆盖） ----
    void setManualOverride(bool shouldOverride);

    //---- 负和声状态刷新（不重置卡片状态） ----
    void refreshNegativeHarmony();

    //---- 应用智能生成的功能进行 ----
    void applyGeneratedProgression(const juce::StringArray& functions);

    //---- 获取当前调性信息 ----
    PitchClass getCurrentTonic() const { return currentTonic; }
    ScaleType getCurrentScaleType() const { return currentType; }

    //==============================================================================
    // 皮肤/语言/智能风格
    //==============================================================================
    void setSkin(SkinType newSkin);
    SkinType getCurrentSkin() const { return currentSkin; }

    void setLanguage(int lang);

    void setSmartStyle(int style) { currentSmartStyle = style; }

    //==============================================================================
    // 试听支持（传递 AudioProcessor 引用给内部卡片）
    //==============================================================================
    void setAudioProcessor(juce::AudioProcessor* p)
    {
        audioProcessorRef = p;
        for (auto* card : chordCards)
            card->setAudioProcessor(p);
    }

    //==============================================================================
    // 外部回调（通知 Editor 调性变更等）
    //==============================================================================
    std::function<void(PitchClass tonic, ScaleType type)> onKeyChanged;

    //==============================================================================
    // 公有子组件（供 Editor 直接访问以保存/恢复状态）
    //==============================================================================
    CircleOfFifths circle;

    //==============================================================================
    // 状态恢复：根据卡片当前状态重新生成扩展和弦
    //==============================================================================
    void refreshCardsFromState();

    juce::String getChordSymbolFromQuality(PitchClass root, ChordQuality quality, int numNotes, const juce::String& extSuffix = {}) const;
    juce::String getChineseChordName(const juce::String& symbol);

    void refreshKeyboardHighlight();

    //==============================================================================
    // 卡片状态快照（供 Editor 保存/恢复）
    //==============================================================================
    struct CardsState
    {
        int inversions[3] = {0, 0, 0};
        ChordExtension chordTypes[3] = {ChordExtension::Triad, ChordExtension::Triad, ChordExtension::Triad};
        ChordQuality qualities[3] = {ChordQuality::MajorTriad, ChordQuality::MajorTriad, ChordQuality::Dominant7};
        juce::String extensionSuffixes[3];
        juce::String functionNames[3] = {"T", "S", "D"};
    };

    CardsState getCardsState() const;
    void restoreCardsState(const CardsState& state);

    //==============================================================================
    // 强制刷新显示（外部调用）
    //==============================================================================
    void updateDisplay();
    void lockUpdates()   { updateLocked = true; }
    void unlockUpdates() { updateLocked = false; }



private:
    //==============================================================================
    // 布局与缩放
    //==============================================================================
    bool pendingResize = false;
    float getCurrentScale() const;
    float currentScale = 1.0f;
    juce::Rectangle<int> infoBarRect;
    juce::FlexBox mainFlexBox;

    //==============================================================================
    // 子组件
    //==============================================================================
    MiniKeyboard miniKeyboard;
    juce::Label brandLink;

    //==============================================================================
    // 调性相关数据
    //==============================================================================
    PitchClass currentTonic = PitchClass::C;
    ScaleType currentType = ScaleType::Major;
    bool isSharpKey = true;
    SkinType currentSkin = SkinType::FluidPurpleOrange;

    int currentLanguage = 0;                     // 0=中文, 1=英文


    //==============================================================================
    // 和弦与手动覆盖状态
    //==============================================================================
    juce::String currentFunction;
    bool manualOverride = false;
    juce::String currentChordSymbol;
    bool updateLocked = false;
    std::vector<int> midiExpressionNotes;
    std::vector<int> midiExpressionVelocities;
    int midiAverageVelocity = 0;
    bool midiSustainPedalDown = false;
    int midiExpressionDisplayTicks = 0;

    //==============================================================================
    // 智能生成相关
    //==============================================================================
    int currentSmartStyle = 0;                   // 当前风格索引
    juce::StringArray lastGeneratedFunctions;    // 用于去重

    juce::String smartGenInfoText;               // 智能生成提示文字
    int smartGenDisplayTicks = 0;                // 提示剩余显示 tick 数

    //==============================================================================
    // 试听用 AudioProcessor 引用
    //==============================================================================
    juce::AudioProcessor* audioProcessorRef = nullptr;

    //==============================================================================
    // 内部辅助方法
    //==============================================================================
    void onCardInversionChanged(ChordCard* card);
    void onCardChordTypeChanged(ChordCard* card);
    void onCardChordQualityChanged(ChordCard* card);
    void updateChordCardFromFunction(const juce::String& function);

    juce::String getKeyInfoText() const;
    juce::String getChordSymbol(PitchClass root, const juce::String& funcName, bool isMajor) const;

    juce::OwnedArray<ChordCard> chordCards;

    //==============================================================================
    // Timer 回调（智能生成提示倒计时）
    //==============================================================================
    void timerCallback() override
    {
        if (smartGenDisplayTicks > 0)
        {
            smartGenDisplayTicks--;
            if (smartGenDisplayTicks == 0)
                smartGenInfoText.clear();
            repaint();
        }

        if (midiSustainPedalDown)
        {
            midiExpressionDisplayTicks = 20;
        }
        else if (midiExpressionDisplayTicks > 0)
        {
            --midiExpressionDisplayTicks;
            if (midiExpressionDisplayTicks == 0)
            {
                midiExpressionNotes.clear();
                midiExpressionVelocities.clear();
                midiAverageVelocity = 0;
                midiSustainPedalDown = false;
                refreshKeyboardHighlight();
                repaint(infoBarRect);
            }
        }
    }
};
