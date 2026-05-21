//==================================================================================================
//  MoodMapper.cpp
//  椤圭洰锛欳hordReference (鍜屽鸡璋冩€ч€熸煡鍣?
//  鍔熻兘锛氬拰寮︽儏缁笌棰滆壊鏄犲皠瀹炵幇
//        - 澶ц皟/灏忚皟鍚勪富闊充笌鍔熻兘缁勫悎鐨勬儏缁弿杩拌〃
//        - T=鏆栭粍, S=姗勬, D=绮夌孩 绛夐鑹叉槧灏?
//  娉ㄦ剰锛氶渶淇濆瓨涓?UTF-8 with BOM
//==================================================================================================

#include "MusicTypes.h"
#include "LocalisedStrings.h"
#include "MoodMapper.h"
#include <map>

//==============================================================================
// 澶ц皟鎯呯华鏄犲皠琛紙閿?= {涓婚煶, 鍔熻兘鍓嶇紑}锛?
//==============================================================================
static std::map<std::pair<PitchClass, juce::String>, juce::String> majorMoods = {
    {{PitchClass::C,       juce::String::fromUTF8(u8"T")}, "auto_stable_calm"},
    {{PitchClass::C,       juce::String::fromUTF8(u8"S")}, "auto_transitional"},
    {{PitchClass::C,       juce::String::fromUTF8(u8"D")}, "auto_tense"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"T")}, "auto_brighter_and_stable"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"S")}, "auto_lively_transition"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::D_Flat,  juce::String::fromUTF8(u8"T")}, "auto_solid"},
    {{PitchClass::D_Flat,  juce::String::fromUTF8(u8"S")}, "auto_calm_transition"},
    {{PitchClass::D_Flat,  juce::String::fromUTF8(u8"D")}, "auto_melancholic_tension"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"T")}, "auto_bright_and_stable_2"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"S")}, "auto_energetic_transition"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"D")}, "auto_heated_tension"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"T")}, "auto_restrained_stability"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"S")}, "auto_subtle_transition"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"D")}, "auto_mysterious_tension"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"T")}, "auto_bright_and_stable"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"S")}, "auto_clear_transition"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"D")}, "auto_strong_tension"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"T")}, "auto_light_and_stable"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"S")}, "auto_fresh_transition"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"D")}, "auto_fantasy_like_tension"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"T")}, "auto_energetic_stability"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"S")}, "auto_nimble_transition"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"D")}, "auto_rich_tension"},
    {{PitchClass::G_Flat,  juce::String::fromUTF8(u8"T")}, "auto_calm_stability"},
    {{PitchClass::G_Flat,  juce::String::fromUTF8(u8"S")}, "auto_deep_transition"},
    {{PitchClass::G_Flat,  juce::String::fromUTF8(u8"D")}, "auto_solemn_tension"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"T")}, "auto_fresh_stability"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"S")}, "auto_open_transition"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"D")}, "auto_romantic_tension"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"T")}, "auto_quiet_stability"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"S")}, "auto_soothing_transition"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"D")}, "auto_ethereal_tension"},
    {{PitchClass::A,       juce::String::fromUTF8(u8"T")}, "auto_energetic_stability"},
    {{PitchClass::A,       juce::String::fromUTF8(u8"S")}, "auto_calm_transition"},
    {{PitchClass::A,       juce::String::fromUTF8(u8"D")}, "auto_passionate_tension"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"T")}, "auto_peaceful_stability"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"S")}, "auto_mysterious_transition"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"D")}, "auto_dreamy_tension"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"T")}, "auto_light_and_stable"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"S")}, "auto_profound_transition"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"D")}, "auto_ornate_tension"},
    {{PitchClass::C_Flat,  juce::String::fromUTF8(u8"T")}, "auto_composed_stability"},
    {{PitchClass::C_Flat,  juce::String::fromUTF8(u8"S")}, "auto_serious_transition"},
    {{PitchClass::C_Flat,  juce::String::fromUTF8(u8"D")}, "auto_grave_tension"},
};

