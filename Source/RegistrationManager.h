//==================================================================================================
//  RegistrationManager.h
//  项目：ChordReference (和弦调性速查器)
//  功能：注册管理（试用期、注册码验证、偏好持久化）
//        - 30 天试用期逻辑
//        - 注册码验证与持久化
//        - 皮肤偏好与窗口大小的跨会话记忆
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class RegistrationManager
{
public:
    static RegistrationManager& getInstance();

    //---- 注册状态查询 ----
    bool isRegistered() const;
    bool isExpired() const;
    int getDaysLeft() const;

    //---- 注册操作 ----
    bool registerWithCode(const juce::String& code);
    void checkFirstRun();

    //---- 偏好持久化（全局性皮肤偏好/窗口大小/语言偏好）----
    int loadSkinPreference() const;
    void saveSkinPreference(int skinIndex);

    std::pair<int, int> loadWindowSize() const;
    void saveWindowSize(int w, int h);

    int loadLanguagePreference() const;
    void saveLanguagePreference(int lang);

private:
    RegistrationManager();
    ~RegistrationManager() = default;

    void save();
    void load();

    juce::int64 firstRunDay = 0;
    bool registered = false;
    juce::String storedCode;

    juce::PropertiesFile::Options options;
    std::unique_ptr<juce::PropertiesFile> properties;

    bool validateRegistrationCode(const juce::String& code) const;
    static juce::int64 getCurrentDayNumber();

    static constexpr int defaultSkinIndex = 0;
    static constexpr int defaultEditorW = 1000;
    static constexpr int defaultEditorH = 800;
};