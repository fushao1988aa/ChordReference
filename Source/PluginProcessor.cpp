//==================================================================================================
//  PluginProcessor.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：音频处理器核心实现
//        - 构造/析构、prepareToPlay/releaseResources
//        - processBlock（和弦识别、内置钢琴试听、外部 MIDI 输出）
//        - MIDI 设备管理、MidiInputCallback 实现
//        - 基础接口（createEditor、getTailLengthSeconds 等）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ScaleEngine.h"
#include "TSDBuilder.h"
#include "RegistrationManager.h"
#include "BinaryData.h"
#include "PianoSamplerVoice.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "LocalisedStrings.h"
#include <cmath>
#include <numeric>

namespace
{
    std::string makePitchSetKey(const std::vector<int>& pitchSet)
    {
        std::string key;
        for (size_t i = 0; i < pitchSet.size(); ++i)
        {
            if (i != 0)
                key += ",";
            key += std::to_string(pitchSet[i]);
        }
        return key;
    }

    std::string makeMidiOutputSignature(const ChordReferenceAudioProcessor::MidiChordResult& result)
    {
        std::string signature = result.function.toStdString() + "|";
        for (size_t i = 0; i < result.notes.size(); ++i)
        {
            if (i != 0)
                signature += ",";

            signature += std::to_string(result.notes[i]);
            signature += ":";
            signature += std::to_string(i < result.velocities.size() ? result.velocities[i] : result.averageVelocity);
        }

        signature += result.sustainPedalDown ? "|s1" : "|s0";
        return signature;
    }
}

//==============================================================================
// 构造与析构
//==============================================================================
ChordReferenceAudioProcessor::ChordReferenceAudioProcessor()
    : stateTree("ChordReferenceState")
{
    buildChordDatabase();
    
    formatManager.registerBasicFormats();
    initPianoSynth();

    availableMidiDevices = juce::MidiInput::getAvailableDevices();
    // 加载语言字串符
    LocalisedStrings::loadLanguage(RegistrationManager::getInstance().loadLanguagePreference());
    
#ifdef DEBUG
    // 单元测试（仅 Debug 模式，且只运行一次）
    {
        static bool testsRun = false;
        if (!testsRun)
        {
            testsRun = true;
            juce::UnitTestRunner runner;
            runner.runAllTests();
            
            juce::String testSummary;
            for (int i = 0; i < runner.getNumResults(); ++i)
            {
                if (const auto* result = runner.getResult(i))
                {
                    testSummary << result->unitTestName << " / " << result->subcategoryName
                                << ": " << result->passes << " passed, "
                                << result->failures << " failed\n";
                    
                    for (const auto& message : result->messages)
                        testSummary << "  " << message << "\n";
                }
            }
            
            juce::Logger::writeToLog("Unit Tests Results:\n" + testSummary);
        }
    }
#endif
}

ChordReferenceAudioProcessor::~ChordReferenceAudioProcessor()
{
    if (currentMidiInput)
        currentMidiInput->stop();
}

//==============================================================================
// 准备与释放资源
//==============================================================================
void ChordReferenceAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    pianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    pianoReverb.setSampleRate(sampleRate);
    pianoReverb.reset();
    updatePianoToneFilters(currentSampleRate);

    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.18f;
    reverbParams.damping = 0.62f;
    reverbParams.wetLevel = 0.075f;
    reverbParams.dryLevel = 0.94f;
    reverbParams.width = 0.86f;
    pianoReverb.setParameters(reverbParams);
}

void ChordReferenceAudioProcessor::releaseResources() {}

