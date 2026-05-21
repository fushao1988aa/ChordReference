//==================================================================================================
//  ArpeggioMatcher.cpp
//  Project: ChordReference
//  Purpose: Pattern scoring implementation for the intelligent arpeggio system.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#include "ArpeggioMatcher.h"
#include <algorithm>

namespace
{
    juce::String cn(const char* text)
    {
        return juce::String::fromUTF8(text);
    }

    bool isAnyOfFamily(const juce::String& family, std::initializer_list<const char*> familyIds)
    {
        for (const auto* id : familyIds)
            if (family == id)
                return true;

        return false;
    }
}

int ArpeggioMatcher::countMatches(const juce::StringArray& a, const juce::StringArray& b)
{
    int count = 0;
    for (const auto& item : a)
        if (b.contains(item, true))
            ++count;
    return count;
}

int ArpeggioMatcher::scoreFamilyFit(const ArpeggioContext& context, const ArpeggioPattern& pattern)
{
    if (pattern.familyId.isEmpty())
        return 0;

    int score = 0;
    const auto family = pattern.familyId;

    if (context.regionTags.contains("chinese", true)
        && (family == "ChineseOpenFamily" || family == "GuofengFlowFamily" || family == "GuzhengRollFamily"
            || family == "PipaSweepFamily" || family == "GuqinSparseFamily" || family == "ChineseOperaOrnamentFamily"
            || family == "CPopBalladPianoFamily" || family == "MandopopVerseFamily" || family == "MandopopChorusFamily"
            || family == "MandopopPrechorusFamily" || family == "MandopopBridgeFamily" || family == "CPopChorusDriveFamily"
            || family == "JayStyleSyncopationFamily" || family == "CampusFolkFamily"))
        score += 36;
    if (context.regionTags.contains("latin", true) && (family == "LatinMontunoFamily" || family == "GuitarPickingFamily"))
        score += 18;
    if (context.regionTags.contains("korean", true)
        && (family == "SyncopatedElectronicFamily" || family == "EDMTranceFamily" || family == "BrokenPianoFamily"))
        score += family == "SyncopatedElectronicFamily" ? 46 : 30;
    if (context.regionTags.contains("japanese", true)
        && (family == "SyncopatedElectronicFamily" || family == "BrokenPianoFamily" || family == "JayStyleSyncopationFamily"))
        score += 38;
    if (context.regionTags.contains("himalayan", true)
        && (family == "RagaDroneFamily" || family == "PedalDroneFamily" || family == "GuitarPickingFamily" || family == "CinematicOstinatoFamily"))
        score += 24;

    if ((context.styleTags.contains("rnb", true) || context.styleTags.contains("neo-soul", true)
         || context.styleTags.contains("lofi", true) || context.styleTags.contains("hip-hop", true))
        && (family == "NeoSoulFamily" || family == "JayStyleSyncopationFamily"))
        score += 42;
    if ((context.styleTags.contains("edm", true) || context.styleTags.contains("electro", true)
         || context.styleTags.contains("future-bass", true) || context.styleTags.contains("synth", true))
        && (family == "SyncopatedElectronicFamily" || family == "EDMTranceFamily"))
        score += 40;
    if (context.styleTags.contains("kpop", true)
        && (family == "SyncopatedElectronicFamily" || family == "EDMTranceFamily" || family == "BrokenPianoFamily"))
        score += family == "SyncopatedElectronicFamily" ? 58 : 32;
    if ((context.styleTags.contains("jpop", true) || context.styleTags.contains("j-pop", true))
        && (family == "SyncopatedElectronicFamily" || family == "BrokenPianoFamily" || family == "JayStyleSyncopationFamily"))
        score += 46;
    if ((context.styleTags.contains("cinematic", true) || context.styleTags.contains("ambient", true)
         || context.styleTags.contains("soundtrack", true))
        && (family == "CinematicOstinatoFamily" || family == "PadFamily" || family == "PedalDroneFamily"
            || family == "MarchPulseFamily" || family == "CadentialBreathFamily" || family == "GuofengFlowFamily"
            || family == "GuqinSparseFamily" || family == "GuzhengRollFamily"))
        score += 38;
    if ((context.styleTags.contains("folk", true) || context.styleTags.contains("country", true)
         || context.styleTags.contains("bossa", true) || context.styleTags.contains("salsa", true))
        && (family == "GuitarPickingFamily" || family == "LatinMontunoFamily" || family == "PedalDroneFamily"
            || family == "GuofengFlowFamily" || family == "CampusFolkFamily" || family == "ChineseOpenFamily"))
        score += 34;
    if ((context.styleTags.contains("classical", true) || context.styleTags.contains("neoclassical", true))
        && family == "BrokenPianoFamily")
        score += 32;
    if ((context.styleTags.contains("pop", true) || context.styleTags.contains("jpop", true)
         || context.styleTags.contains("ballad", true) || context.styleTags.contains("Chinese", true))
        && (family == "BrokenPianoFamily" || family == "PadFamily" || family == "CPopBalladPianoFamily"
            || family == "MandopopVerseFamily" || family == "MandopopChorusFamily" || family == "JayStyleSyncopationFamily"
            || family == "MandopopPrechorusFamily" || family == "MandopopBridgeFamily" || family == "CPopChorusDriveFamily"
            || family == "CampusFolkFamily" || family == "GuofengFlowFamily"))
        score += 30;
    if ((context.styleTags.contains("mandopop", true) || context.styleTags.contains("c-pop", true))
        && (family == "CPopBalladPianoFamily" || family == "MandopopVerseFamily" || family == "MandopopChorusFamily"
            || family == "MandopopPrechorusFamily" || family == "MandopopBridgeFamily" || family == "CPopChorusDriveFamily"
            || family == "JayStyleSyncopationFamily" || family == "CampusFolkFamily"))
        score += 42;
    if (context.styleTags.contains("jazz", true) && (family == "NeoSoulFamily" || family == "JazzWalkingFamily"))
        score += family == "JazzWalkingFamily" ? 44 : 24;

    if ((context.styleTags.contains("mandopop", true) || context.styleTags.contains("c-pop", true))
        && (family == "GuofengFlowFamily" || family == "GuzhengRollFamily" || family == "PipaSweepFamily" || family == "GuqinSparseFamily"))
        score -= context.styleTags.contains("cinematic", true) || context.styleTags.contains("folk", true) ? 0 : 24;
    if ((context.regionTags.contains("japanese", true) || context.regionTags.contains("korean", true))
        && isAnyOfFamily(family, { "CPopBalladPianoFamily", "MandopopVerseFamily", "MandopopChorusFamily",
                                   "MandopopPrechorusFamily", "MandopopBridgeFamily", "CPopChorusDriveFamily",
                                   "GuofengFlowFamily", "GuzhengRollFamily", "PipaSweepFamily", "GuqinSparseFamily",
                                   "ChineseOpenFamily", "ChineseOperaOrnamentFamily", "CampusFolkFamily" }))
        score -= 52;
    if (context.regionTags.contains("himalayan", true)
        && (family == "ChineseOpenFamily" || family == "GuofengFlowFamily" || family == "GuzhengRollFamily"
            || family == "PipaSweepFamily" || family == "GuqinSparseFamily" || family == "CampusFolkFamily"))
        score -= 32;

    if ((context.moodTags.contains("warm", true) || context.moodTags.contains("calm", true)
         || context.moodTags.contains("sad", true) || context.moodTags.contains("dreamy", true))
        && (family == "PadFamily" || family == "NeoSoulFamily" || family == "CinematicOstinatoFamily"
            || family == "PedalDroneFamily" || family == "CadentialBreathFamily" || family == "CPopBalladPianoFamily"
            || family == "MandopopVerseFamily" || family == "GuqinSparseFamily" || family == "GuofengFlowFamily"))
        score += 14;
    if ((context.moodTags.contains("bright", true) || context.moodTags.contains("epic", true)
         || context.moodTags.contains("tension", true))
        && (family == "SyncopatedElectronicFamily" || family == "EDMTranceFamily" || family == "CinematicOstinatoFamily"
            || family == "GamelanOstinatoFamily" || family == "MarchPulseFamily" || family == "MaqamOrnamentFamily"
            || family == "MandopopChorusFamily" || family == "PipaSweepFamily" || family == "ChineseOperaOrnamentFamily"
            || family == "GuzhengRollFamily" || family == "MandopopPrechorusFamily" || family == "MandopopBridgeFamily"
            || family == "CPopChorusDriveFamily"))
        score += 12;

    if (context.styleTags.contains("epic", true) || context.styleTags.contains("ambient", true) || context.formTags.contains("intro", true))
    {
        if (family == "OrchestralStaccatoFamily" || family == "CinematicStringPadFamily" || family == "HybridEpicStringFamily")
        {
            score += 35; // 给予影视/弦乐家族极高的匹配定向加分！
        }
    }

    if ((context.styleTags.contains("epic", true) || context.styleTags.contains("cinematic", true)
         || context.styleTags.contains("soundtrack", true))
        && isAnyOfFamily(family, { "OrchestralStaccatoFamily", "CinematicStringPadFamily", "HybridEpicStringFamily" }))
        score += 34;

    if ((context.styleTags.contains("epic", true) || context.moodTags.contains("epic", true)
         || context.moodTags.contains(cn(u8"史诗"), true) || context.moodTags.contains(cn(u8"震撼"), true)
         || context.moodTags.contains(cn(u8"大气"), true))
        && isAnyOfFamily(family, { "HybridEpicStringFamily", "OrchestralStaccatoFamily", "MarchPulseFamily" }))
        score += 34;

    if ((context.styleTags.contains("edm", true) || context.styleTags.contains("electro", true)
         || context.styleTags.contains("rock", true) || context.styleTags.contains("future-bass", true))
        && family == "ElectronicArpFamily")
        score += 36;

    if ((context.styleTags.contains("rock", true) || context.styleTags.contains("metal", true)
         || context.moodTags.contains("energetic", true) || context.moodTags.contains(cn(u8"愤怒"), true))
        && isAnyOfFamily(family, { "MarchPulseFamily", "HybridEpicStringFamily", "OrchestralStaccatoFamily",
                                   "JayStyleSyncopationFamily", "ElectronicArpFamily" }))
        score += context.styleTags.contains("rock", true) ? 38 : 22;

    if (context.styleTags.contains("march", true) && (family == "MarchPulseFamily" || family == "OrchestralStaccatoFamily"))
        score += 56;

    if (context.styleTags.contains("folk", true) || context.styleTags.contains(cn(u8"Folk/民谣"), true))// 如果用户明确想找民谣风格（预设或上下文选中了民谣）
    {
        // 1. 如果范式属于吉他指弹或校园民谣家族，直接赋予极高的权重保护分
        if (family == "GuitarPickingFamily" || family == "CampusFolkFamily")
        {
            score += 45; // 强行拉高民谣正统指弹的顺位
        }
        
        // 2. 豁免民谣织体因为复杂度带来的扣分惩罚（指弹天然需要细节，不该被判定为乱糟糟的复杂度）
        if (pattern.complexity >= 3)
        {
            score += (pattern.complexity - 3) * 4; // 把前面因为复杂度高被扣掉的分数完全补回来！
        }
    }

    return score;
}

