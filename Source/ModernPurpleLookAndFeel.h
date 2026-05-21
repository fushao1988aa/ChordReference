//==================================================================================================
//  ModernPurpleLookAndFeel.h
//  项目：ChordReference (和弦调性速查器)
//  功能：自定义 LookAndFeel 声明（继承自 LookAndFeel_V4）
//        - 重写按钮、滚动条、文本编辑器、切换按钮、弹出菜单等的绘制
//        - 允许外部设置按钮字体高度与切换按钮字体高度
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class ModernPurpleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernPurpleLookAndFeel();

    //---- 绘制重写 ----
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool isHovered, bool isPressed) override;
    void drawScrollbar(juce::Graphics&, juce::ScrollBar&, int x, int y, int w, int h,
                       bool vertical, int thumbStart, int thumbSize,
                       bool isMouseOver, bool isMouseDown) override;
    void drawTextEditorOutline(juce::Graphics&, int w, int h, juce::TextEditor&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    //---- 字体相关 ----
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;

    //---- 参数设置 ----
    void setTextButtonFontHeight(float height) { textButtonFontHeight = height; }
    void setToggleFontHeight(float height)     { toggleFontHeight = height; }

private:
    float textButtonFontHeight = 16.0f;
    float toggleFontHeight     = 16.0f;
};