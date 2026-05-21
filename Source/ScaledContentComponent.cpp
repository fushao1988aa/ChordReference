//==================================================================================================
//  ScaledContentComponent.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：可缩放内容区实现
//        - 整合五度圈、和弦卡片、迷你键盘与品牌链接
//        - 使用 FlexBox 弹性布局，响应窗口缩放
//        - 管理调性设置、和弦更新、MIDI 识别结果应用
//        - 皮肤/语言切换与智能生成信息提示
//        - 卡片交互回调（转位/类型/质量变化）的级联处理
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ScaledContentComponent.h"
#include "TSDBuilder.h"
#include "PitchNameHelper.h"
#include "ScaleEngine.h"
#include <map>
#include "BinaryData.h"
#include "SkinColours.h"
#include "PluginProcessor.h"
#include "NegativeHarmony.h"
#include "SmartGenerator.h"
#include "ChordNaming.h"
#include "LocalisedStrings.h"
#define TR(key) LocalisedStrings::translate(key)

//==============================================================================
// 构造与初始化
//==============================================================================
ScaledContentComponent::ScaledContentComponent()
{
    addAndMakeVisible(circle);
    for (int i = 0; i < 3; ++i)
    {
        auto* card = new ChordCard();
        chordCards.add(card);
        addAndMakeVisible(card);
        // 卡片回调连接
        card->onInversionChanged = [this, card]() { onCardInversionChanged(card); };
        card->onChordTypeChanged = [this, card]() { onCardChordTypeChanged(card); };
        card->onChordQualityChanged = [this, card]() { onCardChordQualityChanged(card); };
    }
    addAndMakeVisible(miniKeyboard);

    // 品牌链接
    brandLink.setText(LocalisedStrings::translate("auto_meridian_audio_studio"), juce::dontSendNotification);
    brandLink.setJustificationType(juce::Justification::centred);
    brandLink.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.4f));
    brandLink.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(12.0f));
    brandLink.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(brandLink);

    // 五度圈点击回调 → 转调
    circle.onKeySelected = [safeThis = juce::Component::SafePointer<ScaledContentComponent>(this)]
    (PitchClass tonic, ScaleType type) {
        if (safeThis) safeThis->setKey(tonic, type);
    };

    circle.onGenerateRequested = [safeThis = juce::Component::SafePointer<ScaledContentComponent>(this)] {
        if (!safeThis) return;
        // 原逻辑不变，但内部所有对 this 的引用改为 safeThis->
        SmartGenerator::Style style = SmartGenerator::Style::General;
        switch (safeThis->currentSmartStyle)
        {
            case 1: style = SmartGenerator::Style::Pop; break;
            case 2: style = SmartGenerator::Style::Jazz; break;
            case 3: style = SmartGenerator::Style::Ancient; break;
            default: break;
        }
        auto functions = SmartGenerator::generate(style);
        int retries = 0;
        while (functions == safeThis->lastGeneratedFunctions && retries < 5)
        {
            functions = SmartGenerator::generate(style);
            retries++;
        }
        safeThis->lastGeneratedFunctions = functions;
        safeThis->applyGeneratedProgression(functions);
    };

    circle.onStyleChanged = [safeThis = juce::Component::SafePointer<ScaledContentComponent>(this)]
    (int styleIndex) {
        if (!safeThis) return;
        safeThis->currentSmartStyle = styleIndex;
        if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(safeThis->audioProcessorRef))
            proc->setSmartStyle(styleIndex);
        if (safeThis->circle.onGenerateRequested) // 触发原有生成
            safeThis->circle.onGenerateRequested();
    };

    startTimerHz(10);   // 智能生成提示倒计时

    // 初始化为 C 大调
    setKey(PitchClass::C, ScaleType::Major);
}

//==============================================================================
// 鼠标事件：品牌链接跳转
//==============================================================================
void ScaledContentComponent::mouseDown(const juce::MouseEvent& e)
{
    if (brandLink.getBounds().contains(e.position.toInt()))
    {
        juce::URL("https://MeridianAudioStudio.com").launchInDefaultBrowser();
    }
}

//==============================================================================
// 缩放计算
//==============================================================================
float ScaledContentComponent::getCurrentScale() const
{
    auto area = getLocalBounds();
    if (area.getWidth() >= 10 && area.getHeight() >= 10)
    {
        float scaleX = (float)area.getWidth() / 1000.0f;
        float scaleY = (float)area.getHeight() / 800.0f;
        return juce::jmin(scaleX, scaleY);
    }
    return 1.0f;
}

