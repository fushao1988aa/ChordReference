//==================================================================================================
//  PluginEditor.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：插件编辑器界面实现
//        - 初始化 TopBar 与 ScaledContentComponent
//        - 连接所有回调（MIDI、皮肤、语言、锁调、试听、预设等）
//        - 定时获取 MIDI 识别结果并更新界面
//        - 注册检查与覆盖层管理
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "PluginEditor.h"
#include "RegistrationManager.h"
#include "PitchNameHelper.h"
#include "ScaleEngine.h"
#include "LocalisedStrings.h"
#define TR(key) LocalisedStrings::translate(key)

//==============================================================================
// 构造函数
//==============================================================================
ChordReferenceAudioProcessorEditor::ChordReferenceAudioProcessorEditor(ChordReferenceAudioProcessor& p)
    : AudioProcessorEditor(&p)
{
    setWantsKeyboardFocus(true);
    setResizable(true, true);
    setResizeLimits(300, 230, 2400, 1600);
    
    auto savedSize = p.getEditorSize();
    setSize(savedSize.first, savedSize.second);

    RegistrationManager::getInstance().checkFirstRun();
    addAndMakeVisible(topBar);

    topBar.refreshMidiDevices(p.getAvailableMidiDeviceNames(), p.getAvailableMidiDeviceIdentifiers());

    //---- MIDI 设备选择回调 ----
    topBar.midiDeviceSelector.onChange = [this, &p]() {
        int selectedId = topBar.midiDeviceSelector.getSelectedId();
        juce::String identifier = topBar.getMidiDeviceIdentifierForId(selectedId);
        p.setMidiDeviceByIdentifier(identifier);
        p.setCurrentMidiDeviceIdentifier(identifier);
    };

    //---- MIDI 输出开关回调 ----
    topBar.midiOutToggle.onStateChange = [&p, this] {
        p.setMidiOutputEnabled(topBar.midiOutToggle.getToggleState());
    };

    //---- 预设按钮回调 ----
    topBar.onPresetClicked = [this, &p]() {
        if (progressionBrowser && progressionBrowser->isVisible())
        {
            hidePresetBrowser();
            p.setPresetBrowserOpen(false);
        }
        else
        {
            showPresetBrowser();
            p.setPresetBrowserOpen(true);
        }
    };

    //---- 皮肤切换回调 ----
    topBar.onSkinToggle = [this, &p]() {
        if (scaledContent)
        {
            SkinType current = scaledContent->getCurrentSkin();
            SkinType next = SkinType::FluidPurpleOrange;
            switch (current)
            {
            case SkinType::FluidPurpleOrange: next = SkinType::FluidLightShadow; break;
            case SkinType::FluidLightShadow:  next = SkinType::IvorySatin; break;
            case SkinType::IvorySatin:        next = SkinType::RoyalBlackGold; break;
            case SkinType::RoyalBlackGold:    next = SkinType::FluidPurpleOrange; break;
            default: break;
            }
            scaledContent->setSkin(next);
            topBar.setSkin(next);
            if (progressionBrowser)
                progressionBrowser->setSkin(next); 
            p.setSkinType(next);
        }
    };

    topBar.onHelpClicked = [this] { showHelpDialog(); };
    topBar.onRegistrationClicked = [this]() { showRegistrationOverlay(); };

    //---- ScaledContent 创建与回调 ----
    scaledContent = std::make_unique<ScaledContentComponent>();
    scaledContent->setAudioProcessor(&p);
    scaledContent->setSmartStyle(p.getSmartStyle());
    addAndMakeVisible(scaledContent.get());

    scaledContent->onKeyChanged = [this, &p](PitchClass tonic, ScaleType type) {
        p.setCurrentKey(tonic, type);

        // 同步顶部调式下拉框
        int id = 1;
        switch (type) {
            case ScaleType::Major:        id = 1; break;
            case ScaleType::NaturalMinor: id = 2; break;
            case ScaleType::Dorian:       id = 3; break;
            case ScaleType::Phrygian:     id = 4; break;
            case ScaleType::Lydian:       id = 5; break;
            case ScaleType::Mixolydian:   id = 6; break;
            case ScaleType::Locrian:      id = 7; break;
        }
        topBar.scaleTypeSelector.setSelectedId(id, juce::dontSendNotification);
    };

    topBar.setSkin(scaledContent->getCurrentSkin());

    // ---- 根据 Processor 中的调式设置下拉框初始值 ----
    ScaleType initType = p.getCurrentScaleType();
    int initTypeId = 1;
    switch (initType)
    {
        case ScaleType::Major:        initTypeId = 1; break;
        case ScaleType::NaturalMinor: initTypeId = 2; break;
        case ScaleType::Dorian:       initTypeId = 3; break;
        case ScaleType::Phrygian:     initTypeId = 4; break;
        case ScaleType::Lydian:       initTypeId = 5; break;
        case ScaleType::Mixolydian:   initTypeId = 6; break;
        case ScaleType::Locrian:      initTypeId = 7; break;
    }
    topBar.scaleTypeSelector.setSelectedId(initTypeId, juce::dontSendNotification);

    // 根据 Processor 中保存的语言设置按钮文字
    bool isCN = (p.getCurrentLanguage() == 0);
    topBar.languageButton.setButtonText(isCN ? "CN" : "EN");

    bool reg = RegistrationManager::getInstance().isRegistered();
    int daysLeft = RegistrationManager::getInstance().getDaysLeft();
    topBar.setRegistrationStatus(reg, daysLeft);

    checkRegistrationAndShowOverlay();

    dynamic_cast<ChordReferenceAudioProcessor&>(processor).setEditor(this);

    //---- 试听模式选择回调 ----
    topBar.auditionModeSelector.onChange = [this, &p]() {
        int id = topBar.auditionModeSelector.getSelectedId();
        p.setAuditionMode(id - 1);
    };

    //---- 语言切换回调 ----
    topBar.onLanguageToggle = [this, &p]() {
    bool isCN = (topBar.languageButton.getButtonText() == "CN");
    if (isCN)
        p.setCurrentLanguage(0);
    else
        p.setCurrentLanguage(1);

    RegistrationManager::getInstance().saveLanguagePreference(p.getCurrentLanguage());
    // 新增：重新加载本地化字符串
    LocalisedStrings::loadLanguage(p.getCurrentLanguage());

    if (scaledContent)
    {
        scaledContent->setLanguage(p.getCurrentLanguage());
        scaledContent->setSkin(scaledContent->getCurrentSkin());
    }
    topBar.refreshLocalisedText();
    topBar.refreshMidiDevices(p.getAvailableMidiDeviceNames(), p.getAvailableMidiDeviceIdentifiers());
    topBar.setRegistrationStatus(RegistrationManager::getInstance().isRegistered(),
                                 RegistrationManager::getInstance().getDaysLeft());
    if (progressionBrowser)
    {
        hidePresetBrowser();
        showPresetBrowser();
    }
    topBar.repaint();
    repaint();
};

    //---- 调式选择回调 ----
    topBar.scaleTypeSelector.onChange = [this, &p] {
    int id = topBar.scaleTypeSelector.getSelectedId();
    ScaleType types[] = {
        ScaleType::Major, ScaleType::NaturalMinor, ScaleType::Dorian,
        ScaleType::Phrygian, ScaleType::Lydian, ScaleType::Mixolydian, ScaleType::Locrian
    };
    if (id >= 1 && id <= 7)
    {
        ScaleType newType = types[id - 1];
        if (scaledContent)
            scaledContent->setKey(p.getCurrentTonic(), newType);
        // Processor 的更新由 scaledContent->onKeyChanged 回调自动完成
    }
};

    //---- 锁调开关回调 ----
    topBar.lockToggle.onStateChange = [&p, this] { p.setKeyLocked(topBar.lockToggle.getToggleState()); };

    //---- 负和声开关回调 ----
    topBar.negativeHarmonyToggle.onStateChange = [&p, this] {
        p.setNegativeHarmonyEnabled(topBar.negativeHarmonyToggle.getToggleState());
        if (scaledContent)
            scaledContent->refreshNegativeHarmony();
    };

    // 默认开启 Midi Out 并设为内置试听
    // 从 Processor 恢复 MIDI 输出和试听模式（实例存活期间保持）
    topBar.midiOutToggle.setToggleState(p.isMidiOutputEnabled(), juce::dontSendNotification);
    topBar.auditionModeSelector.setSelectedId(p.getAuditionMode() + 1, juce::dontSendNotification);

    juce::Component::SafePointer<ChordReferenceAudioProcessorEditor> safeThis(this);
    juce::MessageManager::callAsync([safeThis] {
        if (safeThis != nullptr)
            safeThis->restoreUIState();
    });

    startTimerHz(10);
    resized();
}

