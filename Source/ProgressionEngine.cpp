//==================================================================================================
//  ProgressionEngine.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：罗马数字解析与和弦进行实现
//        - 支持副属和弦、斜杠低音、升降号及各种质量后缀
//        - 基于 ScaleEngine 将级数转换为实际音高
//        - 提供内置和弦公式库
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ProgressionEngine.h"
#include "ScaleEngine.h"

//==============================================================================
// 内置和弦公式（质量 -> 半音间隔）
//==============================================================================
const ProgressionEngine::ChordFormula ProgressionEngine::chordFormulas[] =
{
    { "major",   { 0, 4, 7 } },
    { "min",     { 0, 3, 7 } },
    { "dim",     { 0, 3, 6 } },
    { "aug",     { 0, 4, 8 } },
    { "dom7",    { 0, 4, 7, 10 } },
    { "maj7",    { 0, 4, 7, 11 } },
    { "min7",    { 0, 3, 7, 10 } },
    { "dim7",    { 0, 3, 6, 9 } },
    { "m7b5",    { 0, 3, 6, 10 } },
    { "sus2",    { 0, 2, 7 } },
    { "sus4",    { 0, 5, 7 } },
    { "7sus4",   { 0, 5, 7, 10 } },
    { "power",   { 0, 7 } },
    { "6",       { 0, 4, 7, 9 } },
    { "min6",    { 0, 3, 7, 9 } },
    { "9",       { 0, 4, 7, 10, 14 } },
    { "maj9",    { 0, 4, 7, 11, 14 } },
    { "min9",    { 0, 3, 7, 10, 14 } },
    { "add9",    { 0, 4, 7, 14 } },
    { "minMaj7", { 0, 3, 7, 11 } }
};

//==============================================================================
// 获取音阶级数对应的半音间隔（大调/自然小调前打底）
//==============================================================================
int ProgressionEngine::getScaleDegreeInterval(int degree, ScaleType type)
{
    static const int majorIntervals[] = { 0, 2, 4, 5, 7, 9, 11 };
    static const int minorIntervals[] = { 0, 2, 3, 5, 7, 8, 10 };

    int idx = (degree - 1) % 7;
    return (type == ScaleType::Major) ? majorIntervals[idx] : minorIntervals[idx];
}

//==============================================================================
// 获取完整七级音阶间隔
//==============================================================================
std::vector<int> ProgressionEngine::getScaleIntervals(ScaleType type)
{
    std::vector<int> intervals(7);
    for (int i = 1; i <= 7; ++i)
        intervals[i - 1] = getScaleDegreeInterval(i, type);
    return intervals;
}

//==============================================================================
// 级数 -> 绝对音高（0‑11）
//==============================================================================
int ProgressionEngine::degreeToPitch(int degree, int alteration, PitchClass tonic, ScaleType type)
{
    int tonicPitch = static_cast<int>(tonic);
    int interval = getScaleDegreeInterval(degree, type);
    return (tonicPitch + interval + alteration) % 12;
}