//==============================================================================
// 布局：使用 FlexBox
//==============================================================================
void ScaledContentComponent::resized()
{
    if (getWidth() < 10 || getHeight() < 10)
    {
        pendingResize = true;
        return;
    }
    
    pendingResize = false;
    currentScale = getCurrentScale();

    // 品牌链接
    float linkFontSize = juce::jlimit(8.0f, 14.0f, 10.0f * currentScale);
    brandLink.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(linkFontSize));
    brandLink.setBounds(getLocalBounds().removeFromBottom((int)linkFontSize + 6).withSizeKeepingCentre(580, (int)linkFontSize + 6));

    auto mainArea = getLocalBounds().withTrimmedBottom((int)linkFontSize + 6);
    if (mainArea.isEmpty()) return;

    int infoBarHeight = juce::jmax(20, (int)(24.0f * currentScale));

    mainFlexBox = juce::FlexBox();
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.items.clear();

    juce::FlexBox upperBox;
    upperBox.flexDirection = juce::FlexBox::Direction::row;
    upperBox.items.add(juce::FlexItem(circle).withFlex(0.35f).withMargin(juce::FlexItem::Margin(6, 4, 6, 8)));

    juce::FlexBox cardBox;
    cardBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto* card : chordCards)
        cardBox.items.add(juce::FlexItem(*card).withFlex(1.0f).withMargin(juce::FlexItem::Margin(6, 6, 6, 6)));
    upperBox.items.add(juce::FlexItem(cardBox).withFlex(0.65f).withMargin(juce::FlexItem::Margin(6, 8, 6, 4)));

    mainFlexBox.items.add(juce::FlexItem(upperBox).withFlex(0.65f));

    // 信息栏占位
    juce::Component infoPlaceholder;
    addAndMakeVisible(infoPlaceholder);
    mainFlexBox.items.add(juce::FlexItem(infoPlaceholder).withHeight(infoBarHeight).withMargin(juce::FlexItem::Margin(0, 8, 0, 8)));

    // 键盘
    mainFlexBox.items.add(juce::FlexItem(miniKeyboard).withFlex(0.35f).withMargin(juce::FlexItem::Margin(4, 8, 4, 8)));

    mainFlexBox.performLayout(mainArea.toFloat());
    infoBarRect = infoPlaceholder.getBounds();
    removeChildComponent(&infoPlaceholder);
}

//==============================================================================
// 父组件层级变化时延迟布局
//==============================================================================
void ScaledContentComponent::parentHierarchyChanged()
{
    if (getParentComponent() != nullptr && getWidth() > 0 && getHeight() > 0)
    {
        if (pendingResize)
            resized();
    }
}

//==============================================================================
// 绘制：背景与信息栏
//==============================================================================
void ScaledContentComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    float w = (float)area.getWidth();
    float h = (float)area.getHeight();

    auto& theme = getSkinTheme(currentSkin);

    if (w < 10 || h < 10)
    {
        auto bgGrad = theme.backgroundGradient(600.0f, 400.0f);
        g.setGradientFill(bgGrad);
        g.fillAll();
        return;
    }

    auto bgGrad = theme.backgroundGradient(w, h);
    g.setGradientFill(bgGrad);
    g.fillAll();

    if (!infoBarRect.isEmpty())
    {
        float fontSize = 12.0f * currentScale;
        if (fontSize < 8.0f) fontSize = 8.0f;
        if (fontSize > 16.0f) fontSize = 16.0f;

        g.setColour(theme.infoPanelAccent);
        g.fillRoundedRectangle(infoBarRect.withRight(infoBarRect.getX() + 3).toFloat(), 2.0f);
        g.setColour(theme.infoPanelBackground);
        g.fillRoundedRectangle(infoBarRect.toFloat(), 4.0f);
        g.setColour(theme.infoTextColour);
        g.setFont(juce::Font(PitchNameHelper::getHarmonyTypeface()).withHeight(fontSize));

        const bool hasMidiExpression = midiAverageVelocity > 0 || midiSustainPedalDown || !midiExpressionNotes.empty();
        auto textRect = infoBarRect.reduced(6, 0);
        juce::Rectangle<int> midiStatusRect;

        if (hasMidiExpression)
        {
            const int minStatusWidth = juce::roundToInt(132.0f * currentScale);
            const int maxStatusWidth = juce::roundToInt(210.0f * currentScale);
            const int statusWidth = juce::jlimit(132, juce::jmax(160, maxStatusWidth),
                                                juce::jlimit(minStatusWidth, maxStatusWidth, infoBarRect.getWidth() / 3));
            midiStatusRect = infoBarRect.withLeft(infoBarRect.getRight() - statusWidth);
            textRect.setRight(midiStatusRect.getX() - 6);

            g.setColour(theme.infoPanelBackground);
            g.fillRoundedRectangle(midiStatusRect.toFloat(), 4.0f);
            g.setColour(theme.infoPanelAccent.withAlpha(0.45f));
            g.fillRoundedRectangle(midiStatusRect.withWidth(2).toFloat(), 1.5f);
        }

        juce::String displayText = smartGenInfoText.isNotEmpty() ? smartGenInfoText : getKeyInfoText();
        g.setColour(theme.infoTextColour);
        g.drawText(displayText, textRect, juce::Justification::centredLeft, true);

        if (hasMidiExpression)
        {
            const auto velocityLabel = LocalisedStrings::translate("midi_velocity");
            const auto sustainLabel = LocalisedStrings::translate("midi_sustain");
            const auto sustainState = LocalisedStrings::translate(midiSustainPedalDown ? "midi_sustain_on" : "midi_sustain_off");
            const juce::String statusText = velocityLabel + " " + juce::String(midiAverageVelocity)
                                          + "   " + sustainLabel + " " + sustainState;

            g.setColour(theme.infoTextColour);
            g.drawText(statusText, midiStatusRect.reduced(10, 0), juce::Justification::centred, true);

            const float velocityRatio = juce::jlimit(0.0f, 1.0f, midiAverageVelocity / 127.0f);
            auto meter = midiStatusRect.reduced(10, 0).removeFromBottom(2).toFloat();
            g.setColour(theme.infoTextColour.withAlpha(0.18f));
            g.fillRect(meter);
            g.setColour(theme.infoPanelAccent.withAlpha(0.75f));
            g.fillRect(meter.withWidth(meter.getWidth() * velocityRatio));
        }
    }
}

