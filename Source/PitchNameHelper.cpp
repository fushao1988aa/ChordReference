//==================================================================================================
//  PitchNameHelper.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：音名转换与中文字体加载实现
//        - 升号/降号记谱名数组
//        - 从 BinaryData 加载鸿蒙字体，失败则降级
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "PitchNameHelper.h"
#include <algorithm>
#include "BinaryData.h"
#include <juce_gui_basics/juce_gui_basics.h>   // 新增：提供 juce::LookAndFeel

//==============================================================================
// 升号调音名数组（C, C#, D, D#, E, F, F#, G, G#, A, A#, B）
//==============================================================================
static const std::string sharpNames[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

//==============================================================================
// 降号调音名数组（C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B）
//   注意：B 对应 Cb（等音，用于降号调记谱）
//==============================================================================
static const std::string flatNames[12] = {
    "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "Cb"
};

//==============================================================================
// 根据音高与调号倾向返回音名
//==============================================================================
std::string PitchNameHelper::getNameForPitch(PitchClass pitch, bool isSharpKey)
{
    int idx = static_cast<int>(pitch);
    return isSharpKey ? sharpNames[idx] : flatNames[idx];
}

//==============================================================================
// 判断主音属于升号调还是降号调
//==============================================================================
bool PitchNameHelper::isSharpKeyForTonic(PitchClass tonic)
{
    static const std::vector<PitchClass> sharpTonionics = {
        PitchClass::C, PitchClass::G, PitchClass::D, PitchClass::A, PitchClass::E,
        PitchClass::B, PitchClass::F_Sharp, PitchClass::C_Sharp
    };
    return std::find(sharpTonionics.begin(), sharpTonionics.end(), tonic) != sharpTonionics.end();
}

//==============================================================================
// 获取全局鸿蒙中文字体（缓存，失败降级）
//==============================================================================
juce::Typeface::Ptr PitchNameHelper::getHarmonyTypeface()
{
    static auto typeface = []() -> juce::Typeface::Ptr {
        // 尝试从 BinaryData 加载鸿蒙字体
        auto data = BinaryData::HarmonyOS_Sans_SC_Regular_ttf;
        auto size = BinaryData::HarmonyOS_Sans_SC_Regular_ttfSize;
        if (data != nullptr && size > 0)
        {
            auto tf = juce::Typeface::createSystemTypefaceFor(data, size);
            if (tf != nullptr)
            {
                juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(tf);
                return tf;
            }
        }

        // 降级方案：Windows 系统常用中文字体
        #if JUCE_WINDOWS
        juce::Font fallbackFont(juce::String("Microsoft YaHei"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        fallbackFont = juce::Font(juce::String("SimHei"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        #elif JUCE_MAC
        juce::Font fallbackFont(juce::String("PingFang SC"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        fallbackFont = juce::Font(juce::String("Helvetica Neue"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        #elif JUCE_LINUX
        juce::Font fallbackFont(juce::String("Noto Sans CJK SC"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        fallbackFont = juce::Font(juce::String("WenQuanYi Zen Hei"), 12.0f, juce::Font::plain);
        if (auto tf = fallbackFont.getTypefacePtr())
            return tf;
        #endif

        // 最终降级：系统默认无衬线字体
        juce::Font defaultFont(juce::Font::getDefaultSansSerifFontName(), 12.0f, juce::Font::plain);
        if (auto tf = defaultFont.getTypefacePtr())
            return tf;

        // 极端情况：返回空指针（调用者需处理）
        return nullptr;
    }();
    return typeface;
}
