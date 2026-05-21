//==================================================================================================
//  TopBar.h
//  项目：ChordReference (和弦调性速查器)
//  功能：顶部栏组件声明
//        - MIDI 活动指示灯、设备选择、输出开关、试听模式选择
//        - 预设浏览器按钮、调式选择、锁调/负和声开关
//        - 皮肤切换按钮、语言切换按钮、帮助按钮、注册状态按钮
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MusicTypes.h"
#include <functional>
#include "ModernPurpleLookAndFeel.h"

//==============================================================================
// MIDI 活动指示灯（圆形，有活动时亮绿色，无活动时灰色）
//==============================================================================
class MidiActivityIndicator : public juce::Component
{
public:
    void setActive(bool active) { this->active = active; repaint(); }
    void paint(juce::Graphics& g) override
    {
        g.setColour(active ? juce::Colours::lime : juce::Colours::darkgrey);
        g.fillEllipse(getLocalBounds().toFloat().reduced(2.0f));
    }
private:
    bool active = false;
};

//==============================================================================
// 顶部栏主组件
//==============================================================================
class TopBar : public juce::Component
{
public:
    //==========================================================================
    // 构造与 JUCE 重写
    //==========================================================================
    TopBar();
    void resized() override;
    void paint(juce::Graphics& g) override;

    //==========================================================================
    // 皮肤与注册状态设置
    //==========================================================================
    void setSkin(SkinType newSkin);
    void setRegistrationStatus(bool registered, int daysLeft);

    //==========================================================================
    // 公有子组件（供 Editor 直接访问/设置回调）
    //==========================================================================
    MidiActivityIndicator midiIndicator;
    juce::ComboBox midiDeviceSelector;
    juce::TextButton presetButton;
    juce::ToggleButton midiOutToggle;
    juce::ComboBox auditionModeSelector;
    juce::TextButton skinButton;
    juce::TextButton languageButton;
    juce::ToggleButton lockToggle;
    juce::ToggleButton negativeHarmonyToggle;
    juce::ComboBox scaleTypeSelector;
    juce::TextButton helpButton;

    //==========================================================================
    // 回调函数
    //==========================================================================
    std::function<void()> onPresetClicked;
    std::function<void()> onSkinToggle;
    std::function<void()> onLanguageToggle;
    std::function<void()> onRegistrationClicked;
    std::function<void()> onHelpClicked;

    //==========================================================================
    // 便利方法
    //==========================================================================
    bool isKeyLocked() const { return lockToggle.getToggleState(); }
    void refreshMidiDevices(const juce::StringArray& deviceNames, const juce::StringArray& identifiers);
    void refreshSkinButtonText() { refreshLocalisedText(); }
    void refreshLocalisedText();
    
    juce::String getMidiDeviceIdentifierForId(int comboId) const;

private:
    //==========================================================================
    // 内部状态
    //==========================================================================
    SkinType currentSkin = SkinType::FluidPurpleOrange;
    juce::TextButton registrationButton;
    ModernPurpleLookAndFeel topBarLAF;
    std::map<int, juce::String> midiDeviceIdMap;   // comboId -> identifier

    //==========================================================================
    // 内部方法
    //==========================================================================
    void applyButtonColours();
    void updateSkinButtonText();
    void rebuildLocalisedComboItems();
};
