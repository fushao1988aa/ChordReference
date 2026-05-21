//==================================================================================================
//  TopBar.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：顶部栏 UI 实现
//        - MIDI 设备选择、试听模式、调式、锁调/负和声开关
//        - 皮肤/语言切换、帮助/预设/注册按钮
//        - 背景绘制（与主界面一致的渐变风格）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "TopBar.h"
#include "SkinColours.h"
#include "PitchNameHelper.h"
#include "LocalisedStrings.h"

//==============================================================================
// 局部：所有下拉框的弹出菜单外观（统一使用鸿蒙字体）
//==============================================================================
namespace
{
    class TopBarPopupLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        TopBarPopupLookAndFeel()
        {
            setDefaultSansSerifTypeface(PitchNameHelper::getHarmonyTypeface());
        }

        void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                               bool isSeparator, bool isActive, bool isHighlighted,
                               bool isTicked, bool hasSubMenu,
                               const juce::String& text, const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColour) override
        {
            if (isHighlighted)
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
            g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(15.0f));

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
    };
}

//==============================================================================
// 构造函数：添加子组件，设置初始状态
//==============================================================================
TopBar::TopBar()
{
    setLookAndFeel(&topBarLAF);
    setOpaque(false);

    //---- 添加所有子组件 ----
    addAndMakeVisible(midiIndicator);
    addAndMakeVisible(midiDeviceSelector);
    
    skinButton.onClick = [this]() { if (onSkinToggle) onSkinToggle(); };
    addAndMakeVisible(skinButton);

    languageButton.setButtonText("CN");
    languageButton.onClick = [this]() {
        bool isCurrentlyCN = (languageButton.getButtonText() == "CN");
        languageButton.setButtonText(isCurrentlyCN ? "EN" : "CN");
        if (onLanguageToggle) onLanguageToggle();
    };
    addAndMakeVisible(languageButton);

    midiOutToggle.setButtonText(LocalisedStrings::translate("top_midi_out"));
    addAndMakeVisible(midiOutToggle);

    presetButton.setButtonText(LocalisedStrings::translate("top_preset_browser"));
    presetButton.onClick = [this]() { if (onPresetClicked) onPresetClicked(); };
    addAndMakeVisible(presetButton);
    
    lockToggle.setButtonText(LocalisedStrings::translate("top_lock_key"));
    lockToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    addAndMakeVisible(lockToggle);

    negativeHarmonyToggle.setButtonText(LocalisedStrings::translate("top_negative_harmony"));
    negativeHarmonyToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    addAndMakeVisible(negativeHarmonyToggle);

    registrationButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    registrationButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    registrationButton.onClick = [this]() { if (onRegistrationClicked) onRegistrationClicked(); };
    registrationButton.setVisible(false);
    addAndMakeVisible(registrationButton);

    helpButton.setButtonText(LocalisedStrings::translate("top_help"));
    helpButton.onClick = [this]() { if (onHelpClicked) onHelpClicked(); };
    addAndMakeVisible(helpButton);
    
    //---- 初始 MIDI 设备列表 ----
    midiDeviceSelector.addItem(LocalisedStrings::translate("top_no_midi_device"), 1);
    midiDeviceSelector.setSelectedId(1, juce::dontSendNotification);
    midiDeviceSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0x33000000));
    midiDeviceSelector.setColour(juce::ComboBox::textColourId, juce::Colours::white.withAlpha(0.8f));

    //---- 试听模式选择器 ----
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_off"), 1);
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_internal"), 2);
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_external"), 3);
    auditionModeSelector.setSelectedId(1);
    addAndMakeVisible(auditionModeSelector);

    //---- 调式选择器 ----
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_major"), 1);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_natural_minor"), 2);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_dorian"), 3);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_phrygian"), 4);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_lydian"), 5);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_mixolydian"), 6);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_locrian"), 7);
    scaleTypeSelector.setSelectedId(1);
    addAndMakeVisible(scaleTypeSelector);

    applyButtonColours();
    updateSkinButtonText();
    topBarLAF.setTextButtonFontHeight(16.0f);
    topBarLAF.setToggleFontHeight(16.0f);
}

