//==================================================================================================
//  ChordCard.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦卡片的绘制、交互、试听、拖拽 MIDI 的完整实现
//        - 包含四套皮肤主题的视觉样式
//        - 支持转位切换、和弦类型（3/7/9/11/13）切换、引申音选择
//        - 质量弹出菜单、节奏选择菜单
//        - 鼠标拖拽生成临时 MIDI 文件
//  注意：文件需保存为 UTF-8 with BOM，所有中文字符串使用 u8 前缀
//==================================================================================================

#include "ChordCard.h"
#include "LocalisedStrings.h"
#include "PitchNameHelper.h"
#include "MoodMapper.h"
#include "SkinColours.h"
#include "PluginProcessor.h"       // ★ 为了使用 ChordReferenceAudioProcessor 的试听功能
#include <map>

//==============================================================================
// 局部：弹出菜单自定义外观（用于质量选择、引申音子菜单等）
//==============================================================================
namespace
{
    class PopupMenuLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        PopupMenuLookAndFeel()
        {
            setDefaultSansSerifTypeface(PitchNameHelper::getHarmonyTypeface());
        }

        void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                               bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                               bool hasSubMenu, const juce::String& text,
                               const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColour) override
        {
            if (isHighlighted && !isSeparator)
                g.fillAll(juce::Colours::white.withAlpha(0.15f));
            else
                g.fillAll(juce::Colours::black.withAlpha(0.0f));

            if (isSeparator)
            {
                g.setColour(juce::Colours::white.withAlpha(0.2f));
                g.drawHorizontalLine(area.getCentreY(), area.getX() + 10.0f, area.getRight() - 10.0f);
                return;
            }

            g.setColour(textColour != nullptr ? *textColour : juce::Colours::white);
            g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(15.0f));

            auto textArea = area.reduced(12, 0);
            g.drawText(text, textArea, juce::Justification::centredLeft);

            if (hasSubMenu)
            {
                juce::Path arrow;
                arrow.addTriangle(0, 0, 8, 4, 0, 8);
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                g.fillPath(arrow, juce::AffineTransform::translation(
                    area.getRight() - 20.0f, area.getCentreY() - 4.0f));
            }
        }
    };
}

ChordCard::~ChordCard()
{
    stopTimer();
}

//==============================================================================
// 静态工具：根据和弦类型返回音符数量
//==============================================================================
int ChordCard::getNumNotesForType(ChordExtension type)
{
    switch (type)
    {
        case ChordExtension::Triad:      return 3;
        case ChordExtension::Seventh:    return 4;
        case ChordExtension::Ninth:      return 5;
        case ChordExtension::Eleventh:   return 6;
        case ChordExtension::Thirteenth: return 7;
        default:                         return 3;
    }
}
//==============================================================================
// 和弦数据设置
//==============================================================================

//---- 设置基础和弦（三和弦） ----
void ChordCard::setChord(const Chord& chord, bool isSharpKey, bool isMajor)
{
    currentChord = chord;
    extendedChord = chord;
    sharpKey = isSharpKey;
    majorMode = isMajor;
    inversion = 0;
    //chordType = ChordExtension::Triad;
    chordSymbol.clear();
    chineseName.clear();
    fontsValid = false;
    updateMoodCache();

    // 低音符号不在此处强制更新，由外部根据需要调用
    repaint();
}

//---- 设置扩展和弦（七/九/十一/十三） ----
void ChordCard::setExtendedChord(ChordExtension type, const Chord& chord)
{
    chordType = type;            // 直接设置为类型标签
    extendedChord = chord;
    fontsValid = false;
    updateMoodCache();
    repaint();
}

void ChordCard::setChordType(ChordExtension type)
{
    chordType = type;
    repaint();
}

ChordExtension ChordCard::getChordType() const
{
    return chordType;
}

void ChordCard::setChordTypeSilent(ChordExtension type)
{
    chordType = type;
}

//---- 辅助：设置和弦符号、中文名称、引申音后缀 ----
void ChordCard::setChordSymbol(const juce::String& symbol) { chordSymbol = symbol; repaint(); }
void ChordCard::setChineseName(const juce::String& name)   { chineseName = name; repaint(); }

void ChordCard::setExtensionSuffix(const juce::String& suffix)
{
    extensionSuffix = suffix;
    chordType = ChordExtension::Seventh;              // 强制 7 骨架
    if (onChordQualityChanged)   // 通知外部重新生成和弦
        onChordQualityChanged();
    repaint();
}


//==============================================================================
// 鼠标交互：点击与拖拽
//==============================================================================

