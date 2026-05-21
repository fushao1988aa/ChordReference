//==================================================================================================
//  MiniKeyboard.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：迷你钢琴键盘绘制实现
//        - 7 白键 + 5 黑键
//        - 高亮和弦音、显示音符名称、C4 标记线
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "MiniKeyboard.h"
#include "SkinColours.h"

// 白键 MIDI 编号 (C4=60 ~ B4=71)
static const int whiteNoteMidi[7]  = { 60, 62, 64, 65, 67, 69, 71 };
// 黑键 MIDI 编号
static const int blackNoteMidi[5]  = { 61, 63, 66, 68, 70 };
// 黑键左侧的白键索引（用于定位）
static const int blackKeyLeftWhiteIndex[5] = { 0, 1, 3, 4, 5 };
static const int numWhiteKeys = 7;
static const int numBlackKeys = 5;

//==============================================================================
// 辅助：将 MIDI 音符转为带八度的音名（如 60 -> "C4"）
//==============================================================================
static juce::String getNoteNameWithOctave(int midiNote)
{
    static const char* noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    int octave = (midiNote / 12) - 1;
    return juce::String(noteNames[midiNote % 12]) + juce::String(octave);
}

//==============================================================================
// 高亮音符设置
//==============================================================================
void MiniKeyboard::setHighlightedNotes(const std::vector<int>& notes)
{
    highlightedNotes = notes;
    highlightedVelocities.fill(0);
    currentAverageVelocity = 0;
    sustainDown = false;
    repaint();
}

void MiniKeyboard::setMidiExpression(const std::vector<int>& notes,
                                     const std::vector<int>& velocities,
                                     int averageVelocity,
                                     bool sustainPedalDown)
{
    highlightedNotes.clear();
    highlightedVelocities.fill(0);

    for (int i = 0; i < static_cast<int>(notes.size()); ++i)
    {
        const int pitchClass = ((notes[static_cast<size_t>(i)] % 12) + 12) % 12;
        const int velocity = i < static_cast<int>(velocities.size())
                               ? velocities[static_cast<size_t>(i)]
                               : averageVelocity;

        highlightedNotes.push_back(pitchClass);
        highlightedVelocities[static_cast<size_t>(pitchClass)] =
            juce::jmax(highlightedVelocities[static_cast<size_t>(pitchClass)], juce::jlimit(1, 127, velocity));
    }

    std::sort(highlightedNotes.begin(), highlightedNotes.end());
    highlightedNotes.erase(std::unique(highlightedNotes.begin(), highlightedNotes.end()), highlightedNotes.end());
    currentAverageVelocity = juce::jlimit(0, 127, averageVelocity);
    sustainDown = sustainPedalDown;
    repaint();
}

//==============================================================================
// 主绘制
//==============================================================================
void MiniKeyboard::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    if (area.isEmpty()) return;

    auto& theme = getSkinTheme(currentSkin);
    const auto& kb = theme.keyboard;

    //---- 白键 ----
    for (int i = 0; i < numWhiteKeys; ++i)
    {
        int note = whiteNoteMidi[i] % 12;
        bool highlighted = std::find(highlightedNotes.begin(), highlightedNotes.end(), note)
                           != highlightedNotes.end();
        int velocity = highlightedVelocities[static_cast<size_t>(note)];
        auto bounds = getWhiteKeyBounds(i, area);
        drawWhiteKey(g, whiteNoteMidi[i], bounds, highlighted, velocity, kb);
        drawNoteLabel(g, bounds.reduced(2), getNoteNameWithOctave(whiteNoteMidi[i]), false);
    }

    //---- 黑键 ----
    for (int i = 0; i < numBlackKeys; ++i)
    {
        int note = blackNoteMidi[i] % 12;
        bool highlighted = std::find(highlightedNotes.begin(), highlightedNotes.end(), note)
                           != highlightedNotes.end();
        int velocity = highlightedVelocities[static_cast<size_t>(note)];
        auto bounds = getBlackKeyBounds(i, area);
        drawBlackKey(g, blackNoteMidi[i], bounds, highlighted, velocity, kb);
        if (bounds.getHeight() > 12.0f)
            drawNoteLabel(g, bounds.reduced(1), getNoteNameWithOctave(blackNoteMidi[i]), true);
    }

    //---- C4 标记线 ----
    auto c4Bounds = getWhiteKeyBounds(0, area);
    g.setColour(kb.c4MarkerColour);
    g.drawLine(c4Bounds.getX(), area.getY(), c4Bounds.getX(), area.getBottom(), 1.5f);

}