//==============================================================================
// processBlock：音频处理
//==============================================================================
void ChordReferenceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    bool noteActivityFromDaw = false;

    // ---- 先解析宿主传入的原始 MIDI，避免把插件自己生成的试听/输出消息反向识别 ----
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if ((msg.isNoteOn() && msg.getVelocity() > 0)
            || msg.isNoteOff()
            || (msg.isNoteOn() && msg.getVelocity() == 0)
            || msg.isAllNotesOff()
            || msg.isAllSoundOff()
            || (msg.isController() && msg.getControllerNumber() == 64))
        {
            handleRecognitionMidiMessage(msg);
            noteActivityFromDaw = true;
        }
    }

    // ---- 处理内置试听（钢琴合成器） ----
    if (auditionMode == 1)
    {
        juce::ScopedLock lock(auditionLock);
        pianoSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }

    if (auditionMode == 1)
    {
        if (buffer.getNumChannels() > 1)
            pianoReverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
        else
            pianoReverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());

        applyPianoToneShaping(buffer);
    }
    
    // ---- 处理外部 MIDI 输出 ----
    if (auditionMode == 2)
    {
        juce::ScopedLock lock(auditionLock);
        midiMessages.addEvents(pendingAuditionNotes, 0, buffer.getNumSamples(), 0);
        pendingAuditionNotes.clear();
    }

    if (noteActivityFromDaw)
        midiActivityFlag = true;

    // ---- 防抖与和弦识别 ----
    {
        juce::ScopedLock lock(noteLock);
        std::vector<int> currentSet(activeNotes.begin(), activeNotes.end());
        std::sort(currentSet.begin(), currentSet.end());
        const auto currentExpressionSignature = getCurrentExpressionSignatureLocked(currentSet);

        if (currentSet != stableNoteSet)
        {
            stableNoteSet = currentSet;
            stableBlockCount = 0;
            recognitionSettleSamplesRemaining = juce::roundToInt(currentSampleRate * RECOGNITION_SETTLE_SECONDS);
        }
        else
        {
            stableBlockCount++;
            recognitionSettleSamplesRemaining = juce::jmax(0, recognitionSettleSamplesRemaining - buffer.getNumSamples());
        }

        if (currentSet.size() >= 3 && currentSet.size() <= 7 &&
            recognitionSettleSamplesRemaining <= 0)
        {
            if (currentSet != lastMatchedPitchSet)
            {
                lastMatchedPitchSet = currentSet;
                tryMatchChord();
                lastMatchedExpressionSignature = currentExpressionSignature;
            }
            else if (currentExpressionSignature != lastMatchedExpressionSignature)
            {
                refreshLastResultExpressionLocked(currentSet);
                lastMatchedExpressionSignature = currentExpressionSignature;
            }
            stableBlockCount = 0;
        }
        else if (currentSet.size() < 3 && lastResult.hasResult)
        {
            juce::ScopedLock resultLockGuard(resultLock);
            if (lastResult.hasResult)
            {
                lastResult.hasResult = false;
                newResultAvailable = true;
            }
            lastMatchedPitchSet.clear();
            lastMatchedExpressionSignature.clear();
            recognitionSettleSamplesRemaining = 0;
        }
    }

    // ---- MIDI 输出模式：识别结果变化时发送一次示例音符 ----
    {
        juce::ScopedLock resultLockGuard(resultLock);
        if (midiOutputEnabled && lastResult.hasResult)
        {
            const auto outputSignature = makeMidiOutputSignature(lastResult);
            if (outputSignature != lastMidiOutputSignature)
            {
                for (int i = 0; i < static_cast<int>(lastResult.notes.size()); ++i)
                {
                    const int note = lastResult.notes[static_cast<size_t>(i)];
                    const int velocity = i < static_cast<int>(lastResult.velocities.size())
                                           ? lastResult.velocities[static_cast<size_t>(i)]
                                           : juce::jmax(1, lastResult.averageVelocity);
                    midiMessages.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(juce::jlimit(1, 127, velocity))), 0);
                    midiMessages.addEvent(juce::MidiMessage::noteOff(1, note), buffer.getNumSamples() - 1);
                }

                lastMidiOutputSignature = outputSignature;
            }
        }
        else if (!lastResult.hasResult)
        {
            lastMidiOutputSignature.clear();
        }
    }
}

//==============================================================================
// AudioProcessor 基础接口
//==============================================================================
juce::AudioProcessorEditor* ChordReferenceAudioProcessor::createEditor()
{
    return new ChordReferenceAudioProcessorEditor(*this);
}

bool ChordReferenceAudioProcessor::hasEditor() const { return true; }

const juce::String ChordReferenceAudioProcessor::getName() const
{
    return LocalisedStrings::translate("auto_chordreference");
}

double ChordReferenceAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ChordReferenceAudioProcessor::getNumPrograms() { return 1; }
int ChordReferenceAudioProcessor::getCurrentProgram() { return 0; }
void ChordReferenceAudioProcessor::setCurrentProgram(int) {}
const juce::String ChordReferenceAudioProcessor::getProgramName(int) { return {}; }
void ChordReferenceAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
// MIDI 和弦识别结果获取（线程安全）
//==============================================================================
ChordReferenceAudioProcessor::MidiChordResult ChordReferenceAudioProcessor::getLastMidiChordResult()
{
    juce::ScopedLock lock(resultLock);
    if (newResultAvailable)
    {
        newResultAvailable = false;
        return lastResult;
    }
    return MidiChordResult{};
}

bool ChordReferenceAudioProcessor::hasMidiActivity() const { return midiActivityFlag; }
void ChordReferenceAudioProcessor::clearMidiActivityFlag() { midiActivityFlag = false; }

//==============================================================================
// MIDI 设备管理
//==============================================================================
juce::StringArray ChordReferenceAudioProcessor::getAvailableMidiDeviceNames() const
{
    juce::StringArray names;
    for (auto& d : availableMidiDevices)
        names.add(d.name);
    return names;
}

juce::StringArray ChordReferenceAudioProcessor::getAvailableMidiDeviceIdentifiers() const
{
    juce::StringArray ids;
    for (auto& d : availableMidiDevices)
        ids.add(d.identifier);
    return ids;
}

bool ChordReferenceAudioProcessor::refreshAvailableMidiDevices()
{
    auto devices = juce::MidiInput::getAvailableDevices();
    if (devices.size() == availableMidiDevices.size())
    {
        bool same = true;
        for (int i = 0; i < devices.size(); ++i)
        {
            if (devices[i].identifier != availableMidiDevices[i].identifier
                || devices[i].name != availableMidiDevices[i].name)
            {
                same = false;
                break;
            }
        }

        if (same)
            return false;
    }

    availableMidiDevices = devices;
    return true;
}

void ChordReferenceAudioProcessor::setMidiDeviceByIdentifier(const juce::String& identifier)
{
    if (currentMidiInput) { currentMidiInput->stop(); currentMidiInput.reset(); }
    if (identifier.isNotEmpty())
    {
        for (auto& d : availableMidiDevices)
        {
            if (d.identifier == identifier)
            {
                currentMidiInput = juce::MidiInput::openDevice(d.identifier, this);
                if (currentMidiInput) currentMidiInput->start();
                break;
            }
        }
    }
}

//==============================================================================
// MidiInputCallback：处理外部硬件 MIDI 消息
//==============================================================================
void ChordReferenceAudioProcessor::handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message)
{
    handleRecognitionMidiMessage(message);
}

