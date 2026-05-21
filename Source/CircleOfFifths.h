//==================================================================================================
//  CircleOfFifths.h
//  项目：ChordReference (和弦调性速查器)
//  功能：五度圈组件声明
//        - 12 个大调（外圈）与 12 个小调（内圈）扇形切片
//        - 点击扇区切换调性，点击中心骰子触发智能生成
//        - 右键骰子切换生成风格
//        - 支持皮肤主题切换
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MusicTypes.h"
#include <functional>

class CircleOfFifths : public juce::Component,
                       private juce::Timer
{
public:
    //==========================================================================
    // 构造与 JUCE 重写
    //==========================================================================
    CircleOfFifths();
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;

    //==========================================================================
    // 回调
    //==========================================================================
    std::function<void(PitchClass tonic, ScaleType type)> onKeySelected;   // 选中调性
    std::function<void()> onGenerateRequested;                             // 智能生成
    std::function<void(int styleIndex)> onStyleChanged;                    // 风格切换

    //==========================================================================
    // 设置当前调性（高亮对应扇区）
    //==========================================================================
    void setCurrentKey(PitchClass tonic, ScaleType type);

    //==========================================================================
    // 皮肤切换
    //==========================================================================
    void setSkin(SkinType skin) { currentSkin = skin; repaint(); }

private:
    //==========================================================================
    // 内部数据结构
    //==========================================================================
    struct KeyButton
    {
        PitchClass tonic;
        float angle;            // 数学角度（弧度）
        juce::Rectangle<float> bounds;
    };
    
    std::vector<KeyButton> majorKeys;   // 外圈 12 个大调
    std::vector<KeyButton> minorKeys;   // 内圈 12 个小调（与大调严格对齐）

    PitchClass currentTonic = PitchClass::C;
    ScaleType currentType = ScaleType::Major;
    SkinType currentSkin = SkinType::FluidPurpleOrange;

    juce::Rectangle<int> lastBounds;

    //==========================================================================
    // 骰子动画
    //==========================================================================
    float diceRotationAngle = 0.0f;
    bool diceIsRolling = false;
    bool generateButtonFlashed = false;

    void timerCallback() override
    {
        diceRotationAngle += 0.2f;
        if (diceRotationAngle >= juce::MathConstants<float>::twoPi)
        {
            diceRotationAngle = 0.0f;
            diceIsRolling = false;
            generateButtonFlashed = false;
            stopTimer();
        }
        repaint();
    }

    //==========================================================================
    // 布局辅助
    //==========================================================================
    void calculateLayout();
    juce::Point<float> getPosition(float angle, float radius, juce::Point<float> center) const;
};