//==============================================================================
// 析构函数卡片状态保存
//==============================================================================
ChordReferenceAudioProcessorEditor::~ChordReferenceAudioProcessorEditor()
{
    stopTimer();
    progressionBrowser.reset();
    registrationOverlay.reset();

    auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);
    proc.setEditor(nullptr);

    if (scaledContent)
    {
        auto state = scaledContent->getCardsState();
        proc.setCardInversions(state.inversions[0], state.inversions[1], state.inversions[2]);
            proc.setCardChordTypes(static_cast<int>(state.chordTypes[0]),
                                   static_cast<int>(state.chordTypes[1]),
                                   static_cast<int>(state.chordTypes[2]));
        proc.setCardChordQualities(state.qualities[0], state.qualities[1], state.qualities[2]);
        proc.setCardExtensionSuffixes(state.extensionSuffixes[0], state.extensionSuffixes[1], state.extensionSuffixes[2]);
        proc.setCardFunctionNames(state.functionNames[0], state.functionNames[1], state.functionNames[2]);
    }
}
//==============================================================================
// 界面布局
//==============================================================================
void ChordReferenceAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    topBar.setBounds(area.removeFromTop(30));

    if (progressionBrowser && progressionBrowser->isVisible())
    {
        progressionBrowser->setBounds(area);
        return;
    }

    if (scaledContent)
    {
        scaledContent->setBounds(area);
        scaledContent->resized();
        scaledContent->repaint();
    }
    if (registrationOverlay)
        registrationOverlay->setBounds(getLocalBounds());

    if (auto* proc = dynamic_cast<ChordReferenceAudioProcessor*>(&processor))
        proc->setEditorSize(getWidth(), getHeight());
}

