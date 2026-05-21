//==================================================================================================
//  TSDBuilder.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：实现 T/S/D 和弦家族构建
//        - 大调/小调/中古调式的 I/i、IV/iv、V 级三和弦
//        - 小调属和弦自动使用和声小调（升高 VII 级）
//        - Locrian 调式属和弦升至大三
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "TSDBuilder.h"

//==============================================================================
// 通用：根据音阶与调式类型构建 T/S/D 家族
//==============================================================================
static TSDFamily buildFamily(const std::vector<PitchClass>& scale, ScaleType type)
{
    // T = I 级和弦
    Chord tonic = ScaleEngine::getChord(scale, 1);
    tonic.name = "T";
    tonic.roman = "I";

    // S = IV 级和弦
    Chord subdominant = ScaleEngine::getChord(scale, 4);
    subdominant.name = "S";
    subdominant.roman = "IV";

    // D = V 级和弦
    Chord dominant = ScaleEngine::getChord(scale, 5);
    dominant.name = "D";
    dominant.roman = "V";

    // 小调色彩调式：T 和 S 改为小调前缀
    if (type == ScaleType::NaturalMinor || type == ScaleType::Dorian ||
        type == ScaleType::Phrygian || type == ScaleType::Locrian)
    {
        tonic.name = "t";
        tonic.roman = "i";
        subdominant.name = "s";
        subdominant.roman = "iv";
    }

    return { tonic, subdominant, dominant };
}

//==============================================================================
// 大调 T/S/D
//==============================================================================
TSDFamily TSDBuilder::getMajor(PitchClass tonic)
{
    auto scale = ScaleEngine::getScale(tonic, ScaleType::Major);
    return buildFamily(scale, ScaleType::Major);
}

//==============================================================================
// 小调 T/S/D（属和弦使用和声小调）
//==============================================================================
TSDFamily TSDBuilder::getMinor(PitchClass tonic)
{
    auto scale = ScaleEngine::getScale(tonic, ScaleType::NaturalMinor);
    // 属和弦使用和声小调（升高 VII 级）
    auto harmonicScale = scale;
    int seventhIdx = 6;
    int orig = static_cast<int>(harmonicScale[seventhIdx]);
    harmonicScale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
    auto family = buildFamily(harmonicScale, ScaleType::NaturalMinor);
    return family;
}

//==============================================================================
// 通用调式 T/S/D（支持全部 ScaleType）
//==============================================================================
TSDFamily TSDBuilder::getFamily(PitchClass tonic, ScaleType type)
{
    if (type == ScaleType::Major)
        return getMajor(tonic);

    if (type == ScaleType::NaturalMinor)
        return getMinor(tonic);

    // 其他调式：直接使用自然音阶
    auto scale = ScaleEngine::getScale(tonic, type);

    // Locrian：V 级原本是 b5，需升半音变为大三和弦
    if (type == ScaleType::Locrian)
    {
        auto vScale = scale;
        vScale[4] = static_cast<PitchClass>((static_cast<int>(vScale[4]) + 1) % 12);
        auto family = buildFamily(vScale, type);
        return family;
    }

    return buildFamily(scale, type);
}