void ChordReferenceAudioProcessor::handleRecognitionMidiMessage(const juce::MidiMessage& message)
{
    if (message.isNoteOn() && message.getVelocity() > 0)
    {
        juce::ScopedLock lock(noteLock);
        const int note = message.getNoteNumber();
        heldMidiNotes[note] = juce::jlimit(1, 127, static_cast<int>(message.getVelocity()));
        sustainedMidiNotes.erase(note);
        rebuildActiveNoteStateLocked();
        midiActivityFlag = true;
    }
    else if (message.isNoteOff() || (message.isNoteOn() && message.getVelocity() == 0))
    {
        juce::ScopedLock lock(noteLock);
        const int note = message.getNoteNumber();
        auto held = heldMidiNotes.find(note);
        if (sustainPedalDown && held != heldMidiNotes.end())
            sustainedMidiNotes[note] = held->second;

        heldMidiNotes.erase(note);
        if (!sustainPedalDown)
            sustainedMidiNotes.erase(note);

        rebuildActiveNoteStateLocked();
        midiActivityFlag = true;
    }
    else if (message.isController() && message.getControllerNumber() == 64)
    {
        juce::ScopedLock lock(noteLock);
        const bool wasDown = sustainPedalDown;
        sustainPedalDown = message.getControllerValue() >= 64;

        if (wasDown && !sustainPedalDown)
            sustainedMidiNotes.clear();

        rebuildActiveNoteStateLocked();
        midiActivityFlag = true;
    }
    else if (message.isAllNotesOff() || message.isAllSoundOff())
    {
        juce::ScopedLock lock(noteLock);
        heldMidiNotes.clear();
        sustainedMidiNotes.clear();
        sustainPedalDown = false;
        rebuildActiveNoteStateLocked();
        midiActivityFlag = true;
    }
}

void ChordReferenceAudioProcessor::rebuildActiveNoteStateLocked()
{
    activeNotes.clear();
    activeNoteVelocities.clear();
    activeBassPitchClass = -1;
    int lowestMidiNote = 128;

    auto addNote = [this, &lowestMidiNote](int midiNote, int velocity)
    {
        const int pitchClass = midiNote % 12;
        activeNotes.insert(pitchClass);

        if (midiNote < lowestMidiNote)
        {
            lowestMidiNote = midiNote;
            activeBassPitchClass = pitchClass;
        }

        auto it = activeNoteVelocities.find(pitchClass);
        if (it == activeNoteVelocities.end())
            activeNoteVelocities[pitchClass] = velocity;
        else
            it->second = std::max(it->second, velocity);
    };

    for (const auto& note : heldMidiNotes)
        addNote(note.first, note.second);

    for (const auto& note : sustainedMidiNotes)
        addNote(note.first, note.second);
}

std::vector<int> ChordReferenceAudioProcessor::getMatchedVelocitiesForPitchSet(const std::vector<int>& pitchSet) const
{
    std::vector<int> velocities;
    velocities.reserve(pitchSet.size());

    int total = 0;
    int count = 0;
    for (const auto& item : activeNoteVelocities)
    {
        total += item.second;
        ++count;
    }
    const int fallback = count > 0 ? juce::jlimit(1, 127, total / count) : 100;

    for (int pitchClass : pitchSet)
    {
        auto it = activeNoteVelocities.find(pitchClass);
        velocities.push_back(it != activeNoteVelocities.end() ? it->second : fallback);
    }

    return velocities;
}

std::vector<int> ChordReferenceAudioProcessor::getCurrentExpressionSignatureLocked(const std::vector<int>& pitchSet) const
{
    std::vector<int> signature;
    signature.reserve(pitchSet.size() * 2 + 1);
    signature.push_back(sustainPedalDown ? 1 : 0);

    for (int pitchClass : pitchSet)
    {
        signature.push_back(pitchClass);
        auto it = activeNoteVelocities.find(pitchClass);
        signature.push_back(it != activeNoteVelocities.end() ? it->second : 0);
    }

    return signature;
}

void ChordReferenceAudioProcessor::refreshLastResultExpressionLocked(const std::vector<int>& pitchSet)
{
    juce::ScopedLock resultLockGuard(resultLock);
    if (!lastResult.hasResult)
        return;

    lastResult.velocities = getMatchedVelocitiesForPitchSet(pitchSet);
    lastResult.averageVelocity = lastResult.velocities.empty()
                                   ? 0
                                   : static_cast<int>(std::accumulate(lastResult.velocities.begin(), lastResult.velocities.end(), 0)
                                                      / lastResult.velocities.size());
    lastResult.sustainPedalDown = sustainPedalDown;
    newResultAvailable = true;
}

//==============================================================================
// 调性设置（外部调用）
//==============================================================================
void ChordReferenceAudioProcessor::setCurrentKey(PitchClass tonic, ScaleType type)
{
    juce::ScopedLock lock(keyLock);
    currentTonic = tonic;
    currentScaleType = type;
}