//==============================================================================
// 窗口显示时恢复尺寸
//==============================================================================
void ChordReferenceAudioProcessorEditor::visibilityChanged()
{
    if (isVisible())
    {
        auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);
        auto size = proc.getEditorSize();
        setSize(size.first, size.second);
    }
}

bool ChordReferenceAudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);

    if (progressionBrowser && progressionBrowser->isVisible() && progressionBrowser->handleShortcut(key))
        return true;

    if (key == juce::KeyPress::F1Key || key.getTextCharacter() == '?')
    {
        showHelpDialog();
        return true;
    }

    auto ch = juce::CharacterFunctions::toLowerCase(key.getTextCharacter());
    if (ch == 'p')
    {
        if (progressionBrowser && progressionBrowser->isVisible())
            hidePresetBrowser();
        else
            showPresetBrowser();
        proc.setPresetBrowserOpen(progressionBrowser != nullptr && progressionBrowser->isVisible());
        return true;
    }

    if (ch == 's')
    {
        topBar.skinButton.triggerClick();
        return true;
    }

    if (ch == 'l')
    {
        bool next = !topBar.lockToggle.getToggleState();
        topBar.lockToggle.setToggleState(next, juce::sendNotification);
        return true;
    }

    if (ch == 'n')
    {
        bool next = !topBar.negativeHarmonyToggle.getToggleState();
        topBar.negativeHarmonyToggle.setToggleState(next, juce::sendNotification);
        return true;
    }

    if (ch == 'm')
    {
        bool next = !topBar.midiOutToggle.getToggleState();
        topBar.midiOutToggle.setToggleState(next, juce::sendNotification);
        return true;
    }

    if (key == juce::KeyPress::escapeKey && progressionBrowser && progressionBrowser->isVisible())
    {
        hidePresetBrowser();
        proc.setPresetBrowserOpen(false);
        return true;
    }

    return false;
}