//==============================================================================
// 调性设置
//==============================================================================
void ScaledContentComponent::setKey(PitchClass tonic, ScaleType type)
{
    currentTonic = tonic;
    currentType = type;
    currentFunction.clear();
    currentChordSymbol.clear();
    manualOverride = false;

    if (type == ScaleType::Major)
        isSharpKey = PitchNameHelper::isSharpKeyForTonic(tonic);
    else
        isSharpKey = PitchNameHelper::isSharpKeyForTonic(ScaleEngine::getRelativeMajor(tonic));

    circle.setCurrentKey(tonic, type);
    updateDisplay();

    if (onKeyChanged)
        onKeyChanged(tonic, type);
}

//==============================================================================
// 设置调性并指定功能和弦（MIDI 识别结果）
//==============================================================================
void ScaledContentComponent::setKeyAndChord(PitchClass tonic, ScaleType type, const juce::String& function)
{
    setKey(tonic, type);
    currentFunction = function;
    manualOverride = false;

    if (function.isNotEmpty())
    {
        updateChordCardFromFunction(function);

        juce::String funcName = function;
        while (funcName.isNotEmpty() && juce::CharacterFunctions::isDigit(funcName.getLastCharacter()))
            funcName = funcName.dropLastCharacters(1);

        int cardIndex = -1;
        if (funcName == "T" || funcName == "t") cardIndex = 0;
        else if (funcName == "S" || funcName == "s") cardIndex = 1;
        else if (funcName == "D") cardIndex = 2;

        if (cardIndex >= 0 && cardIndex < chordCards.size())
            currentChordSymbol = chordCards[cardIndex]->getChordSymbol();
        else
            currentChordSymbol.clear();
    }
    repaint();
}

//==============================================================================
// 仅更新和弦功能（锁调模式）
//==============================================================================
void ScaledContentComponent::setChordOnly(const juce::String& function)
{
    if (function.isEmpty())
    {
        resetToDefaultTriads();
        return;
    }

    currentFunction = function;
    manualOverride = false;
    updateChordCardFromFunction(function);

    juce::String funcName = function;
    while (funcName.isNotEmpty() && juce::CharacterFunctions::isDigit(funcName.getLastCharacter()))
        funcName = funcName.dropLastCharacters(1);

    int cardIndex = -1;
    if (funcName == "T" || funcName == "t") cardIndex = 0;
    else if (funcName == "S" || funcName == "s") cardIndex = 1;
    else if (funcName == "D") cardIndex = 2;

    if (cardIndex >= 0 && cardIndex < chordCards.size())
        currentChordSymbol = chordCards[cardIndex]->getChordSymbol();
    else
        currentChordSymbol.clear();

    repaint();
}

void ScaledContentComponent::setMidiExpression(const std::vector<int>& notes,
                                               const std::vector<int>& velocities,
                                               int averageVelocity,
                                               bool sustainPedalDown)
{
    midiExpressionNotes = notes;
    midiExpressionVelocities = velocities;
    int peakVelocity = averageVelocity;
    for (int velocity : velocities)
        peakVelocity = juce::jmax(peakVelocity, velocity);

    midiAverageVelocity = juce::jlimit(0, 127, peakVelocity);
    midiSustainPedalDown = sustainPedalDown;
    midiExpressionDisplayTicks = (midiAverageVelocity > 0 || midiSustainPedalDown || !notes.empty()) ? 20 : 0;
    miniKeyboard.setMidiExpression(notes, velocities, averageVelocity, sustainPedalDown);
    repaint(infoBarRect);
}

//==============================================================================
// 重置为默认三和弦
//==============================================================================
void ScaledContentComponent::resetToDefaultTriads()
{
    if (manualOverride) return;

    if (!currentFunction.isEmpty() || chordCards[0]->getChordType() != ChordExtension::Triad
        || chordCards[1]->getChordType() != ChordExtension::Triad || chordCards[2]->getChordType() != ChordExtension::Triad)
    {
        currentFunction.clear();
        currentChordSymbol.clear();
        updateDisplay();
    }
}
//==============================================================================
// 卡片回调：转位变化
//==============================================================================
void ScaledContentComponent::onCardInversionChanged(ChordCard* card)
{
    setManualOverride(true);
    refreshKeyboardHighlight();
}

