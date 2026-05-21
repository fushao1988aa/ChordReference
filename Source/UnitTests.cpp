//==================================================================================================
//  UnitTests.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：核心音乐理论单元测试
//        - 测试音阶生成、和弦构建、TSD家族构建
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include <juce_core/juce_core.h>
#include "ScaleEngine.h"
#include "TSDBuilder.h"

class ChordReferenceUnitTests : public juce::UnitTest
{
public:
    ChordReferenceUnitTests() : juce::UnitTest("ChordReference Core Tests") {}

    void runTest() override
    {
        // 测试大三和弦构建
        beginTest("ScaleEngine::getChord - Major Triad");
        {
            auto scaleC = ScaleEngine::getScale(PitchClass::C, ScaleType::Major);
            auto chordI = ScaleEngine::getChord(scaleC, 1);
            expectEquals(chordI.intervals.size(), (size_t)3);
            expectEquals(static_cast<int>(chordI.root), 0);          // C
            expectEquals(chordI.intervals[0], 0);                    // 根音
            expectEquals(chordI.intervals[1], 4);                    // 大三度
            expectEquals(chordI.intervals[2], 7);                    // 纯五度
        }

        // 测试小三和弦构建
        beginTest("ScaleEngine::getChord - Minor Triad");
        {
            auto scaleA = ScaleEngine::getScale(PitchClass::A, ScaleType::NaturalMinor);
            auto chordI = ScaleEngine::getChord(scaleA, 1);
            expectEquals(chordI.intervals.size(), (size_t)3);
            expectEquals(static_cast<int>(chordI.root), 9);          // A
            expectEquals(chordI.intervals[0], 0);
            expectEquals(chordI.intervals[1], 3);                    // 小三度
            expectEquals(chordI.intervals[2], 7);
        }

        // 测试大七和弦构建
        beginTest("ScaleEngine::getChord - Major 7th");
        {
            auto scaleC = ScaleEngine::getScale(PitchClass::C, ScaleType::Major);
            auto chordI7 = ScaleEngine::getChord(scaleC, 1, 4);
            expectEquals(chordI7.intervals.size(), (size_t)4);
            expectEquals(chordI7.intervals[3], 11);                   // 大七音
        }

        // 测试属七和弦构建（V7）
        beginTest("ScaleEngine::getChord - Dominant 7th (V7)");
        {
            auto scaleC = ScaleEngine::getScale(PitchClass::C, ScaleType::Major);
            auto chordV7 = ScaleEngine::getChord(scaleC, 5, 4);
            // G7: G B D F → 0, 4, 7, 10
            expectEquals(chordV7.intervals[0], 0);
            expectEquals(chordV7.intervals[1], 4);
            expectEquals(chordV7.intervals[2], 7);
            expectEquals(chordV7.intervals[3], 10);
        }

        // 测试大调 TSD 家族
        beginTest("TSDBuilder::getMajor");
        {
            auto family = TSDBuilder::getMajor(PitchClass::C);
            expectEquals(static_cast<int>(family.tonic.root), 0);         // C
            expectEquals(family.tonic.name, juce::String("T"));
            expectEquals(static_cast<int>(family.subdominant.root), 5);   // F
            expectEquals(family.subdominant.name, juce::String("S"));
            expectEquals(static_cast<int>(family.dominant.root), 7);      // G
            expectEquals(family.dominant.name, juce::String("D"));
        }

        // 测试小调 TSD 家族（属和弦应含 G#）
        beginTest("TSDBuilder::getMinor");
        {
            auto family = TSDBuilder::getMinor(PitchClass::A);
            expectEquals(static_cast<int>(family.tonic.root), 9);         // A
            expectEquals(family.tonic.name, juce::String("t"));
            expectEquals(static_cast<int>(family.subdominant.root), 2);   // D
            expectEquals(family.subdominant.name, juce::String("s"));
            expectEquals(static_cast<int>(family.dominant.root), 4);      // E
            expectEquals(family.dominant.name, juce::String("D"));

            // 检查属和弦是否包含 G# (8)
            bool hasGSharp = false;
            for (int interval : family.dominant.intervals)
            {
                if ((static_cast<int>(family.dominant.root) + interval) % 12 == 8)
                    hasGSharp = true;
            }
            expect(hasGSharp);
        }

        // 可以继续添加更多测试...
    }
};

// 静态注册到 JUCE 单元测试系统
static ChordReferenceUnitTests chordReferenceUnitTests;