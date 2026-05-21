//==================================================================================================
//  ScaleEngine.h
//  项目：ChordReference (和弦调性速查器)
//  功能：音阶与和弦构建核心算法声明
//        - 七种调式音阶生成
//        - 关系大小调转换
//        - 三度堆叠和弦（三和弦至十三和弦），支持 ChordQuality 修正
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include "MusicTypes.h"
#include <vector>

class ScaleEngine
{
public:
    //==============================================================================
    // 音阶生成：根据主音和调式返回7个音高类
    //==============================================================================
    static std::vector<PitchClass> getScale(PitchClass tonic, ScaleType type);

    //==============================================================================
    // 关系调转换
    //==============================================================================
    static PitchClass getRelativeMinor(PitchClass majorTonic);
    static PitchClass getRelativeMajor(PitchClass minorTonic);

    //==============================================================================
    // 构建和弦（三度堆叠）
    //   scale    : 音阶（7个音）
    //   degree   : 根音级数 1~7
    //   numNotes : 音符数量 3/4/5/6/7（对应三和弦至十三和弦）
    //   quality  : 可选的和弦性质修正（默认行为等价于 getChord(scale,degree,numNotes) ）
    //==============================================================================
    static Chord getChord(const std::vector<PitchClass>& scale, int degree);
    static Chord getChord(const std::vector<PitchClass>& scale, int degree, int numNotes);
    static Chord getChord(const std::vector<PitchClass>& scale, int degree, int numNotes, ChordQuality quality);
};