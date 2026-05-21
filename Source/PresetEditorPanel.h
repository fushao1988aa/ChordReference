//==================================================================================================
//  PresetEditorPanel.h
//  项目：ChordReference (和弦调性速查器)
//  功能：预设编辑器面板声明
//        - 提供名称、罗马序列、描述、封面图片的编辑
//        - 多类别标签选择（复用浏览器标签定义）
//        - 支持新建与编辑模式
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ProgressionData.h"
#include "ProgressionEngine.h"
#include "MusicTypes.h"
#include <functional>

class PresetEditorPanel : public juce::Component
{
public:
    PresetEditorPanel(ProgressionData& data,
                      std::function<PitchClass()> getTonic,
                      std::function<ScaleType()> getScaleType);

    //---- 填充编辑数据（进入编辑模式）----
    void setEntry(const ProgressionEntry& entry);
    juce::String getOriginalName() const { return originalName; }

    //---- 回调 ----
    std::function<void(const ProgressionEntry&)> onSave;
    std::function<void()> onCancel;

    //---- 绘制与布局 ----
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ProgressionData& progressionData;
    std::function<PitchClass()> getCurrentTonic;
    std::function<ScaleType()> getCurrentScaleType;

    // 编辑控件
    juce::Label titleLabel;
    juce::TextEditor nameEditor;
    juce::TextEditor romanEditor;
    juce::Label previewLabel;
    juce::TextEditor descriptionEditor;

    // 标签多选（滚动区域）
    juce::OwnedArray<juce::ToggleButton> tagButtons;
    juce::Viewport tagViewport;
    juce::Component tagContainer;

    // 操作按钮
    juce::TextButton saveButton;
    juce::TextButton cancelButton;

    // 封面相关
    juce::TextButton selectCoverButton;
    juce::Image previewCoverImage;
    juce::Rectangle<int> coverPreviewRect;
    juce::String coverBase64;
    std::unique_ptr<juce::FileChooser> chooser;

    // 编辑模式状态
    bool isEditing = false;
    juce::String originalName;

    // 辅助方法
    void updatePreview();
    void collectTags(juce::StringArray& tags);
    void buildTagPanel();
    bool validate();
    void chooseCoverImage();
};