//==============================================================================
// 和弦数据库构建（大调/小调三和弦至十三和弦，含转位与引申音）
//==============================================================================
void ChordReferenceAudioProcessor::buildChordDatabase()
{
    chordDatabaseBySize.clear();
    chordDatabaseByPitchSet.clear();
    std::vector<PitchClass> majorTonionics = {
        PitchClass::C, PitchClass::G, PitchClass::D, PitchClass::A, PitchClass::E,
        PitchClass::B, PitchClass::F_Sharp, PitchClass::C_Sharp,
        PitchClass::F, PitchClass::B_Flat, PitchClass::E_Flat, PitchClass::A_Flat,
        PitchClass::D_Flat, PitchClass::G_Flat, PitchClass::C_Flat
    };

    auto toPitchSet = [](const Chord& chord) -> std::vector<int> {
        int root = static_cast<int>(chord.root);
        std::vector<int> pitchSet;
        for (int interval : chord.intervals) pitchSet.push_back((root + interval) % 12);
        std::sort(pitchSet.begin(), pitchSet.end());
        pitchSet.erase(std::unique(pitchSet.begin(), pitchSet.end()), pitchSet.end());
        return pitchSet;
    };

    auto addKnownChord = [&](const KnownChord& chord)
    {
        chordDatabaseBySize[chord.pitchSet.size()].push_back(chord);
        chordDatabaseByPitchSet[makePitchSetKey(chord.pitchSet)].push_back(chord);
    };

    //---- 辅助：添加原位及转位条目 ----
    auto addChordWithInversions = [&](PitchClass tonic, ScaleType scaleType, const std::string& funcBase,
                                      const Chord& chord, int numNotes)
    {
        // 原位（bassNote = 根音）
        KnownChord kc;
        kc.tonic = tonic;
        kc.scaleType = scaleType;
        kc.function = funcBase;
        kc.pitchSet = toPitchSet(chord);
        kc.bassNote = static_cast<int>(chord.root);
        addKnownChord(kc);

        // 转位（bassNote 为转位后的低音）
        if (numNotes > 1)
        {
            std::vector<int> intervals = chord.intervals;
            int root = static_cast<int>(chord.root);
            for (int inv = 1; inv < numNotes; ++inv)
            {
                std::vector<int> invIntervals = intervals;
                for (int i = 0; i < inv; ++i)
                    invIntervals.push_back(invIntervals[i] + 12);
                invIntervals.erase(invIntervals.begin(), invIntervals.begin() + inv);

                int newBass = (root + intervals[inv]) % 12;

                Chord invChord;
                invChord.root = chord.root;
                invChord.intervals = invIntervals;
                auto pitches = toPitchSet(invChord);

                KnownChord invKc;
                invKc.tonic = tonic;
                invKc.scaleType = scaleType;
                invKc.function = funcBase;
                invKc.pitchSet = pitches;
                invKc.bassNote = newBass;
                addKnownChord(invKc);
            }
        }
    };

    //---- 构建大调和弦 ----
    for (auto tonic : majorTonionics)
    {
        auto scale = ScaleEngine::getScale(tonic, ScaleType::Major);
        auto family = TSDBuilder::getMajor(tonic);

        addChordWithInversions(tonic, ScaleType::Major, "T", family.tonic, 3);
        addChordWithInversions(tonic, ScaleType::Major, "S", family.subdominant, 3);
        addChordWithInversions(tonic, ScaleType::Major, "D", family.dominant, 3);

        Chord I7 = ScaleEngine::getChord(scale, 1, 4);
        addChordWithInversions(tonic, ScaleType::Major, "T7", I7, 4);
        Chord V7 = ScaleEngine::getChord(scale, 5, 4);
        addChordWithInversions(tonic, ScaleType::Major, "D7", V7, 4);

        Chord I9 = ScaleEngine::getChord(scale, 1, 5);
        addChordWithInversions(tonic, ScaleType::Major, "T9", I9, 5);
        Chord V9 = ScaleEngine::getChord(scale, 5, 5);
        addChordWithInversions(tonic, ScaleType::Major, "D9", V9, 5);

        Chord I11 = ScaleEngine::getChord(scale, 1, 6);
        addChordWithInversions(tonic, ScaleType::Major, "T11", I11, 6);
        Chord V11 = ScaleEngine::getChord(scale, 5, 6);
        addChordWithInversions(tonic, ScaleType::Major, "D11", V11, 6);

        Chord I13 = ScaleEngine::getChord(scale, 1, 7);
        addChordWithInversions(tonic, ScaleType::Major, "T13", I13, 7);
        Chord V13 = ScaleEngine::getChord(scale, 5, 7);
        addChordWithInversions(tonic, ScaleType::Major, "D13", V13, 7);
    }

    //---- 构建小调和弦 ----
    for (auto majorTonic : majorTonionics)
    {
        PitchClass minorTonic = ScaleEngine::getRelativeMinor(majorTonic);
        auto naturalScale = ScaleEngine::getScale(minorTonic, ScaleType::NaturalMinor);
        auto family = TSDBuilder::getMinor(minorTonic);

        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "t", family.tonic, 3);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "s", family.subdominant, 3);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "D", family.dominant, 3);

        Chord i7 = ScaleEngine::getChord(naturalScale, 1, 4);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "t7", i7, 4);
        auto harmonicScale = naturalScale;
        int seventhIdx = 6;
        int orig = static_cast<int>(harmonicScale[seventhIdx]);
        harmonicScale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
        Chord V7 = ScaleEngine::getChord(harmonicScale, 5, 4);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "D7", V7, 4);

        Chord i9 = ScaleEngine::getChord(naturalScale, 1, 5);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "t9", i9, 5);
        Chord V9 = ScaleEngine::getChord(harmonicScale, 5, 5);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "D9", V9, 5);

        Chord i11 = ScaleEngine::getChord(naturalScale, 1, 6);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "t11", i11, 6);
        Chord V11 = ScaleEngine::getChord(harmonicScale, 5, 6);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "D11", V11, 6);

        Chord i13 = ScaleEngine::getChord(naturalScale, 1, 7);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "t13", i13, 7);
        Chord V13 = ScaleEngine::getChord(harmonicScale, 5, 7);
        addChordWithInversions(minorTonic, ScaleType::NaturalMinor, "D13", V13, 7);
    }

    //---- 引申音扩展（为属七和弦添加变化） ----
    auto addAlteredDominants = [&](PitchClass tonic, ScaleType scaleType, const Chord& baseDom7) {
        struct { const char* suffix; std::vector<int> addIntervals; } alterations[] = {
            {"b9", {13}},         // b9 = 半音13
            {"#9", {15}},         // #9 = 半音15
            {"#11", {18}},        // #11 = 半音18
            {"b13", {20}},        // b13 = 半音20
            {"b9#11", {13, 18}},
            {"#9#11", {15, 18}},
            {"b9b13", {13, 20}}
        };

        for (auto& alt : alterations)
        {
            auto pitches = toPitchSet(baseDom7);
            int root = static_cast<int>(baseDom7.root);
            for (int interval : alt.addIntervals)
                pitches.push_back((root + interval) % 12);
            std::sort(pitches.begin(), pitches.end());
            pitches.erase(std::unique(pitches.begin(), pitches.end()), pitches.end());

            juce::String funcName = "D7" + juce::String(alt.suffix);
            KnownChord kc;
            kc.tonic = tonic;
            kc.scaleType = scaleType;
            kc.function = funcName.toStdString();
            kc.pitchSet = pitches;
            kc.bassNote = static_cast<int>(baseDom7.root);
            addKnownChord(kc);
        }
    };

    //---- 为所有大调和小调属七和弦添加引申音 ---- 
    for (auto tonic : majorTonionics)
    {
        auto majorScale = ScaleEngine::getScale(tonic, ScaleType::Major);
        Chord majorDom7 = ScaleEngine::getChord(majorScale, 5, 4);
        addAlteredDominants(tonic, ScaleType::Major, majorDom7);

        PitchClass minorTonic = ScaleEngine::getRelativeMinor(tonic);
        auto naturalScale = ScaleEngine::getScale(minorTonic, ScaleType::NaturalMinor);
        auto harmonicScale = naturalScale;
        int seventhIdx = 6;
        int orig = static_cast<int>(harmonicScale[seventhIdx]);
        harmonicScale[seventhIdx] = static_cast<PitchClass>((orig + 1) % 12);
        Chord minorDom7 = ScaleEngine::getChord(harmonicScale, 5, 4);
        addAlteredDominants(minorTonic, ScaleType::NaturalMinor, minorDom7);
    }

        //---- 新增和弦类型：增七、属七挂四、小大九 ----
    auto addSpecialChords = [&](PitchClass tonic, ScaleType scaleType,
                                const std::string& funcPrefix,
                                const std::vector<int>& intervals,
                                PitchClass root) {
        KnownChord kc;
        kc.tonic = tonic;
        kc.scaleType = scaleType;
        kc.function = funcPrefix;
        kc.pitchSet = intervals;
        std::sort(kc.pitchSet.begin(), kc.pitchSet.end());
        kc.pitchSet.erase(std::unique(kc.pitchSet.begin(), kc.pitchSet.end()), kc.pitchSet.end());
        kc.bassNote = static_cast<int>(root);
        addKnownChord(kc);
    };

    // 为每个大调主音添加：Iaug7, V7sus4
    for (auto tonic : majorTonionics)
    {
        auto scale = ScaleEngine::getScale(tonic, ScaleType::Major);
        PitchClass dom = scale[4]; // V 级

        // Iaug7 (增七)
        addSpecialChords(tonic, ScaleType::Major, "Taug7",
                         {0, 4, 8, 10}, tonic);
        // V7sus4
        addSpecialChords(tonic, ScaleType::Major, "D7sus4",
                         {0, 5, 7, 10}, dom);
    }

    // 为每个小调主音添加：i9(maj9) = iminMaj9, v7sus4 (和声小调)
    for (auto majorTonic : majorTonionics)
    {
        PitchClass minorTonic = ScaleEngine::getRelativeMinor(majorTonic);
        auto naturalScale = ScaleEngine::getScale(minorTonic, ScaleType::NaturalMinor);
        auto harmonicScale = naturalScale;
        harmonicScale[6] = static_cast<PitchClass>((static_cast<int>(harmonicScale[6]) + 1) % 12);
        PitchClass dom = harmonicScale[4]; // 和声小调 V 级

        // iminMaj9
        addSpecialChords(minorTonic, ScaleType::NaturalMinor, "tminMaj9",
                         {0, 3, 7, 11, 14}, minorTonic);
        // v7sus4
        addSpecialChords(minorTonic, ScaleType::NaturalMinor, "D7sus4",
                         {0, 5, 7, 10}, dom);
    }
}