int ArpeggioMatcher::scoreDensity(ArpeggioDensity requested, ArpeggioDensity candidate)
{
    if (requested == ArpeggioDensity::Mixed || candidate == ArpeggioDensity::Mixed)
        return 6;

    return requested == candidate ? 10 : 0;
}

int ArpeggioMatcher::scoreTempo(double bpm, const ArpeggioPattern& pattern)
{
    if (bpm < 72.0)
        return pattern.density == ArpeggioDensity::Whole || pattern.density == ArpeggioDensity::Half ? 14 : 2;

    if (bpm > 138.0)
        return pattern.density == ArpeggioDensity::Sixteenth ? 1 : 10;

    if (bpm >= 100.0 && bpm <= 132.0 && (pattern.density == ArpeggioDensity::Eighth || pattern.density == ArpeggioDensity::Mixed))
        return 9;

    if (bpm >= 120.0 && pattern.grooveAffinity.equalsIgnoreCase("Tight"))
        return 5;

    return 6;
}

int ArpeggioMatcher::scoreGrooveFit(const ArpeggioContext& context, double bpm, const ArpeggioPattern& pattern)
{
    const auto groove = pattern.grooveAffinity;
    int score = 0;

    if ((context.styleTags.contains("rnb", true) || context.styleTags.contains("lofi", true)
         || context.styleTags.contains("hip-hop", true) || context.styleTags.contains("neo-soul", true))
        && (groove.equalsIgnoreCase("Late") || groove.equalsIgnoreCase("Shuffle")))
        score += 18;
    if ((context.styleTags.contains("edm", true) || context.styleTags.contains("electro", true)
         || context.styleTags.contains("future-bass", true) || context.styleTags.contains("latin", true)
         || context.styleTags.contains("bossa", true) || context.styleTags.contains("salsa", true))
        && groove.equalsIgnoreCase("Tight"))
        score += 16;
    if ((context.styleTags.contains("pop", true) || context.styleTags.contains("classical", true)
         || context.styleTags.contains("folk", true) || context.styleTags.contains("cinematic", true))
        && groove.equalsIgnoreCase("Straight"))
        score += 9;
    if ((context.styleTags.contains("rock", true) || context.styleTags.contains("edm", true)
         || context.styleTags.contains("dance", true) || context.moodTags.contains("energetic", true))
        && groove.equalsIgnoreCase("Early"))
        score += 13;

    if (bpm < 82.0 && (groove.equalsIgnoreCase("Late") || groove.equalsIgnoreCase("Straight")))
        score += 5;
    if (bpm > 128.0 && groove.equalsIgnoreCase("Shuffle"))
        score -= 6;
    if (bpm < 72.0 && groove.equalsIgnoreCase("Early"))
        score -= 5;

    return score;
}

int ArpeggioMatcher::scoreFormDynamics(const ArpeggioContext& context, const ArpeggioPattern& pattern)
{
    int score = countMatches(context.formTags, pattern.formFit) * 18;

    if (context.formTags.contains("intro", true) || context.formTags.contains("verse", true))
    {
        if (pattern.energy <= 3 && pattern.density != ArpeggioDensity::Sixteenth)
            score += 12;
        if (pattern.energy >= 5 || pattern.density == ArpeggioDensity::Sixteenth)
            score -= 10;
    }

    if (context.formTags.contains("chorus", true))
    {
        if (pattern.energy >= 3 && (pattern.density == ArpeggioDensity::Eighth || pattern.density == ArpeggioDensity::Mixed))
            score += 14;
        if (pattern.familyId == "MandopopChorusFamily" || pattern.familyId == "CPopChorusDriveFamily")
            score += 12;
        if (pattern.energy <= 1 || pattern.density == ArpeggioDensity::Whole)
            score -= 8;
    }

    if (context.formTags.contains("bridge", true))
    {
        if (pattern.complexity >= 3 || context.moodTags.contains("tension", true))
            score += 10;
        if (pattern.familyId == "MandopopPrechorusFamily" || pattern.familyId == "MandopopBridgeFamily")
            score += 14;
    }

    if (context.formTags.contains("outro", true) || context.formTags.contains("cadence", true))
    {
        if (pattern.density == ArpeggioDensity::Whole || pattern.density == ArpeggioDensity::Half)
            score += 14;
        if (pattern.energy >= 4)
            score -= 8;
    }

    return score;
}

