//==================================================================================================
//  ProgressionCard.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：预设列表项卡片绘制与交互实现
//        - 根据当前调性动态生成和弦符号与罗马级数
//        - 绘制封面缩略图（带失败兜底渐变）
//        - (U) 标记用户预设，红色加粗
//        - 右键菜单支持收藏、编辑（用户预设）、删除（用户预设）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ProgressionCard.h"
#include "LocalisedStrings.h"
#include "TagLocalizer.h"
#include "PitchNameHelper.h"
#include "ProgressionEngine.h"

ProgressionCard::ProgressionCard()
{
    shadow = juce::DropShadow(juce::Colours::black.withAlpha(0.35f), 12, {0, 4});
}

//==============================================================================
// 设置预设条目（解析封面 base64）
//==============================================================================
void ProgressionCard::setEntry(const ProgressionEntry& e, PitchClass tonic, ScaleType type)
{
    entry = e;
    currentTonic = tonic;
    currentScale = type;

    cachedCoverImage = juce::Image();
    if (entry.coverImageBase64.isNotEmpty())
    {
        // 策略A：MemoryBlock 直接解码
        juce::MemoryBlock mb;
        mb.fromBase64Encoding(entry.coverImageBase64);
        if (mb.getSize() > 0)
        {
            const void* data = mb.getData();
            size_t dataSize = mb.getSize();
            cachedCoverImage = juce::ImageFileFormat::loadFrom(data, dataSize);
        }

        // 策略B：严格过滤 + PNG 强制解码
        if (!cachedCoverImage.isValid())
        {
            juce::String clean = entry.coverImageBase64.trim();
            if (clean.startsWithIgnoreCase("data:image"))
                clean = clean.fromFirstOccurrenceOf(",", false, false).trim();
            clean = clean.retainCharacters(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");

            if (clean.isNotEmpty())
            {
                juce::MemoryOutputStream mos;
                if (juce::Base64::convertFromBase64(mos, clean) && mos.getDataSize() > 0)
                {
                    const void* data2 = mos.getData();
                    size_t dataSize2 = mos.getDataSize();
                    cachedCoverImage = juce::ImageFileFormat::loadFrom(data2, dataSize2);
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
// 选中与收藏设置
//==============================================================================
void ProgressionCard::setSelected(bool sel) { selected = sel; repaint(); }
void ProgressionCard::setFavourite(bool fav) { isFavourite = fav; repaint(); }
void ProgressionCard::setFrequent(bool freq) { isFrequent = freq; repaint(); }

//==============================================================================
// 根据当前调性生成和弦符号字符串
//==============================================================================
juce::String ProgressionCard::getChordSymbols() const
{
    auto parsed = ProgressionEngine::parseProgression(entry.romanSequence);
    juce::String res;
    for (auto& pc : parsed)
    {
        int pitch = (static_cast<int>(currentTonic) +
                     ProgressionEngine::getScaleDegreeInterval(pc.scaleDegree, currentScale) +
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

//==============================================================================
// 获取罗马级数字符串
//==============================================================================
juce::String ProgressionCard::getRoman() const { return entry.romanSequence; }

//==============================================================================
// 绘制
//==============================================================================
void ProgressionCard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);

    // 背景
    juce::Colour bg = juce::Colour(0xff1F1835);
    if (selected) bg = juce::Colour(0xff5E3BEE).withAlpha(0.55f);
    else if (isMultiSelected) bg = juce::Colour(0xff3A82F3).withAlpha(0.35f);  // 蓝色半透明标记多选
    else if (hovered) bg = juce::Colour(0xffFFFFFF).withAlpha(0.05f);
    g.setColour(bg);
    g.fillRoundedRectangle(bounds, 6.0f);

    // 选中边框
    if (selected)
    {
        g.setColour(juce::Colour(0xff9D6BFF).withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
    }

    auto area = bounds.reduced(12, 0);

    // ---- 封面缩略图 ----
    const float coverSize = 44.0f;
    auto coverArea = area.removeFromLeft(coverSize + 8.0f)
                         .reduced(0, (area.getHeight() - coverSize) * 0.5f);
    coverArea = coverArea.withSizeKeepingCentre(coverSize, coverSize);
    if (cachedCoverImage.isValid())
    {
        g.drawImage(cachedCoverImage, coverArea, juce::RectanglePlacement::centred);
    }
    else
    {
        juce::ColourGradient grad(juce::Colour(0xff9D6BFF), coverArea.getTopLeft(),
                                  juce::Colour(0xff5E3BEE), coverArea.getBottomRight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(coverArea, 6.0f);
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(22.0f);
        g.drawText(juce::String::fromUTF8(u8"♫"), coverArea.toNearestInt(), juce::Justification::centred);
    }

    // ---- 名称与标签区域 ----
    auto nameArea = area.removeFromLeft(area.getWidth() * 0.4f);
    {
        auto nameTop = nameArea.removeFromTop(nameArea.getHeight() * 0.5f); // 名称占上半
        if (entry.isUserPreset)
        {
            juce::String prefix = LocalisedStrings::translate("preset_user_marker") + " ";
            g.setColour(juce::Colour(0xffE85D75));
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            g.drawText(prefix, nameTop.removeFromLeft(juce::Font(14.0f, juce::Font::bold).getStringWidth(prefix)),
                       juce::Justification::bottomLeft);
            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(14.0f, juce::Font::plain));
            g.drawText(entry.name, nameTop, juce::Justification::bottomLeft);
        }
        else
        {
            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(14.0f, juce::Font::plain));
            g.drawText(entry.name, nameTop, juce::Justification::bottomLeft);
        }

        // ---- 标签（小字，在名称下面） ----
        if (!entry.tags.isEmpty())
        {
            g.setColour(juce::Colour(0xffB8ACD9).withAlpha(0.7f));
            g.setFont(11.0f);
            g.drawText(TagLocalizer::translateTags(entry.tags), nameArea.toNearestInt(), juce::Justification::topLeft);
        }
    }

    // ---- 和弦进行符号 ----
    auto chordArea = area.removeFromLeft(area.getWidth() * 0.6f);
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(juce::Font("Consolas", 13.0f, 0));
    g.drawText(getChordSymbols(), chordArea, juce::Justification::centred);

    // ---- 级数与收藏图标 ----
    auto rightArea = area;
    auto favArea = rightArea.removeFromRight(30);
    g.setColour(isFavourite ? juce::Colour(0xffE85D75) : juce::Colours::white.withAlpha(0.25f));
    g.setFont(16.0f);
    g.drawText(isFavourite ? juce::String::fromUTF8(u8"♥") : juce::String::fromUTF8(u8"♡"),
               favArea, juce::Justification::centredRight);

    g.setColour(juce::Colour(0xffD5CFFF).withAlpha(0.7f));
    g.setFont(13.0f);
    g.drawText(getRoman(), rightArea, juce::Justification::centredRight);
}

//==============================================================================
// 鼠标交互(右键)
//==============================================================================
void ProgressionCard::mouseUp(const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        juce::PopupMenu menu;

        // 收藏/取消收藏
        bool fav = isFavourite;
        menu.addItem(fav ? LocalisedStrings::translate("auto_remove_favorite")
                         : LocalisedStrings::translate("auto_favorite"),
                     true, false,
                     [this] { if (onFavouriteToggle) onFavouriteToggle(); });

        // 常用专属菜单
        menu.addItem(isFrequent ? LocalisedStrings::translate("auto_remove_frequent")
                                : LocalisedStrings::translate("auto_add_to_frequent"),
                     true, false,
                     [this] { if (onFrequentToggle) onFrequentToggle(); });

        // 用户预设专属操作
        if (entry.isUserPreset)
        {
            menu.addSeparator();
            if (onBuildExtraMenu)
                onBuildExtraMenu(menu);
            menu.addItem(LocalisedStrings::translate("auto_edit"), [this] { if (onEdit) onEdit(); });
            menu.addItem(LocalisedStrings::translate("auto_delete"), [this] { if (onDelete) onDelete(); });
        }

        auto screenPos = e.getScreenPosition().toInt();
        menu.showMenuAsync(juce::PopupMenu::Options()
                            .withTargetScreenArea(juce::Rectangle<int>(screenPos.x, screenPos.y, 1, 1)));
        return;
    }

    if (onClick) onClick();
}

void ProgressionCard::mouseEnter(const juce::MouseEvent&) { hovered = true; repaint(); }
void ProgressionCard::mouseExit(const juce::MouseEvent&)  { hovered = false; repaint(); }
