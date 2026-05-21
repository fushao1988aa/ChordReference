//==================================================================================================
//  MiniKeyboard.h
//  项目：ChordReference (和弦调性速查器)
//  功能：迷你钢琴键盘组件声明
//        - 一个八度 (C4~B4) 的白键与黑键
//        - 支持高亮指定音高类 (0~11)
//        - 与皮肤主题联动
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MusicTypes.h"
#include "SkinColours.h"
#include <array>

class MiniKeyboard : public juce::Component
{
public:
    // 设置高亮音符（传入音高类集合，如 {0,4,7}）
    void setHighlightedNotes(const std::vector<int>& notes);
    void setMidiExpression(const std::vector<int>& notes,
                           const std::vector<int>& velocities,
                           int averageVelocity,
                           bool sustainPedalDown);

    // 皮肤
    void setSkin(SkinType skin) { currentSkin = skin; repaint(); }

    void paint(juce::Graphics&) override;

private:
    std::vector<int> highlightedNotes;              // 当前高亮的音高类
    std::array<int, 12> highlightedVelocities {};
    int currentAverageVelocity = 0;
    bool sustainDown = false;
    SkinType currentSkin = SkinType::FluidPurpleOrange;

    //---- 键位计算 ----
    juce::Rectangle<float> getWhiteKeyBounds(int index, const juce::Rectangle<float>& area) const;
    juce::Rectangle<float> getBlackKeyBounds(int index, const juce::Rectangle<float>& area) const;

    //---- 绘制单键 ----
    void drawWhiteKey(juce::Graphics& g, int noteNumber, juce::Rectangle<float> area,
                      bool highlighted, int velocity, const SkinTheme::KeyboardTheme& kb);
    void drawBlackKey(juce::Graphics& g, int noteNumber, juce::Rectangle<float> area,
                      bool highlighted, int velocity, const SkinTheme::KeyboardTheme& kb);
    void drawNoteLabel(juce::Graphics& g, juce::Rectangle<float> area,
                       const juce::String& text, bool isBlackKey);
};