int ArpeggioMatcher::scoreComplexityFit(const ArpeggioContext& context, const ArpeggioPattern& pattern)
{
    if (context.complexityTags.isEmpty())
        return 0;

    int score = 0;
    if (context.complexityTags.contains("Triad", true))
        score += pattern.complexity <= 2 ? 8 : -4;
    if (context.complexityTags.contains("Seventh", true))
        score += pattern.supportsExtensions ? 8 : 0;
    if (context.complexityTags.contains("Ninth", true) || context.complexityTags.contains("Extended", true))
        score += pattern.supportsExtensions && pattern.complexity >= 3 ? 12 : -6;
    if (context.complexityTags.contains("Modal", true))
        score += pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "CinematicOstinatoFamily"
              || pattern.familyId == "PedalDroneFamily" || pattern.familyId == "MaqamOrnamentFamily"
              || pattern.familyId == "GuofengFlowFamily" || pattern.familyId == "GuzhengRollFamily"
              || pattern.familyId == "PipaSweepFamily" || pattern.familyId == "GuqinSparseFamily"
              || pattern.familyId == "ChineseOperaOrnamentFamily" || pattern.familyId == "CinematicStringPadFamily" ? 8 : 0;
    if (context.complexityTags.contains("Chromatic", true))
        score += pattern.familyId == "NeoSoulFamily" || pattern.familyId == "JazzWalkingFamily"
              || pattern.familyId == "MaqamOrnamentFamily" || pattern.familyId == "JayStyleSyncopationFamily"
              || pattern.familyId == "ElectronicArpFamily" || pattern.familyId == "HybridEpicStringFamily" ? 10 : -4;

    return score;
}

int ArpeggioMatcher::scoreTextureFit(const ArpeggioContext& context, const ArpeggioPattern& pattern)
{
    if (context.textureTags.isEmpty())
        return 0;

    int score = 0;

    if (context.textureTags.contains("PentatonicOpen", true))
    {
        if ((context.styleTags.contains("mandopop", true) || context.styleTags.contains("c-pop", true))
            && (pattern.familyId == "JayStyleSyncopationFamily" || pattern.familyId == "CPopBalladPianoFamily"
                || pattern.familyId == "MandopopVerseFamily" || pattern.familyId == "MandopopChorusFamily"
                || pattern.familyId == "CPopChorusDriveFamily"))
        {
            score += 36;
        }
        if (pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "GuofengFlowFamily"
            || pattern.familyId == "GuzhengRollFamily" || pattern.familyId == "PipaSweepFamily"
            || pattern.familyId == "GuqinSparseFamily")
            score += 32;
    }
    if (context.textureTags.contains("ChineseFolk", true))
    {
        if ((context.styleTags.contains("mandopop", true) || context.styleTags.contains("c-pop", true))
            && (pattern.familyId == "CPopBalladPianoFamily" || pattern.familyId == "MandopopVerseFamily"
                || pattern.familyId == "MandopopChorusFamily" || pattern.familyId == "CPopChorusDriveFamily"
                || pattern.familyId == "JayStyleSyncopationFamily"))
            score += 30;
        else if (pattern.familyId == "GuofengFlowFamily" || pattern.familyId == "ChineseOpenFamily")
            score += 34;
        else if (pattern.familyId == "CampusFolkFamily" || pattern.familyId == "MandopopVerseFamily"
                 || pattern.familyId == "MandopopPrechorusFamily" || pattern.familyId == "MandopopBridgeFamily"
                 || pattern.familyId == "CPopChorusDriveFamily"
                 || pattern.familyId == "GuzhengRollFamily" || pattern.familyId == "PipaSweepFamily"
                 || pattern.familyId == "GuqinSparseFamily")
            score += 24;
    }
    if (context.textureTags.contains("ChineseOpera", true))
    {
        if (pattern.familyId == "ChineseOperaOrnamentFamily")
            score += 44;
        else if (pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "GuofengFlowFamily")
            score += 22;
    }
    if (context.textureTags.contains("EthnicFolk", true))
    {
        if (context.regionTags.contains("chinese", true))
            score += pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "GuitarPickingFamily"
                  || pattern.familyId == "GuofengFlowFamily" || pattern.familyId == "PedalDroneFamily"
                  || pattern.familyId == "CampusFolkFamily" || pattern.familyId == "MaqamOrnamentFamily" ? 16 : 0;
        else
            score += pattern.familyId == "GuitarPickingFamily" || pattern.familyId == "PedalDroneFamily"
                  || pattern.familyId == "CinematicOstinatoFamily" || pattern.familyId == "RagaDroneFamily" ? 18 : 0;
    }
    if (context.textureTags.contains("MaqamOrnament", true))
    {
        if (pattern.familyId == "MaqamOrnamentFamily")
            score += 42;
        else if (pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "CinematicOstinatoFamily")
            score += 12;
    }
    if (context.textureTags.contains("RagaDrone", true))
    {
        if (pattern.familyId == "PedalDroneFamily")
            score += 30;
        else if (pattern.familyId == "PadFamily" || pattern.familyId == "CinematicOstinatoFamily")
            score += 18;
    }
    if (context.textureTags.contains("GamelanOstinato", true))
    {
        if (pattern.familyId == "GamelanOstinatoFamily")
            score += 42;
        else if (pattern.familyId == "CinematicOstinatoFamily" || pattern.familyId == "SyncopatedElectronicFamily")
            score += 16;
    }
    if (context.textureTags.contains("PedalDrone", true) || context.textureTags.contains("SingleChordPulse", true))
    {
        if (pattern.familyId == "PedalDroneFamily" || pattern.familyId == "GuqinSparseFamily")
            score += 44;
        else if (pattern.familyId == "PadFamily" || pattern.familyId == "CinematicOstinatoFamily")
            score += 24;
    }
    if (context.textureTags.contains("ChromaticTension", true))
        score += pattern.familyId == "NeoSoulFamily" || pattern.familyId == "CinematicOstinatoFamily"
              || pattern.familyId == "JayStyleSyncopationFamily" || pattern.familyId == "ElectronicArpFamily"
              || pattern.familyId == "HybridEpicStringFamily" ? 18 : 0;
    if (context.textureTags.contains("DiminishedPassing", true))
        score += pattern.familyId == "NeoSoulFamily" || pattern.familyId == "JazzWalkingFamily" ? 16 : 0;
    if (context.textureTags.contains("CircleMotion", true))
        score += pattern.familyId == "BrokenPianoFamily" || pattern.familyId == "JazzWalkingFamily"
              || pattern.familyId == "CPopBalladPianoFamily" || pattern.familyId == "MandopopVerseFamily"
              || pattern.familyId == "MandopopPrechorusFamily" || pattern.familyId == "MandopopBridgeFamily" ? 14 : 0;
    if (context.textureTags.contains("CadentialBreath", true))
    {
        if (pattern.familyId == "CadentialBreathFamily" || pattern.familyId == "CPopBalladPianoFamily")
            score += 36;
        else if (pattern.familyId == "MandopopVerseFamily" || pattern.familyId == "MandopopBridgeFamily"
                 || pattern.familyId == "GuqinSparseFamily")
            score += 24;
        else if (pattern.familyId == "PadFamily" || pattern.density == ArpeggioDensity::Half || pattern.density == ArpeggioDensity::Whole)
            score += 18;
    }
    if (context.textureTags.contains("MarchPulse", true))
    {
        if (pattern.familyId == "MarchPulseFamily")
            score += 78;
        else if (pattern.familyId == "OrchestralStaccatoFamily" || pattern.familyId == "HybridEpicStringFamily")
            score += 52;
        else if (pattern.grooveAffinity.equalsIgnoreCase("Straight") && pattern.energy >= 3)
            score += 4;

        if (pattern.familyId == "GuofengFlowFamily" || pattern.familyId == "MandopopVerseFamily"
            || pattern.familyId == "ChineseOpenFamily" || pattern.familyId == "CPopBalladPianoFamily")
            score -= 28;
    }
    if (context.textureTags.contains("LatinMontuno", true))
    {
        if (pattern.familyId == "LatinMontunoFamily")
            score += 42;
        else if (pattern.familyId == "GuitarPickingFamily" && pattern.grooveAffinity.equalsIgnoreCase("Tight"))
            score += 20;
    }
    if (context.textureTags.contains("TripleMeter", true))
    {
        if (pattern.styleTags.contains("3/4", true) || pattern.styleTags.contains("6/8", true)
            || pattern.styleTags.contains("waltz", true))
            score += 44;
        else if (pattern.cycleBeats == 3.0)
            score += 20;
        else if (pattern.density == ArpeggioDensity::Sixteenth && pattern.energy >= 4)
            score -= 8;
    }
    if (context.textureTags.contains("CompoundMeter", true))
    {
        if (pattern.styleTags.contains("12/8", true))
            score += 44;
        else if (pattern.grooveAffinity.equalsIgnoreCase("Shuffle"))
            score += 16;
    }

    return score;
}