void ChordCard::mouseDown(const juce::MouseEvent& e)
{
    if (extendedChord.intervals.empty()) return;
    updateButtonAreas();

    auto bounds = getLocalBounds().toFloat();
    float totalH = bounds.getHeight();
    float totalControlHeight = juce::jmax(32.0f, totalH * 0.18f);
    float qualityH = totalControlHeight * 0.48f;
    float gapH = totalControlHeight * 0.04f;
    float qualityY = totalH - totalControlHeight;
    float buttonY = qualityY + qualityH + gapH;

    // ---- 质量条点击 ----
    if (qualityButtonRect.contains(e.position))
    {
        showQualityPopup();
        return;
    }

    // ---- 数字按钮点击 ----
    if (numberButtonsArea.contains(e.position))
    {
        float relativeX = e.position.x - numberButtonsArea.getX();
        float btnW = numberButtonsArea.getWidth() / 5.0f;
        int clicked = (int)(relativeX / btnW);
        if (clicked >= 0 && clicked < 5)
        {
            static const ChordExtension types[] = {ChordExtension::Triad, ChordExtension::Seventh, ChordExtension::Ninth, ChordExtension::Eleventh, ChordExtension::Thirteenth};
            ChordExtension newType = types[clicked];
            if (isNumberAllowed(newType) && newType != chordType)
            {
                chordType = newType;
                if (onChordTypeChanged) onChordTypeChanged();
                repaint();
            }
        }
        return;   // 防止继续触发顶部按钮
    }
    // ---- 顶部三个按钮区域检测 ----
    auto invBtn = getButtonBounds(0);
    auto rhythmBtn = getButtonBounds(1);
    auto dragBtn = getButtonBounds(2);

    if (invBtn.contains(e.position))
    {
        // 转位循环并更新符号
        int numNotes = static_cast<int>(extendedChord.intervals.size());
        inversion = (inversion + 1) % numNotes;
        repaint();
        if (onInversionChanged) onInversionChanged();

        if (chordSymbol.isNotEmpty())
        {
            juce::String baseSymbol = chordSymbol;
            int slashIndex = chordSymbol.indexOfChar('/');
            if (slashIndex >= 0)
                baseSymbol = chordSymbol.substring(0, slashIndex);

            if (inversion == 0)
            {
                chordSymbol = baseSymbol;                     // 原位
            }
            else
            {
                juce::String newBass = PitchNameHelper::getNameForPitch(
                    static_cast<PitchClass>(getCurrentNotes().front()), sharpKey);
                chordSymbol = baseSymbol + "/" + newBass;    // 转位
            }
            repaint();
        }
        return;
    }

    if (rhythmBtn.contains(e.position))
    {
        showRhythmPopup(rhythmBtn);   // 传入按钮的局部矩形
        return;
    }

    if (dragBtn.contains(e.position))
    {
        dragButtonPressed = true;
        dragStartPos = e.getScreenPosition().toInt();   // 记录屏幕坐标，用于拖拽起点
        repaint();
        return;
    }

    // ---- 其他区域：试听当前和弦 ----
    if (audioProcessor != nullptr)
    {
        auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(audioProcessor);
        if (proc != nullptr)
        {
            std::vector<int> midiNotes;
            std::vector<int> velocities;
            int rootMidi = 60;
            for (int i = 0; i < static_cast<int>(extendedChord.intervals.size()); ++i)
            {
                const int interval = extendedChord.intervals[static_cast<size_t>(i)];
                midiNotes.push_back(rootMidi + interval);
                const int accent = (i == 0 ? 102 : 88 - juce::jmin(i, 5) * 3);
                const int drift = juce::Random::getSystemRandom().nextInt(9) - 4;
                velocities.push_back(juce::jlimit(58, 108, accent + drift));
            }

            stopTimer();
            proc->triggerAudition(midiNotes, velocities);
            startTimer(2000);
        }
    }
}

void ChordCard::mouseDrag(const juce::MouseEvent& e)
{
    // 1. 仅当从拖拽按钮发起
    if (!dragButtonPressed) return;

    // 2. 最小拖拽距离防误触
    if (e.getDistanceFromDragStart() < 10.0f) return;

    // 3. 创建临时 MIDI 文件
    juce::File midiFile = createMidiFile();
    if (!midiFile.existsAsFile())
    {
        dragButtonPressed = false;
        return;
    }

    // 4. 执行外部文件拖放
    juce::DragAndDropContainer::performExternalDragDropOfFiles(
        { midiFile.getFullPathName() }, false);

    // 5. 重置拖拽状态
    dragButtonPressed = false;
    repaint();
}

void ChordCard::mouseUp(const juce::MouseEvent&)
{
    if (dragButtonPressed)
    {
        dragButtonPressed = false;
        repaint();
    }
}

