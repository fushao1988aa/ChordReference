//==================================================================================================
//  ModernPurpleLookAndFeel.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：自定义 LookAndFeel 实现
//        - 初始化默认颜色
//        - 绘制圆角按钮、细滚动条、半透明文本编辑器边框
//        - 切换按钮与弹出菜单自定义
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ModernPurpleLookAndFeel.h"
#include "PitchNameHelper.h"

//==============================================================================
// 构造函数：设置默认颜色
//==============================================================================
ModernPurpleLookAndFeel::ModernPurpleLookAndFeel()
{
    using namespace juce;

    setColour(ResizableWindow::backgroundColourId, Colour(0xff140F24));
    setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    setColour(ScrollBar::thumbColourId, Colour(0xff8B5CF6));
    setColour(TextButton::buttonColourId, Colour(0xff6D48FF));
    setColour(TextButton::textColourOffId, Colours::white);
    setColour(Label::textColourId, Colour(0xffECE7FF));
    setColour(TextEditor::backgroundColourId, Colour(0x221FFFFFF));
    setColour(TextEditor::outlineColourId, Colours::transparentBlack);
    setColour(TextEditor::textColourId, Colours::white);
    setColour(TextEditor::highlightColourId, Colour(0xff8B5CF6));
    if (auto typeface = PitchNameHelper::getHarmonyTypeface())
        setDefaultSansSerifTypeface(typeface);
}

//==============================================================================
// 按钮背景绘制：圆角、悬停/按下变色
//==============================================================================
void ModernPurpleLookAndFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool isHovered,
    bool isPressed)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto colour = backgroundColour;

    if (isPressed)
        colour = colour.brighter(0.2f);
    else if (isHovered)
        colour = colour.brighter(0.08f);

    g.setColour(colour);
    g.fillRoundedRectangle(bounds, 12.0f);

    auto borderCol = button.findColour(juce::TextButton::textColourOffId).withAlpha(0.3f);
    g.setColour(borderCol);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
}

//==============================================================================
// 滚动条绘制：细圆角滑块
//==============================================================================
void ModernPurpleLookAndFeel::drawScrollbar(juce::Graphics& g,
    juce::ScrollBar& scrollbar,
    int x, int y, int w, int h,
    bool vertical,
    int thumbStart, int thumbSize,
    bool isMouseOver, bool isMouseDown)
{
    juce::Rectangle<float> thumb;
    if (vertical)
        thumb = { (float)x + 1.5f, (float)thumbStart, (float)w - 3.0f, (float)thumbSize };
    else
        thumb = { (float)thumbStart, (float)y + 1.5f, (float)thumbSize, (float)h - 3.0f };

    auto colour = juce::Colour(0xff8B5CF6);
    if (isMouseOver)
        colour = colour.brighter(0.15f);
    g.setColour(colour.withAlpha(0.9f));
    g.fillRoundedRectangle(thumb, 4.0f);
}

//==============================================================================
// 文本编辑器边框：半透明白色圆角
//==============================================================================
void ModernPurpleLookAndFeel::drawTextEditorOutline(juce::Graphics& g,
    int w, int h,
    juce::TextEditor& editor)
{
    if (editor.getName() == "SearchBox")
        return; // 搜索框无边框，仅下划线

    auto bounds = juce::Rectangle<float>(0, 0, (float)w, (float)h);
    g.setColour(juce::Colour(0x30FFFFFF));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
}

//==============================================================================
// 切换按钮（ToggleButton）绘制
//==============================================================================
void ModernPurpleLookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    juce::Font font(PitchNameHelper::getHarmonyTypeface());
    font.setHeight(toggleFontHeight);
    auto tickWidth = font.getHeight() * 0.9f;

    juce::Rectangle<float> tickBounds(
        bounds.getX(),
        bounds.getY() + (bounds.getHeight() - tickWidth) / 2.0f,
        tickWidth, tickWidth
    );

    auto textColour = button.findColour(juce::ToggleButton::textColourId);

    // 勾选框边框
    g.setColour(textColour.withAlpha(0.4f));
    g.drawRoundedRectangle(tickBounds, 4.0f, 1.2f);

    // 选中时填充
    if (button.getToggleState())
    {
        auto tickColour = button.findColour(juce::ToggleButton::tickColourId);
        g.setColour(tickColour.withAlpha(0.8f));
        g.fillRoundedRectangle(tickBounds.reduced(3.0f), 3.0f);
    }

    // 文字
    g.setColour(textColour);
    g.setFont(font);
    auto textArea = bounds.withTrimmedLeft(tickWidth + 6.0f);
    g.drawText(button.getButtonText(), textArea, juce::Justification::centredLeft);
}

//==============================================================================
// 弹出菜单项绘制
//==============================================================================
void ModernPurpleLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
    bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
    bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText,
    const juce::Drawable* icon, const juce::Colour* textColour)
{
    if (isHighlighted && !isSeparator)
        g.fillAll(juce::Colours::white.withAlpha(0.15f));
    else
        g.fillAll(juce::Colours::black.withAlpha(0.0f));

    if (isSeparator)
    {
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawHorizontalLine(area.getCentreY(), area.getX() + 10.0f, area.getRight() - 10.0f);
        return;
    }

    g.setColour(textColour != nullptr ? *textColour : juce::Colours::white);
    g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(textButtonFontHeight));
    auto textArea = area.reduced(12, 0);
    g.drawText(text, textArea, juce::Justification::centredLeft);

    if (hasSubMenu)
    {
        juce::Path arrow;
        arrow.addTriangle(0.0f, 0.0f, 8.0f, 4.0f, 0.0f, 8.0f);
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.fillPath(arrow, juce::AffineTransform::translation(
            area.getRight() - 20.0f, area.getCentreY() - 4.0f));
    }
}

//==============================================================================
// 字体获取
//==============================================================================
juce::Font ModernPurpleLookAndFeel::getTextButtonFont(juce::TextButton&, int)
{
    return juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(textButtonFontHeight).withTypefaceStyle("SemiBold");
}

juce::Font ModernPurpleLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(textButtonFontHeight);
}