//==============================================================================
// 定时器：检查 MIDI 活动与识别结果
//==============================================================================
void ChordReferenceAudioProcessorEditor::timerCallback()
{
    auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);
    juce::Component::SafePointer<ScaledContentComponent> safeContent(scaledContent.get());

    if (++midiDeviceRefreshTicks >= 20)
    {
        midiDeviceRefreshTicks = 0;
        const auto selectedIdentifier = proc.getCurrentMidiDeviceIdentifier();

        if (proc.refreshAvailableMidiDevices())
        {
            topBar.refreshMidiDevices(proc.getAvailableMidiDeviceNames(), proc.getAvailableMidiDeviceIdentifiers());

            int selectedId = 1;
            if (selectedIdentifier.isNotEmpty())
            {
                for (int i = 2; i <= topBar.midiDeviceSelector.getNumItems(); ++i)
                {
                    if (topBar.getMidiDeviceIdentifierForId(i) == selectedIdentifier)
                    {
                        selectedId = i;
                        break;
                    }
                }
            }

            topBar.midiDeviceSelector.setSelectedId(selectedId, juce::dontSendNotification);
            if (selectedId == 1 && selectedIdentifier.isNotEmpty())
            {
                proc.setMidiDeviceByIdentifier({});
                proc.setCurrentMidiDeviceIdentifier({});
            }
        }
    }

    bool active = proc.hasMidiActivity();
    topBar.midiIndicator.setActive(active);
    if (active) proc.clearMidiActivityFlag();

    auto result = proc.getLastMidiChordResult();
    bool locked = topBar.isKeyLocked();

    if (result.hasResult)
    {
        midiResultReceived = true;

        if (safeContent != nullptr)
        {
            if (locked)
                safeContent->setChordOnly(result.function);
            else
                safeContent->setKeyAndChord(result.tonic, result.scale, result.function);

            safeContent->setMidiExpression(result.notes,
                                           result.velocities,
                                           result.averageVelocity,
                                           result.sustainPedalDown);
        }
    }

    if (safeContent != nullptr)
    {
        auto state = safeContent->getCardsState();

        // 仅当状态发生变化时才写回 Processor
        bool changed = false;
        for (int i = 0; i < 3; ++i)
        {
            if (state.inversions[i] != lastSavedCardState.inv[i] ||
                state.chordTypes[i] != lastSavedCardState.typ[i] ||
                state.qualities[i] != lastSavedCardState.quals[i] ||
                state.extensionSuffixes[i] != lastSavedCardState.suffixes[i] ||
                state.functionNames[i] != lastSavedCardState.funcNames[i])
            {
                changed = true;
                break;
            }
        }

        if (changed)
        {
            proc.setCardInversions(state.inversions[0], state.inversions[1], state.inversions[2]);
            proc.setCardChordTypes(static_cast<int>(state.chordTypes[0]),
                                   static_cast<int>(state.chordTypes[1]),
                                   static_cast<int>(state.chordTypes[2]));
            proc.setCardChordQualities(state.qualities[0], state.qualities[1], state.qualities[2]);
            proc.setCardExtensionSuffixes(state.extensionSuffixes[0], state.extensionSuffixes[1], state.extensionSuffixes[2]);
            proc.setCardFunctionNames(state.functionNames[0], state.functionNames[1], state.functionNames[2]);

            // 更新缓存
            for (int i = 0; i < 3; ++i)
            {
                lastSavedCardState.inv[i]        = state.inversions[i];
                lastSavedCardState.typ[i]        = state.chordTypes[i];
                lastSavedCardState.quals[i]      = state.qualities[i];
                lastSavedCardState.suffixes[i]   = state.extensionSuffixes[i];
                lastSavedCardState.funcNames[i]  = state.functionNames[i];
            }
        }
    }
} 
//==============================================================================
// 注册覆盖层
//==============================================================================
void ChordReferenceAudioProcessorEditor::checkRegistrationAndShowOverlay()
{
    auto& regMgr = RegistrationManager::getInstance();
    if (!regMgr.isRegistered() && regMgr.isExpired())
        showRegistrationOverlay();
}