//==============================================================================
// 生成临时 MIDI 文件（用于拖拽）
//==============================================================================
juce::File ChordCard::createMidiFile() const
{
    const int ppq = 960;
    juce::MidiFile midi;
    midi.setTicksPerQuarterNote(ppq);

    juce::MidiMessageSequence track;

    // 1. 转位后的音高类
    std::vector<int> pitchClasses = getCurrentNotes();
    if (pitchClasses.empty()) return {};

    // 2. 根音 pitch class
    int rootPitchClass = static_cast<int>(extendedChord.root);

    // 3. 低音倍增
    int bassMidi = 36 + rootPitchClass;
    if (bassMidi < 36) bassMidi += 12;

    // 4. 和弦音从 C4 附近开始，按转位顺序布局
    std::vector<int> chordNotes;
    int baseOctave = 60; // C4
    int prevMidi = baseOctave - 1;
    for (int pc : pitchClasses)
    {
        int candidate = baseOctave + pc;
        while (candidate <= prevMidi) candidate += 12;
        chordNotes.push_back(candidate);
        prevMidi = candidate;
    }

    // 5. 获取宿主 BPM
    double bpm = 120.0;
    if (audioProcessor != nullptr)
        if (auto* ph = audioProcessor->getPlayHead())
            if (auto pos = ph->getPosition(); pos.hasValue())
                bpm = pos->getBpm().orFallback(120.0);

    // 6. 节奏时值映射
    static const std::map<juce::String, double> beatMap = {
        {"1/1", 4.0}, {"1/2", 2.0}, {"1/4", 1.0},
        {"1/8", 0.5}, {"1/16", 0.25}
    };
    double chordBeats = beatMap.count(currentDragLength) ? beatMap.at(currentDragLength) : 1.0;
    double beatDuration = 60.0 / bpm;
    double chordSeconds = chordBeats * beatDuration;
    const double totalBeats = 4.0;
    double totalSeconds = totalBeats * 60.0 / bpm;

    // 7. 速度与拍号
    track.addEvent(juce::MidiMessage::tempoMetaEvent(
        static_cast<int>(60000000.0 / bpm)), 0.0);
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0.0);

    // 8. 填充一小节音符
    auto& random = juce::Random::getSystemRandom();
    const double humanizeWindowSeconds = 0.012;
    double currentTime = 0.0;
    while (currentTime + 0.001 < totalSeconds)
    {
        double noteLen = juce::jmin(chordSeconds, totalSeconds - currentTime);
        const int beatIndex = static_cast<int>((currentTime / beatDuration) + 0.0001) % 4;
        const int meterAccent = beatIndex == 0 ? 12 : (beatIndex == 2 ? 5 : -5);

        // 和弦音
        for (int i = 0; i < static_cast<int>(chordNotes.size()); ++i)
        {
            const int n = chordNotes[static_cast<size_t>(i)];
            int safeNote = juce::jlimit(0, 127, n);
            const int baseVelocity = (i == 0 ? 100 : 86 - juce::jmin(i, 5) * 3);
            const int velocity = juce::jlimit(52, 116, baseVelocity + meterAccent + random.nextInt(13) - 6);
            const double strumOffset = juce::jlimit(0.0, humanizeWindowSeconds,
                                                    i * 0.003 + (random.nextDouble() - 0.5) * 0.004);
            const double noteStart = juce::jlimit(currentTime, currentTime + noteLen * 0.2, currentTime + strumOffset);
            const double noteEnd = juce::jmax(noteStart + 0.05, currentTime + noteLen * (0.88 + random.nextDouble() * 0.06));

            track.addEvent(juce::MidiMessage::noteOn(1, safeNote, static_cast<juce::uint8>(velocity)), noteStart);
            track.addEvent(juce::MidiMessage::noteOff(1, safeNote), noteEnd);
        }

        // 低音
        int safeBass = juce::jlimit(0, 127, bassMidi);
        const int bassVelocity = juce::jlimit(56, 110, 88 + meterAccent + random.nextInt(11) - 5);
        track.addEvent(juce::MidiMessage::noteOn(1, safeBass, static_cast<juce::uint8>(bassVelocity)), currentTime);
        track.addEvent(juce::MidiMessage::noteOff(1, safeBass), currentTime + noteLen * (0.90 + random.nextDouble() * 0.04));

        currentTime += chordSeconds;
    }

    // 10. 结束标记
    track.addEvent(juce::MidiMessage::endOfTrack(), totalSeconds);

    // 11. 时间转换：秒 -> tick
    double ticksPerSecond = ppq * bpm / 60.0;
    juce::MidiMessageSequence trackTicks;
    for (int i = 0; i < track.getNumEvents(); ++i)
    {
        double secs = track.getEventTime(i);
        double ticks = secs * ticksPerSecond;
        trackTicks.addEvent(track.getEventPointer(i)->message, ticks);
    }

    midi.addTrack(trackTicks);

    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    auto midiFile = tempDir.getNonexistentChildFile("ChordCard", ".mid");
    juce::FileOutputStream out(midiFile);
    if (out.openedOk()) midi.writeTo(out);
    else return {};

    return midiFile;
}

//==============================================================================
// 音符访问与转换
//==============================================================================

std::vector<int> ChordCard::getCurrentNotes() const
{
    std::vector<int> notes;
    getOrderedNotes(notes);
    for (int& n : notes)
        n = (n % 12 + 12) % 12;   // 转为 0~11 的音高类
    return notes;
}

void ChordCard::getOrderedNotes(std::vector<int>& notes) const
{
    if (extendedChord.intervals.empty()) { notes.clear(); return; }
    int root = static_cast<int>(extendedChord.root);
    notes.clear();
    for (int interval : extendedChord.intervals)
        notes.push_back((root + interval) % 12);

    // 应用转位：将前 inversion 个音升高八度
    for (int i = 0; i < inversion; ++i)
    {
        int lowest = notes.front();
        notes.erase(notes.begin());
        notes.push_back(lowest + 12);
    }
}

juce::String ChordCard::getExtendedNoteString() const
{
    std::vector<int> notes;
    getOrderedNotes(notes);
    juce::String str;
    for (int n : notes)
    {
        PitchClass pc = static_cast<PitchClass>(n % 12);
        str += juce::String(PitchNameHelper::getNameForPitch(pc, sharpKey)) + " ";
    }
    return str.trim();
}

//==============================================================================
// 绘制缓存更新
//==============================================================================

void ChordCard::resized()
{
    fontsValid = false;
    updateButtonAreas();
}

void ChordCard::timerCallback()
{
    stopTimer();
    if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(audioProcessor))
        proc->releaseAudition();
}

void ChordCard::updateButtonAreas()
{
    auto bounds = getLocalBounds().toFloat();
    if (bounds.isEmpty())
    {
        qualityButtonRect = {};
        numberButtonsArea = {};
        return;
    }

    auto area = bounds.reduced(14.0f);
    auto headerArea = area.removeFromTop(area.getHeight() * 0.10f);
    juce::ignoreUnused(headerArea);
    auto titleArea = area.removeFromTop(area.getHeight() * 0.22f);
    juce::ignoreUnused(titleArea);
    auto middleArea = area.removeFromTop(area.getHeight() * 0.32f);
    juce::ignoreUnused(middleArea);
    area.removeFromTop(2.5f);
    auto moodArea = area.removeFromTop(area.getHeight() * 0.15f);
    juce::ignoreUnused(moodArea);

    auto panel = area.reduced(8.0f);
    float rowUnit = panel.getHeight() / 3.2f;
    panel.removeFromTop(rowUnit);
    qualityButtonRect = panel.removeFromTop(rowUnit);
    numberButtonsArea = panel;
}

