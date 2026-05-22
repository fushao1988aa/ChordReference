//==================================================================================================
//  RegistrationManager.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：注册管理与偏好持久化实现
//        - 试用天数计算（基于系统时间）
//        - 注册码硬编码验证
//        - PropertiesFile 读写用户偏好
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "RegistrationManager.h"
#include <chrono>
#include <set>

//==============================================================================
// 单例获取
//==============================================================================
RegistrationManager& RegistrationManager::getInstance()
{
    static RegistrationManager instance;
    return instance;
}

//==============================================================================
// 构造函数：初始化 PropertiesFile 并加载数据
//==============================================================================
RegistrationManager::RegistrationManager()
{
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto meridianDir = appDataDir.getChildFile("MeridianAudio");
    if (!meridianDir.exists())
        meridianDir.createDirectory();
    auto chordDir = meridianDir.getChildFile("ChordReference");
    if (!chordDir.exists())
        chordDir.createDirectory();

    options.applicationName = "ChordReference";
    options.filenameSuffix = ".xml";
    options.folderName = "MeridianAudio/ChordReference";
    options.osxLibrarySubFolder = "Application Support";

    properties = std::make_unique<juce::PropertiesFile>(options);
    load();
}

//==============================================================================
// 加载/保存
//==============================================================================
void RegistrationManager::load()
{
    if (properties)
    {
        firstRunDay = properties->getIntValue("firstRunDay", 0);
        registered = properties->getBoolValue("registered", false);
        storedCode = properties->getValue("registrationCode", "");
    }
}

void RegistrationManager::save()
{
    if (properties)
    {
        properties->setValue("firstRunDay", firstRunDay);
        properties->setValue("registered", registered);
        properties->setValue("registrationCode", storedCode);
        properties->saveIfNeeded();
    }
}

//==============================================================================
// 首次运行检测
//==============================================================================
void RegistrationManager::checkFirstRun()
{
    if (firstRunDay == 0)
    {
        firstRunDay = getCurrentDayNumber();
        save();
    }
}

//==============================================================================
// 获取当前天数（自 epoch 以来的天数）
//==============================================================================
juce::int64 RegistrationManager::getCurrentDayNumber()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto days = duration_cast<duration<int64_t, std::ratio<86400>>>(now.time_since_epoch()).count();
    return static_cast<juce::int64>(days);
}

//==============================================================================
// 注册状态查询
//==============================================================================
bool RegistrationManager::isRegistered() const { return registered; }

bool RegistrationManager::isExpired() const
{
    if (registered) return false;
    if (firstRunDay == 0) return false;
    juce::int64 daysPassed = getCurrentDayNumber() - firstRunDay;
    return daysPassed > 30;
}

int RegistrationManager::getDaysLeft() const
{
    if (registered) return -1;
    if (firstRunDay == 0) return 30;
    juce::int64 daysPassed = getCurrentDayNumber() - firstRunDay;
    int left = 30 - static_cast<int>(daysPassed);
    return left < 0 ? 0 : left;
}

//==============================================================================
// 注册码验证与注册
//==============================================================================
bool RegistrationManager::registerWithCode(const juce::String& code)
{
    if (!validateRegistrationCode(code))
        return false;

    registered = true;
    storedCode = code;
    save();
    return true;
}

bool RegistrationManager::validateRegistrationCode(const juce::String& code) const
{
    static const std::set<juce::String> validCodes = {
        //暂时清空
    };
    
    juce::String trimmed = code.trim();
    return validCodes.find(trimmed) != validCodes.end();
}

//==============================================================================
// 偏好持久化：皮肤索引
//==============================================================================
int RegistrationManager::loadSkinPreference() const
{
    if (properties)
        return properties->getIntValue("skinPreference", defaultSkinIndex);
    return defaultSkinIndex;
}

void RegistrationManager::saveSkinPreference(int skinIndex)
{
    if (properties)
    {
        properties->setValue("skinPreference", skinIndex);
        properties->saveIfNeeded();
    }
}
//==============================================================================
// 偏好持久化：语言偏好
//==============================================================================
int RegistrationManager::loadLanguagePreference() const {
    if (properties) return properties->getIntValue("language", 0);   // 0 = 中文
    return 0;
}

void RegistrationManager::saveLanguagePreference(int lang) {
    if (properties) {
        properties->setValue("language", lang);
        properties->saveIfNeeded();
    }
}

//==============================================================================
// 偏好持久化：窗口大小
//==============================================================================
std::pair<int, int> RegistrationManager::loadWindowSize() const
{
    if (properties)
        return { properties->getIntValue("editorW", defaultEditorW),
                 properties->getIntValue("editorH", defaultEditorH) };
    return { defaultEditorW, defaultEditorH };
}

void RegistrationManager::saveWindowSize(int w, int h)
{
    if (properties)
    {
        properties->setValue("editorW", w);
        properties->setValue("editorH", h);
        properties->saveIfNeeded();
    }
}
