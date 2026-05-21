//==================================================================================================
//  RegistrationOverlay.h
//  项目：ChordReference (和弦调性速查器)
//  功能：注册覆盖层组件声明
//        - 半透明背景，阻止下层交互
//        - 包含标题、消息、注册码输入框与注册按钮
//        - 注册成功后通过回调通知编辑器
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class RegistrationOverlay : public juce::Component
{
public:
    RegistrationOverlay();

    // 注册成功时回调
    std::function<void()> onRegistrationSuccess;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Label titleLabel;
    juce::Label messageLabel;
    juce::TextEditor codeEditor;
    juce::TextButton registerButton;
    juce::Label infoLabel;

    void checkRegistration();
};