void ChordCard::updateCachedFontsAndColors(const juce::Rectangle<float>& bounds)
{
    auto& theme = getSkinTheme(currentSkin);
    const auto& card = theme.card;
    cachedMainTextColour = card.mainTextCol;
    cachedSubTextColour = card.subTextCol;
    cachedMoodTextColour = card.moodTextCol;

    float h = bounds.getHeight();
    float largeFontH = h * 0.13f;
    float midFontScale = (extendedChord.intervals.size() <= 4) ? 1.0f : 0.7f;
    float midFontH   = h * 0.10f * midFontScale;
    float smallFontH = h * 0.075f;

    if (largeFontH < 8.0f) largeFontH = 8.0f;
    if (midFontH < 5.5f)   midFontH = 5.5f;
    if (smallFontH < 4.5f) smallFontH = 4.5f;

    auto typeface = PitchNameHelper::getHarmonyTypeface();
    cachedLargeFont = juce::Font(typeface); cachedLargeFont.setHeight(largeFontH); cachedLargeFont.setBold(true);
    cachedMidFont   = juce::Font(typeface); cachedMidFont.setHeight(midFontH);
    cachedSmallFont = juce::Font(typeface); cachedSmallFont.setHeight(smallFontH);
    fontsValid = true;
}

void ChordCard::updateMoodCache()
{
    auto mood = MoodMapper::getMood(currentChord.root, currentChord.name, majorMode);
    cachedMoodLines.clear();
    if (mood.description.isNotEmpty())
        cachedMoodLines.addTokens(mood.description, "\n", "");

    // 根据和弦性质附加细化情绪
    juce::String extra;
    switch (chordQuality)
    {
        case ChordQuality::Diminished:  extra = LocalisedStrings::translate("auto_contracted_dark"); break;
        case ChordQuality::Augmented:   extra = LocalisedStrings::translate("auto_expanded_dreamy"); break;
        case ChordQuality::Sus2:        extra = LocalisedStrings::translate("auto_floating_expectant"); break;
        case ChordQuality::Sus4:        extra = LocalisedStrings::translate("auto_suspended_yearning"); break;
        case ChordQuality::MinorMajor7: extra = LocalisedStrings::translate("auto_sad_with_a_glimmer_of_light"); break;
        case ChordQuality::Minor7b5:    extra = LocalisedStrings::translate("auto_cold_uncertain"); break;
        default: break;
    }
    if (extra.isNotEmpty())
    {
        if (cachedMoodLines.size() >= 1)
            cachedMoodLines.add(extra);
        else if (cachedMoodLines.isEmpty())
            cachedMoodLines.add(extra);
    }
}

//==============================================================================
// 绘制辅助：方案 A 视觉元素
//==============================================================================

//---- 获取当前皮肤的强调色 ----
juce::Colour ChordCard::getAccentColour() const
{
    switch (currentSkin)
    {
        case SkinType::FluidPurpleOrange:
            return juce::Colour::fromRGB(168, 85, 247);    // 现代紫高亮色
        case SkinType::FluidLightShadow:
            return juce::Colour::fromRGB(56, 189, 248);     // 未来蓝高亮色
        case SkinType::IvorySatin:
            return juce::Colour::fromRGB(140, 110, 60);     // 深金棕（与米白底对比足够）
        case SkinType::RoyalBlackGold:
            return juce::Colour::fromRGB(212, 175, 55);     // 亮金
        default:
            return juce::Colours::white;                    // 兜底
    }
}

//---- 发光面板 ----
void ChordCard::drawGlowPanel(juce::Graphics& g,
                              juce::Rectangle<float> area,
                              juce::Colour colour,
                              float alpha)
{
    juce::ColourGradient grad(
        colour.withAlpha(alpha),
        area.getCentreX(), area.getY(),
        juce::Colours::transparentBlack,
        area.getCentreX(), area.getBottom(),
        false);

    g.setGradientFill(grad);
    g.fillRoundedRectangle(area, 12.0f);
}

//---- 胶囊音符 ----
void ChordCard::drawCapsuleNotes(juce::Graphics& g,
                                 juce::Rectangle<float> area,
                                 const juce::String& text,
                                 juce::Colour accent)
{
    auto capsule = area.reduced(12.0f, 2.0f);   // 左右略收紧，上下留呼吸
    bool isLightSkin = (currentSkin == SkinType::IvorySatin);

    g.setColour(isLightSkin ? juce::Colour::fromRGB(120, 110, 100).withAlpha(0.25f)
                            : juce::Colours::black.withAlpha(0.22f));
    g.fillRoundedRectangle(capsule, 10.0f);

    g.setColour(accent.withAlpha(0.15f));
    g.drawRoundedRectangle(capsule, 10.0f, 1.0f);

    juce::String pretty = text.trim();
    pretty = pretty.replace(" ", juce::String::fromUTF8(u8" · "));
    g.setColour(cachedMainTextColour);
    g.setFont(cachedMidFont.withHeight(20.0f));
    g.drawFittedText(pretty, capsule.reduced(6, 0).toNearestInt(), juce::Justification::centred, 1);
}

//---- 罗马数字圆环 ----
void ChordCard::drawRomanCircle(juce::Graphics& g,
                                juce::Rectangle<float> area,
                                const juce::String& roman,
                                juce::Colour accent)
{
    float circleSize = juce::jmin(area.getWidth(), area.getHeight()) * 0.85f;
    auto r = area.withSizeKeepingCentre(circleSize, circleSize);
    bool isLightSkin = (currentSkin == SkinType::IvorySatin);

    juce::ColourGradient glow(
        accent.withAlpha(0.15f),
        r.getCentreX(), r.getCentreY(),
        juce::Colours::transparentBlack,
        r.getRight() + 12.0f, r.getBottom() + 12.0f,
        true);
    g.setGradientFill(glow);
    g.fillEllipse(r.expanded(24.0f));

    g.setColour(accent.withAlpha(0.2f));
    g.drawEllipse(r.expanded(10.0f), 1.0f);

    g.setColour(accent.withAlpha(0.4f));
    g.drawEllipse(r, 2.0f);

    g.setColour(isLightSkin ? juce::Colours::white.withAlpha(0.4f)
                            : juce::Colours::black.withAlpha(0.35f));
    g.fillEllipse(r.reduced(6.0f));

    g.setColour(cachedMainTextColour);
    g.setFont(cachedLargeFont.withHeight(circleSize * 0.58f));
    g.drawText(roman, r.toNearestInt(), juce::Justification::centred);
}