//==============================================================================
// 布局：水平三等分，垂直居中
//==============================================================================
void TopBar::resized()
{
    const int barHeight = getHeight();
    const int controlHeight = 26;
    const int marginH = 5;
    const int marginV = (barHeight - controlHeight) / 2;
    auto area = getLocalBounds().reduced(marginH, marginV).withHeight(controlHeight);

    const float unifiedFontSize = 16.0f;
    topBarLAF.setTextButtonFontHeight(unifiedFontSize);
    topBarLAF.setToggleFontHeight(unifiedFontSize);

    //---- 右侧控件组（从右向左排列） ----
    const int buttonWidth     = 60;
    const int negativeWidth   = LocalisedStrings::translate("top_negative_harmony").length() > 12 ? 112 : 76;
    const int scaleTypeWidth  = 130;
    const int regButtonWidth  = 85;
    const int gap             = 6;

    int rightTotalWidth = 0;
    if (registrationButton.isVisible())
        rightTotalWidth += regButtonWidth + gap;
    rightTotalWidth += buttonWidth + gap;          // 皮肤
    rightTotalWidth += buttonWidth + gap;          // 语言
    rightTotalWidth += negativeWidth + gap;        // 负和声
    rightTotalWidth += scaleTypeWidth + gap;       // 调式
    rightTotalWidth += buttonWidth;                // 锁调

    int rightEdge = area.getRight();
    auto placeRight = [&](juce::Component& comp, int compWidth)
    {
        comp.setBounds(rightEdge - compWidth, area.getY(), compWidth, controlHeight);
        rightEdge -= (compWidth + gap);
    };

    if (registrationButton.isVisible())
        placeRight(registrationButton, regButtonWidth);
    placeRight(skinButton, buttonWidth);
    placeRight(languageButton, buttonWidth);
    placeRight(negativeHarmonyToggle, negativeWidth);
    placeRight(lockToggle, buttonWidth);
    placeRight(scaleTypeSelector, scaleTypeWidth);

    //---- 左侧控件组（从左向右排列） ----
    const int ledSize        = 12;
    const int availableLeftWidth = juce::jmax(220, rightEdge - area.getX() - 18);
    const int deviceWidth    = juce::jlimit(76, 95, availableLeftWidth / 5);
    const int midiOutWidth   = juce::jlimit(58, 75, availableLeftWidth / 6);
    const int auditionWidth  = juce::jlimit(76, 95, availableLeftWidth / 5);
    const int presetWidth    = juce::jlimit(70, 85, availableLeftWidth / 5);
    const int leftGap        = 4;

    int leftEdge = area.getX();

    const int helpButtonWidth = 40;
    helpButton.setBounds(leftEdge, area.getY(), helpButtonWidth, controlHeight);
    leftEdge += helpButtonWidth + leftGap;

    midiIndicator.setBounds(leftEdge, area.getY() + (controlHeight - ledSize) / 2,
                            ledSize, ledSize);
    leftEdge += ledSize + leftGap;

    midiDeviceSelector.setBounds(leftEdge, area.getY(), deviceWidth, controlHeight);
    leftEdge += deviceWidth + leftGap;

    midiOutToggle.setBounds(leftEdge, area.getY(), midiOutWidth, controlHeight);
    leftEdge += midiOutWidth + leftGap;

    auditionModeSelector.setBounds(leftEdge, area.getY(), auditionWidth, controlHeight);
    leftEdge += auditionWidth + leftGap;

    presetButton.setBounds(leftEdge, area.getY(), presetWidth, controlHeight);
}