//==============================================================================
// 和弦匹配（优先当前调性，转位优先，全局回退）
//==============================================================================
void ChordReferenceAudioProcessor::tryMatchChord()
{
    std::vector<int> currentSet(activeNotes.begin(), activeNotes.end());
    if (currentSet.empty()) return;
    std::sort(currentSet.begin(), currentSet.end());

    int currentBass = activeBassPitchClass >= 0 ? activeBassPitchClass : *activeNotes.begin();

    const std::vector<KnownChord>* candidates = nullptr;
    auto itPitchSet = chordDatabaseByPitchSet.find(makePitchSetKey(currentSet));
    if (itPitchSet != chordDatabaseByPitchSet.end())
    {
        candidates = &itPitchSet->second;
    }
    else
    {
        auto itDB = chordDatabaseBySize.find(currentSet.size());
        if (itDB == chordDatabaseBySize.end()) goto noMatch;
        candidates = &itDB->second;
    }

    //---- 优先匹配当前调性（转位优先） ----
    {
        juce::ScopedLock lock(keyLock);
        PitchClass targetTonic = currentTonic;
        ScaleType targetType = currentScaleType;

        for (const auto& known : *candidates)
        {
            if (known.tonic == targetTonic && known.scaleType == targetType &&
                known.bassNote == currentBass && known.pitchSet == currentSet)
            {
                juce::ScopedLock resultLockGuard(resultLock);
                lastResult.hasResult = true;
                lastResult.tonic = known.tonic;
                lastResult.scale = known.scaleType;
                lastResult.function = known.function;
                newResultAvailable = true;
                // 生成示例 MIDI 音符
                int rootMidi = 60;
                int rootPitch = static_cast<int>(known.tonic);
                lastResult.notes.clear();
                for (int pc : known.pitchSet)
                {
                    int interval = (pc - rootPitch + 12) % 12;
                    int midiNote = rootMidi + interval;
                    if (midiNote < rootMidi) midiNote += 12;
                    lastResult.notes.push_back(midiNote);
                }
                lastResult.velocities = getMatchedVelocitiesForPitchSet(known.pitchSet);
                lastResult.averageVelocity = lastResult.velocities.empty()
                                               ? 0
                                               : static_cast<int>(std::accumulate(lastResult.velocities.begin(), lastResult.velocities.end(), 0)
                                                                  / lastResult.velocities.size());
                lastResult.sustainPedalDown = sustainPedalDown;
                return;
            }
        }
        // 回退到无转位偏好匹配
        for (const auto& known : *candidates)
        {
            if (known.tonic == targetTonic && known.scaleType == targetType &&
                known.pitchSet == currentSet)
            {
                juce::ScopedLock resultLockGuard(resultLock);
                lastResult.hasResult = true;
                lastResult.tonic = known.tonic;
                lastResult.scale = known.scaleType;
                lastResult.function = known.function;
                newResultAvailable = true;
                int rootMidi = 60;
                int rootPitch = static_cast<int>(known.tonic);
                lastResult.notes.clear();
                for (int pc : known.pitchSet)
                {
                    int interval = (pc - rootPitch + 12) % 12;
                    int midiNote = rootMidi + interval;
                    if (midiNote < rootMidi) midiNote += 12;
                    lastResult.notes.push_back(midiNote);
                }
                lastResult.velocities = getMatchedVelocitiesForPitchSet(known.pitchSet);
                lastResult.averageVelocity = lastResult.velocities.empty()
                                               ? 0
                                               : static_cast<int>(std::accumulate(lastResult.velocities.begin(), lastResult.velocities.end(), 0)
                                                                  / lastResult.velocities.size());
                lastResult.sustainPedalDown = sustainPedalDown;
                return;
            }
        }
    }

    //---- 全局匹配（转位优先） ----
    for (const auto& known : *candidates)
    {
        if (known.bassNote == currentBass && known.pitchSet == currentSet)
        {
            juce::ScopedLock resultLockGuard(resultLock);
            lastResult.hasResult = true;
            lastResult.tonic = known.tonic;
            lastResult.scale = known.scaleType;
            lastResult.function = known.function;
            newResultAvailable = true;
            int rootMidi = 60;
            int rootPitch = static_cast<int>(known.tonic);
            lastResult.notes.clear();
            for (int pc : known.pitchSet)
            {
                int interval = (pc - rootPitch + 12) % 12;
                int midiNote = rootMidi + interval;
                if (midiNote < rootMidi) midiNote += 12;
                lastResult.notes.push_back(midiNote);
            }
            lastResult.velocities = getMatchedVelocitiesForPitchSet(known.pitchSet);
            lastResult.averageVelocity = lastResult.velocities.empty()
                                           ? 0
                                           : static_cast<int>(std::accumulate(lastResult.velocities.begin(), lastResult.velocities.end(), 0)
                                                              / lastResult.velocities.size());
            lastResult.sustainPedalDown = sustainPedalDown;
            return;
        }
    }
    for (const auto& known : *candidates)
    {
        if (known.pitchSet == currentSet)
        {
            juce::ScopedLock resultLockGuard(resultLock);
            lastResult.hasResult = true;
            lastResult.tonic = known.tonic;
            lastResult.scale = known.scaleType;
            lastResult.function = known.function;
            newResultAvailable = true;
            int rootMidi = 60;
            int rootPitch = static_cast<int>(known.tonic);
            lastResult.notes.clear();
            for (int pc : known.pitchSet)
            {
                int interval = (pc - rootPitch + 12) % 12;
                int midiNote = rootMidi + interval;
                if (midiNote < rootMidi) midiNote += 12;
                lastResult.notes.push_back(midiNote);
            }
            lastResult.velocities = getMatchedVelocitiesForPitchSet(known.pitchSet);
            lastResult.averageVelocity = lastResult.velocities.empty()
                                           ? 0
                                           : static_cast<int>(std::accumulate(lastResult.velocities.begin(), lastResult.velocities.end(), 0)
                                                              / lastResult.velocities.size());
            lastResult.sustainPedalDown = sustainPedalDown;
            return;
        }
    }

noMatch:
    juce::ScopedLock resultLockGuard(resultLock);
    if (lastResult.hasResult)
    {
        lastResult.hasResult = false;
        newResultAvailable = true;
    }
}

