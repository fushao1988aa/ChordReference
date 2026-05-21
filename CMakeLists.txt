cmake_minimum_required(VERSION 3.21)
project(ChordReference VERSION 1.8.0)

# ---------- JUCE 路径配置 ----------
set(JUCE_ROOT "C:/SDKs/JUCE-8.0.12" CACHE PATH "Path to the JUCE SDK directory")
add_subdirectory(${JUCE_ROOT} ${CMAKE_BINARY_DIR}/juce)

set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "Disable extras")
set(CMAKE_CXX_STANDARD 17)

set(CHORDREFERENCE_FORMATS VST3)
if(APPLE)
    list(APPEND CHORDREFERENCE_FORMATS AU)
endif()

option(CHORDREFERENCE_BUILD_AAX "Build AAX format when the AAX SDK/PACE setup is available" OFF)
if(CHORDREFERENCE_BUILD_AAX)
    list(APPEND CHORDREFERENCE_FORMATS AAX)
endif()

juce_add_plugin(ChordReference
    COMPANY_NAME "Meridian Audio"
    BUNDLE_ID "com.meridian.chordreference"
    PLUGIN_MANUFACTURER_CODE MERI
    PLUGIN_CODE ChRD
    FORMATS ${CHORDREFERENCE_FORMATS}
    PRODUCT_NAME "Chordreference"
    IS_A_SYNTH TRUE
    IS_MIDI_EFFECT FALSE
    VST3_CATEGORIES "Instrument|Synth"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
)

target_link_libraries(ChordReference PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_data_structures
    juce::juce_audio_basics
    juce::juce_audio_processors
    juce::juce_core
)

target_sources(ChordReference PRIVATE
    Source/MusicTypes.h
    Source/ScaleEngine.h
    Source/ScaleEngine.cpp
    Source/TSDBuilder.h
    Source/TSDBuilder.cpp
    Source/PitchNameHelper.h
    Source/PitchNameHelper.cpp
    Source/MoodMapper.h
    Source/MoodMapper.cpp
    Source/CircleOfFifths.h
    Source/CircleOfFifths.cpp
    Source/ChordCard.h
    Source/ChordCard.cpp
    Source/MiniKeyboard.h
    Source/MiniKeyboard.cpp
    Source/ScaledContentComponent.h
    Source/ScaledContentComponent.cpp
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/TopBar.h
    Source/TopBar.cpp
    Source/SkinColours.h
    Source/RegistrationManager.h
    Source/RegistrationManager.cpp
    Source/RegistrationOverlay.h
    Source/RegistrationOverlay.cpp
    Source/UnitTests.cpp
    Source/ProgressionData.h
    Source/ProgressionData.cpp
    Source/ProgressionEngine.h
    Source/ProgressionEngine.cpp
    Source/ProgressionRenderer.h
    Source/ProgressionRenderer.cpp
    Source/ProgressionBrowser.h
    Source/ProgressionBrowser.cpp
    Source/PresetEditorPanel.h
    Source/PresetEditorPanel.cpp
    Source/SmartGenerator.h
    Source/ProgressionCard.cpp
    Source/ProgressionCard.h
    Source/PresetInspectorPanel.cpp
    Source/PresetInspectorPanel.h
    Source/ModernPurpleLookAndFeel.cpp
    Source/ModernPurpleLookAndFeel.h
    Source/TagDefinitions.h
    Source/TagDefinitions.cpp
    Source/ChordNaming.h
    Source/ChordNaming.cpp
    Source/LocalisedStrings.h
    Source/LocalisedStrings.cpp
    Source/ArpeggioTypes.h
    Source/ArpeggioPatternLibrary.h
    Source/ArpeggioPatternLibrary.cpp
    Source/ArpeggioMatcher.h
    Source/ArpeggioMatcher.cpp
    Source/ArpeggioHumanizer.h
    Source/ArpeggioHumanizer.cpp
    Source/ArpeggioMidiRenderer.h
    Source/ArpeggioMidiRenderer.cpp
    Source/ArpeggioEngine.h
    Source/ArpeggioEngine.cpp
    Source/ArpeggioPreviewState.h

)

target_include_directories(ChordReference PRIVATE Source)
target_compile_definitions(ChordReference PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# 嵌入中文字体 + 钢琴采样 + 预设 JSON
juce_add_binary_data(ChordReferenceData
    SOURCES
        Resources/Fonts/HarmonyOS_Sans_SC_Regular.ttf
        Resources/PianoSamples/Piano_1_C.wav
        Resources/PianoSamples/Piano_2_C.wav
        Resources/PianoSamples/Piano_3_C.wav
        Resources/PianoSamples/Piano_4_C.wav
        Resources/PianoSamples/Piano_5_C.wav
        Resources/PianoSamples/Piano_6_C.wav
        Resources/PianoSamples/Piano_7_C.wav
        Resources/Presets/FactoryPresets.zip
        Resources/Languages/strings_zh.json
        Resources/Languages/strings_en.json
)

target_link_libraries(ChordReference PRIVATE ChordReferenceData)

# 强制 MSVC 编译器以 UTF-8 解析源文件
if (MSVC)
    target_compile_options(ChordReference PRIVATE /utf-8)
endif()