//==============================================================================
// 卡片回调：和弦类型变化（3/7/9/11/13）
//==============================================================================
void ScaledContentComponent::onCardChordTypeChanged(ChordCard* card)
{
    setManualOverride(true);
    
    int degree = (card == chordCards[0]) ? 1 : (card == chordCards[1]) ? 4 : 5;
    ChordExtension chordTypeTag = card->getChordType();
    int numNotes = ChordCard::getNumNotesForType(chordTypeTag);
    ChordQuality quality = card->getChordQuality();
    
    std::vector<PitchClass> scale;
    bool isMajor = (currentType == ScaleType::Major);
    if (isMajor)
        scale = ScaleEngine::getScale(currentTonic, ScaleType::Major);
    else
    {
        scale = ScaleEngine::getScale(currentTonic, ScaleType::NaturalMinor);
        if (degree == 5)
        {
            int seventhIdx = 6;
            int orig = static_cast<int>(scale[seventhIdx]);
            scale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
        }
    }
    
    try
    {
        Chord extended = ScaleEngine::getChord(scale, degree, numNotes, quality);
        card->setExtendedChord(chordTypeTag, extended);  
        
        juce::String extSuffix = card->getExtensionSuffix();
        juce::String symbol = getChordSymbolFromQuality(extended.root, quality, numNotes, extSuffix);
        card->setChordSymbol(symbol);
        card->setChineseName(getChineseChordName(symbol));
        
        card->setChordType(chordTypeTag);
        card->repaint();
    }
    catch (...)
    {
        juce::Logger::writeToLog("ScaledContentComponent::onCardChordTypeChanged failed to rebuild chord");
    }
    
    refreshKeyboardHighlight();
}

//==============================================================================
// 卡片回调：和弦性质变化（大三/小三/属七等）
//==============================================================================
void ScaledContentComponent::onCardChordQualityChanged(ChordCard* card)
{
    setManualOverride(true);
    int degree = (card == chordCards[0]) ? 1 : (card == chordCards[1]) ? 4 : 5;
    ChordQuality quality = card->getChordQuality();

    if (quality != ChordQuality::Dominant7 && card->getExtensionSuffix().isNotEmpty())
    {
        card->clearExtensionSuffixSilent();
        ChordExtension curType = card->getChordType();
        if (curType != ChordExtension::Triad && curType != ChordExtension::Seventh)
            card->setChordType(ChordExtension::Triad);
    }

    ChordExtension currentNum = card->getChordType();
    bool allowed = false;
    switch (quality)
    {
        case ChordQuality::MajorTriad:
        case ChordQuality::MinorTriad:
        case ChordQuality::Dominant7:
            allowed = true; break;
        case ChordQuality::Diminished:
        case ChordQuality::Sus4:
            allowed = (currentNum == ChordExtension::Triad || currentNum == ChordExtension::Seventh); break;
        case ChordQuality::Augmented:
        case ChordQuality::Sus2:
            allowed = (currentNum == ChordExtension::Triad); break;
        case ChordQuality::Minor7b5:
        case ChordQuality::MinorMajor7:
            allowed = (currentNum == ChordExtension::Seventh); break;
    }
    if (!allowed)
    {
        if (quality == ChordQuality::Diminished || quality == ChordQuality::Augmented ||
            quality == ChordQuality::Sus2 || quality == ChordQuality::Sus4)
            card->setChordType(ChordExtension::Triad);
        else if (quality == ChordQuality::Minor7b5 || quality == ChordQuality::MinorMajor7)
            card->setChordType(ChordExtension::Seventh);
    }
    int numNotes = ChordCard::getNumNotesForType(card->getChordType());
    
    std::vector<PitchClass> scale;
    if (currentType == ScaleType::Major)
        scale = ScaleEngine::getScale(currentTonic, ScaleType::Major);
    else {
        scale = ScaleEngine::getScale(currentTonic, ScaleType::NaturalMinor);
        if (degree == 5) {
            int seventhIdx = 6;
            int orig = static_cast<int>(scale[seventhIdx]);
            scale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
        }
    }
    try {
        Chord extended = ScaleEngine::getChord(scale, degree, numNotes, quality);
        card->setExtendedChord(card->getChordType(), extended);
        
        juce::String extSuffix = card->getExtensionSuffix();
        juce::String symbol = getChordSymbolFromQuality(extended.root, quality, numNotes, extSuffix);
        card->setChordSymbol(symbol);
        card->setChineseName(getChineseChordName(symbol));
        
        card->setChordType(card->getChordType());
        card->repaint();
    }
    catch (...)
    {
        juce::Logger::writeToLog("ScaledContentComponent::onCardChordQualityChanged failed to rebuild chord");
    }
    
    refreshKeyboardHighlight();
}

//==============================================================================
// 和弦符号生成（基于质量与扩展数字）
//==============================================================================
juce::String ScaledContentComponent::getChordSymbolFromQuality(PitchClass root, ChordQuality quality, int numNotes, const juce::String& extSuffix) const
{
    return ChordNaming::getSymbol(root, isSharpKey, quality, numNotes, extSuffix);
}

