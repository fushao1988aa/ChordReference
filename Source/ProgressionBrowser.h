//==================================================================================================
//  ProgressionBrowser.h
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦进行预设浏览器组件声明
//        - 左侧类别列表、中间标签过滤、中央预设卡片、右侧详情面板
//        - 底部播放控制栏、导入/导出菜单
//        - 拖拽生成 MIDI 文件到宿主轨道
//        - 编辑、删除、收藏用户预设
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ProgressionData.h"
#include "LocalisedStrings.h"
#include "ProgressionEngine.h"
#include "ProgressionRenderer.h"
#include "SkinColours.h"
#include "ModernPurpleLookAndFeel.h"
#include "ProgressionCard.h"
#include "PresetInspectorPanel.h"
#include "ArpeggioPreviewState.h"
#include "ArpeggioPatternLibrary.h"
#include <functional>
#include <unordered_map>

class ChordReferenceAudioProcessor;

class ProgressionBrowser : public juce::Component,
                           public juce::Timer,
                           public juce::DragAndDropContainer
{
public:
    ProgressionBrowser(ChordReferenceAudioProcessor& proc,
                       ProgressionData& data,
                       std::function<PitchClass()> getCurrentTonic,
                       std::function<ScaleType()> getCurrentScaleType);
    ~ProgressionBrowser() override;

    //---- JUCE 重写 ----
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

    //---- 播放控制 ----
    void startAudition();
    void stopAudition();
    bool handleShortcut(const juce::KeyPress& key);

    //---- 皮肤 ----
    void setSkin(SkinType newSkin);

    //---- 关闭回调 ----
    std::function<void()> onClose;

    //==========================================================================
    // 左侧类别与标签选择（公开，供模型访问）
    //==========================================================================
    juce::ListBox categoryList;
    juce::Viewport tagViewport;
    juce::Component tagContainer;
    juce::OwnedArray<juce::ToggleButton> tagButtons;
    juce::StringArray currentCategoryTags;
    int selectedCategoryIndex = 0;

    juce::TextButton importButton;
    juce::TextButton exportButton;

    juce::Array<int> categoryTagCounts;

    //==========================================================================
    // 类别列表模型
    //==========================================================================
    class CategoryListModel : public juce::ListBoxModel
    {
    public:
        CategoryListModel(ProgressionBrowser& owner) : owner(owner) {}
        int getNumRows() override;
        void paintListBoxItem(int, juce::Graphics&, int, int, bool) override;
        void selectedRowsChanged(int) override;
        void listBoxItemClicked(int, const juce::MouseEvent&) override;
    private:
        ProgressionBrowser& owner;
    };
    CategoryListModel categoryModel { *this };

    // 类别名称
    juce::StringArray categoryNames = {
        LocalisedStrings::translate("auto_mode"),
        LocalisedStrings::translate("auto_region_genre"),
        LocalisedStrings::translate("auto_mood"),
        LocalisedStrings::translate("auto_complexity"),
        LocalisedStrings::translate("auto_song_section"),
        LocalisedStrings::translate("auto_chinese_style_tags")
    };

private:
    //==========================================================================
    // 内部数据引用
    //==========================================================================
    ChordReferenceAudioProcessor& processor;
    ProgressionData& progressionData;
    std::function<PitchClass()> getCurrentTonic;
    std::function<ScaleType()> getCurrentScaleType;
    SkinType currentSkin = SkinType::FluidPurpleOrange;

    //==========================================================================
    // 过滤模式
    //==========================================================================
    enum FilterMode { All, Category, Favorites, UserPresets, FrequentUsed, UserFolder };
    FilterMode currentFilterMode = FilterMode::All;

    //==========================================================================
    // 工具栏按钮
    //==========================================================================
    juce::TextButton closeButton;
    juce::TextButton clearTagsButton;
    juce::TextButton addPresetButton;
    juce::TextButton playPauseButton;
    juce::TextButton prevPresetButton;
    juce::TextButton nextPresetButton;
    juce::TextButton arpeggioSmartButton;
    juce::ComboBox arpeggioDensityComboBox;
    juce::TextButton arpeggioPatternButton;
    juce::TextButton arpeggioColourLayerButton;
    juce::TextButton arpeggioLeftRootButton;
    juce::TextButton arpeggioBassDoubleButton;
    juce::Slider arpeggioHumanizeSlider;
    juce::Label arpeggioHumanizeLabel;
    juce::TextEditor searchBox;
    juce::TextButton batchDeleteButton;
    juce::TextButton batchExportButton;

    //==========================================================================
    // 中间区域控件
    //==========================================================================
    ModernPurpleLookAndFeel modernLAF;
    juce::Viewport cardViewport;
    juce::Component cardContainer;
    juce::OwnedArray<ProgressionCard> cards;
    PresetInspectorPanel inspector;
    juce::Label categoryTitleLabel;
    juce::Label matchCountLabel;
    juce::ComboBox sortComboBox;

    //==========================================================================
    // 底部信息栏
    //==========================================================================
    juce::Label keyLabel;
    juce::Label bottomInfoLabel;

    //==========================================================================
    // 数据/状态
    //==========================================================================
    juce::Array<ProgressionEntry> filteredEntries;
    juce::StringArray activeTags;
    juce::String currentSearchText; //---记录搜索关键词---
    juce::String selectedUserFolderId;
    std::unordered_map<std::string, int> tagCountCache;
    int tagCountCacheLanguage = -1;
    int tagCountCacheEntryCount = -1;
    ArpeggioPreviewState arpeggioState;
    std::vector<ArpeggioPattern> arpeggioPatterns;

    //==========================================================================
    // 播放相关
    //==========================================================================
    int currentAuditionStep = -1;
    std::vector<std::vector<int>> auditionChordNotes;
    std::vector<std::vector<int>> auditionVelocities;
    double auditionBpm = 120.0;
    int auditionChordIndex = 0;
    PitchClass lastTonic = PitchClass::C;
    ScaleType lastType = ScaleType::Major;

    //==========================================================================
    // 拖拽相关
    //==========================================================================
    enum class MidiDragMode { None, Progression, Arpeggio };
    juce::Point<int> dragStartPos;
    bool dragButtonPressed = false;
    MidiDragMode midiDragMode = MidiDragMode::None;

    //==========================================================================
    // 导入/导出对话框
    //==========================================================================
    std::unique_ptr<juce::FileChooser> importChooser;
    std::unique_ptr<juce::FileChooser> exportChooser;

    //==========================================================================
    // 绘制分割线的区域记录
    //==========================================================================
    juce::Rectangle<int> topBarRect;
    juce::Rectangle<int> bottomBarRect;
    juce::Rectangle<int> leftBarRect;

    //==========================================================================
    // 内部方法
    //==========================================================================
    void buildTagPanel();
    void updateFilteredList();
    void applySearchFilter(juce::Array<ProgressionEntry>& entries);
    void deselectAllTags();
    void applySkinColours();
    int countTagMatches(const juce::String& tag) const;
    bool entryHasDisplayTag(const ProgressionEntry& entry, const juce::String& displayTag) const;
    void rebuildTagCountCache();
    void invalidatePresetBrowserCaches();
    void resetDragButtonColor();
    void applySorting(juce::Array<ProgressionEntry>& entries);
    void initialiseArpeggioControls();
    void updateArpeggioControlsFromState();
    void updateArpeggioStatusText();
    ArpeggioGenerationSettings createArpeggioSettingsFromState() const;
    juce::String getSelectedArpeggioPatternName() const;
    void showArpeggioPatternMenu();
    void selectArpeggioPatternIndex(int index);
    std::vector<ArpeggioRankedPattern> getCurrentArpeggioRecommendations() const;
    bool cycleSmartArpeggioRecommendation(int delta);

    // 预设编辑器
    void showPresetEditor();
    void showPresetEditor(const ProgressionEntry& entry);

    // 导入/导出
    void showImportMenu();
    void importSingleJson(const juce::File& file);
    void importZipPackage(const juce::File& file);
    void addImportedEntries(const juce::Array<ProgressionEntry>& entries);
    void showExportMenu();
    void exportEntries(const juce::Array<ProgressionEntry>& entries, bool singleJson);
    ProgressionEntry getSelectedEntry() const;
    void showSuccessMessage(bool singleFile, int count, const juce::File& path);
    void showMessageDialog(const juce::String& title, const juce::String& message,
                           juce::AlertWindow::AlertIconType iconType) const;

    // 预设卡片列表中添加多选机制（Shift+点击）
    void updateBatchButtonVisibility();
    void batchDeleteSelected();
    void batchExportSelected();
    void selectAllCards();
    void deselectAllCards();
    void showFolderMenu(int rowNumber, juce::Point<int> screenPos);
    void promptCreateFolder();
    void promptRenameFolder(const juce::String& folderId, const juce::String& oldName);
    void addMoveToFolderMenu(juce::PopupMenu& menu, const ProgressionEntry& entry);

    // 和弦符号生成
    juce::String getChordSymbolsForEntry(const ProgressionEntry& entry) const;
    juce::String getRomanDisplay(const ProgressionEntry& entry) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionBrowser)
};
