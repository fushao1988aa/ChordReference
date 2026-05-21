#include "LocalisedStrings.h"
#include "BinaryData.h"   // 新增

std::unordered_map<std::string, juce::String> LocalisedStrings::strings;
int LocalisedStrings::currentLanguage = 0;

void LocalisedStrings::loadLanguage(int languageId)
{
    currentLanguage = languageId;
    strings.clear();
    
    const void* data = nullptr;
    int dataSize = 0;
    
    if (languageId == 0) {
        data = BinaryData::strings_zh_json;
        dataSize = BinaryData::strings_zh_jsonSize;
    } else {
        data = BinaryData::strings_en_json;
        dataSize = BinaryData::strings_en_jsonSize;
    }
    
    if (data == nullptr || dataSize == 0) return;
    
    juce::String content = juce::String::createStringFromData(data, dataSize);
    auto json = juce::JSON::parse(content);
    if (auto* obj = json.getDynamicObject())
    {
        for (const auto& pair : obj->getProperties())
            strings[pair.name.toString().toStdString()] = pair.value.toString();
    }
}

juce::String LocalisedStrings::translate(const juce::String& key)
{
    auto it = strings.find(key.toStdString());
    if (it != strings.end()) return it->second;
    return key;
}