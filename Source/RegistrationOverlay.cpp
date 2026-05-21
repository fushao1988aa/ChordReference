//==================================================================================================
//  RegistrationOverlay.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：注册覆盖层实现
//        - 显示剩余天数或过期提示
//        - 提交注册码，成功则触发回调
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "RegistrationOverlay.h"
#include "LocalisedStrings.h"
#include "RegistrationManager.h"

RegistrationOverlay::RegistrationOverlay()
{
    setInterceptsMouseClicks(true, true);
    setOpaque(false);

    //---- 标题 ----
    titleLabel.setText(LocalisedStrings::translate("auto_software_unregistered"), juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    //---- 消息提示 ----
    messageLabel.setJustificationType(juce::Justification::centred);
    messageLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(messageLabel);

    //---- 注册码输入框 ----
    codeEditor.setMultiLine(false);
    codeEditor.setPasswordCharacter(0);
    codeEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    codeEditor.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    codeEditor.setTextToShowWhenEmpty(LocalisedStrings::translate("auto_enter_registration_code"), juce::Colours::grey);
    codeEditor.setJustification(juce::Justification::centred);
    addAndMakeVisible(codeEditor);

    //---- 注册按钮 ----
    registerButton.setButtonText(LocalisedStrings::translate("auto_register"));
    registerButton.onClick = [this] { checkRegistration(); };
    addAndMakeVisible(registerButton);

    //---- 底部说明 ----
    infoLabel.setText(LocalisedStrings::translate("auto_please_purchase_a_valid_license_and_enter_the_re"), juce::dontSendNotification);
    infoLabel.setFont(juce::Font(12.0f));
    infoLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(infoLabel);

    // 更新消息文本
    auto& regMgr = RegistrationManager::getInstance();
    int daysLeft = regMgr.getDaysLeft();
    if (daysLeft > 0)
    {
        messageLabel.setText(LocalisedStrings::translate("auto_the_30_day_trial_has_expired_please_register_to_") + juce::String(daysLeft),
                             juce::dontSendNotification);
    }
    else
    {
        messageLabel.setText(LocalisedStrings::translate("auto_the_trial_has_ended_enter_a_registration_code_to"),
                             juce::dontSendNotification);
    }
}

void RegistrationOverlay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xCC000000));   // 半透明黑色遮罩
}

void RegistrationOverlay::resized()
{
    auto area = getLocalBounds();
    int panelWidth = 420;
    int panelHeight = 320;
    auto panelRect = area.withSizeKeepingCentre(panelWidth, panelHeight);

    float y = panelRect.getY();
    float spacing = 8.0f;

    titleLabel.setBounds(panelRect.getX(), y, panelRect.getWidth(), 40);
    y += 40 + spacing;

    messageLabel.setBounds(panelRect.getX(), y, panelRect.getWidth(), 45);
    y += 45 + spacing;

    codeEditor.setBounds(panelRect.getX() + 20, y, panelRect.getWidth() - 40, 38);
    y += 38 + spacing;

    registerButton.setBounds(panelRect.getX() + 20, y, panelRect.getWidth() - 40, 38);
    y += 38 + spacing;

    infoLabel.setBounds(panelRect.getX(), y, panelRect.getWidth(), 25);
}

void RegistrationOverlay::checkRegistration()
{
    juce::String code = codeEditor.getText();
    if (RegistrationManager::getInstance().registerWithCode(code))
    {
        if (onRegistrationSuccess)
            onRegistrationSuccess();
    }
    else
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                                LocalisedStrings::translate("auto_registration_failed"),
                                                LocalisedStrings::translate("auto_invalid_registration_code_please_check_and_try_a"));
    }
}