//==============================================================================
// 状态持久化
//==============================================================================
void ChordReferenceAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    stateTree.setProperty("keyLocked", keyLocked, nullptr);

    stateTree.setProperty("cardInv_T", cardInversions[0], nullptr);
    stateTree.setProperty("cardInv_S", cardInversions[1], nullptr);
    stateTree.setProperty("cardInv_D", cardInversions[2], nullptr);
    stateTree.setProperty("cardTyp_T", cardChordTypes[0], nullptr);
    stateTree.setProperty("cardTyp_S", cardChordTypes[1], nullptr);
    stateTree.setProperty("cardTyp_D", cardChordTypes[2], nullptr);

    juce::MemoryOutputStream mos(destData, false);
    stateTree.writeToStream(mos);
}

void ChordReferenceAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (sizeInBytes == 0) return;
    juce::MemoryInputStream mis(data, sizeInBytes, false);
    auto newTree = juce::ValueTree::readFromStream(mis);
    if (newTree.isValid() && newTree.hasType("ChordReferenceState"))
    {
        stateTree = newTree;
        keyLocked = stateTree.getProperty("keyLocked", false);
        cardInversions[0] = stateTree.getProperty("cardInv_T", 0);
        cardInversions[1] = stateTree.getProperty("cardInv_S", 0);
        cardInversions[2] = stateTree.getProperty("cardInv_D", 0);
        cardChordTypes[0] = stateTree.getProperty("cardTyp_T", 3);
        cardChordTypes[1] = stateTree.getProperty("cardTyp_S", 3);
        cardChordTypes[2] = stateTree.getProperty("cardTyp_D", 3);
    }

    if (editor != nullptr)
    {
        juce::Component::SafePointer<ChordReferenceAudioProcessorEditor> safeEditor(editor);
        juce::MessageManager::callAsync([safeEditor] {
            if (safeEditor != nullptr)
                safeEditor->restoreUIState();
        });
    }
}