void ChordReferenceAudioProcessorEditor::showRegistrationOverlay()
{
    if (registrationOverlay) return;

    registrationOverlay = std::make_unique<RegistrationOverlay>();
    registrationOverlay->onRegistrationSuccess = [this]()
    {
        registrationOverlay.reset();
        bool reg = RegistrationManager::getInstance().isRegistered();
        int daysLeft = RegistrationManager::getInstance().getDaysLeft();
        topBar.setRegistrationStatus(reg, daysLeft);
        repaint();
    };
    addAndMakeVisible(registrationOverlay.get());
    registrationOverlay->setBounds(getLocalBounds());
}

//==============================================================================
// 状态恢复（从 Processor 同步 UI）
//==============================================================================
void ChordReferenceAudioProcessorEditor::restoreUIState()
{
    auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);

    // 加锁，防止恢复过程中的操作触发 updateDisplay 重置卡片状态
    if (scaledContent) scaledContent->lockUpdates();

    SkinType savedSkin = proc.getSkinType();
    topBar.setSkin(savedSkin);
    if (scaledContent) scaledContent->setSkin(savedSkin);

    scaledContent->setKey(proc.getCurrentTonic(), proc.getCurrentScaleType());
    // 根据 Processor 中记忆的调式，设置调式选择器 (scaleTypeSelector) 的选中项
    ScaleType savedType = proc.getCurrentScaleType();
    int typeIndex = 0; // 默认大调
    switch (savedType)
    {
        case ScaleType::Major:        typeIndex = 1; break;
        case ScaleType::NaturalMinor: typeIndex = 2; break;
        case ScaleType::Dorian:       typeIndex = 3; break;
        case ScaleType::Phrygian:     typeIndex = 4; break;
        case ScaleType::Lydian:       typeIndex = 5; break;
        case ScaleType::Mixolydian:   typeIndex = 6; break;
        case ScaleType::Locrian:      typeIndex = 7; break;
        default:                      typeIndex = 1; break;
    }
    topBar.scaleTypeSelector.setSelectedId(typeIndex, juce::dontSendNotification);

    topBar.lockToggle.setToggleState(proc.isKeyLocked(), juce::dontSendNotification);

    topBar.negativeHarmonyToggle.setToggleState(proc.isNegativeHarmonyEnabled(), juce::dontSendNotification);

    // 恢复智能生成风格
    int style = proc.getLastGeneratedStyle();
    if (scaledContent) scaledContent->setSmartStyle(style);

    // 恢复卡片全部状态（inversion, chordType, quality, suffix）
    // 恢复卡片全部状态
    if (scaledContent)
    {
        ScaledContentComponent::CardsState state;
        auto inv = proc.getCardInversions();
        auto typ = proc.getCardChordTypes();
        auto quals = proc.getCardChordQualities();
        auto suffixes = proc.getCardExtensionSuffixes();
        auto funcNames = proc.getCardFunctionNames();

        for (int i = 0; i < 3; ++i)
        {
            state.inversions[i]        = inv[i];
            state.chordTypes[i]        = static_cast<ChordExtension>(typ[i]);
            state.qualities[i]         = quals[i];
            state.extensionSuffixes[i] = suffixes[i];
            state.functionNames[i]     = funcNames[i];
        }
        scaledContent->restoreCardsState(state);
    }
    
    // ★ 新增：恢复试听模式、MIDI 输出、语言按钮、智能生成风格
    topBar.auditionModeSelector.setSelectedId(proc.getAuditionMode() + 1, juce::dontSendNotification);
    topBar.midiOutToggle.setToggleState(proc.isMidiOutputEnabled(), juce::dontSendNotification);
    bool isCN = (proc.getCurrentLanguage() == 0);
    topBar.languageButton.setButtonText(isCN ? "CN" : "EN");
    if (scaledContent) scaledContent->setSmartStyle(proc.getSmartStyle());
    

    // 恢复 MIDI 设备选择
    juce::String savedID = proc.getCurrentMidiDeviceIdentifier();
    if (savedID.isNotEmpty())
    {
        for (int i = 2; i <= topBar.midiDeviceSelector.getNumItems(); ++i)
        {
            if (topBar.getMidiDeviceIdentifierForId(i) == savedID)
            {
                topBar.midiDeviceSelector.setSelectedId(i, juce::dontSendNotification);
                break;
            }
        }
    }
    else
    {
        topBar.midiDeviceSelector.setSelectedId(1, juce::dontSendNotification);
    }

    if (scaledContent) scaledContent->unlockUpdates();
    if (scaledContent)
    {
        scaledContent->circle.setCurrentKey(proc.getCurrentTonic(), proc.getCurrentScaleType());
        scaledContent->circle.repaint();
    }
}

