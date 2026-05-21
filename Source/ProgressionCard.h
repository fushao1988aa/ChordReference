//==================================================================================================
//  ProgressionCard.h
//  项目：ChordReference (和弦调性速查器)
//  功能：预设列表项卡片组件声明
//        - 显示预设名称、和弦进行符号、罗马级数、封面缩略图
//        - 选中高亮、悬停效果、收藏心形图标
//        - 右键菜单（收藏、编辑、删除用户预设）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "MusicTypes.h"
#include "ProgressionData.h"

class ProgressionCard : public juce::Component
{
public:
    ProgressionCard();

    //---- 设置预设条目与当前调性 ----
    void setEntry(const ProgressionEntry& entry, PitchClass tonic, ScaleType type);
    const ProgressionEntry& getEntry() const { return entry; }

    //---- 选中与收藏状态 ----
    void setSelected(bool sel);
    bool getSelected() const { return selected; }
    void setFavourite(bool fav);

    //---- 选中与常用状态 ----
    void setFrequent(bool freq);
    bool isFrequent = false;  // 常用标记

    //---- 预设卡片列表中添加多选机制（Shift+点击）---- 
    bool isMultiSelected = false;

    void setMultiSelected(bool ms) { isMultiSelected = ms; repaint(); }
    bool getMultiSelected() const { return isMultiSelected; }

    //---- 交互回调 ----
    std::function<void()> onClick;
    std::function<void()> onFavouriteToggle;
    std::function<void()> onEdit;
    std::function<void()> onDelete;
    std::function<void()> onFrequentToggle;
    std::function<void(juce::PopupMenu&)> onBuildExtraMenu;

    //---- JUCE 重写 ----
    void paint(juce::Graphics&) override;
    void mouseEnter(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

private:
    ProgressionEntry entry;
    PitchClass currentTonic = PitchClass::C;
    ScaleType currentScale = ScaleType::Major;
    bool hovered = false;
    bool selected = false;
    bool isFavourite = false;
    juce::DropShadow shadow;

    juce::Image cachedCoverImage;    // ★ 解码后的封面缩略图

    //---- 根据当前调性生成显示文本 ----
    juce::String getChordSymbols() const;
    juce::String getRoman() const;
};