//==============================================================================
// 窗口皮肤与语言状态全局设置
//==============================================================================
//窗口全局
std::pair<int, int> ChordReferenceAudioProcessor::getEditorSize() const
{
    return RegistrationManager::getInstance().loadWindowSize();
}

void ChordReferenceAudioProcessor::setEditorSize(int w, int h)
{
    RegistrationManager::getInstance().saveWindowSize(w, h);
}

//皮肤全局
SkinType ChordReferenceAudioProcessor::getSkinType() const
{
    return static_cast<SkinType>(RegistrationManager::getInstance().loadSkinPreference());
}

void ChordReferenceAudioProcessor::setSkinType(SkinType skin)
{
    RegistrationManager::getInstance().saveSkinPreference(static_cast<int>(skin));
}

//语言全局
void ChordReferenceAudioProcessor::setCurrentLanguage(int lang)
{
    RegistrationManager::getInstance().saveLanguagePreference(lang);
}

int ChordReferenceAudioProcessor::getCurrentLanguage() const
{
    return RegistrationManager::getInstance().loadLanguagePreference();
}

//==============================================================================
// 临时状态记忆接口
//==============================================================================
void ChordReferenceAudioProcessor::setCardInversions(int t, int s, int d)
{
    cardInversions[0] = t; cardInversions[1] = s; cardInversions[2] = d;
}

void ChordReferenceAudioProcessor::setCardChordTypes(int t, int s, int d)
{
    cardChordTypes[0] = t; cardChordTypes[1] = s; cardChordTypes[2] = d;
}

void ChordReferenceAudioProcessor::setCardFunctionNames(const juce::String& t, const juce::String& s, const juce::String& d)
{
    cardFunctionNames[0] = t; cardFunctionNames[1] = s; cardFunctionNames[2] = d;
}

//==============================================================================
// 试听触发与释放
//==============================================================================
void ChordReferenceAudioProcessor::triggerAudition(const std::vector<int>& midiNotes)
{
    triggerAudition(midiNotes, {});
}

void ChordReferenceAudioProcessor::triggerAudition(const std::vector<int>& midiNotes,
                                                   const std::vector<int>& velocities)
{
    juce::ScopedLock lock(auditionLock);

    if (auditionMode == 1)
    {
        for (int i = 0; i < pianoSynth.getNumVoices(); ++i)
            if (auto* voice = dynamic_cast<PianoSamplerVoice*>(pianoSynth.getVoice(i)))
                voice->stopNote(0.9f, true);
    }
    else if (auditionMode == 2)
    {
        for (int note : activeAuditionNotes)
            pendingAuditionNotes.addEvent(juce::MidiMessage::noteOff(1, juce::jlimit(0, 127, note)), 0);
    }

    activeAuditionNotes.clear();

    if (auditionMode == 1)
    {
        for (int i = 0; i < static_cast<int>(midiNotes.size()); ++i)
        {
            const int note = juce::jlimit(0, 127, midiNotes[static_cast<size_t>(i)]);
            const int velocity = (i < static_cast<int>(velocities.size()))
                                   ? velocities[static_cast<size_t>(i)]
                                   : 100;
            const float normalizedVelocity = juce::jlimit(20, 127, velocity) / 127.0f;
            const float shapedVelocity = juce::jlimit(0.10f, 0.92f, 0.10f + 0.82f * std::pow(normalizedVelocity, 0.92f));
            pianoSynth.noteOn(1, note, shapedVelocity);
            activeAuditionNotes.push_back(note);
        }
    }
    else if (auditionMode == 2)
    {
        for (int i = 0; i < static_cast<int>(midiNotes.size()); ++i)
        {
            const int note = juce::jlimit(0, 127, midiNotes[static_cast<size_t>(i)]);
            const int velocity = (i < static_cast<int>(velocities.size()))
                                   ? velocities[static_cast<size_t>(i)]
                                   : 100;
            pendingAuditionNotes.addEvent(
                juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(juce::jlimit(20, 127, velocity))), 0);
            activeAuditionNotes.push_back(note);
        }
    }
}