//==============================================================================
// 根据功能字符串更新对应卡片
//==============================================================================
void ScaledContentComponent::updateChordCardFromFunction(const juce::String& function)
{
    juce::String funcName = function;
    ChordExtension extNumber = ChordExtension::Triad;

    if (funcName.isNotEmpty() && juce::CharacterFunctions::isDigit(funcName.getLastCharacter()))
    {
        int parsedNum = funcName.substring(1).getIntValue();
        extNumber = intToChordExtension(parsedNum);
        while (funcName.isNotEmpty() && juce::CharacterFunctions::isDigit(funcName.getLastCharacter()))
            funcName = funcName.dropLastCharacters(1);
    }

    if (funcName.isEmpty())
        funcName = function;

    int cardIndex = -1;
    if (funcName == "T" || funcName == "t") cardIndex = 0;
    else if (funcName == "S" || funcName == "s") cardIndex = 1;
    else if (funcName == "D") cardIndex = 2;

    if (cardIndex >= 0 && cardIndex < chordCards.size())
    {
        ChordCard* card = chordCards[cardIndex];
        int degree = (cardIndex == 0) ? 1 : (cardIndex == 1) ? 4 : 5;
        int numNotes = ChordCard::getNumNotesForType(extNumber);

        std::vector<PitchClass> scale;
        if (currentType == ScaleType::Major)
            scale = ScaleEngine::getScale(currentTonic, ScaleType::Major);
        else
        {
            scale = ScaleEngine::getScale(currentTonic, ScaleType::NaturalMinor);
            if (degree == 5)
            {
                int seventhIdx = 6;
                int orig = static_cast<int>(scale[seventhIdx]);
                scale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
            }
        }

        try
        {
            Chord extended = ScaleEngine::getChord(scale, degree, numNotes);
            card->setExtendedChord(extNumber, extended);
            if (extended.intervals.size() > 1 && extended.intervals[1] == 3)
                card->setChordQualitySilent(ChordQuality::MinorTriad);
            else
                card->setChordQualitySilent(ChordQuality::MajorTriad);
            juce::String symbol = getChordSymbol(extended.root, function, (currentType == ScaleType::Major));
            card->setChordSymbol(symbol);
            card->setChineseName(getChineseChordName(symbol));
        }
        catch (...)
        {
            juce::Logger::writeToLog("ScaledContentComponent::updateChordCardFromFunction failed to rebuild chord");
        }
    }
}

//==============================================================================
// 刷新键盘高亮
//==============================================================================
void ScaledContentComponent::refreshKeyboardHighlight()
{
    std::vector<int> notes;
    for (auto* card : chordCards) {
        auto cn = card->getCurrentNotes();
        notes.insert(notes.end(), cn.begin(), cn.end());
    }
    std::sort(notes.begin(), notes.end());
    notes.erase(std::unique(notes.begin(), notes.end()), notes.end());
    miniKeyboard.setHighlightedNotes(notes);
}

//==============================================================================
// 信息栏文字
//==============================================================================
juce::String ScaledContentComponent::getKeyInfoText() const
{
    juce::String tonicName = PitchNameHelper::getNameForPitch(currentTonic, isSharpKey);
    juce::String typeName = (currentType == ScaleType::Major) 
        ? TR("major")
        : TR("minor");

    int sharpsOrFlats = 0;
    std::map<PitchClass, int> majorKeySignatures = {
        {PitchClass::C, 0}, {PitchClass::G, 1}, {PitchClass::D, 2}, {PitchClass::A, 3},
        {PitchClass::E, 4}, {PitchClass::B, 5}, {PitchClass::F_Sharp, 6}, {PitchClass::C_Sharp, 7},
        {PitchClass::F, -1}, {PitchClass::B_Flat, -2}, {PitchClass::E_Flat, -3}, {PitchClass::A_Flat, -4},
        {PitchClass::D_Flat, -5}, {PitchClass::G_Flat, -6}, {PitchClass::C_Flat, -7}
    };

    if (currentType == ScaleType::Major)
    {
        auto it = majorKeySignatures.find(currentTonic);
        if (it != majorKeySignatures.end()) sharpsOrFlats = it->second;
    }
    else
    {
        PitchClass relMajor = ScaleEngine::getRelativeMajor(currentTonic);
        auto it = majorKeySignatures.find(relMajor);
        if (it != majorKeySignatures.end()) sharpsOrFlats = it->second;
    }

    juce::String keySigStr;
    if (sharpsOrFlats == 0)
        keySigStr = TR("no_accidentals");
    else if (sharpsOrFlats > 0)
        keySigStr = juce::String(sharpsOrFlats) + TR("sharps");
    else
        keySigStr = juce::String(-sharpsOrFlats) + TR("flats");

    juce::String info = tonicName + " " + typeName 
        + TR("key_sig") + keySigStr;

    if (currentFunction.isNotEmpty())
    {
        info += TR("chord") + currentFunction;
        if (currentChordSymbol.isNotEmpty())
            info += " (" + currentChordSymbol + ")";
    }

    info += TR("info_tip");
    return info;
}