//==============================================================================
// 解析单个罗马数字标记
//==============================================================================
ParsedChord ProgressionEngine::parseRomanToken(const juce::String& token)
{
    ParsedChord result;
    juce::String remaining = token.trim().removeCharacters(" ");

    if (remaining.isEmpty()) return result;

    // ---- 副属和弦检测 (V/IV) ----
    int slashIndex = remaining.indexOfChar('/');
    if (slashIndex > 0)
    {
        juce::String beforeSlash = remaining.substring(0, slashIndex);
        juce::String afterSlash  = remaining.substring(slashIndex + 1);

        juce::String bassToken = afterSlash.trim();
        if (bassToken.length() >= 1)
        {
            juce::String firstChar = bassToken.substring(0, 1);
            if (firstChar.containsOnly("IViv"))
            {
                result.isSecondaryDominant = true;
                result.secondaryTarget = (firstChar == "I") ? 1
                                       : (firstChar == "V") ? 5
                                       : (firstChar == "i") ? 1
                                       : (firstChar == "v") ? 5
                                       : 1;
                remaining = beforeSlash;
            }
            else
            {
                int bassDegree = afterSlash.getIntValue();
                if (bassDegree >= 1 && bassDegree <= 7)
                    result.bassOverride = bassDegree;
                remaining = beforeSlash;
            }
        }
    }

    // ---- 升降前缀 ----
    if (remaining.startsWith("b"))
    {
        result.alteration = -1;
        remaining = remaining.substring(1);
    }
    else if (remaining.startsWith("#"))
    {
        result.alteration = +1;
        remaining = remaining.substring(1);
    }

    // ---- 级数（罗马数字） ----
    if (remaining.isEmpty()) return result;
    juce::String firstChar = remaining.substring(0, 1);
    if (firstChar == "I") result.scaleDegree = 1;
    else if (firstChar == "V") result.scaleDegree = 5;
    else if (firstChar == "i") { result.scaleDegree = 1; result.quality = "min"; }
    else if (firstChar == "v") { result.scaleDegree = 5; result.quality = "min"; }
    else return result;

    remaining = remaining.substring(1);

    // 处理多字符罗马数字（II, iii 等）
    while (remaining.isNotEmpty() && (remaining[0] == 'I' || remaining[0] == 'i'))
    {
        if (remaining[0] == 'I') result.scaleDegree++;
        else if (remaining[0] == 'i') { result.scaleDegree++; result.quality = "min"; }
        remaining = remaining.substring(1);
    }

    // ---- 质量后缀 ----
    juce::String suffix = remaining.trim();
    if (suffix.isEmpty())
    {
        // 未指定时，根据级数赋予默认质量
        if (result.quality.isEmpty())
        {
            if (result.scaleDegree == 1 || result.scaleDegree == 4 || result.scaleDegree == 5)
                result.quality = "major";
            else if (result.scaleDegree == 7)
                result.quality = "dim";
            else
                result.quality = "min";
        }
    }
    else
    {
        if (suffix == "maj7" || suffix == "M7") result.quality = "maj7";
        else if (suffix == "7") result.quality = "dom7";
        else if (suffix == "m7" || suffix == "min7") result.quality = "min7";
        else if (suffix == "dim7") result.quality = "dim7";
        else if (suffix == "m7b5") result.quality = "m7b5";
        else if (suffix == "sus2") result.quality = "sus2";
        else if (suffix == "sus4") result.quality = "sus4";
        else if (suffix == "aug" || suffix == "+") result.quality = "aug";
        else if (suffix == "dim") result.quality = "dim";
        else if (suffix == "6") result.quality = "6";
        else if (suffix == "m6" || suffix == "min6") result.quality = "min6";
        else if (suffix == "9") result.quality = "9";
        else if (suffix == "maj9" || suffix == "M9") result.quality = "maj9";
        else if (suffix == "m9" || suffix == "min9") result.quality = "min9";
        else if (suffix == "add9") result.quality = "add9";
        else if (suffix == "m" || suffix == "min") result.quality = "min";
        else if (suffix == "sus") result.quality = "sus4";
        else if (suffix == "(sus2)") result.quality = "sus2";
        else if (suffix == "(sus4)") result.quality = "sus4";
        else if (suffix == "(no3)" || suffix == "5") result.quality = "power";
        else result.quality = "major";
    }

    return result;
}

//==============================================================================
// 解析整个进行序列
//==============================================================================
juce::Array<ParsedChord> ProgressionEngine::parseProgression(const juce::String& romanSequence)
{
    juce::Array<ParsedChord> chords;
    juce::StringArray tokens;
    tokens.addTokens(romanSequence, " ", "\"");

    for (auto& token : tokens)
        if (token.isNotEmpty())
            chords.add(parseRomanToken(token));

    return chords;
}

//==============================================================================
// 将罗马序列转换为实际音高集合（每个和弦返回一组 0‑11 音符）
//==============================================================================
std::vector<std::vector<int>> ProgressionEngine::getChordNotes(
    const juce::String& romanSequence,
    PitchClass tonic,
    ScaleType type)
{
    std::vector<std::vector<int>> result;
    auto parsedChords = parseProgression(romanSequence);

    for (auto& pc : parsedChords)
    {
        std::vector<int> notes;
        int rootPitch = degreeToPitch(pc.scaleDegree, pc.alteration, tonic, type);

        // 查找对应质量公式
        const ChordFormula* formula = nullptr;
        for (auto& f : chordFormulas)
        {
            if (f.quality == pc.quality)
            {
                formula = &f;
                break;
            }
        }

        if (formula == nullptr)
        {
            notes = { rootPitch, (rootPitch + 4) % 12, (rootPitch + 7) % 12 };
        }
        else
        {
            for (int interval : formula->intervals)
                notes.push_back((rootPitch + interval) % 12);
        }

        // 斜杠低音
        if (pc.bassOverride >= 1 && pc.bassOverride <= 7)
        {
            int bassPitch = degreeToPitch(pc.bassOverride, 0, tonic, type);
            if (std::find(notes.begin(), notes.end(), bassPitch) == notes.end())
                notes.insert(notes.begin(), bassPitch);
        }

        // 副属和弦
        if (pc.isSecondaryDominant)
        {
            notes.clear();
            int targetDegree = pc.secondaryTarget;
            int targetPitch = degreeToPitch(targetDegree, 0, tonic, type);
            int secDomRoot = (targetPitch + 7) % 12;
            notes = { secDomRoot, (secDomRoot + 4) % 12, (secDomRoot + 7) % 12, (secDomRoot + 10) % 12 };
        }

        std::sort(notes.begin(), notes.end());
        notes.erase(std::unique(notes.begin(), notes.end()), notes.end());
        result.push_back(notes);
    }

    return result;
}

//==============================================================================
// 判定罗马序列是否合法
//==============================================================================
bool ProgressionEngine::isValidProgression(const juce::String& romanSequence)
{
    if (romanSequence.isEmpty()) return false;

    auto tokens = juce::StringArray::fromTokens(romanSequence, " ", "\"");
    for (auto& token : tokens)
    {
        auto parsed = parseRomanToken(token);
        if (parsed.scaleDegree < 1 || parsed.scaleDegree > 7)
            return false;
    }
    return !tokens.isEmpty();
}