//==============================================================================
// 各部分绘制函数
//==============================================================================

//---- 背景与情绪色 ----
void ChordCard::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, const MoodInfo& mood)
{
    auto& theme = getSkinTheme(currentSkin);
    const auto& card = theme.card;

    g.fillAll(card.background.withAlpha(0.10f));

    g.setColour(mood.colour.withAlpha(card.moodBlendAlpha));
    g.fillRoundedRectangle(bounds, 6.0f);

    juce::Colour leftBarColour = card.leftBarT;
    if (currentChord.name == "S" || currentChord.name == "s") leftBarColour = card.leftBarS;
    else if (currentChord.name == "D") leftBarColour = card.leftBarD;
    g.setColour(leftBarColour);
    g.fillRoundedRectangle(bounds.getX(), bounds.getY(), 4.0f, bounds.getHeight(), 2.0f);

    juce::ColourGradient glass(
        juce::Colours::white.withAlpha(0.08f),
        bounds.getX(), bounds.getY(),
        juce::Colours::transparentBlack,
        bounds.getRight(), bounds.getBottom(),
        false);
    g.setGradientFill(glass);
    g.fillRoundedRectangle(bounds, 18.0f);
}

//---- 边框与阴影 ----
void ChordCard::drawBorderAndShadow(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    auto& theme = getSkinTheme(currentSkin);
    const auto& card = theme.card;
    g.setColour(card.borderColour);
    g.drawRoundedRectangle(bounds.reduced(1.0f), 6.0f, card.borderThickness);
    if (card.useShadow)
    {
        juce::DropShadow shadow(card.shadowColour, card.shadowRadius, card.shadowOffset);
        shadow.drawForRectangle(g, bounds.toNearestInt());
    }
}

//---- 顶部按钮行（转位/节奏/拖拽） ----
void ChordCard::drawHeader(juce::Graphics& g, juce::Rectangle<float>& area)
{
    auto bounds = getLocalBounds().toFloat();
    float headerRowHeight = bounds.getHeight() * 0.10f;
    auto inversionRow = area.removeFromTop(headerRowHeight);

    auto invBtn = getButtonBounds(0);
    auto rhythmBtn = getButtonBounds(1);
    auto dragBtn = getButtonBounds(2);

    drawIconButton(g, invBtn, 0);
    drawIconButton(g, rhythmBtn, 1);
    drawIconButton(g, dragBtn, 2);
}

//---- 和弦符号与中文名称 ----
void ChordCard::drawChordSymbolAndChinese(juce::Graphics& g, juce::Rectangle<float>& area)
{
    auto accent = getAccentColour();
    float totalH = area.getHeight();
    
    auto symbolArea  = area.removeFromTop(totalH * 0.55f);
    auto chineseArea = area.removeFromTop(totalH * 0.30f);
    auto sepArea     = area.removeFromTop(totalH * 0.15f);

    juce::String displaySymbol = chordSymbol.isNotEmpty() ? chordSymbol : currentChord.name;
    g.setColour(cachedMainTextColour);
    g.setFont(cachedLargeFont.withHeight(symbolArea.getHeight() * 0.8f));
    g.drawFittedText(displaySymbol, symbolArea.toNearestInt(), juce::Justification::centred, 1);

    if (chineseName.isNotEmpty())
    {
        g.setColour(cachedSubTextColour.withAlpha(0.65f));  
        g.setFont(cachedMidFont.withHeight(chineseArea.getHeight() * 0.80f));
        g.drawFittedText(chineseName, chineseArea.toNearestInt(), juce::Justification::centred, 1);
    }

    g.setColour(accent.withAlpha(0.18f));
    float centerX = symbolArea.getCentreX();
    float lineY = sepArea.getCentreY();
    float gap = 20.0f;
    g.drawLine(symbolArea.getX() + 20.0f, lineY, centerX - gap, lineY, 1.0f);
    g.drawLine(centerX + gap, lineY, symbolArea.getRight() - 20.0f, lineY, 1.0f);
}

//---- 音符胶囊与罗马数字圆环 ----
void ChordCard::drawNotesAndRoman(juce::Graphics& g, juce::Rectangle<float>& area)
{
    auto accent = getAccentColour();
    float totalH = area.getHeight();

    auto notesArea = area.removeFromTop(totalH * 0.22f);
    auto gapArea   = area.removeFromTop(totalH * 0.05f);
    auto romanArea = area;

    drawCapsuleNotes(g, notesArea, getExtendedNoteString(), accent);
    drawRomanCircle(g, romanArea, currentChord.roman, accent);
}

//---- 情绪文字 ----
void ChordCard::drawMoodText(juce::Graphics& g, juce::Rectangle<float>& area)
{
    if (cachedMoodLines.isEmpty()) return;

    auto accent = getAccentColour();
    auto panel = area.reduced(26.0f, 0.0f);
    drawGlowPanel(g, panel, accent, 0.08f);

    auto textPanel = panel.reduced(8.0f, 3.0f);
    auto primaryArea = textPanel.removeFromTop(cachedMoodLines.size() > 1 ? textPanel.getHeight() * 0.52f
                                                                          : textPanel.getHeight());
    auto secondaryArea = textPanel;

    g.setColour(cachedMainTextColour);
    g.setFont(cachedMidFont.withHeight(juce::jlimit(12.0f, 19.0f, primaryArea.getHeight() * 0.62f)));
    g.drawFittedText(cachedMoodLines[0], primaryArea.toNearestInt(),
                     juce::Justification::centred, 2);

    if (cachedMoodLines.size() > 1)
    {
        juce::StringArray extraLines;
        for (int i = 1; i < cachedMoodLines.size(); ++i)
            extraLines.add(cachedMoodLines[i]);

        g.setColour(cachedSubTextColour.withAlpha(0.62f));
        g.setFont(cachedSmallFont.withHeight(juce::jlimit(9.0f, 13.0f, secondaryArea.getHeight() * 0.48f)));
        g.drawFittedText(extraLines.joinIntoString("  "),
                         secondaryArea.toNearestInt(),
                         juce::Justification::centredTop, 2);
    }
}