//==============================================================================
// 和弦符号生成（基于功能名）
//==============================================================================
juce::String ScaledContentComponent::getChordSymbol(PitchClass root, const juce::String& funcName, bool isMajor) const
{
    return ChordNaming::getSymbolFromFunction(root, isSharpKey, funcName, isMajor);
}

//==============================================================================
// 和弦符号转中文名称
//==============================================================================
juce::String ScaledContentComponent::getChineseChordName(const juce::String& symbol)
{
    return ChordNaming::getChineseName(symbol);
}
//==============================================================================
// 皮肤切换函数
//==============================================================================
void ScaledContentComponent::setSkin(SkinType newSkin)
{
    if (currentSkin != newSkin)
    {
        currentSkin = newSkin;
        
        // ★ 立即同步所有子组件的皮肤，不依赖 updateDisplay()
        circle.setSkin(newSkin);
        for (auto* card : chordCards)
            card->setSkin(newSkin);
        miniKeyboard.setSkin(newSkin);
        
        brandLink.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.4f));
        juce::Timer::callAfterDelay(300, [this] { repaint(); });
        repaint();
    }
}
//==============================================================================
//语言切换函数
//==============================================================================
void ScaledContentComponent::setLanguage(int lang)
{
    if (currentLanguage != lang)
    {
        currentLanguage = lang;
        updateDisplay();
        repaint();
    }
}
//==============================================================================
// 手动覆盖标志
//==============================================================================
void ScaledContentComponent::setManualOverride(bool shouldOverride)
{
    manualOverride = shouldOverride;
}

//==============================================================================
// 从当前卡片状态恢复扩展和弦显示
//==============================================================================
void ScaledContentComponent::refreshCardsFromState()
{
    for (int i = 0; i < chordCards.size(); ++i)
    {
        auto* card = chordCards[i];
        int degree = (i == 0) ? 1 : (i == 1) ? 4 : 5;
        ChordExtension chordTypeTag = card->getChordType();
        int numNotes = ChordCard::getNumNotesForType(chordTypeTag);
        
        bool isMajor = (currentType == ScaleType::Major);
        std::vector<PitchClass> scale;
        if (isMajor)
            scale = ScaleEngine::getScale(currentTonic, ScaleType::Major);
        else
        {
            scale = ScaleEngine::getScale(currentTonic, ScaleType::NaturalMinor);
            if (degree == 5)
            {
                int seventhIdx = 6;
                int orig = static_cast<int>(scale[seventhIdx]);
                scale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
            }
        }
        
        try
        {
            Chord extended = ScaleEngine::getChord(scale, degree, numNotes);
            card->setExtendedChord(chordTypeTag, extended);
            
            juce::String funcName = (i == 0) ? (isMajor ? "T" : "t") :
                                    (i == 1) ? (isMajor ? "S" : "s") : "D";
            funcName += juce::String(static_cast<int>(chordTypeTag));
            juce::String symbol = getChordSymbol(extended.root, funcName, isMajor);
            card->setChordSymbol(symbol);
            card->setChineseName(getChineseChordName(symbol));
        }
        catch (...)
        {
            juce::Logger::writeToLog("ScaledContentComponent::refreshCardsFromState failed to rebuild chord");
        }
    }
    refreshKeyboardHighlight();
    repaint();
}

//==============================================================================
// 应用智能生成的进行
//==============================================================================
void ScaledContentComponent::applyGeneratedProgression(const juce::StringArray& functions)
{
    if (functions.size() < 3) return;

    setManualOverride(false);

    bool isMajor = (currentType == ScaleType::Major);
    auto scale = ScaleEngine::getScale(currentTonic, currentType);

    for (int i = 0; i < 3; ++i)
    {
        ChordCard* card = chordCards[i];
        int degree = 0;
        juce::String func = functions[i];

        if (func.startsWith("T")) degree = 1;
        else if (func.startsWith("S")) degree = 4;
        else if (func.startsWith("D")) degree = 5;
        else if (func.startsWith("vi")) degree = 6;

        int numNotes = 3;
        if (func.length() > 1 && func[1] >= '0' && func[1] <= '9')
        {
            numNotes = func.substring(1).getIntValue();
            if (numNotes < 3) numNotes = 3;
        }

        Chord extended;
        try {
            extended = ScaleEngine::getChord(scale, degree, numNotes);
        } catch (...) { continue; }

        // ★ 补完功能名和罗马数字
        extended.name = func;   // 完整功能字符串，如 "T", "S7", "D", "vi" 等
        juce::String funcBase = func.substring(0, 1);
        if (func.startsWith("vi")) funcBase = "vi";

        if (funcBase == "T")           extended.roman = "I";
        else if (funcBase == "S")      extended.roman = "IV";
        else if (funcBase == "D")      extended.roman = "V";
        else if (funcBase == "t")      extended.roman = "i";
        else if (funcBase == "s")      extended.roman = "iv";
        else if (funcBase == "v")      extended.roman = "v";
        else if (funcBase == "vi")     extended.roman = "vi";
        else                           extended.roman = "?";

        card->setChord(extended, isSharpKey, isMajor);
        card->setChordQualitySilent(isMajor ? ChordQuality::MajorTriad : ChordQuality::MinorTriad);
            if (numNotes > 3) card->setChordType(intToChordExtension(numNotes));

        juce::String symbol = getChordSymbol(extended.root, func, isMajor);
        card->setChordSymbol(symbol);
        card->setChineseName(getChineseChordName(symbol));
    }

    refreshKeyboardHighlight();

    juce::String info;
    for (auto& f : functions) info += f + " ";
    smartGenInfoText = LocalisedStrings::translate("auto_new_progression") + info.trim();
    smartGenDisplayTicks = 30;

    // 同步智能生成序列与风格到 Processor（用于窗口重开恢复）
    if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(audioProcessorRef))
    {
        proc->setLastGeneratedFunctions(functions);
        proc->setLastGeneratedStyle(currentSmartStyle);

        // 同步卡片功能名（确保恢复时知道每个卡片的 T/S/D 身份）
        juce::String names[3];
        for (int i = 0; i < 3; ++i)
            names[i] = chordCards[i]->getFunctionName();
        proc->setCardFunctionNames(names[0], names[1], names[2]);
    }

    repaint();
}

