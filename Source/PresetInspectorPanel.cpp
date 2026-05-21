//==================================================================================================
//  PresetInspectorPanel.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：预设详情面板绘制与交互实现
//        - 解析 Base64 封面（支持多种解码策略）
//        - 动态生成和弦进行符号与罗马级数
//        - 拖拽按钮布局
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "PresetInspectorPanel.h"
#include "LocalisedStrings.h"
#include "TagLocalizer.h"
#include "PitchNameHelper.h"
#include "ProgressionEngine.h"

PresetInspectorPanel::PresetInspectorPanel()
{
    setSize(300, 500);
    dragToTrackButton.setButtonText(LocalisedStrings::translate("auto_drag_progression_to_track"));
    addAndMakeVisible(dragToTrackButton);

    dragArpeggioToTrackButton.setButtonText(juce::String::fromUTF8(u8"智能分解 MIDI"));
    addAndMakeVisible(dragArpeggioToTrackButton);
}

//==============================================================================
// 设置预设条目（解析封面）
//==============================================================================
void PresetInspectorPanel::setEntry(const ProgressionEntry& entry, PitchClass t, ScaleType s)
{
    currentEntry = entry;
    tonic = t;
    scaleType = s;

    // 封面解码（多重策略）
    cachedCoverImage = juce::Image();
    if (currentEntry.coverImageBase64.isNotEmpty())
    {
        // 策略A：MemoryBlock 直接解码
        juce::MemoryBlock mb;
        mb.fromBase64Encoding(currentEntry.coverImageBase64);
        if (mb.getSize() > 0)
        {
            cachedCoverImage = juce::ImageFileFormat::loadFrom(mb.getData(), mb.getSize());
        }

        // 策略B：严格过滤 + PNG 解码
        if (!cachedCoverImage.isValid())
        {
            juce::String clean = currentEntry.coverImageBase64.trim();
            if (clean.startsWithIgnoreCase("data:image"))
                clean = clean.fromFirstOccurrenceOf(",", false, false).trim();
            clean = clean.retainCharacters(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
            if (clean.isNotEmpty())
            {
                juce::MemoryOutputStream mos;
                if (juce::Base64::convertFromBase64(mos, clean) && mos.getDataSize() > 0)
                {
                    cachedCoverImage = juce::ImageFileFormat::loadFrom(mos.getData(), mos.getDataSize());
                    if (!cachedCoverImage.isValid())
                    {
                        juce::MemoryInputStream stream(mos.getMemoryBlock(), false);
                        juce::PNGImageFormat png;
                        cachedCoverImage = png.decodeImage(stream);
                    }
                }
            }
        }
    }

    repaint();
}

//==============================================================================
// 和弦符号生成
//==============================================================================
juce::String PresetInspectorPanel::getChordSymbols() const
{
    auto parsed = ProgressionEngine::parseProgression(currentEntry.romanSequence);
    juce::String res;
    for (auto& pc : parsed)
    {
        int pitch = (static_cast<int>(tonic) +
                     ProgressionEngine::getScaleDegreeInterval(pc.scaleDegree, scaleType) +
                     pc.alteration) % 12;
        juce::String name = PitchNameHelper::getNameForPitch(static_cast<PitchClass>(pitch), true);
        if (pc.quality == "min" || pc.quality == "minor") name += "m";
        else if (pc.quality == "dom7") name += "7";
        else if (pc.quality == "maj7") name += "maj7";
        else if (pc.quality == "dim") name += "dim";
        else if (pc.quality == "aug") name += "aug";
        else if (pc.quality == "sus2") name += "sus2";
        else if (pc.quality == "sus4") name += "sus4";
        res += name + " ";
    }
    return res.trim();
}

juce::String PresetInspectorPanel::getRoman() const
{
    return currentEntry.romanSequence;
}

//==============================================================================
// 绘制
//==============================================================================
void PresetInspectorPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(15);

    // ---- 封面（方形） ----
    float artSize = juce::jmin(area.getWidth() * 0.5f, 180.0f);
    auto artArea = area.removeFromTop(artSize);
    artArea = artArea.withSizeKeepingCentre(artSize, artSize);

    if (cachedCoverImage.isValid())
    {
        g.drawImage(cachedCoverImage, artArea, juce::RectanglePlacement::centred);
    }
    else
    {
        juce::ColourGradient grad(juce::Colour(0xff9D6BFF), artArea.getTopLeft(),
                                  juce::Colour(0xff5E3BEE), artArea.getBottomRight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(artArea, 12.0f);
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(40.0f));
        g.drawText(juce::String::fromUTF8(u8"♫"), artArea, juce::Justification::centred);
    }

    area.removeFromTop(20);

    // ---- 预设名称 ----
    auto titleArea = area.removeFromTop(36);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText(currentEntry.name, titleArea, juce::Justification::centredLeft);

    // ---- 标签 ----
    g.setColour(juce::Colour(0xffB8ACD9).withAlpha(0.7f));
    g.setFont(13.0f);
    g.drawText(TagLocalizer::translateTags(currentEntry.tags), area.removeFromTop(22), juce::Justification::centredLeft);

    area.removeFromTop(16);
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawHorizontalLine(area.getY(), area.getX(), area.getRight());
    area.removeFromTop(16);

    // ---- 和弦进行标题 ----
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(13.0f);
    g.drawText(LocalisedStrings::translate("auto_chord_progression"), area.removeFromTop(22), juce::Justification::centredLeft);

    // ---- 和弦符号（第一行） ----
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText(getChordSymbols(), area.removeFromTop(36), juce::Justification::centredLeft);

    // ---- 级数（第二行） ----
    g.setColour(juce::Colour(0xffD5CFFF));
    g.setFont(juce::Font(16.0f, juce::Font::italic));
    g.drawText(getRoman(), area.removeFromTop(24), juce::Justification::centredLeft);

    area.removeFromTop(8);

    // ---- 描述 ----
    g.setColour(juce::Colour(0xffB8ACD9).withAlpha(0.8f));
    g.setFont(14.0f);
    g.drawText(currentEntry.description, area, juce::Justification::topLeft);
}

//==============================================================================
// 布局
//==============================================================================
void PresetInspectorPanel::resized()
{
    auto b = getLocalBounds().reduced(20);
    dragArpeggioToTrackButton.setBounds(b.removeFromBottom(40));
    b.removeFromBottom(8);
    dragToTrackButton.setBounds(b.removeFromBottom(40));
}