//==============================================================================
// 键位矩形计算
//==============================================================================
juce::Rectangle<float> MiniKeyboard::getWhiteKeyBounds(int index,
                                                        const juce::Rectangle<float>& area) const
{
    float w = area.getWidth() / (float)numWhiteKeys;
    float x = area.getX() + index * w;
    return juce::Rectangle<float>(x, area.getY(), w - 1.0f, area.getHeight());
}

juce::Rectangle<float> MiniKeyboard::getBlackKeyBounds(int index,
                                                        const juce::Rectangle<float>& area) const
{
    float whiteKeyWidth = area.getWidth() / (float)numWhiteKeys;
    float blackWidth = whiteKeyWidth * 0.6f;
    float blackHeight = area.getHeight() * 0.6f;
    int leftWhiteIndex = blackKeyLeftWhiteIndex[index];
    float leftWhiteRight = area.getX() + (leftWhiteIndex + 1) * whiteKeyWidth;
    float x = leftWhiteRight - blackWidth * 0.5f;
    return juce::Rectangle<float>(x, area.getY(), blackWidth, blackHeight);
}

//==============================================================================
// 单键绘制
//==============================================================================
void MiniKeyboard::drawWhiteKey(juce::Graphics& g, int noteNumber,
                                juce::Rectangle<float> area, bool highlighted,
                                int velocity, const SkinTheme::KeyboardTheme& kb)
{
    if (highlighted)
    {
        g.setColour(kb.whiteKeyHighlightFill);
        g.fillRect(area);
        g.setColour(kb.whiteKeyHighlightBorder);
        g.drawRect(area.reduced(1.5f), 2.0f);
    }
    else
    {
        g.setColour(kb.whiteKeyBase);
        g.fillRect(area);
        g.setColour(kb.whiteKeyFrame);
        g.drawRect(area, 0.5f);
    }
}

void MiniKeyboard::drawBlackKey(juce::Graphics& g, int noteNumber,
                                juce::Rectangle<float> area, bool highlighted,
                                int velocity, const SkinTheme::KeyboardTheme& kb)
{
    if (highlighted)
    {
        // 高亮时直接用高亮色填充整个黑键区域
        g.setColour(kb.blackKeyHighlightFill);
        g.fillRect(area);
        g.setColour(kb.blackKeyHighlightBorder);
        g.drawRect(area.reduced(1.0f), 2.0f);
    }
    else
    {
        // 未点亮时正常绘制
        g.setColour(kb.blackKeyBase);
        g.fillRect(area);
        g.setColour(kb.blackKeyFrame);
        g.drawRect(area, 0.5f);
    }
}

//==============================================================================
// 音符标签绘制
//==============================================================================
void MiniKeyboard::drawNoteLabel(juce::Graphics& g, juce::Rectangle<float> area,
                                 const juce::String& text, bool isBlackKey)
{
    float fontSize = area.getHeight() * 0.22f;
    if (fontSize < 5.5f) fontSize = 5.5f;
    if (fontSize > 14.0f) fontSize = 14.0f;
    g.setFont(juce::Font(fontSize));
    g.setColour(isBlackKey ? juce::Colours::white : juce::Colours::black);
    auto labelArea = area.withTrimmedTop(area.getHeight() * 0.55f);
    g.drawText(text, labelArea, juce::Justification::centred, false);
}
