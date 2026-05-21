//==================================================================================================
//  ChordNaming.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦命名逻辑实现
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ChordNaming.h"
#include "LocalisedStrings.h"
#include "PitchNameHelper.h"

juce::String ChordNaming::getSymbol(PitchClass root, bool isSharpKey,
                                    ChordQuality quality, int numNotes,
                                    const juce::String& extSuffix)
{
    juce::String rootName = PitchNameHelper::getNameForPitch(root, isSharpKey);

    if (numNotes == 3)
    {
        switch (quality)
        {
            case ChordQuality::MajorTriad:  return rootName;
            case ChordQuality::MinorTriad:  return rootName + "m";
            case ChordQuality::Dominant7:   return rootName;
            case ChordQuality::Diminished:  return rootName + "dim";
            case ChordQuality::Augmented:   return rootName + "aug";
            case ChordQuality::Sus2:        return rootName + "sus2";
            case ChordQuality::Sus4:        return rootName + "sus4";
            default:                        return rootName;
        }
    }
    else if (numNotes == 4)
    {
        switch (quality)
        {
            case ChordQuality::MajorTriad:  return rootName + "maj7";
            case ChordQuality::MinorTriad:  return rootName + "m7";
            case ChordQuality::Dominant7:
                if (extSuffix.isNotEmpty()) return rootName + "7" + extSuffix;
                else                        return rootName + "7";
            case ChordQuality::Diminished:  return rootName + "dim7";
            case ChordQuality::Sus4:        return rootName + "7sus4";
            case ChordQuality::Minor7b5:    return rootName + "m7b5";
            case ChordQuality::MinorMajor7: return rootName + "mMaj7";
            default:                        return rootName + "7";
        }
    }
    else
    {
        juce::String base;
        switch (quality)
        {
            case ChordQuality::MajorTriad: base = "maj"; break;
            case ChordQuality::MinorTriad: base = "m";   break;
            case ChordQuality::Dominant7:  base = "";    break;
            default:                       base = "";    break;
        }
        juce::String ext;
        switch (numNotes)
        {
            case 5:  ext = "9";  break;
            case 6:  ext = "11"; break;
            case 7:  ext = "13"; break;
            default: ext = "";
        }
        return rootName + base + ext;
    }
}

juce::String ChordNaming::getChineseName(const juce::String& symbol)
{
    if (symbol.isEmpty()) return {};

    juce::String root, suffix;
    if (symbol.length() > 1 && (symbol[1] == '#' || symbol[1] == 'b'))
    {
        root = symbol.substring(0, 2);
        suffix = symbol.substring(2);
    }
    else
    {
        root = symbol.substring(0, 1);
        suffix = symbol.substring(1);
    }

    struct Rule { const char* suffix; const char* nameKey; };
    static const Rule rules[] = {
        {"",        "auto_major_triad"},
        {"m",       "auto_minor_triad"},
        {"maj7",    "auto_major_seventh"},
        {"7",       "auto_dominant_seventh"},
        {"m7",      "auto_minor_seventh"},
        {"maj9",    "auto_major_ninth"},
        {"9",       "auto_dominant_ninth"},
        {"m9",      "auto_minor_ninth"},
        {"maj11",   "auto_major_eleventh"},
        {"11",      "auto_dominant_eleventh"},
        {"m11",     "auto_minor_eleventh"},
        {"maj13",   "auto_major_thirteenth"},
        {"13",      "auto_dominant_thirteenth"},
        {"m13",     "auto_minor_thirteenth"},
        {"dim",     "auto_diminished_triad"},
        {"aug",     "auto_augmented_triad"},
        {"sus2",    "auto_suspended_second"},
        {"sus4",    "auto_suspended_fourth"},
        {"mMaj7",   "auto_minor_major_seventh"},
        {"m7b5",    "auto_half_diminished_seventh"},
        {"7b9",     "auto_english_translation_pending_7"},
        {"7#9",     "auto_english_translation_pending_4"},
        {"7#11",    "auto_english_translation_pending_6"},
        {"7b13",    "auto_english_translation_pending_10"},
        {"7b9#11",  "auto_english_translation_pending_8"},
        {"7#9#11",  "auto_english_translation_pending_5"},
        {"7b9b13",  "auto_english_translation_pending_9"},
    };

    for (auto& rule : rules)
        if (suffix == juce::String(rule.suffix))
            return root + LocalisedStrings::translate(rule.nameKey);

    return symbol;
}

juce::String ChordNaming::getSymbolFromFunction(PitchClass root, bool isSharpKey,
                                                const juce::String& funcName, bool isMajor)
{
    juce::String rootName = PitchNameHelper::getNameForPitch(root, isSharpKey);
    if (funcName.isEmpty())
        return rootName;

    juce::String funcBase = funcName.substring(0, 1);
    int ext = 3;
    if (funcName.length() > 1)
        ext = funcName.substring(1).getIntValue();

    struct Rule { juce::String func; int ext; juce::String quality; };

    if (isMajor) {
        static const Rule majorRules[] = {
            {"T", 3, ""}, {"T", 7, "maj7"}, {"T", 9, "maj9"}, {"T", 11, "maj11"}, {"T", 13, "maj13"},
            {"S", 3, ""}, {"S", 7, "maj7"}, {"S", 9, "maj9"}, {"S", 11, "maj11"}, {"S", 13, "maj13"},
            {"D", 3, ""}, {"D", 7, "7"},    {"D", 9, "9"},    {"D", 11, "11"},   {"D", 13, "13"}
        };
        for (auto& r : majorRules)
            if (funcBase == r.func && ext == r.ext)
                return rootName + r.quality;
    }
    else {
        static const Rule minorRules[] = {
            {"t", 3, "m"},  {"t", 7, "m7"},  {"t", 9, "m9"},  {"t", 11, "m11"},  {"t", 13, "m13"},
            {"s", 3, "m"},  {"s", 7, "m7"},  {"s", 9, "m9"},  {"s", 11, "m11"},  {"s", 13, "m13"},
            {"D", 3, ""},   {"D", 7, "7"},   {"D", 9, "9"},   {"D", 11, "11"},   {"D", 13, "13"}
        };
        for (auto& r : minorRules)
            if (funcBase == r.func && ext == r.ext)
                return rootName + r.quality;
    }
    return rootName;
}