//---- 底部控制台（转位/功能、质量、数字按钮） ----
void ChordCard::drawBottomButtons(juce::Graphics& g, juce::Rectangle<float> bottomArea,
                                  const juce::Typeface::Ptr& typeface, float /*buttonHeight*/)
{
    auto accent = getAccentColour();
    auto& theme = getSkinTheme(currentSkin);
    auto panel = bottomArea.reduced(8.0f);
    bool isLightSkin = (currentSkin == SkinType::IvorySatin);

    g.setColour(isLightSkin ? juce::Colours::white.withAlpha(0.25f)
                            : juce::Colours::black.withAlpha(0.28f));
    g.fillRoundedRectangle(panel, 14.0f);
    g.setColour(accent.withAlpha(0.12f));
    g.drawRoundedRectangle(panel, 14.0f, 1.0f);

    float totalInnerH = panel.getHeight();
    float rowUnit = totalInnerH / 3.2f;
    auto infoRow    = panel.removeFromTop(rowUnit);
    auto qualityRow = panel.removeFromTop(rowUnit);
    qualityButtonRect = qualityRow;                      // 质量条点击区域
    auto numBtnArea = panel;
    numberButtonsArea = numBtnArea;                      // 数字按钮点击区域

    // ---- 转位/功能信息行 ----
    juce::String inversionLabel;
    if (inversion == 0)
        inversionLabel = LocalisedStrings::translate("auto_root_position");
    else
        inversionLabel = LocalisedStrings::translate("auto_inversion") + juce::String(inversion) + LocalisedStrings::translate("auto_inversion_2");
    juce::String funcLabel = currentChord.name;
    if (chordType != ChordExtension::Triad)
        funcLabel += juce::String(static_cast<int>(chordType));
    g.setColour(cachedMainTextColour);
    g.setFont(juce::Font(typeface).withHeight(juce::jmin(16.0f, infoRow.getHeight() * 0.7f)));
    auto leftHalf  = infoRow.withTrimmedRight(infoRow.getWidth() * 0.5f);
    auto rightHalf = infoRow.withTrimmedLeft(infoRow.getWidth() * 0.5f);
    g.drawText(inversionLabel, leftHalf,  juce::Justification::centred);
    g.drawText(funcLabel,       rightHalf, juce::Justification::centred);

    // ---- 质量行 ----
    g.setColour(isLightSkin ? juce::Colours::white.withAlpha(0.2f)
                            : juce::Colours::black.withAlpha(0.15f));
    g.fillRoundedRectangle(qualityRow.reduced(4.0f), 6.0f);
    g.setColour(accent.withAlpha(0.25f));
    g.drawRoundedRectangle(qualityRow.reduced(4.0f), 6.0f, 1.0f);
    g.setColour(cachedMainTextColour);
    g.setFont(juce::Font(typeface).withHeight(juce::jmin(16.0f, qualityRow.getHeight() * 0.7f)));
    g.drawText(getEffectiveQualityDisplayName(), qualityRow.toNearestInt(), juce::Justification::centred);

    // ---- 数字按钮行 ----
    static const ChordExtension typeValues[] = {ChordExtension::Triad, ChordExtension::Seventh, ChordExtension::Ninth, ChordExtension::Eleventh, ChordExtension::Thirteenth};
    float btnW = numBtnArea.getWidth() / 5.0f;
    juce::Font btnFont(typeface);
    btnFont.setHeight(juce::jmin(14.0f, numBtnArea.getHeight() * 0.6f));

    for (int i = 0; i < 5; ++i)
    {
        auto btn = numBtnArea.removeFromLeft(btnW).reduced(2.0f);
        bool allowed = isNumberAllowed(typeValues[i]);
        bool isActive = (chordType == typeValues[i]);

        if (isActive)
        {
            g.setColour(accent.withAlpha(0.95f));
            g.fillRoundedRectangle(btn, 8.0f);
            g.setColour(accent.withAlpha(0.4f));
            g.drawRoundedRectangle(btn.expanded(1.0f), 8.0f, 1.2f);
            g.setColour(theme.card.background);
        }
        else
        {
            g.setColour(juce::Colours::white.withAlpha(allowed ? 0.08f : 0.015f));
            g.fillRoundedRectangle(btn, 8.0f);
            g.setColour(allowed ? cachedMainTextColour : cachedSubTextColour.withAlpha(0.18f));
        }

        g.setFont(btnFont);
                g.drawText(juce::String(static_cast<int>(typeValues[i])), btn.toNearestInt(), juce::Justification::centred);
    }
}

//==============================================================================
// 质量显示与弹出菜单
//==============================================================================

//---- 对象级质量显示名称 ----
juce::String ChordCard::getQualityDisplayName() const
{
    switch (chordQuality)
    {
        case ChordQuality::MajorTriad:  return LocalisedStrings::translate("auto_major_triad_maj_2");
        case ChordQuality::MinorTriad:  return LocalisedStrings::translate("auto_minor_triad_m_2");
        case ChordQuality::Dominant7:   return LocalisedStrings::translate("auto_dominant_seventh_7");
        case ChordQuality::Diminished:  return LocalisedStrings::translate("auto_diminished_triad_dim");
        case ChordQuality::Augmented:   return LocalisedStrings::translate("auto_augmented_triad_aug");
        case ChordQuality::Sus2:        return LocalisedStrings::translate("auto_suspended_second_sus2");
        case ChordQuality::Sus4:        return LocalisedStrings::translate("auto_suspended_fourth_sus4");
        case ChordQuality::MinorMajor7: return LocalisedStrings::translate("auto_minor_major_seventh_mmaj7");
        case ChordQuality::Minor7b5:    return LocalisedStrings::translate("auto_half_diminished_seventh_m7b5");
        default:                        return LocalisedStrings::translate("auto_major_triad_maj_2");
    }
}

