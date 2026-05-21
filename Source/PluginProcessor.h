//==================================================================================================
//  PluginProcessor.h
//  椤圭洰锛欳hordReference (鍜屽鸡璋冩€ч€熸煡鍣?
//  鍔熻兘锛氶煶棰戝鐞嗗櫒鏍稿績澹版槑
//        - MIDI 杈撳叆澶勭悊涓庡拰寮﹁瘑鍒紙闃叉姈銆佷紭鍏堝綋鍓嶈皟鎬с€佸叏灞€鍖归厤锛?//        - 鍜屽鸡鏁版嵁搴撴瀯寤猴紙鍚浆浣嶃€佸紩鐢抽煶锛?//        - 璇曞惉绯荤粺锛堝唴缃挗鐞撮噰鏍?/ 澶栭儴 MIDI 杈撳嚭锛?//        - 鐘舵€佹寔涔呭寲锛堣皟鎬с€佺毊鑲ゃ€侀攣瀹氥€佺獥鍙ｅぇ灏忕瓑锛?//        - 渚?UI 绾跨▼瀹夊叏鑾峰彇璇嗗埆缁撴灉
//  娉ㄦ剰锛氶渶淇濆瓨涓?UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "MusicTypes.h"
#include <set>
#include <atomic>
#include <unordered_map>
#include <juce_data_structures/juce_data_structures.h>

class ChordReferenceAudioProcessorEditor;

