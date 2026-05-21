//==================================================================================================
//  NegativeHarmony.h
//  项目：ChordReference (和弦调性速查器)
//  功能：负和声映射算法（纯静态工具类）
//        - 计算给定主音的负和声轴（属音上方小三度）
//        - 翻转单个音高或整个和弦的音符集合
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include "MusicTypes.h"
#include <algorithm>

class NegativeHarmony
{
public:
    // 计算负和声轴：tonic 向上纯五度 = (tonic + 7) % 12
    static int getAxis(PitchClass tonic)
    {
        return (static_cast<int>(tonic) + 7) % 12;
    }

    // 翻转单个音高（0‑11），结果保证非负
    static int flipPitch(int pitch, PitchClass tonic)
    {
        int axis = getAxis(tonic);
        int flipped = (2 * axis - pitch) % 12;
        if (flipped < 0) flipped += 12;
        return flipped;
    }

    // 翻转和弦中的全部音符，保持八度信息，结果从小到大排序
    static std::vector<int> flipChord(const std::vector<int>& notes, PitchClass tonic)
    {
        std::vector<int> result;
        for (int n : notes)
        {
            int pc = n % 12;
            if (pc < 0) pc += 12;
            int octave = n / 12;
            int flippedPc = flipPitch(pc, tonic);
            result.push_back(flippedPc + octave * 12);
        }
        std::sort(result.begin(), result.end());
        return result;
    }
};