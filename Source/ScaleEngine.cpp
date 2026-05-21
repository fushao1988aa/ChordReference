//==================================================================================================
//  ScaleEngine.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：音阶与和弦构建实现
//        - 支持大调、自然小调及5种中古调式音阶
//        - 关系大小调转换
//        - 三度堆叠和弦（3~7个音），支持 ChordQuality 修正
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ScaleEngine.h"
#include <stdexcept>
#include <algorithm>

//==============================================================================
// 音阶生成（7种调式）
//==============================================================================
std::vector<PitchClass> ScaleEngine::getScale(PitchClass tonic, ScaleType type)
{
    static const int majorIntervals[]       = {0, 2, 4, 5, 7, 9, 11};   // Ionian
    static const int naturalMinorIntervals[] = {0, 2, 3, 5, 7, 8, 10};  // Aeolian
    static const int dorianIntervals[]       = {0, 2, 3, 5, 7, 9, 10};
    static const int phrygianIntervals[]     = {0, 1, 3, 5, 7, 8, 10};
    static const int lydianIntervals[]       = {0, 2, 4, 6, 7, 9, 11};
    static const int mixolydianIntervals[]   = {0, 2, 4, 5, 7, 9, 10};
    static const int locrianIntervals[]      = {0, 1, 3, 5, 6, 8, 10};

    const int* intervals = majorIntervals;   // 默认大调

    switch (type)
    {
        case ScaleType::Major:         intervals = majorIntervals; break;
        case ScaleType::NaturalMinor:  intervals = naturalMinorIntervals; break;
        case ScaleType::Dorian:        intervals = dorianIntervals; break;
        case ScaleType::Phrygian:      intervals = phrygianIntervals; break;
        case ScaleType::Lydian:        intervals = lydianIntervals; break;
        case ScaleType::Mixolydian:    intervals = mixolydianIntervals; break;
        case ScaleType::Locrian:       intervals = locrianIntervals; break;
    }

    int tonicInt = static_cast<int>(tonic);
    std::vector<PitchClass> scale;
    for (int i = 0; i < 7; ++i)
    {
        int pitchInt = (tonicInt + intervals[i]) % 12;
        scale.push_back(static_cast<PitchClass>(pitchInt));
    }
    return scale;
}

//==============================================================================
// 关系调转换
//==============================================================================
PitchClass ScaleEngine::getRelativeMinor(PitchClass majorTonic)
{
    auto majorScale = getScale(majorTonic, ScaleType::Major);
    return majorScale[5];   // 大调VI级 = 关系小调主音
}

PitchClass ScaleEngine::getRelativeMajor(PitchClass minorTonic)
{
    auto minorScale = getScale(minorTonic, ScaleType::NaturalMinor);
    return minorScale[2];   // 小调III级 = 关系大调主音
}

//==============================================================================
// 和弦构建（默认三和弦，degree 1~7）
//==============================================================================
Chord ScaleEngine::getChord(const std::vector<PitchClass>& scale, int degree)
{
    return getChord(scale, degree, 3);
}

//==============================================================================
// 和弦构建（三度堆叠，numNotes 3~7）
//==============================================================================
Chord ScaleEngine::getChord(const std::vector<PitchClass>& scale, int degree, int numNotes)
{
    if (degree < 1 || degree > 7 || numNotes < 2 || numNotes > 7)
        throw std::out_of_range("Invalid degree or numNotes");

    int rootInt = static_cast<int>(scale[degree - 1]);
    std::vector<int> intervals;

    for (int i = 0; i < numNotes; ++i)
    {
        int scaleIndex = (degree - 1 + i * 2) % 7;
        int octave = (degree - 1 + i * 2) / 7;
        int pitch = static_cast<int>(scale[scaleIndex]);
        int noteAbsolute = pitch + octave * 12;
        int intervalFromRoot = noteAbsolute - rootInt;
        intervals.push_back(intervalFromRoot);
    }

    return { scale[degree - 1], intervals, juce::String(), juce::String() };
}

//==============================================================================
// 和弦构建（三度堆叠 + ChordQuality 修正）
//   当前仅对三和弦（numNotes=3）和七和弦（numNotes=4）进行修正
//==============================================================================
Chord ScaleEngine::getChord(const std::vector<PitchClass>& scale, int degree, int numNotes, ChordQuality quality)
{
    Chord chord = getChord(scale, degree, numNotes);

    //---- 三和弦质量修正 ----
    if (numNotes == 3)
    {
        switch (quality)
        {
            case ChordQuality::MajorTriad:   break;               // 默认大三和弦
            case ChordQuality::MinorTriad:   chord.intervals = {0, 3, 7}; break;
            case ChordQuality::Diminished:   chord.intervals = {0, 3, 6}; break;
            case ChordQuality::Augmented:    chord.intervals = {0, 4, 8}; break;
            case ChordQuality::Sus2:         chord.intervals = {0, 2, 7}; break;
            case ChordQuality::Sus4:         chord.intervals = {0, 5, 7}; break;
            default: break;
        }
    }
    //---- 七和弦质量修正 ----
    else if (numNotes == 4)
    {
        switch (quality)
        {
            case ChordQuality::MajorTriad:   chord.intervals = {0, 4, 7, 11}; break;   // maj7
            case ChordQuality::MinorTriad:   chord.intervals = {0, 3, 7, 10}; break;   // m7
            case ChordQuality::Dominant7:    chord.intervals = {0, 4, 7, 10}; break;   // 7
            case ChordQuality::MinorMajor7:  chord.intervals = {0, 3, 7, 11}; break;   // mMaj7
            case ChordQuality::Minor7b5:     chord.intervals = {0, 3, 6, 10}; break;   // m7b5
            case ChordQuality::Diminished:   chord.intervals = {0, 3, 6, 9};  break;   // dim7
            default: break;
        }
    }

    return chord;
}