//==============================================================================
// 强制刷新显示（外部调用）
//==============================================================================
void ScaledContentComponent::updateDisplay()
{
    if (updateLocked) return;
    if (manualOverride)
    {
        refreshKeyboardHighlight();
        repaint();
        return;
    }
    static bool isUpdating = false;
    if (isUpdating) return;
    
    struct Guard { Guard() { isUpdating = true; } ~Guard() { isUpdating = false; } } guard;

    miniKeyboard.setSkin(currentSkin);

    TSDFamily family = TSDBuilder::getFamily(currentTonic, currentType);

    bool isMajor = (currentType == ScaleType::Major ||
                    currentType == ScaleType::Lydian ||
                    currentType == ScaleType::Mixolydian);
    bool domSharpKey = (currentType == ScaleType::Major) ? isSharpKey : true;

    bool isNH = false;
    if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(audioProcessorRef))
        isNH = proc->isNegativeHarmonyEnabled();

    if (isNH)
    {
        // 辅助函数：翻转一个 Chord
        auto flipChordData = [&](Chord chord) -> Chord {
            int rootPc = static_cast<int>(chord.root);
            std::vector<int> origNotes;
            for (int interval : chord.intervals)
                origNotes.push_back((rootPc + interval) % 12);
            std::sort(origNotes.begin(), origNotes.end());
            origNotes.erase(std::unique(origNotes.begin(), origNotes.end()), origNotes.end());
            std::vector<int> flipped = NegativeHarmony::flipChord(origNotes, currentTonic);
            int newRoot = flipped[0];
            std::vector<int> newIntervals;
            for (int note : flipped)
                newIntervals.push_back((note - newRoot + 12) % 12);
            Chord res;
            res.root = static_cast<PitchClass>(newRoot);
            res.intervals = newIntervals;
            res.name = chord.name;
            res.roman = chord.roman;
            return res;
        };
        family.tonic = flipChordData(family.tonic);
        family.subdominant = flipChordData(family.subdominant);
        family.dominant = flipChordData(family.dominant);

        chordCards[0]->setChord(family.tonic, isSharpKey, isMajor);
        chordCards[1]->setChord(family.subdominant, isSharpKey, isMajor);
        chordCards[2]->setChord(family.dominant, domSharpKey, isMajor);

        chordCards[0]->setChordQualitySilent(ChordQuality::MajorTriad);
        chordCards[1]->setChordQualitySilent(ChordQuality::MajorTriad);
        chordCards[2]->setChordQualitySilent(ChordQuality::MajorTriad);

        chordCards[0]->setChordTypeSilent(ChordExtension::Triad);
        chordCards[1]->setChordTypeSilent(ChordExtension::Triad);
        chordCards[2]->setChordTypeSilent(ChordExtension::Triad);

        chordCards[0]->setChordSymbol(juce::String(PitchNameHelper::getNameForPitch(family.tonic.root, isSharpKey)) + " NH");
        chordCards[1]->setChordSymbol(juce::String(PitchNameHelper::getNameForPitch(family.subdominant.root, isSharpKey)) + " NH");
        chordCards[2]->setChordSymbol(juce::String(PitchNameHelper::getNameForPitch(family.dominant.root, isSharpKey)) + " NH");
        for (auto* card : chordCards) card->setChineseName(LocalisedStrings::translate("auto_negative_harmony"));
    } else {
        chordCards[0]->setChord(family.tonic, isSharpKey, isMajor);
        chordCards[1]->setChord(family.subdominant, isSharpKey, isMajor);
        chordCards[2]->setChord(family.dominant, domSharpKey, isMajor);

        auto defaultQualityFor = [](const Chord& chord) -> ChordQuality
        {
            if (chord.intervals.size() > 1 && chord.intervals[1] == 3)
                return ChordQuality::MinorTriad;
            return ChordQuality::MajorTriad;
        };

        chordCards[0]->setChordQualitySilent(defaultQualityFor(family.tonic));
        chordCards[1]->setChordQualitySilent(defaultQualityFor(family.subdominant));
        chordCards[2]->setChordQualitySilent(defaultQualityFor(family.dominant));

        // 明确设置默认三和弦类型
        chordCards[0]->setChordTypeSilent(ChordExtension::Triad);
        chordCards[1]->setChordTypeSilent(ChordExtension::Triad);
        chordCards[2]->setChordTypeSilent(ChordExtension::Triad);

        juce::String sym0 = getChordSymbol(family.tonic.root, family.tonic.name, isMajor);
        juce::String sym1 = getChordSymbol(family.subdominant.root, family.subdominant.name, isMajor);
        juce::String sym2 = getChordSymbol(family.dominant.root, family.dominant.name, isMajor);
        chordCards[0]->setChordSymbol(sym0); chordCards[0]->setChineseName(getChineseChordName(sym0));
        chordCards[1]->setChordSymbol(sym1); chordCards[1]->setChineseName(getChineseChordName(sym1));
        chordCards[2]->setChordSymbol(sym2); chordCards[2]->setChineseName(getChineseChordName(sym2));
    }

    for (auto* card : chordCards) card->setSkin(currentSkin);
    refreshKeyboardHighlight();
    repaint();
}
//==============================================================================
// 获取卡片状态快照
//==============================================================================
ScaledContentComponent::CardsState ScaledContentComponent::getCardsState() const
{
    CardsState state;
    if (chordCards.size() >= 3)
    {
        for (int i = 0; i < 3; ++i)
        {
            state.inversions[i]        = chordCards[i]->getCurrentInversion();
            state.chordTypes[i]        = chordCards[i]->getChordType();
            state.qualities[i]         = chordCards[i]->getChordQuality();
            state.extensionSuffixes[i] = chordCards[i]->getExtensionSuffix();
            state.functionNames[i]     = chordCards[i]->getFunctionName();
        }
    }
    return state;
}