//==============================================================================
// 绘制：背景渐变、分隔线、标题
//==============================================================================
void TopBar::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    auto& theme = getSkinTheme(currentSkin);

    auto bgGrad = theme.backgroundGradient(area.getWidth(), area.getHeight());
    g.setGradientFill(bgGrad);
    g.fillAll();

    g.setColour(theme.topBarSeparatorCol);
    g.drawHorizontalLine(area.getHeight() - 1, area.getX(), area.getRight());

    juce::Colour textCol = theme.topBarTextCol;
    juce::Colour subTextCol = theme.topBarSubTextCol;

    float midX = area.getX() + area.getWidth() / 3.0f;
    float midWidth = area.getWidth() / 3.0f;
    juce::Rectangle<float> midArea(midX, area.getY(), midWidth, area.getHeight());

    g.setColour(textCol);
    g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(27.0f).boldened());
    g.drawText(LocalisedStrings::translate("app_title"),
               midArea.withTrimmedTop(1.5f).withHeight(25.0f),
               juce::Justification::centred);
}
//==============================================================================
// MIDI 设备列表刷新
//==============================================================================
juce::String TopBar::getMidiDeviceIdentifierForId(int comboId) const
{
    auto it = midiDeviceIdMap.find(comboId);
    if (it != midiDeviceIdMap.end()) return it->second;
    return {};
}
//填充 map
void TopBar::refreshMidiDevices(const juce::StringArray& deviceNames, const juce::StringArray& identifiers)
{
    midiDeviceSelector.clear(juce::dontSendNotification);
    midiDeviceSelector.addItem(LocalisedStrings::translate("top_no_midi_device"), 1);
    midiDeviceIdMap.clear();
    midiDeviceIdMap[1] = {}; // 空标识符
    int index = 2;
    for (int i = 0; i < deviceNames.size(); ++i)
    {
        midiDeviceSelector.addItem(deviceNames[i], index);
        midiDeviceIdMap[index] = identifiers[i];
        ++index;
    }
    if (midiDeviceSelector.getSelectedId() < 1 || midiDeviceSelector.getSelectedId() >= index)
        midiDeviceSelector.setSelectedId(1, juce::dontSendNotification);
}

//==============================================================================
// 皮肤设置
//==============================================================================
void TopBar::setSkin(SkinType newSkin)
{
    if (currentSkin != newSkin)
    {
        currentSkin = newSkin;
        applyButtonColours();
        updateSkinButtonText();
        topBarLAF.setTextButtonFontHeight(14.0f);
        topBarLAF.setToggleFontHeight(14.0f);
        juce::Timer::callAfterDelay(300, [this] { repaint(); });
        repaint();
    }
}

//==============================================================================
// 根据当前皮肤刷新所有子控件颜色
//==============================================================================
void TopBar::applyButtonColours()
{
    auto& theme = getSkinTheme(currentSkin);
    const juce::Colour transparentBg = juce::Colours::transparentBlack;

    //---- TextButton ----
    skinButton.setColour(juce::TextButton::buttonColourId, transparentBg);
    skinButton.setColour(juce::TextButton::textColourOffId, theme.topBarButtonTextCol);

    languageButton.setColour(juce::TextButton::buttonColourId, transparentBg);
    languageButton.setColour(juce::TextButton::textColourOffId, theme.topBarButtonTextCol);

    presetButton.setColour(juce::TextButton::buttonColourId, transparentBg);
    presetButton.setColour(juce::TextButton::textColourOffId, theme.topBarButtonTextCol);

    registrationButton.setColour(juce::TextButton::buttonColourId, transparentBg);
    registrationButton.setColour(juce::TextButton::textColourOffId, theme.topBarButtonTextCol);

    helpButton.setColour(juce::TextButton::buttonColourId, transparentBg);
    helpButton.setColour(juce::TextButton::textColourOffId, theme.topBarButtonTextCol);

    //---- ToggleButton ----
    lockToggle.setColour(juce::ToggleButton::tickColourId, theme.topBarToggleTickCol);
    lockToggle.setColour(juce::ToggleButton::textColourId, theme.topBarToggleTextCol);

    midiOutToggle.setColour(juce::ToggleButton::tickColourId, theme.topBarToggleTickCol);
    midiOutToggle.setColour(juce::ToggleButton::textColourId, theme.topBarToggleTextCol);

    negativeHarmonyToggle.setColour(juce::ToggleButton::tickColourId, theme.topBarToggleTickCol);
    negativeHarmonyToggle.setColour(juce::ToggleButton::textColourId, theme.topBarToggleTextCol);

    //---- ComboBox ----
    midiDeviceSelector.setColour(juce::ComboBox::backgroundColourId, transparentBg);
    midiDeviceSelector.setColour(juce::ComboBox::textColourId, theme.topBarComboTextCol);
    midiDeviceSelector.setColour(juce::ComboBox::arrowColourId, theme.topBarComboTextCol);
    midiDeviceSelector.setColour(juce::ComboBox::outlineColourId, theme.topBarComboTextCol.withAlpha(0.5f));

    auditionModeSelector.setColour(juce::ComboBox::backgroundColourId, transparentBg);
    auditionModeSelector.setColour(juce::ComboBox::textColourId, theme.topBarComboTextCol);
    auditionModeSelector.setColour(juce::ComboBox::arrowColourId, theme.topBarComboTextCol);
    auditionModeSelector.setColour(juce::ComboBox::outlineColourId, theme.topBarComboTextCol.withAlpha(0.5f));

    scaleTypeSelector.setColour(juce::ComboBox::backgroundColourId, transparentBg);
    scaleTypeSelector.setColour(juce::ComboBox::textColourId, theme.topBarComboTextCol);
    scaleTypeSelector.setColour(juce::ComboBox::arrowColourId, theme.topBarComboTextCol);
    scaleTypeSelector.setColour(juce::ComboBox::outlineColourId, theme.topBarComboTextCol.withAlpha(0.5f));

    //---- 弹出菜单配色 ----
    auto setComboPopupColours = [&](juce::ComboBox& combo)
    {
        combo.setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xDD222222));
        combo.setColour(juce::PopupMenu::textColourId, theme.topBarTextCol);
        combo.setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.topBarButtonBgCol);
        combo.setColour(juce::PopupMenu::highlightedTextColourId, theme.topBarTextCol);
    };
    setComboPopupColours(midiDeviceSelector);
    setComboPopupColours(auditionModeSelector);
    setComboPopupColours(scaleTypeSelector);
}