class ChordReferenceAudioProcessor : public juce::AudioProcessor,
                                     private juce::MidiInputCallback
{
public:
    //==============================================================================
    // 鏋勯€犱笌鏋愭瀯
    //==============================================================================
    ChordReferenceAudioProcessor();
    ~ChordReferenceAudioProcessor() override;

    //==============================================================================
    // AudioProcessor 鍩虹鎺ュ彛瀹炵幇
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override;

    //---- 绋嬪簭绠＄悊锛堝０鏄庯紝瀹炵幇瑙?.cpp锛?----
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    //---- 鐘舵€佹寔涔呭寲 ----
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    struct MidiChordResult
    {
        bool hasResult = false;
        PitchClass tonic = PitchClass::C;
        ScaleType scale = ScaleType::Major;
        std::vector<int> notes;
        std::vector<int> velocities;
        juce::String function;
        int averageVelocity = 0;
        bool sustainPedalDown = false;
    };
    MidiChordResult getLastMidiChordResult();

    //---- MIDI 娲诲姩鏍囧織锛堢敤浜?UI 鎸囩ず鐏級 ----
    bool hasMidiActivity() const;
    void clearMidiActivityFlag();

    //==============================================================================
    // MIDI 璁惧绠＄悊
    //==============================================================================
    juce::StringArray getAvailableMidiDeviceNames() const;
    juce::StringArray getAvailableMidiDeviceIdentifiers() const;
    bool refreshAvailableMidiDevices();

    //==============================================================================
    // 璋冩€т笌鐘舵€佽缃紙渚?UI 璋冪敤锛?    //==============================================================================

    // 缂栬緫鍣ㄥ昂瀵革紙鍏ㄥ眬鍋忓ソ锛?    void setEditorSize(int w, int h);
    void setEditorSize(int w, int h);
    std::pair<int, int> getEditorSize() const;

    // 鐨偆锛堝叏灞€鍋忓ソ锛?    void setSkinType(SkinType skin);
    void setSkinType(SkinType skin);
    SkinType getSkinType() const;

    // 璇█锛堝叏灞€鍋忓ソ锛?    void setCurrentLanguage(int lang);
    void setCurrentLanguage(int lang);
    int getCurrentLanguage() const;

    void setCurrentKey(PitchClass tonic, ScaleType type);
    PitchClass getCurrentTonic() const { return currentTonic; }
    ScaleType getCurrentScaleType() const { return currentScaleType; }

    void setKeyLocked(bool locked) { keyLocked = locked; }
    bool isKeyLocked() const { return keyLocked; }

    //---- 璐熷拰澹版槧灏勫紑鍏?----
    void setNegativeHarmonyEnabled(bool enabled) { negativeHarmonyEnabled = enabled; }
    bool isNegativeHarmonyEnabled() const { return negativeHarmonyEnabled; }

    //---- 鏅鸿兘鐢熸垚椋庢牸锛堜复鏃惰蹇嗭紝涓嶆寔涔呭寲锛?---
    void setSmartStyle(int style) { currentSmartStyle = style; }
    int getSmartStyle() const { return currentSmartStyle; }

    // 鏅鸿兘鐢熸垚搴忓垪璁板繂锛堢敤浜庣獥鍙ｅ叧闂悗鎭㈠锛?    void setLastGeneratedFunctions(const juce::StringArray& funcs) { lastGeneratedFunctions = funcs; }
    void setLastGeneratedFunctions(const juce::StringArray& funcs) { lastGeneratedFunctions = funcs; }
    juce::StringArray getLastGeneratedFunctions() const { return lastGeneratedFunctions; }
    void setLastGeneratedStyle(int style) { lastGeneratedStyle = style; }
    int getLastGeneratedStyle() const { return lastGeneratedStyle; }

    //---- 鍗＄墖楂樼骇鐘舵€佽蹇嗭紙杞綅/绫诲瀷/璐ㄩ噺/寮曠敵闊筹級----
    void setCardChordQualities(ChordQuality t, ChordQuality s, ChordQuality d);
    std::array<ChordQuality, 3> getCardChordQualities() const { return cardChordQualities; }
    void setCardExtensionSuffixes(const juce::String& t, const juce::String& s, const juce::String& d);
    std::array<juce::String, 3> getCardExtensionSuffixes() const { return cardExtensionSuffixes; }

    void setCardFunctionNames(const juce::String& t, const juce::String& s, const juce::String& d);
    std::array<juce::String, 3> getCardFunctionNames() const { return cardFunctionNames; }

    //---- Editor 鍏宠仈 ----
    void setEditor(ChordReferenceAudioProcessorEditor* ed) { editor = ed; }

    //==============================================================================
    // 璇曞惉绯荤粺
    //==============================================================================
    void setAuditionMode(int mode) { auditionMode = mode; }   // 鈽?鍐呰仈瀹炵幇
    int getAuditionMode() const { return auditionMode; }

    void setMidiOutputEnabled(bool enabled) { midiOutputEnabled = enabled; } // 鈽?鍐呰仈瀹炵幇
    bool isMidiOutputEnabled() const { return midiOutputEnabled; }

    void triggerAudition(const std::vector<int>& midiNotes);
    void triggerAudition(const std::vector<int>& midiNotes, const std::vector<int>& velocities);
    void releaseAudition();

    //==============================================================================
    // 鐘舵€佽蹇嗭紙鍗＄墖杞綅/绫诲瀷銆佺獥鍙ｅぇ灏忥級
    //==============================================================================
    void setCardInversions(int t, int s, int d);
    std::array<int, 3> getCardInversions() const { return cardInversions; }
    void setCardChordTypes(int t, int s, int d);
    std::array<int, 3> getCardChordTypes() const { return cardChordTypes; }
    void setPresetBrowserOpen(bool open) { presetBrowserOpen = open; }
    bool isPresetBrowserOpen() const { return presetBrowserOpen; }
    //瀛樺偍璁惧鏍囪瘑绗﹁€岄潪鍚嶇О
    void setMidiDeviceByIdentifier(const juce::String& identifier);
    void setCurrentMidiDeviceIdentifier(const juce::String& id) { currentMidiDeviceIdentifier = id; }
    juce::String getCurrentMidiDeviceIdentifier() const { return currentMidiDeviceIdentifier; }

private:
    //==============================================================================
    // 鍐呴儴鏁版嵁
    //==============================================================================
    std::set<int> activeNotes;                          // 褰撳墠鎸変笅鐨勯煶楂樼被
    MidiChordResult lastResult;
    bool newResultAvailable = false;
    std::atomic<bool> midiActivityFlag{ false };
    std::unordered_map<int, int> heldMidiNotes;
    std::unordered_map<int, int> sustainedMidiNotes;
    std::unordered_map<int, int> activeNoteVelocities;
    int activeBassPitchClass = -1;
    bool sustainPedalDown = false;

    std::unique_ptr<juce::MidiInput> currentMidiInput;
    juce::Array<juce::MidiDeviceInfo> availableMidiDevices;
    juce::String currentMidiDeviceIdentifier;   // 褰撳墠閫変腑鐨?MIDI 璁惧鍚嶏紙绌?鏃犺澶囷級

    //---- 璇曞惉鐩稿叧 ----
    int auditionMode = 1;                               // 0/1/2
    juce::Synthesiser pianoSynth;
    juce::Reverb pianoReverb;
    std::array<juce::IIRFilter, 2> pianoHighPassFilters;
    std::array<juce::IIRFilter, 2> pianoMudFilters;
    std::array<juce::IIRFilter, 2> pianoAirFilters;
    juce::AudioFormatManager formatManager;
    juce::MidiBuffer pendingAuditionNotes;
    juce::CriticalSection auditionLock;
    std::vector<int> activeAuditionNotes;

    //---- 绾跨▼閿?----
    juce::CriticalSection noteLock;
    juce::CriticalSection resultLock;

    //---- 褰撳墠璋冩€?----
    PitchClass currentTonic = PitchClass::C;
    ScaleType currentScaleType = ScaleType::Major;
    juce::CriticalSection keyLock;

    //---- 鍜屽鸡鏁版嵁搴擄紙鎸夐煶闆嗗ぇ灏忓垎鍖猴級 ----
    struct KnownChord
    {
        PitchClass tonic;
        ScaleType scaleType;
        std::string function;
        std::vector<int> pitchSet;      // 鎺掑簭鍚庣殑闊抽珮闆嗗悎
        int bassNote = -1;              // 鏈€浣庨煶锛?1 琛ㄧず涓嶅弬涓庤浆浣嶅尮閰嶏級
    };
    std::unordered_map<int, std::vector<KnownChord>> chordDatabaseBySize;
    std::unordered_map<std::string, std::vector<KnownChord>> chordDatabaseByPitchSet;

    //---- 鐘舵€佹寔涔呭寲 ----
    juce::ValueTree stateTree;
    bool keyLocked = false;
    bool midiOutputEnabled = true;
    bool negativeHarmonyEnabled = false;
    ChordReferenceAudioProcessorEditor* editor = nullptr;

    //---- 涓存椂鐘舵€佽蹇?----
    std::array<int, 3> cardInversions = {0, 0, 0};     // T/S/D 杞綅
    std::array<int, 3> cardChordTypes = {3, 3, 3};     // T/S/D 鍜屽鸡绫诲瀷
    int currentSmartStyle = 0;          //鏅鸿兘鐢熸垚椋庢牸// 0=閫氱敤,1=娴佽,2=鐖靛＋,3=鍙ら
    
    //鏅鸿兘鐢熸垚鐘舵€佽蹇?    juce::StringArray lastGeneratedFunctions;               // 涓婃鏅鸿兘鐢熸垚鐨勫簭鍒?    int lastGeneratedStyle = 0;                             // 涓婃鐢熸垚鏃剁殑椋庢牸
    juce::StringArray lastGeneratedFunctions;
    int lastGeneratedStyle = 0;
    std::array<ChordQuality, 3> cardChordQualities = {ChordQuality::MajorTriad, ChordQuality::MajorTriad, ChordQuality::Dominant7};
    std::array<juce::String, 3> cardExtensionSuffixes;      // T/S/D 寮曠敵闊冲悗缂€
    std::array<juce::String, 3> cardFunctionNames = {"T", "S", "D"};

    //棰勮绐楀彛鎵撳紑璁板繂
    bool presetBrowserOpen = false;

    //---- 闃叉姈妯″潡 ----
    std::vector<int> stableNoteSet;
    int stableBlockCount = 0;
    static constexpr int STABILITY_THRESHOLD = 3;
    static constexpr double RECOGNITION_SETTLE_SECONDS = 0.075;
    double currentSampleRate = 44100.0;
    int recognitionSettleSamplesRemaining = 0;
    std::vector<int> lastMatchedPitchSet;
    std::vector<int> lastMatchedExpressionSignature;
    std::string lastMidiOutputSignature;

    void tryMatchChord();
    void initPianoSynth();
    void updatePianoToneFilters(double sampleRate);
    void applyPianoToneShaping(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // 鍐呴儴鏂规硶
    //==============================================================================
    void buildChordDatabase();          // 鏋勫缓鎵€鏈夊彲鑳藉拰寮︼紙鍚浆浣嶃€佸紩鐢抽煶锛?    void tryMatchChord();               // 灏濊瘯鍖归厤褰撳墠 activeNotes 瀵瑰簲鐨勫拰寮?    void initPianoSynth();              // 鍔犺浇鍐呯疆閽㈢惔閲囨牱

    //---- MidiInputCallback 鎺ュ彛 ----
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void handleRecognitionMidiMessage(const juce::MidiMessage& message);
    void rebuildActiveNoteStateLocked();
    std::vector<int> getMatchedVelocitiesForPitchSet(const std::vector<int>& pitchSet) const;
    std::vector<int> getCurrentExpressionSignatureLocked(const std::vector<int>& pitchSet) const;
    void refreshLastResultExpressionLocked(const std::vector<int>& pitchSet);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordReferenceAudioProcessor)
};