//---- 静态版本（弹出菜单使用） ----
juce::String ChordCard::getQualityDisplayName(ChordQuality q)
{
    switch (q)
    {
        case ChordQuality::MajorTriad:  return LocalisedStrings::translate("auto_major_triad_maj_2");
        case ChordQuality::MinorTriad:  return LocalisedStrings::translate("auto_minor_triad_m_2");
        case ChordQuality::Dominant7:   return LocalisedStrings::translate("auto_dominant_seventh_7");
        case ChordQuality::Diminished:  return LocalisedStrings::translate("auto_diminished_triad_dim");
        case ChordQuality::Augmented:   return LocalisedStrings::translate("auto_augmented_triad_aug");
        case ChordQuality::Sus2:        return LocalisedStrings::translate("auto_suspended_second_sus2");
        case ChordQuality::Sus4:        return LocalisedStrings::translate("auto_suspended_fourth_sus4");
        case ChordQuality::MinorMajor7: return LocalisedStrings::translate("auto_minor_major_seventh_mmaj7");
        case ChordQuality::Minor7b5:    return LocalisedStrings::translate("auto_half_diminished_seventh_m7b5");
        default:                        return LocalisedStrings::translate("auto_major_triad_maj_2");
    }
}

//---- 弹出质量选择菜单（含属七引申音子菜单） ----
void ChordCard::showQualityPopup()
{
    // 使用静态局部变量确保生命周期覆盖异步弹出菜单
    static PopupMenuLookAndFeel popLaf;
    juce::PopupMenu menu;
    menu.setLookAndFeel(&popLaf);

    std::vector<ChordQuality> qualities = {
        ChordQuality::MajorTriad, ChordQuality::MinorTriad, ChordQuality::Dominant7,
        ChordQuality::Diminished, ChordQuality::Augmented,  ChordQuality::Sus2,
        ChordQuality::Sus4,      ChordQuality::MinorMajor7, ChordQuality::Minor7b5
    };

    for (auto q : qualities)
    {
        juce::String itemText = getQualityDisplayName(q);
        if (q == ChordQuality::Dominant7)
        {
            juce::PopupMenu domSub;
            domSub.setLookAndFeel(&popLaf);

            domSub.addItem(LocalisedStrings::translate("auto_no_extension_7"), [this] {
                chordQuality = ChordQuality::Dominant7;
                extensionSuffix.clear();
                if (onChordQualityChanged) onChordQualityChanged();
                repaint();
            });
            domSub.addSeparator();
            domSub.addItem(LocalisedStrings::translate("chord_alt_b9"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("b9");
            });
            domSub.addItem(LocalisedStrings::translate("chord_alt_sharp9"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("#9");
            });
            domSub.addItem(LocalisedStrings::translate("chord_alt_sharp11"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("#11");
            });
            domSub.addItem(LocalisedStrings::translate("chord_alt_b13"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("b13");
            });
            domSub.addSeparator();
            domSub.addItem(LocalisedStrings::translate("chord_alt_b9_sharp11"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("b9#11");
            });
            domSub.addItem(LocalisedStrings::translate("chord_alt_sharp9_sharp11"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("#9#11");
            });
            domSub.addItem(LocalisedStrings::translate("chord_alt_b9_b13"), [this] {
                chordQuality = ChordQuality::Dominant7;
                setExtensionSuffix("b9b13");
            });

            menu.addSubMenu(itemText, domSub);
        }
        else
        {
            menu.addItem(itemText, [this, q] {
                chordQuality = q;
                extensionSuffix.clear();
                if (onChordQualityChanged) onChordQualityChanged();
                repaint();
            });
        }
    }

    auto screenRect = juce::Rectangle<int>(
        getScreenX() + (int)qualityButtonRect.getX(),
        getScreenY() + (int)qualityButtonRect.getY(),
        (int)qualityButtonRect.getWidth(),
        (int)qualityButtonRect.getHeight()
    );

    auto opts = juce::PopupMenu::Options()
        .withTargetScreenArea(screenRect)
        .withMinimumWidth(180)
        .withStandardItemHeight(30);

    menu.showMenuAsync(opts);
}

//---- 底部质量栏显示的有效文本（合并类型与引申音） ----
juce::String ChordCard::getEffectiveQualityDisplayName() const
{
    if (chordType == ChordExtension::Triad)
        return getQualityDisplayName();

    juce::String extStr;
    switch (chordType) {
        case ChordExtension::Seventh: extStr = "7"; break;
        case ChordExtension::Ninth:   extStr = "9"; break;
        case ChordExtension::Eleventh: extStr = "11"; break;
        case ChordExtension::Thirteenth: extStr = "13"; break;
        default: extStr = "7"; break;
    }

    switch (chordQuality)
    {
        case ChordQuality::MajorTriad:
            return LocalisedStrings::translate("auto_major_triad_maj") + extStr + ")";
        case ChordQuality::MinorTriad:
            return LocalisedStrings::translate("auto_minor_triad_m") + extStr + ")";
        case ChordQuality::Dominant7:
        {
            juce::String display = LocalisedStrings::translate("auto_dominant_seventh_2");
            if (extensionSuffix.isNotEmpty())
                display += extensionSuffix;
            else
                display += extStr;
            display += ")";
            return display;
        }
        case ChordQuality::Diminished:
            if (chordType == ChordExtension::Seventh) return LocalisedStrings::translate("auto_english_translation_pending");
            else return LocalisedStrings::translate("auto_diminished_triad_dim");
        case ChordQuality::Augmented:
            if (chordType == ChordExtension::Seventh) return LocalisedStrings::translate("auto_english_translation_pending_3");
            else return LocalisedStrings::translate("auto_augmented_triad_aug");
        case ChordQuality::Sus2:
            return LocalisedStrings::translate("auto_suspended_second_sus2");
        case ChordQuality::Sus4:
            if (chordType == ChordExtension::Seventh) return LocalisedStrings::translate("auto_suspended_fourth_7sus4");
            else return LocalisedStrings::translate("auto_suspended_fourth_sus4");
        case ChordQuality::MinorMajor7:
            return LocalisedStrings::translate("auto_minor_major_seventh_mmaj7");
        case ChordQuality::Minor7b5:
            return LocalisedStrings::translate("auto_half_diminished_seventh_m7b5");
        default:
            return getQualityDisplayName();
    }
}