//==============================================================================
// 预设浏览器显示与隐藏
//==============================================================================
void ChordReferenceAudioProcessorEditor::showPresetBrowser()
{
    if (progressionBrowser != nullptr)
    {
        progressionBrowser->setVisible(true);
        return;
    }

    progressionBrowser = std::make_unique<ProgressionBrowser>(
        dynamic_cast<ChordReferenceAudioProcessor&>(processor),
        progressionData,
        [this]() -> PitchClass {
            return scaledContent ? scaledContent->getCurrentTonic() : PitchClass::C;
        },
        [this]() -> ScaleType {
            return scaledContent ? scaledContent->getCurrentScaleType() : ScaleType::Major;
        }
    );

    if (scaledContent)
        progressionBrowser->setSkin(scaledContent->getCurrentSkin());

    addAndMakeVisible(progressionBrowser.get());
    progressionBrowser->setBounds(getLocalBounds());
    progressionBrowser->grabKeyboardFocus();

    progressionBrowser->onClose = [this]() { hidePresetBrowser(); };
}

void ChordReferenceAudioProcessorEditor::hidePresetBrowser()
{
    progressionBrowser.reset();
    resized();
}

//==============================================================================
// 帮助对话框
//==============================================================================
void ChordReferenceAudioProcessorEditor::showHelpDialog()
{
    auto& proc = dynamic_cast<ChordReferenceAudioProcessor&>(processor);
    const bool isEnglish = (proc.getCurrentLanguage() == 1);

    juce::String helpText;
    if (isEnglish) {
        helpText = LocalisedStrings::translate("help_section1") + "\n\n"
                 + LocalisedStrings::translate("help_section2") + "\n\n"
                 + LocalisedStrings::translate("help_section3") + "\n\n"
                 + LocalisedStrings::translate("help_section4") + "\n\n"
                 + LocalisedStrings::translate("help_section5") + "\n\n"
                 + LocalisedStrings::translate("help_section6") + "\n\n"
                 + LocalisedStrings::translate("help_section7");
    } else {
        helpText = LocalisedStrings::translate("help_section1") + "\n\n"
                 + LocalisedStrings::translate("help_section2") + "\n\n"
                 + LocalisedStrings::translate("help_section3") + "\n\n"
                 + LocalisedStrings::translate("help_section4") + "\n\n"
                 + LocalisedStrings::translate("help_section5") + "\n\n"
                 + LocalisedStrings::translate("help_section6") + "\n\n"
                 + LocalisedStrings::translate("help_section7");
    }

    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withTitle(LocalisedStrings::translate("help_title"))
            .withMessage(helpText)
            .withIconType(juce::AlertWindow::InfoIcon)
            .withButton(LocalisedStrings::translate("dialog_ok")),
        nullptr);
}