//==============================================================================
// 灏忚皟鎯呯华鏄犲皠琛紙閿?= {涓婚煶, 鍔熻兘鍓嶇紑}锛?
//==============================================================================
static std::map<std::pair<PitchClass, juce::String>, juce::String> minorMoods = {
    {{PitchClass::A,       juce::String::fromUTF8(u8"T")}, "auto_soft_stability"},
    {{PitchClass::A,       juce::String::fromUTF8(u8"S")}, "auto_plain_transition"},
    {{PitchClass::A,       juce::String::fromUTF8(u8"D")}, "auto_tense"},
    {{PitchClass::A_Sharp, juce::String::fromUTF8(u8"T")}, "auto_lively_stability"},
    {{PitchClass::A_Sharp, juce::String::fromUTF8(u8"S")}, "auto_lively_transition"},
    {{PitchClass::A_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"T")}, "auto_calm_stability"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"S")}, "auto_restrained_transition"},
    {{PitchClass::B_Flat,  juce::String::fromUTF8(u8"D")}, "auto_melancholic_tension"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"T")}, "auto_mysterious_stability"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"S")}, "auto_clear_transition"},
    {{PitchClass::B,       juce::String::fromUTF8(u8"D")}, "auto_mysterious_tension"},
    {{PitchClass::C,       juce::String::fromUTF8(u8"T")}, "auto_serious_stability"},
    {{PitchClass::C,       juce::String::fromUTF8(u8"S")}, "auto_deep_transition"},
    {{PitchClass::C,       juce::String::fromUTF8(u8"D")}, "auto_tense"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"T")}, "auto_nimble_stability"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"S")}, "auto_nimble_transition"},
    {{PitchClass::C_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"T")}, "auto_soothing_stability"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"S")}, "auto_deep_transition"},
    {{PitchClass::D,       juce::String::fromUTF8(u8"D")}, "auto_heated_tension"},
    {{PitchClass::D_Sharp, juce::String::fromUTF8(u8"T")}, "auto_calm_stability"},
    {{PitchClass::D_Sharp, juce::String::fromUTF8(u8"S")}, "auto_open_transition"},
    {{PitchClass::D_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"T")}, "auto_mysterious_stability"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"S")}, "auto_serious_transition"},
    {{PitchClass::E_Flat,  juce::String::fromUTF8(u8"D")}, "auto_mysterious_tension"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"T")}, "auto_light_and_stable"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"S")}, "auto_profound_transition"},
    {{PitchClass::E,       juce::String::fromUTF8(u8"D")}, "auto_strong_tension"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"T")}, "auto_grave_stability"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"S")}, "auto_serious_transition"},
    {{PitchClass::F,       juce::String::fromUTF8(u8"D")}, "auto_tense"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"T")}, "auto_calm_stability"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"S")}, "auto_open_transition"},
    {{PitchClass::F_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"T")}, "auto_fresh_stability"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"S")}, "auto_open_transition"},
    {{PitchClass::G,       juce::String::fromUTF8(u8"D")}, "auto_romantic_tension"},
    {{PitchClass::G_Sharp, juce::String::fromUTF8(u8"T")}, "auto_energetic_stability"},
    {{PitchClass::G_Sharp, juce::String::fromUTF8(u8"S")}, "auto_calm_transition"},
    {{PitchClass::G_Sharp, juce::String::fromUTF8(u8"D")}, "auto_more_intense_tension"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"T")}, "auto_composed_stability"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"S")}, "auto_serious_transition"},
    {{PitchClass::A_Flat,  juce::String::fromUTF8(u8"D")}, "auto_ethereal_tension"},
};

//==============================================================================
// 棰滆壊鏄犲皠锛氭牴鎹姛鑳藉墠缂€鍜岃皟寮忚繑鍥炴皼鍥磋壊
//==============================================================================
static juce::Colour getMoodColour(const juce::String& funcBase, bool isMajor)
{
    if (isMajor) {
        if (funcBase == juce::String::fromUTF8(u8"T")) return juce::Colour(0xFFFFE4B5);   // 鏆栭粍
        if (funcBase == juce::String::fromUTF8(u8"S")) return juce::Colour(0xFFF0E68C);   // 姗勬
        if (funcBase == juce::String::fromUTF8(u8"D")) return juce::Colour(0xFFFF69B4);   // 绮夌孩
    } else {
        if (funcBase == juce::String::fromUTF8(u8"t")) return juce::Colour(0xFFE6E6FA);   // 娣＄传
        if (funcBase == juce::String::fromUTF8(u8"s")) return juce::Colour(0xFF808000);   // 娣辨﹦姒?
        if (funcBase == juce::String::fromUTF8(u8"D")) return juce::Colour(0xFFFF6B94);   // 绮夌孩
    }
    return juce::Colours::grey;
}

//==============================================================================
// 澶栭儴鎺ュ彛锛氭牴鎹富闊炽€佸姛鑳界鍙枫€佽皟寮忚繑鍥炴儏缁俊鎭?
//==============================================================================
MoodInfo MoodMapper::getMood(PitchClass tonic, const juce::String& function, bool isMajor)
{
    juce::String funcBase = function.substring(0, 1);   // 鎻愬彇鍔熻兘鍓嶇紑锛堝幓鏁板瓧锛?
    
    juce::String description;
    if (isMajor) {
        auto it = majorMoods.find({tonic, funcBase});
        if (it != majorMoods.end())
            description = LocalisedStrings::translate(it->second);
    } else {
        auto it = minorMoods.find({tonic, funcBase});
        if (it != minorMoods.end())
            description = LocalisedStrings::translate(it->second);
    }
    
    // 鏈尮閰嶆椂鐨勯粯璁ゅ€?
    if (description.isEmpty()) {
        if (isMajor) {
            if (funcBase == juce::String::fromUTF8(u8"T")) description = LocalisedStrings::translate("auto_stable_calm");
            else if (funcBase == juce::String::fromUTF8(u8"S")) description = LocalisedStrings::translate("auto_transitional");
            else if (funcBase == juce::String::fromUTF8(u8"D")) description = LocalisedStrings::translate("auto_tense");
        } else {
            if (funcBase == juce::String::fromUTF8(u8"t")) description = LocalisedStrings::translate("auto_soft_stability");
            else if (funcBase == juce::String::fromUTF8(u8"s")) description = LocalisedStrings::translate("auto_plain_transition");
            else if (funcBase == juce::String::fromUTF8(u8"D")) description = LocalisedStrings::translate("auto_tense");
        }
    }
    
    juce::Colour colour = getMoodColour(funcBase, isMajor);
    return { description, colour };
}