//---- 判断数字按钮是否可用于当前品质 ----
bool ChordCard::isNumberAllowed(ChordExtension number) const
{
    if (extensionSuffix.isNotEmpty())
        return (number == ChordExtension::Seventh);
    switch (chordQuality)
    {
        case ChordQuality::MajorTriad:
        case ChordQuality::MinorTriad:
        case ChordQuality::Dominant7:
            return true;
        case ChordQuality::Diminished:
        case ChordQuality::Sus4:
            return (number == ChordExtension::Triad || number == ChordExtension::Seventh);
        case ChordQuality::Augmented:
        case ChordQuality::Sus2:
            return (number == ChordExtension::Triad);
        case ChordQuality::Minor7b5:
        case ChordQuality::MinorMajor7:
            return (number == ChordExtension::Seventh);
        default:
            return false;
    }
}

//==============================================================================
// 节奏选择弹出菜单
//==============================================================================
void ChordCard::showRhythmPopup(juce::Rectangle<float> buttonArea)
{
    juce::PopupMenu menu;
    juce::StringArray lengths = {"1/1", "1/2", "1/4", "1/8", "1/16"};
    for (auto& len : lengths)
    {
        bool isTicked = (currentDragLength == len);
        menu.addItem(len, true, isTicked, [this, len] {
            currentDragLength = len;
        });
    }

    auto screenRect = juce::Rectangle<int>(
        getScreenX() + (int)buttonArea.getX(),
        getScreenY() + (int)buttonArea.getY(),
        (int)buttonArea.getWidth(),
        (int)buttonArea.getHeight()
    );

    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetScreenArea(screenRect)
    );
}

//==============================================================================
// 按钮布局与图标绘制
//==============================================================================

juce::Rectangle<float> ChordCard::getButtonBounds(int index) const
{
    auto bounds = getLocalBounds().toFloat();
    float headerRowHeight = bounds.getHeight() * 0.10f;
    float btnSize = headerRowHeight * 0.8f;
    float gap = 4.0f;

    float totalWidth = btnSize * 3 + gap * 2;
    float startX = bounds.getX() + (bounds.getWidth() - totalWidth) * 0.5f;
    float y = bounds.getY() + (headerRowHeight - btnSize) * 0.5f;

    return juce::Rectangle<float>(startX + index * (btnSize + gap), y, btnSize, btnSize);
}

void ChordCard::drawIconButton(juce::Graphics& g, const juce::Rectangle<float>& bounds, int buttonType)
{
    auto accent = getAccentColour();
    bool isLightSkin = (currentSkin == SkinType::IvorySatin);

    g.setColour(isLightSkin ? juce::Colours::white.withAlpha(0.35f)
                            : juce::Colours::black.withAlpha(0.24f));
    g.fillEllipse(bounds);

    g.setColour(accent.withAlpha(0.18f));
    g.drawEllipse(bounds, 1.0f);

    juce::String icon;
    switch (buttonType)
    {
        case 0: icon = juce::String::fromUTF8(u8"↻"); break;
        case 1: icon = juce::String::fromUTF8(u8"♫"); break;
        case 2: icon = juce::String::fromUTF8(u8"⬇"); break;
    }

    g.setColour((buttonType == 2 && dragButtonPressed) ? accent.withAlpha(0.4f)
                                                       : cachedMainTextColour);
    g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(bounds.getHeight() * 0.5f));
    g.drawText(icon, bounds, juce::Justification::centred, false);
}

//==============================================================================
// 主绘制入口
//==============================================================================
void ChordCard::paint(juce::Graphics& g)
{
    if (extendedChord.intervals.empty())
    {
        auto& theme = getSkinTheme(currentSkin);
        g.fillAll(theme.card.background);
        return;
    }

    auto bounds = getLocalBounds().toFloat();
    if (!fontsValid)
        updateCachedFontsAndColors(bounds);

    auto mood = MoodMapper::getMood(currentChord.root, currentChord.name, majorMode);

    // 背景与阴影
    drawBackground(g, bounds, mood);
    drawBorderAndShadow(g, bounds);

    // 方案 A 布局比例
    auto area = bounds.reduced(14.0f);
    auto headerArea = area.removeFromTop(area.getHeight() * 0.10f);
    auto titleArea  = area.removeFromTop(area.getHeight() * 0.22f);
    auto middleArea = area.removeFromTop(area.getHeight() * 0.30f);
    area.removeFromTop(2.5f);                   
    auto moodArea   = area.removeFromTop(area.getHeight() * 0.26f); 
    auto bottomArea = area;

    drawHeader(g, headerArea);
    drawChordSymbolAndChinese(g, titleArea);
    drawNotesAndRoman(g, middleArea);
    drawMoodText(g, moodArea);
    drawBottomButtons(g, bottomArea,
                      PitchNameHelper::getHarmonyTypeface(),
                      32.0f);
}