std::vector<ArpeggioRankedPattern> ArpeggioMatcher::rankPatterns(const ArpeggioContext& context,
                                                                 const ArpeggioGenerationSettings& settings,
                                                                 const std::vector<ArpeggioPattern>& patterns)
{
    std::vector<ArpeggioRankedPattern> ranked;
    ranked.reserve(patterns.size());

    for (const auto& pattern : patterns)
    {
        int score = 0;
        juce::StringArray reasons;

        const int styleHits = countMatches(context.styleTags, pattern.styleTags);
        const int moodHits = countMatches(context.moodTags, pattern.moodTags);
        const int regionHits = countMatches(context.regionTags, pattern.regionTags);
        const int functionHits = countMatches(context.functionTags, pattern.functionFit);
        const int formHits = countMatches(context.formTags, pattern.formFit);
        const int familyFit = scoreFamilyFit(context, pattern);
        const int formFit = scoreFormDynamics(context, pattern);
        const int grooveFit = scoreGrooveFit(context, context.bpm, pattern);
        const int complexityFit = scoreComplexityFit(context, pattern);
        const int textureFit = scoreTextureFit(context, pattern);

        score += familyFit;
        score += styleHits * 18;
        score += moodHits * 12;
        score += regionHits * 12;
        score += functionHits * 12;
        score += formFit;
        score += grooveFit;
        score += complexityFit;
        score += textureFit;
        score += scoreDensity(settings.density, pattern.density);
        score += scoreTempo(context.bpm, pattern);

        if (!settings.smartMode && settings.preferredPatternId == pattern.id)
        {
            score += 80;
            reasons.add("preferred");
        }

        if (!settings.smartMode && pattern.direction == settings.manualDirection)
            score += 30;

        if (settings.includeBassAnchor && pattern.bassAnchored)
            score += 8;

        if (settings.includePedal && pattern.pedalFriendly)
            score += 5;

        score -= juce::jmax(0, pattern.complexity - 3) * 4;

        // ========================================================================
    // 核心新增：基于预设名称（Preset Name）相似度的终极加分护盾
    // ========================================================================
    if (context.presetName.isNotEmpty())
    {
        const auto pNameOnly = context.presetName.toLowerCase();
        const auto pName = (context.presetName + " " + context.progressionName + " " + context.presetDescription).toLowerCase();
        const auto patId = pattern.id.toLowerCase();
        const auto patCN = pattern.nameCN.toLowerCase();
        const auto patEN = pattern.nameEN.toLowerCase();

        int nameMatchBonus = 0;

        // ========================================================================
        // 核心追加：通用核心底座（百搭基础款）基础安全垫保底机制
        // ========================================================================

        // 如果范式属于 gen_univ 系列（绝对安全的百搭通用底座）
        if (pattern.id.startsWith("gen_univ_"))
        {
            // 1. 基础保底分：确保它们天然获得一个稳定的中游分数，不容易被边缘偏门织体挤掉
            score += 25;

            // 2. 检查曲式段落的匹配（Verse/Chorus），如果段落对上了，再加分
            if (context.formTags.contains(pattern.formFit[0], true))
            {
                score += 15;
            }

            // 3. 检查大调/小调的宏观和声契合度
            if (context.scaleType == ScaleType::Major && pattern.functionFit.contains("T", true))
            {
                score += 10; // 大调契合
            }
            else if (context.scaleType == ScaleType::NaturalMinor && pattern.functionFit.contains("t", true))
            {
                score += 10; // 小调契合
            }
        }

        // 1. 【王道盲区狙击】日系、二次元、ACG、王道、黑田
        if (pName.contains(cn(u8"王道")) || pName.contains(cn(u8"黑田")) || pName.contains(cn(u8"日系")) || pName.contains("j-pop") || pName.contains("jpop") || pName.contains("anime"))
        {
            if (patId.contains("jpop") || patId.contains("anime") || pattern.regionTags.contains("japanese", true)
                || patCN.contains(cn(u8"二次元")) || patCN.contains(cn(u8"王道")))
                nameMatchBonus += 75; // 强行拉高ACG热血切分
        }

        // 1b. 【K-Pop精确拦截】韩系、副歌爆发、半音过渡
        if (pName.contains("k-pop") || pName.contains("kpop") || pName.contains(cn(u8"韩系")))
        {
            if (patId.contains("kpop") || pattern.regionTags.contains("korean", true))
                nameMatchBonus += 85;
            else if (pattern.familyId == "SyncopatedElectronicFamily" || pattern.familyId == "EDMTranceFamily")
                nameMatchBonus += 45;
        }

        // 2. 【间谍/大片盲区狙击】007、间谍、黑暗骑士、蝙蝠侠、汉斯
        if (pName.contains("007") || pName.contains(cn(u8"间谍")) || pName.contains(cn(u8"黑暗骑士")) || pName.contains(cn(u8"蝙蝠侠")) || pName.contains(cn(u8"汉斯")))
        {
            if (patId.contains("cinematic_dark") || patId.contains("tritone") || patCN.contains(cn(u8"半音")))
                nameMatchBonus += 60; // 专属暗黑摩擦织体一秒上位
        }

        // 3. 【周董风格狙击】周杰伦、周氏、十一月、晴天、R&B
        if (pName.contains(cn(u8"周杰伦")) || pName.contains(cn(u8"周氏")) || pName.contains(cn(u8"十一月")) || pName.contains("r&b") || pName.contains("rnb"))
        {
            if (pattern.familyId == "JayStyleSyncopationFamily" || patId.contains("jay") || patId.contains("rnb"))
                nameMatchBonus += 55; 
        }

        // 3b. 【摇滚 / 金属 / 高燃电吉他】直推强节奏家族
        if (pName.contains(cn(u8"摇滚")) || pName.contains("rock") || pName.contains(cn(u8"电吉他"))
            || pName.contains(cn(u8"金属")) || pName.contains("metal"))
        {
            if (patId.contains("rock") || patId.contains("metal") || pattern.styleTags.contains("rock", true)
                || pattern.familyId == "MarchPulseFamily" || pattern.familyId == "HybridEpicStringFamily")
                nameMatchBonus += 70;
        }

        // 4. 【戏曲国风精确打击】黄梅戏、女驸马、戏曲
        if (pName.contains(cn(u8"黄梅戏")) || pName.contains(cn(u8"女驸马")))
        {
            if (pattern.familyId == "ChineseOperaOrnamentFamily" || patId.contains("huangmei") || patId.contains("opera"))
                nameMatchBonus += 65;
        }

        // 5. 【民谣特异性保护】特拉维斯、指弹、校园民谣、白衣飘飘
        if (pName.contains(cn(u8"特拉维斯")) || pName.contains(cn(u8"指弹")) || pName.contains(cn(u8"校园民谣")) || pName.contains(cn(u8"民谣")))
        {
            if (pattern.familyId == "GuitarPickingFamily" || pattern.familyId == "CampusFolkFamily" || patId.contains("picking") || patId.contains("folk"))
                nameMatchBonus += 45;
        }

        // 6. 【万能兜底模糊匹配】如果预设名字和琶音的中文名或ID有任何字符串重叠（比如都写了“史诗”或“加勒比”）
        if (pNameOnly.length() >= 2) // 避免单字误伤
        {
            // 比如预设叫“加勒比海盗”，琶音叫“加勒比战役律动”，直接命中
            if (patCN.contains(pNameOnly) || pNameOnly.contains(patCN) || patEN.contains(pNameOnly) || pNameOnly.contains(patEN))
            {
                nameMatchBonus += 40; 
            }
        }

        // 在您之前写的【基于预设名称相似度的加分机制】中追加这几个精准命中点：

        // 7. 【加勒比海盗 / 史诗电影精确打击】
        if (pName.contains(cn(u8"加勒比")) || pName.contains(cn(u8"海盗")) || pName.contains(cn(u8"史诗战役")))
        {
            if (patId.contains("pirates") || patId.contains("epic_march"))
                nameMatchBonus += 70; // 强行拉高 12/8 奔腾律动！
        }

        // 8. 【卡农进行精确打击】
        if (pName.contains(cn(u8"卡农")) || pName.contains(cn(u8"canon")))
        {
            if (patId.contains("canon") || patCN.contains(cn(u8"卡农")))
                nameMatchBonus += 60; // 黄金流线钢琴一秒上位
        }

        // 9. 【金属 / 极端重型音程打击】
        if (pName.contains(cn(u8"金属")) || pName.contains("metal") || pName.contains("tritone") || pName.contains(cn(u8"减和弦")) || pName.contains(cn(u8"半音")))
        {
            if (patId.contains("metal") || patId.contains("tritone") || patId.contains("dark") || patId.contains("phrygian") || patId.contains("locrian"))
                nameMatchBonus += 65; // 低音锯齿闷音强力输出
        }

        // 10. 【大漠 / 西北苍凉风格打击】
        if (pName.contains(cn(u8"大漠")) || pName.contains(cn(u8"西北")) || pName.contains(cn(u8"边关")))
        {
            if (patId.contains("gansu") || patCN.contains(cn(u8"大漠")))
                nameMatchBonus += 55; // 触发空灵五度留白织体
        }

        // ========================================================================
        // 核心追加：针对曲式与作曲技法（前奏/副歌/模进等）名称深度加分拦截
        // ========================================================================

        // 11. 前奏 / 引入精准拦截
        if (pName.contains(cn(u8"前奏")) || pName.contains("intro") || pName.contains(cn(u8"引入")))
        {
            if (patId.contains("form_intro") || pattern.formFit.contains("intro", true))
                nameMatchBonus += 55; // 保送空间长音氛围一秒置顶
        }

        // 12. 副歌 / 高潮爆发精确拦截
        if (pName.contains(cn(u8"副歌")) || pName.contains("chorus") || pName.contains(cn(u8"高潮")) || pName.contains(cn(u8"巅峰")) || pName.contains(cn(u8"爆发")))
        {
            if (patId.contains("form_chorus") || patId.contains("kpop_gate") || pattern.familyId == "CPopChorusDriveFamily")
                nameMatchBonus += 60; // 强行拉高全声部黄金八度推动织体
        }

        // 13. 尾奏 / 落幕 / 终结精准拦截
        if (pName.contains(cn(u8"尾奏")) || pName.contains("outro") || pName.contains(cn(u8"落幕")) || pName.contains(cn(u8"终止")))
        {
            if (patId.contains("form_outro") || patId.contains("cadence"))
                nameMatchBonus += 65; // 触发渐慢自由散板或正格铁血阻截
        }

        // 14. 模进 / 级进 / 阶梯作战技法拦截
        if (pName.contains(cn(u8"模进")) || pName.contains("sequence") || pName.contains(cn(u8"级进")) || pName.contains(cn(u8"过渡")))
        {
            if (patId.contains("tech_sequence") || patId.contains("prechorus") || patId.contains("bridge")
                || patCN.contains(cn(u8"模进")) || pattern.formFit.contains("prechorus", true) || pattern.formFit.contains("pre-chorus", true))
                nameMatchBonus += 50; // 强制激活严格阶梯音程爬行律动
        }

        // 15. 循环进行 / 无穷动 / 莫比乌斯环拦截
        if (pName.contains(cn(u8"循环")) || pName.contains("loop") || pName.contains(cn(u8"万能")))
        {
            if (patId.contains("tech_loop") || patCN.contains(cn(u8"循环")))
                nameMatchBonus += 45;
        }

        // ========================================================================
        // 核心追加：针对情感色彩（伤感/治愈/愤怒/史诗等）关键字深度加分拦截
        // ========================================================================

        // 16. 【伤感 / 忧郁 / 悲伤】情感精准拦截
        if (pName.contains(cn(u8"伤感")) || pName.contains(cn(u8"忧郁")) || pName.contains(cn(u8"悲伤")) || pName.contains(cn(u8"情歌")))
        {
            if (patId.contains("melancholy") || pattern.moodTags.contains(cn(u8"伤感"), true)
                || pattern.moodTags.contains("sad", true) || pattern.moodTags.contains("nostalgic", true))
                nameMatchBonus += 55; // 瞬间触发雨滴滑落低连线，哀而不伤
        }

        // 17. 【治愈 / 温暖 / 纯净】情感精准拦截
        if (pName.contains(cn(u8"治愈")) || pName.contains(cn(u8"温暖")) || pName.contains(cn(u8"晴天")) || pName.contains(cn(u8"舒适")))
        {
            if (patId.contains("healing") || pattern.moodTags.contains(cn(u8"治愈"), true)
                || pattern.moodTags.contains("warm", true) || pattern.moodTags.contains("calm", true))
                nameMatchBonus += 50; // 星光摇篮波浪线平滑推出
        }

        // 18. 【欢快 / 快乐 / 元气 / 阳光】情感精准拦截
        if (pName.contains(cn(u8"欢快")) || pName.contains(cn(u8"快乐")) || pName.contains(cn(u8"元气")) || pName.contains(cn(u8"游乐园")))
        {
            if (patId.contains("sunshine") || pattern.moodTags.contains(cn(u8"欢快"), true)
                || pattern.moodTags.contains("happy", true) || pattern.moodTags.contains("energetic", true))
                nameMatchBonus += 55; // 跃动切分阳光一秒暴走
        }

        // 19. 【孤独 / 寂寞 / 荒凉 / 空灵】情感精准拦截
        if (pName.contains(cn(u8"孤独")) || pName.contains(cn(u8"寂寞")) || pName.contains(cn(u8"孤身")) || pName.contains(cn(u8"荒凉")))
        {
            if (patId.contains("lonely") || pattern.moodTags.contains(cn(u8"孤独"), true))
                nameMatchBonus += 60; // 强制抽离三音，抛出冷色空心双音
        }

        // 20. 【愤怒 / 狂躁 / 咆哮 / 金属】情感精准拦截
        if (pName.contains(cn(u8"愤怒")) || pName.contains(cn(u8"狂躁")) || pName.contains(cn(u8"咆哮")) || pName.contains(cn(u8"宣泄")))
        {
            if (patId.contains("angry") || pattern.moodTags.contains(cn(u8"愤怒"), true)
                || pattern.moodTags.contains("energetic", true))
                nameMatchBonus += 65; // 铁血重锤锯齿风暴疯狂砸下
        }

        // 21. 【迷幻 / 微醺 / 梦幻 / 霓虹】情感精准拦截
        if (pName.contains(cn(u8"迷幻")) || pName.contains(cn(u8"微醺")) || pName.contains(cn(u8"霓虹")) || pName.contains(cn(u8"太空")))
        {
            if (patId.contains("psychedelic") || pattern.moodTags.contains(cn(u8"迷幻"), true))
                nameMatchBonus += 50; // 流光九和弦带来摇摆慵懒的高级感
        }

        // 22. 【史诗 / 大气 / 壮阔 / 震撼】情感精准拦截
        if (pName.contains(cn(u8"史诗")) || pName.contains(cn(u8"壮阔")) || pName.contains(cn(u8"震撼")) || pName.contains(cn(u8"巨浪")))
        {
            if (patId.contains("epic_grand") || pattern.moodTags.contains(cn(u8"史诗"), true)
                || pattern.moodTags.contains("epic", true))
                nameMatchBonus += 60; // 唤醒全景视听山海巨浪
        }

        // ========================================================================
        // 核心追加：针对调式色彩（Dorian/Phrygian/和声小调等）关键字深度加分拦截
        // ========================================================================

        // 23. Dorian 多利亚调式保送
        if (pName.contains("dorian") || pName.contains(cn(u8"多利亚")))
        {
            if (patId.contains("dorian") || pattern.styleTags.contains("Dorian", true))
                nameMatchBonus += 65; // 瞬间亮起深夜大六度流光
        }

        // 24. Phrygian 弗里吉亚调式保送
        if (pName.contains("phrygian") || pName.contains(cn(u8"弗里吉亚")))
        {
            if (patId.contains("phrygian") || pattern.styleTags.contains("Phrygian", true))
                nameMatchBonus += 65; // 西班牙弗拉门戈异域半音狂暴压迫
        }

        // 25. Lydian 利底亚太空悬浮调式保送
        if (pName.contains("lydian") || pName.contains(cn(u8"利底亚")))
        {
            if (patId.contains("lydian") || pattern.styleTags.contains("Lydian", true))
                nameMatchBonus += 65; // 抛出增四度太空科幻悬浮织体
        }

        // 26. Mixolydian 混合利底亚复古摇滚保送
        if (pName.contains("mixolydian") || pName.contains(cn(u8"混合利底亚")))
        {
            if (patId.contains("mixo") || pattern.styleTags.contains("Mixolydian", true))
                nameMatchBonus += 60; // 释放叛逆洒脱的小七度苏格兰民谣/老摇滚律动
        }

        // 27. Locrian 洛克里亚深海极渊保送
        if (pName.contains("locrian") || pName.contains(cn(u8"洛克里亚")))
        {
            if (patId.contains("locrian") || pattern.styleTags.contains("Locrian", true))
                nameMatchBonus += 70; // 锁定减五度极度窒息不安稳长音
        }

        // 28. 和声小调交响史诗保送
        if (pName.contains(cn(u8"和声小调")) || pName.contains(cn(u8"和声小")))
        {
            if (patId.contains("harmonic_minor") || pattern.styleTags.contains(cn(u8"和声小调"), true)
                || patId.contains("phrygian") || patId.contains("locrian") || patId.contains("metal"))
                nameMatchBonus += 75; // 极速触发新古典大七度巴洛克华彩级进
        }

        // 29. 传统五声/七声角调式保送
        if (pName.contains(cn(u8"角调")) || pName.contains(cn(u8"角调式")))
        {
            if (patId.contains("chinese_jue") || pattern.styleTags.contains(cn(u8"角调"), true))
                nameMatchBonus += 55; // 顺滑流出江南春水墨线
        }


        // ========================================================================
        // 核心追加：针对地域民族、特殊织体与和声张力的深度加分拦截
        // ========================================================================

        // 30. 【蒙古草原 / 马头琴长调精确打击】
        if (pName.contains(cn(u8"蒙古")) || pName.contains(cn(u8"草原")) || pName.contains(cn(u8"马头琴")))
        {
            if (patId.contains("mongolian") || pattern.regionTags.contains(cn(u8"蒙古"), true))
                nameMatchBonus += 70; // 强制保送长调马蹄疾织体
        }

        // 31. 【台湾校园民谣 / 闽南语经典精细拦截】
        if (pName.contains(cn(u8"民谣")) || pName.contains(cn(u8"台湾")) || pName.contains(cn(u8"闽南")) || pName.contains(cn(u8"东北")))
        {
            if (patId.contains("taiwan_folk") || patId.contains("travis") || patId.contains("folk_pick")
                || patId.contains("campus_folk") || pattern.familyId == "CampusFolkFamily")
                nameMatchBonus += 80; // 标准民谣吉他指弹一秒上位
        }

        // 34. 【军乐/进行曲】解放军、军乐、行进类预设
        if (pName.contains(cn(u8"解放军")) || pName.contains(cn(u8"进行曲")) || pName.contains(cn(u8"军乐")))
        {
            if (patId.contains("military") || patId.contains("march") || pattern.familyId == "MarchPulseFamily")
                nameMatchBonus += 90;
        }

        // 35. 【东乡族/古尔邦】回族、东乡族、节庆小调庄严
        if (pName.contains(cn(u8"东乡")) || pName.contains(cn(u8"古尔邦")) || pName.contains(cn(u8"回族")))
        {
            if (pattern.familyId == "MaqamOrnamentFamily" || patId.contains("maqam") || patId.contains("muqam")
                || patId.contains("persian") || patId.contains("oud") || patId.contains("dombra"))
                nameMatchBonus += 80;
        }

        // 32. 【现代摇摆律动 / City-Pop 精准拦截】
        if (pName.contains("city") || (pName.contains("pop") && (pName.contains(cn(u8"摇摆")) || pName.contains("swing") || pName.contains("blues"))))
        {
            if (patId.contains("swing_city"))
                nameMatchBonus += 55; // 启动 2:1 前长后短微醺拖拍
        }

        // 33. 【古筝流水 / 快速扫弦滚奏拦截】
        if (pName.contains(cn(u8"古筝")) || pName.contains(cn(u8"流水")) || pName.contains(cn(u8"新世纪")) || pName.contains(cn(u8"琵琶")))
        {
            if (patId.contains("sweep_cascade") || pattern.familyId == "GuzhengRollFamily")
                nameMatchBonus += 60; // 激活高内聚音符极速扫弦
        }

        // ========================================================================
        // 核心追加：针对氛围、长音铺底、电影环境配乐关键字深度加分拦截
        // ========================================================================

        // 34. 【前奏 / 尾奏 / 氛围 / 铺底】曲式环境精准拦截
        if (pName.contains(cn(u8"前奏")) || pName.contains("intro") || pName.contains(cn(u8"尾奏")) || pName.contains("outro") 
            || pName.contains(cn(u8"氛围")) || pName.contains("ambient") || pName.contains("pad") || pName.contains(cn(u8"铺底")))
        {
            if (pattern.id.startsWith("amb_") || pattern.familyId == "CinematicStringPadFamily" || pattern.familyId == "GuqinSparseFamily")
            {
                nameMatchBonus += 75; // 绝对高分轰开通道，确保长音在 Intro/Outro 进入 Top 3 甚至霸榜
            }
        }

        // 35. 【太空 / 极光 / 寒冷 / 旷野 / 漂流】极端环境保送
        if (pName.contains(cn(u8"太空")) || pName.contains(cn(u8"极光")) || pName.contains(cn(u8"旷野")) 
            || pName.contains(cn(u8"深海")) || pName.contains(cn(u8"冷")))
        {
            if (pattern.id.startsWith("amb_ice") || pattern.id.startsWith("amb_aurora") || pattern.id.startsWith("amb_deep"))
                nameMatchBonus += 70; // 精准触发冰晶、极光、深海等高级折射长音
        }

        // 36. 【禅意 / 虚空 / 空灵 / 残音】国风意境拦截
        if (pName.contains(cn(u8"禅")) || pName.contains(cn(u8"空谷")) || pName.contains(cn(u8"留白")) || pName.contains(cn(u8"隐居")))
        {
            if (pattern.id == "amb_zen_void_144" || pattern.id == "mode_chinese_jue_126")
                nameMatchBonus += 65; // 江南烟雨或空谷禅意瞬间上位
        }

        // 37. 【大片 / 史诗长音 / 宏大圣咏】影院级巨墙拦截
        if (pName.contains(cn(u8"史诗")) || pName.contains(cn(u8"宏大")) || pName.contains(cn(u8"全景")) || pName.contains(cn(u8"巨墙")))
        {
            if (pattern.id == "amb_epic_wall_146" || pattern.familyId == "HybridEpicStringFamily")
                score += 50; // 与单音史诗跑音形成互补，平铺出全景和声墙
        }

        // ========================================================================
        // 核心追加：针对影视科幻、人工智能、信息时代关键字深度加分拦截
        // ========================================================================

        // 38. 【人工智能 / AI / 神经网络 / 大数据 / 代码数字流】拦截
        if (pName.contains("ai") || pName.contains("AI") || pName.contains(cn(u8"智能")) 
            || pName.contains(cn(u8"网络")) || pName.contains(cn(u8"数据")) || pName.contains(cn(u8"信息")))
        {
            if (pattern.id == "scifi_neural_net_150" || pattern.id == "scifi_fiber_matrix_153")
                nameMatchBonus += 75; // 强力唤醒量子放电点阵与光纤级进线
        }

        // 39. 【科幻 / 星际 / 宇宙 / 漂流 / 引力 / 悬浮】影视大片拦截
        if (pName.contains(cn(u8"科幻")) || pName.contains(cn(u8"星际")) || pName.contains(cn(u8"宇宙")) 
            || pName.contains(cn(u8"黑洞")) || pName.contains("scifi") || pName.contains("Sci-Fi"))
        {
            if (pattern.id.startsWith("scifi_gravity") || pattern.id == "scifi_quantum_155" || pattern.id == "mode_lydian_space_122")
                nameMatchBonus += 70; // 离调增四度悬浮与小二度双音残影就位
        }

        // 40. 【赛博朋克 / 黑客 / 脉冲 / 碎音 / Glitch / 霓虹】次世代科技拦截
        if (pName.contains(cn(u8"赛博")) || pName.contains(cn(u8"朋克")) || pName.contains(cn(u8"黑客")) 
            || pName.contains("cyber") || pName.contains("Cyber") || pName.contains("glitch"))
        {
            if (pattern.id == "scifi_cyber_glitch_152" || pattern.id == "amb_cyber_pulse_143")
                nameMatchBonus += 70; // 启动非对称机械脉冲碎音
        }

        // 41. 【奇点 / 坍缩 / 终局 / 毁灭 / 工业机械自动化】极限硬核科技拦截
        if (pName.contains(cn(u8"奇点")) || pName.contains(cn(u8"坍缩")) || pName.contains(cn(u8"震撼")) 
            || pName.contains(cn(u8"机械")) || pName.contains(cn(u8"自动化")))
        {
            if (pattern.id == "scifi_singularity_156" || pattern.id == "scifi_clockwork_154")
                nameMatchBonus += 65; // 触发齿轮式咬合大跳或史诗级黑洞下行瀑布
        }

        // ========================================================================
        // 核心追加：针对进阶影视科幻、次世代黑客、量子理论关键字深度加分拦截
        // ========================================================================

        // 42. 【极简主义 / 相位交错 / 镜像 / 平行时空】高级艺术感科幻拦截
        if (pName.contains(cn(u8"极简")) || pName.contains(cn(u8"镜像")) || pName.contains(cn(u8"多维")) 
            || pName.contains(cn(u8"空间")) || pName.contains("phase") || pName.contains("Richter"))
        {
            if (pattern.id == "scifi_mirror_160" || pattern.id == "scifi_quantum_155")
                nameMatchBonus += 75; // 唤醒3+3+2非对称高空悬浮相位差
        }

        // 43. 【黑客 / 代码雨 / 危机 / 矩阵 / 反乌托邦】硬核赛博危机拦截
        if (pName.contains(cn(u8"黑客")) || pName.contains(cn(u8"代码")) || pName.contains(cn(u8"矩阵")) 
            || pName.contains(cn(u8"网络战")) || pName.contains("matrix") || pName.contains("Matrix"))
        {
            if (pattern.id == "scifi_cyber_atk_161" || pattern.id == "scifi_rain_163")
                nameMatchBonus += 75; // 二进制指令疯狂爆破下坠
        }

        // 44. 【量子计算机 / 叠加态 / 粒子 / 科学家 / 实验室】拦截
        if (pName.contains(cn(u8"量子")) || pName.contains(cn(u8"粒子")) || pName.contains(cn(u8"微观")) 
            || pName.contains(cn(u8"控制论")) || pName.contains("quantum") || pName.contains("Quantum"))
        {
            if (pattern.id == "scifi_quantum_162" || pattern.id == "scifi_neural_net_150")
                nameMatchBonus += 70; // 启动非线性混合时值叠加态碎音
        }

        // 45. 【星际穿越 / 虫洞 / 量子跃迁 / 奇点 / 引擎 / 飞船高潮】极限史诗科技拦截
        if (pName.contains(cn(u8"星际")) || pName.contains(cn(u8"跃迁")) || pName.contains(cn(u8"黑洞")) 
            || pName.contains(cn(u8"终局")) || pName.contains("interstellar") || pName.contains("Interstellar"))
        {
            if (pattern.id == "scifi_warp_166" || pattern.id == "scifi_dark_165" || pattern.id == "scifi_singularity_156")
                nameMatchBonus += 70; // 强行拉满4拍螺旋上升粒子引擎线或低空暗物质压迫墙
        }

        // ========================================================================
        // 核心追加：针对媒体、新闻报道、信息传播关键字深度加分拦截
        // ========================================================================

        // 46. 【突发新闻 / 紧急播报 / 现场快讯 / 时钟脉冲电报】权威拦截
        if (pName.contains(cn(u8"新闻")) || pName.contains(cn(u8"播报")) || pName.contains(cn(u8"突发")) 
            || pName.contains("news") || pName.contains("News") || pName.contains("breaking"))
        {
            if (pattern.id == "media_breaking_170" || pattern.id == "media_typewriter_175")
                nameMatchBonus += 75; // 瞬间保送高频电报时钟或流动快讯线
        }

        // 47. 【社会纪实 / 深度调查 / 纪录片 / 人物专题】冷静客观拦截
        if (pName.contains(cn(u8"纪实")) || pName.contains(cn(u8"调查")) || pName.contains(cn(u8"纪录片")) 
            || pName.contains(cn(u8"访谈")) || pName.contains("doc") || pName.contains("Documentary"))
        {
            if (pattern.id == "media_doc_invest_171" || pattern.id == "media_interview_173")
                nameMatchBonus += 75; // 激理性的九音慢跑线或高端留白对话体上位
        }

        // 48. 【财经周刊 / 全球化 / 科技发布会 / 股市K线数据】高端商业拦截
        if (pName.contains(cn(u8"财经")) || pName.contains(cn(u8"金融")) || pName.contains(cn(u8"发布会")) 
            || pName.contains(cn(u8"论坛")) || pName.contains("finance") || pName.contains("corporate"))
        {
            if (pattern.id == "media_finance_172" || pattern.id == "scifi_fiber_matrix_153")
                nameMatchBonus += 70; // 启动精密图表闪烁开放式华彩
        }

        // 49. 【国家宏大开场 / 进行曲 / 庄严声明 / 时代号角】电台发布拦截
        if (pName.contains(cn(u8"联播")) || pName.contains(cn(u8"庄严")) || pName.contains(cn(u8"宣言")) 
            || pName.contains(cn(u8"地缘")) || pName.contains(cn(u8"博弈")))
        {
            if (pattern.id == "media_anthem_174" || pattern.id == "media_geopol_176")
                nameMatchBonus += 65; // 唤醒八度同击震撼大柱式或地缘冲突下行暗流线
        }

        // ========================================================================
        // 核心追加：针对自媒体、ACG高燃游戏、魔法童话、企业高端、历史文旅深度加分拦截
        // ========================================================================

        // 50. 【短视频 / Vlog / 阳光 / 生活 / 治愈 / 松弛】日常自媒体拦截
        if (pName.contains("vlog") || pName.contains("Vlog") || pName.contains(cn(u8"生活")) 
            || pName.contains(cn(u8"日常")) || pName.contains(cn(u8"阳光")) || pName.contains(cn(u8"松弛")))
        {
            if (pattern.id == "media_vlog_180" || pattern.id == "gen_univ_sync_104")
                nameMatchBonus += 75; // 抛出极具生活亲和力的律动
        }

        // 51. 【网易云 / EMO / 深夜 / 抑郁 / 孤独 / 独自一人的叹息】微观心理情绪拦截
        if (pName.contains("emo") || pName.contains("EMO") || pName.contains(cn(u8"深夜")) 
            || pName.contains(cn(u8"孤独")) || pName.contains(cn(u8"抑郁")))
        {
            if (pattern.id == "media_emo_181" || pattern.id == "amb_ice_crystal_141")
                nameMatchBonus += 75; // 强制保送极简二度挂留摩擦叹息线
        }

        // 52. 【ACG / 战斗 / 高燃 / 热血 / 二次元 / 动漫 / 动作爆裂】高能竞技拦截
        if (pName.contains("acg") || pName.contains("ACG") || pName.contains(cn(u8"热血")) 
            || pName.contains(cn(u8"高燃")) || pName.contains(cn(u8"动漫")) || pName.contains(cn(u8"战斗")))
        {
            if (pattern.id == "game_acg_burn_190" || pattern.id == "scifi_singularity_156")
                nameMatchBonus += 75; // 瞬间激活二次元16分极限爆发切分
        }

        // 53. 【红白机 / 8Bit / 像素 / 街机 / 怀旧 / 复古电子】FC时代拦截
        if (pName.contains("8bit") || pName.contains("8-Bit") || pName.contains(cn(u8"像素")) 
            || pName.contains(cn(u8"复古")) || pName.contains(cn(u8"红白机")))
        {
            if (pattern.id == "game_pixel_191" || pattern.id == "scifi_clockwork_154")
                nameMatchBonus += 70; // 纯四五度无三音方波脉冲上位
        }

        // 54. 【迪士尼 / 魔法 / 八音盒 / 童话 / 璀璨 / 星光金粉】梦幻拦截
        if (pName.contains(cn(u8"魔法")) || pName.contains(cn(u8"童话")) || pName.contains(cn(u8"八音盒")) 
            || pName.contains(cn(u8"梦幻")) || pName.contains("magic") || pName.contains("fairy"))
        {
            if (pattern.id == "child_magic_200" || pattern.id == "amb_aurora_glow_142")
                nameMatchBonus += 75; // 启动极高区璀璨晶莹倒卷下行线
        }

        // 55. 【旋转木马 / 华尔兹 / 三拍子 / 圆舞曲 / 温馨童谣】拦截
        if (pName.contains(cn(u8"木马")) || pName.contains(cn(u8"圆舞曲")) || pName.contains(cn(u8"华尔兹")) 
            || pName.contains("waltz") || pName.contains("Waltz") || pName.contains("3/4"))
        {
            if (pattern.id == "child_waltz_201")
                nameMatchBonus += 80; // 稳固的嘭恰恰浪漫圆舞曲上位
        }

        // 56. 【企业宣传片 / 高端形象 / 发布会 / 车企大片 / 格局未来】精英资本拦截
        if (pName.contains(cn(u8"企业")) || pName.contains(cn(u8"宣传片")) || pName.contains(cn(u8"高端")) 
            || pName.contains(cn(u8"豪车")) || pName.contains("corporate") || pName.contains("premium"))
        {
            if (pattern.id == "corp_luxury_210" || pattern.id == "media_finance_172")
                nameMatchBonus += 75; // 触发企业发布会高格局未来感流线
        }

        // 57. 【河西走廊 / 历史纪录片 / 大漠文旅 / 丝绸之路 / 沧桑华夏 / 敦煌悲壮】厚重拦截
        if (pName.contains(cn(u8"历史")) || pName.contains(cn(u8"大漠")) || pName.contains(cn(u8"悲壮")) 
            || pName.contains(cn(u8"沧桑")) || pName.contains(cn(u8"文旅")) || pName.contains(cn(u8"走廊")))
        {
            if (pattern.id == "hist_desert_220" || pattern.id == "amb_zen_void_144")
                nameMatchBonus += 75; // 一眼万年，黄土大漠重击凄美色彩线瞬间制霸
        }

        // ========================================================================
        // 核心追加：针对都市流行、时尚奢华、R&B/Neo-Soul关键字深度加分拦截
        // ========================================================================

        // 58. 【Neo-Soul / R&B / 慵懒 / 微醺 / 酒廊 / 爵士小酌】高级感拦截
        if (pName.contains("R&B") || pName.contains("soul") || pName.contains("Soul") 
            || pName.contains(cn(u8"都市")) || pName.contains(cn(u8"慵懒")) || pName.contains(cn(u8"微醺")))
        {
            if (pattern.id == "urban_soul_230" || pattern.id == "urban_lounge_233")
                nameMatchBonus += 80; // 强力唤醒Laid-back后半拍切分线或深夜酒廊小调
        }

        // 59. 【时尚秀场 / 奢华广告 / 豪车大片 / 精英大都会 / 华丽矩阵】高端拦截
        if (pName.contains(cn(u8"时尚")) || pName.contains(cn(u8"秀场")) || pName.contains(cn(u8"奢华")) 
            || pName.contains(cn(u8"高端")) || pName.contains("fashion") || pName.contains("premium"))
        {
            if (pattern.id == "urban_fashion_231" || pattern.id == "urban_premium_236")
                nameMatchBonus += 80; // 瞬间保送全音域轻奢下行瀑布或秀场开放流线
        }

        // 60. 【CityPop / 霓虹 / 复古摩登 / 东京 / 欢快蹦跳】经典律动拦截
        if (pName.contains("citypop") || pName.contains("City") || pName.contains(cn(u8"霓虹")) 
            || pName.contains(cn(u8"复古")) || pName.contains(cn(u8"跳跃")))
        {
            if (pattern.id == "urban_citypop_232" || pattern.id == "game_pixel_191")
                nameMatchBonus += 75; // 触发经典的日系八分交错打字机线
        }

        // 61. 【知性 / 独立音乐 / 轻奢 / 半音阶内声部 / 叙事】文学知性拦截
        if (pName.contains(cn(u8"知性")) || pName.contains(cn(u8"独立")) || pName.contains(cn(u8"轻奢")) 
            || pName.contains("cliche") || pName.contains("Line"))
        {
            if (pattern.id == "urban_cliche_235")
                nameMatchBonus += 75; // 唤醒极其知性的半音阶内声部平滑下滑线
        }

        // ========================================================================
        // 核心追加：针对超高级都市、格莱美变音、J-Fusion、中音突变关键字深度加分拦截
        // ========================================================================

        // 62. 【变音和弦 / 属变音 / 复杂和声 / 融合乐 / 爵士延伸】天花板拦截
        if (pName.contains(cn(u8"变音")) || pName.contains(cn(u8"融合")) || pName.contains(cn(u8"复杂")) 
            || pName.contains("Fusion") || pName.contains("alt") || pName.contains("Altered"))
        {
            if (pattern.id == "urban_friction_237" || pattern.id == "urban_soul_230")
                nameMatchBonus += 85; // 绝杀保送：启动内声部二度音程密集摩擦织体
        }

        // 63. 【借调 / 离调 / 双重离调 / 日本九十年代 / 霓虹浪潮】高级流行拦截
        if (pName.contains(cn(u8"离调")) || pName.contains(cn(u8"借调")) || pName.contains(cn(u8"日系高级")) 
            || pName.contains(cn(u8"东京")) || pName.contains("neon") || pName.contains("Neon"))
        {
            if (pattern.id == "urban_neon_238" || pattern.id == "urban_citypop_232")
                nameMatchBonus += 85; // 强行拉满 CityPop 反拍双音切分推搡律动
        }

        // 64. 【中音突变 / 奢华流体 / 全音域华彩 / 华尔街精英 / 豪车广告】大格局拦截
        if (pName.contains(cn(u8"中音")) || pName.contains(cn(u8"华彩")) || pName.contains(cn(u8"大都会")) 
            || pName.contains(cn(u8"精英")) || pName.contains("cascade") || pName.contains("Cascade"))
        {
            if (pattern.id == "urban_cascade_239" || pattern.id == "urban_premium_236")
                nameMatchBonus += 80; // 保送大都会轻奢全音域悬浮华彩瀑布
        }

        if (pattern.id.startsWith("gen_univ_"))
            reasons.add("UniversalBaseFit");

        nameMatchBonus = juce::jmin(nameMatchBonus, 95);
        score += nameMatchBonus;
        if (nameMatchBonus > 0) reasons.add("PresetNameMatch(" + juce::String(nameMatchBonus) + ")");
    }

        if (styleHits > 0) reasons.add("style");
        if (moodHits > 0) reasons.add("mood");
        if (regionHits > 0) reasons.add("region");
        if (functionHits > 0) reasons.add("function");
        if (formHits > 0 || formFit != 0) reasons.add("form");
        if (familyFit > 0) reasons.add("family");
        if (grooveFit > 0) reasons.add("groove");
        if (complexityFit > 0) reasons.add("complexity");
        if (textureFit > 0) reasons.add("texture");

        ArpeggioRankedPattern item;
        item.pattern = pattern;
        item.score = score;
        item.reason = reasons.joinIntoString(", ");
        ranked.push_back(item);
    }

    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        return a.score > b.score;
    });

    return ranked;
}