//==============================================================================
// 恢复卡片状态
//==============================================================================
void ScaledContentComponent::restoreCardsState(const CardsState& state)
{
    if (chordCards.size() < 3) return;

    PitchClass tonic = currentTonic;
    ScaleType type   = currentType;

    for (int i = 0; i < 3; ++i)
    {
        auto* card = chordCards[i];

        // 构建扩展和弦
        juce::String funcName = state.functionNames[i];
        int degree = 0;
        if (funcName.startsWith("T") || funcName.startsWith("t")) degree = 1;
        else if (funcName.startsWith("S") || funcName.startsWith("s")) degree = 4;
        else if (funcName.startsWith("D") || funcName.startsWith("v")) degree = 5;
        else degree = 6;

        auto scale = ScaleEngine::getScale(tonic, type);
        Chord extended = ScaleEngine::getChord(
            scale, degree,
            ChordCard::getNumNotesForType(state.chordTypes[i]),
            state.qualities[i]);

        extended.name = funcName;
        juce::String funcBase = funcName.substring(0, 1);
        if (funcBase == "T")           extended.roman = "I";
        else if (funcBase == "S")      extended.roman = "IV";
        else if (funcBase == "D")      extended.roman = "V";
        else if (funcBase == "t")      extended.roman = "i";
        else if (funcBase == "s")      extended.roman = "iv";
        else if (funcBase == "v")      extended.roman = "v";
        else if (funcBase == "vi")     extended.roman = "vi";
        else                           extended.roman = "?";

        // 直接设置和弦数据，不通过 setChord（避免重置 chordType）
        card->setChordQualitySilent(state.qualities[i]);
        if (state.extensionSuffixes[i].isNotEmpty())
            card->setExtensionSuffixSilent(state.extensionSuffixes[i]);
        else
            card->clearExtensionSuffixSilent();
        card->setChordTypeSilent(state.chordTypes[i]);
        card->setInversion(state.inversions[i]);

        // 设置扩展和弦
        card->setExtendedChord(state.chordTypes[i], extended);

        juce::String symbol = ChordNaming::getSymbol(
            extended.root, isSharpKey,
            state.qualities[i],
            ChordCard::getNumNotesForType(state.chordTypes[i]),
            state.extensionSuffixes[i]);
        card->setChordSymbol(symbol);
        card->setChineseName(ChordNaming::getChineseName(symbol));
    }

    refreshKeyboardHighlight();
    setManualOverride(true);
}

void ScaledContentComponent::refreshNegativeHarmony()
{
    if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(audioProcessorRef))
    {
        if (!proc->isNegativeHarmonyEnabled() && manualOverride)
        {
            refreshKeyboardHighlight();
            repaint();
            return;
        }
    }

    manualOverride = false;
    updateDisplay();
}
