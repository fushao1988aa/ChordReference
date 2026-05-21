//==================================================================================================
//  ChordCard.h
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦卡片 UI 组件声明
//        - 显示和弦符号、中文名称、组成音、罗马数字、情绪文字
//        - 支持转位切换、和弦类型（3/7/9/11/13）切换、和弦质量切换
//        - 支持引申音后缀设置与清除
//        - 内置试听（通过 AudioProcessor）与拖拽 MIDI 文件
//        - 四套皮肤主题自适应
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "MusicTypes.h"
#include "MoodMapper.h"
#include <functional>

class ChordCard : public juce::Component,
                  private juce::Timer
{
public:
    //==============================================================================
    // 公有接口：设置和弦数据与外观
    //==============================================================================
    ~ChordCard() override;

    //---- 基础和弦设置（三和弦） ----
    void setChord(const Chord& chord, bool isSharpKey, bool isMajor);

    //---- 扩展和弦设置（七/九/十一/十三） ----
    void setExtendedChord(ChordExtension type, const Chord& chord);

    //---- 和弦符号与中文名称 ----
    void setChordSymbol(const juce::String& symbol);
    void setChineseName(const juce::String& name);

    //---- 试听支持（需传入 AudioProcessor 裸指针，生命周期由外部管理） ----
    void setAudioProcessor(juce::AudioProcessor* proc) { audioProcessor = proc; }

    //---- 转位与和弦类型的手动设置与获取 ----
    int getCurrentInversion() const { return inversion; }
    void setInversion(int inv) { inversion = inv; repaint(); }
    void setChordType(ChordExtension type);
    ChordExtension getChordType() const;

    //---- 和弦质量设置与获取 ----
    void setChordQuality(ChordQuality quality) { chordQuality = quality; repaint(); }
    ChordQuality getChordQuality() const { return chordQuality; }

    //---- 引申音后缀 ----
    void setExtensionSuffix(const juce::String& suffix);
    juce::String getExtensionSuffix() const { return extensionSuffix; }
    void clearExtensionSuffixSilent() { extensionSuffix.clear(); }
    void setExtensionSuffixSilent(const juce::String& suffix) { extensionSuffix = suffix; chordType = ChordExtension::Seventh; }

    //---- 获取当前和弦符号 ----
    juce::String getChordSymbol() const { return chordSymbol; }

    // 获取当前基础和弦的功能名（T/S/D/t/s 等）
    juce::String getFunctionName() const { return currentChord.name; }

    //---- 静默设置（不触发回调，用于批量更新） ----
    void setChordQualitySilent(ChordQuality quality) { chordQuality = quality; }
    void setChordTypeSilent(ChordExtension type);

    //---- 获取当前和弦包含的音高类（0~11，按转位顺序） ----
    std::vector<int> getCurrentNotes() const;

    //----  获取当前扩展和弦结构（只读）---- 
    const Chord& getExtendedChord() const { return extendedChord; }

    //---- 皮肤设置 ----
    void setSkin(SkinType skin) { currentSkin = skin; fontsValid = false; repaint(); }

    //---- 静态工具：和弦类型对应的音符数 ----
    static int getNumNotesForType(ChordExtension type);

    //==============================================================================
    // 回调函数（由外部连接以响应交互）
    //==============================================================================
    std::function<void()> onInversionChanged;
    std::function<void()> onChordTypeChanged;
    std::function<void()> onChordQualityChanged;

    //==============================================================================
    // JUCE 重写
    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

private:
    //==============================================================================
    // 内部数据
    //==============================================================================
    Chord currentChord;
    Chord extendedChord;
    bool sharpKey = true;
    bool majorMode = true;
    int inversion = 0;
    ChordExtension chordType = ChordExtension::Triad;
    ChordQuality chordQuality = ChordQuality::MajorTriad;
    SkinType currentSkin = SkinType::FluidPurpleOrange;
    juce::String chordSymbol;
    juce::String chineseName;
    juce::AudioProcessor* audioProcessor = nullptr;   // 非拥有，生命周期由上层保证

    juce::String extensionSuffix;
    float buttonAreaHeight = 20.0f;

    //---- 拖拽与节奏 ----
    juce::String currentDragLength = "1/4";
    bool dragButtonPressed = false;
    juce::Point<int> dragStartPos;

    //---- 按钮区域记录（用于点击检测） ----
    juce::Rectangle<float> qualityButtonRect;
    juce::Rectangle<float> numberButtonsArea;

    //==============================================================================
    // 私有方法
    //==============================================================================

    //---- 音符处理 ----
    void getOrderedNotes(std::vector<int>& notes) const;

    //---- 按钮布局与绘制 ----
    juce::Rectangle<float> getButtonBounds(int index) const;   // 0=转位, 1=节奏, 2=拖拽
    void drawIconButton(juce::Graphics& g, const juce::Rectangle<float>& bounds, int buttonType);

    //---- 弹出菜单 ----
    void showRhythmPopup(juce::Rectangle<float> buttonArea);
    void showQualityPopup();

    //---- MIDI 文件生成（用于拖拽） ----
    juce::File createMidiFile() const;

    //---- 质量显示文本 ----
    juce::String getQualityDisplayName() const;
    static juce::String getQualityDisplayName(ChordQuality q);
    juce::String getEffectiveQualityDisplayName() const;
    bool isNumberAllowed(ChordExtension number) const;

    //---- 绘制子步骤 ----
    void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds, const MoodInfo& mood);
    void drawBorderAndShadow(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawHeader(juce::Graphics& g, juce::Rectangle<float>& area);
    void drawChordSymbolAndChinese(juce::Graphics& g, juce::Rectangle<float>& area);
    void drawNotesAndRoman(juce::Graphics& g, juce::Rectangle<float>& area);
    void drawMoodText(juce::Graphics& g, juce::Rectangle<float>& area);
    void drawBottomButtons(juce::Graphics& g, juce::Rectangle<float> bottomArea,
                        const juce::Typeface::Ptr& typeface, float buttonHeight);

    juce::String getExtendedNoteString() const;
    void updateButtonAreas();
    void timerCallback() override;

    //---- 绘制缓存 ----
    void updateCachedFontsAndColors(const juce::Rectangle<float>& bounds);
    void updateMoodCache();

    juce::Font cachedLargeFont;
    juce::Font cachedMidFont;
    juce::Font cachedSmallFont;
    juce::Colour cachedMainTextColour;
    juce::Colour cachedSubTextColour;
    juce::StringArray cachedMoodLines;
    juce::Colour cachedMoodTextColour;
    bool fontsValid = false;

    //==============================================================================
    // 方案 A 视觉辅助（新增绘制风格）
    //==============================================================================
    void drawRomanCircle(juce::Graphics& g,
                         juce::Rectangle<float> area,
                         const juce::String& roman,
                         juce::Colour accent);

    void drawCapsuleNotes(juce::Graphics& g,
                          juce::Rectangle<float> area,
                          const juce::String& text,
                          juce::Colour accent);

    void drawGlowPanel(juce::Graphics& g,
                       juce::Rectangle<float> area,
                       juce::Colour colour,
                       float alpha = 0.15f);

    juce::Colour getAccentColour() const;
};