void ChordReferenceAudioProcessor::releaseAudition()
{
    juce::ScopedLock lock(auditionLock);
    if (auditionMode == 1)
    {
        for (int i = 0; i < pianoSynth.getNumVoices(); ++i)
            if (auto* voice = dynamic_cast<PianoSamplerVoice*>(pianoSynth.getVoice(i)))
                voice->stopNote(0.9f, true);
    }
    else if (auditionMode == 2)
    {
        for (int note : activeAuditionNotes)
            pendingAuditionNotes.addEvent(juce::MidiMessage::noteOff(1, juce::jlimit(0, 127, note)), 0);
    }

    activeAuditionNotes.clear();
}

//==============================================================================
// 初始化内置钢琴合成器
//==============================================================================
void ChordReferenceAudioProcessor::initPianoSynth()
{
    pianoSynth.clearSounds();
    pianoSynth.clearVoices();
    
    struct PianoSample {
        const void* data;
        int dataSize;
        int rootNote;
        int lowNote;
        int highNote;
    };
    
    std::vector<PianoSample> samples = {
        { BinaryData::Piano_1_C_wav, BinaryData::Piano_1_C_wavSize, 24, 24, 35 },
        { BinaryData::Piano_2_C_wav, BinaryData::Piano_2_C_wavSize, 36, 36, 47 },
        { BinaryData::Piano_3_C_wav, BinaryData::Piano_3_C_wavSize, 48, 48, 59 },
        { BinaryData::Piano_4_C_wav, BinaryData::Piano_4_C_wavSize, 60, 60, 71 },
        { BinaryData::Piano_5_C_wav, BinaryData::Piano_5_C_wavSize, 72, 72, 83 },
        { BinaryData::Piano_6_C_wav, BinaryData::Piano_6_C_wavSize, 84, 84, 95 },
        { BinaryData::Piano_7_C_wav, BinaryData::Piano_7_C_wavSize, 96, 96, 108 }
    };
    
    for (auto& s : samples)
    {
        auto memoryInputStream = std::make_unique<juce::MemoryInputStream>(s.data, s.dataSize, false);
        if (auto reader = formatManager.createReaderFor(std::move(memoryInputStream)))
        {
            juce::BigInteger allNotes;
            allNotes.setRange(s.lowNote, s.highNote - s.lowNote + 1, true);
            pianoSynth.addSound(new juce::SamplerSound("piano", *reader, allNotes, s.rootNote, 0.008, 1.15, 10.0));
        }
    }
    
    // 让旧和弦自然释放时仍有足够 voice 给新和弦发声，避免切换时硬抢音。
    for (int i = 0; i < 32; ++i)
        pianoSynth.addVoice(new PianoSamplerVoice());
}

void ChordReferenceAudioProcessor::updatePianoToneFilters(double sampleRate)
{
    const auto highPass = juce::IIRCoefficients::makeHighPass(sampleRate, 42.0, 0.72);
    const auto mudDip = juce::IIRCoefficients::makePeakFilter(sampleRate, 320.0, 0.82, juce::Decibels::decibelsToGain(-2.2f));
    const auto airLift = juce::IIRCoefficients::makeHighShelf(sampleRate, 5200.0, 0.62, juce::Decibels::decibelsToGain(1.6f));

    for (int ch = 0; ch < 2; ++ch)
    {
        pianoHighPassFilters[static_cast<size_t>(ch)].setCoefficients(highPass);
        pianoMudFilters[static_cast<size_t>(ch)].setCoefficients(mudDip);
        pianoAirFilters[static_cast<size_t>(ch)].setCoefficients(airLift);
        pianoHighPassFilters[static_cast<size_t>(ch)].reset();
        pianoMudFilters[static_cast<size_t>(ch)].reset();
        pianoAirFilters[static_cast<size_t>(ch)].reset();
    }
}

void ChordReferenceAudioProcessor::applyPianoToneShaping(juce::AudioBuffer<float>& buffer)
{
    const int channelsToProcess = juce::jmin(buffer.getNumChannels(), 2);

    for (int ch = 0; ch < channelsToProcess; ++ch)
    {
        auto* samples = buffer.getWritePointer(ch);
        const int numSamples = buffer.getNumSamples();
        auto index = static_cast<size_t>(ch);

        pianoHighPassFilters[index].processSamples(samples, numSamples);
        pianoMudFilters[index].processSamples(samples, numSamples);
        pianoAirFilters[index].processSamples(samples, numSamples);

        for (int i = 0; i < numSamples; ++i)
        {
            const float driven = samples[i] * 1.04f;
            samples[i] = std::tanh(driven) * 0.94f;
        }
    }
}

void ChordReferenceAudioProcessor::setCardChordQualities(ChordQuality t, ChordQuality s, ChordQuality d)
{
    cardChordQualities[0] = t; cardChordQualities[1] = s; cardChordQualities[2] = d;
}

void ChordReferenceAudioProcessor::setCardExtensionSuffixes(const juce::String& t, const juce::String& s, const juce::String& d)
{
    cardExtensionSuffixes[0] = t; cardExtensionSuffixes[1] = s; cardExtensionSuffixes[2] = d;
}
//==============================================================================
// 插件工厂函数
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordReferenceAudioProcessor();
}