//==============================================================================
// 皮肤按钮文字更新
//==============================================================================
void TopBar::updateSkinButtonText()
{
    midiOutToggle.setButtonText(LocalisedStrings::translate("top_midi_out"));
    presetButton.setButtonText(LocalisedStrings::translate("top_preset_browser"));
    lockToggle.setButtonText(LocalisedStrings::translate("top_lock_key"));
    negativeHarmonyToggle.setButtonText(LocalisedStrings::translate("top_negative_harmony"));
    helpButton.setButtonText(LocalisedStrings::translate("top_help"));

    switch (currentSkin)
    {
    case SkinType::FluidPurpleOrange: skinButton.setButtonText(LocalisedStrings::translate("skin_modern_purple")); break;
    case SkinType::FluidLightShadow:  skinButton.setButtonText(LocalisedStrings::translate("skin_future_blue")); break;
    case SkinType::IvorySatin:        skinButton.setButtonText(LocalisedStrings::translate("skin_ivory")); break;
    case SkinType::RoyalBlackGold:    skinButton.setButtonText(LocalisedStrings::translate("skin_black_gold")); break;
    default: skinButton.setButtonText(LocalisedStrings::translate("top_skin")); break;
    }
}

void TopBar::rebuildLocalisedComboItems()
{
    const int auditionId = auditionModeSelector.getSelectedId();
    auditionModeSelector.clear(juce::dontSendNotification);
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_off"), 1);
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_internal"), 2);
    auditionModeSelector.addItem(LocalisedStrings::translate("top_audition_external"), 3);
    auditionModeSelector.setSelectedId(juce::jlimit(1, 3, auditionId), juce::dontSendNotification);

    const int scaleId = scaleTypeSelector.getSelectedId();
    scaleTypeSelector.clear(juce::dontSendNotification);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_major"), 1);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_natural_minor"), 2);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_dorian"), 3);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_phrygian"), 4);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_lydian"), 5);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_mixolydian"), 6);
    scaleTypeSelector.addItem(LocalisedStrings::translate("scale_locrian"), 7);
    scaleTypeSelector.setSelectedId(juce::jlimit(1, 7, scaleId), juce::dontSendNotification);
}

void TopBar::refreshLocalisedText()
{
    updateSkinButtonText();
    rebuildLocalisedComboItems();
}

//==============================================================================
// 注册状态标签更新
//==============================================================================
void TopBar::setRegistrationStatus(bool registered, int daysLeft)
{
    if (registered)
    {
        registrationButton.setVisible(false);
    }
    else
    {
        registrationButton.setVisible(true);
        if (daysLeft > 0)
            registrationButton.setButtonText(LocalisedStrings::translate("auto_unregistered") + juce::String(daysLeft) + LocalisedStrings::translate("auto_days"));
        else
            registrationButton.setButtonText(LocalisedStrings::translate("auto_trial_expired"));
    }
    resized();
    repaint();
}
