#pragma once
#include <juce_core/juce_core.h>
#include <unordered_map>

class LocalisedStrings
{
public:
    // 加载语言文件（JSON 格式）
    static void loadLanguage(int languageId);
    // 获取本地化字符串
    static juce::String translate(const juce::String& key);
    static int getCurrentLanguage() { return currentLanguage; }
private:
    static std::unordered_map<std::string, juce::String> strings;
    static int currentLanguage;
};
