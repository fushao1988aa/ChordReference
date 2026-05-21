//==================================================================================================
//  PresetEditorPanel.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：预设编辑器面板实现
//        - 复用 ProgressionBrowser 的六类标签数组
//        - 封面选择（限制最大宽度 100px）并编码为 Base64
//        - 保存前验证名称与罗马序列非空
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "PresetEditorPanel.h"
#include "LocalisedStrings.h"
#include "PitchNameHelper.h"
#include "ScaleEngine.h"
#include "TagDefinitions.h"

PresetEditorPanel::PresetEditorPanel(ProgressionData& data,
                                     std::function<PitchClass()> getTonic,
                                     std::function<ScaleType()> getScaleType)
    : progressionData(data), getCurrentTonic(getTonic), getCurrentScaleType(getScaleType)
{
    setSize(600, 530);

    titleLabel.setText(LocalisedStrings::translate("auto_custom_chord_progression"), juce::dontSendNotification);
    titleLabel.setFont(18.0f);
    addAndMakeVisible(titleLabel);

    nameEditor.setTextToShowWhenEmpty(LocalisedStrings::translate("auto_preset_name"), juce::Colours::grey);
    addAndMakeVisible(nameEditor);

    romanEditor.setTextToShowWhenEmpty(LocalisedStrings::translate("auto_roman_numeral_sequence_e_g_i_v_vi_iv"), juce::Colours::grey);
    romanEditor.onTextChange = [this] { updatePreview(); };
    addAndMakeVisible(romanEditor);

    previewLabel.setFont(14.0f);
    addAndMakeVisible(previewLabel);

    descriptionEditor.setTextToShowWhenEmpty(LocalisedStrings::translate("auto_description_optional"), juce::Colours::grey);
    descriptionEditor.setMultiLine(true);
    addAndMakeVisible(descriptionEditor);

    selectCoverButton.setButtonText(LocalisedStrings::translate("auto_choose_cover"));
    selectCoverButton.onClick = [this] { chooseCoverImage(); };
    addAndMakeVisible(selectCoverButton);

    addAndMakeVisible(tagViewport);
    tagViewport.setViewedComponent(&tagContainer, false);
    buildTagPanel();

    saveButton.setButtonText(LocalisedStrings::translate("auto_save"));
    saveButton.onClick = [this] {
        if (!validate()) return;
        ProgressionEntry entry;
        entry.name = nameEditor.getText();
        entry.romanSequence = romanEditor.getText();
        entry.description = descriptionEditor.getText();
        entry.coverImageBase64 = coverBase64;
        collectTags(entry.tags);
        if (onSave) onSave(entry);
    };
    addAndMakeVisible(saveButton);

    cancelButton.setButtonText(LocalisedStrings::translate("auto_cancel"));
    cancelButton.onClick = [this] { if (onCancel) onCancel(); };
    addAndMakeVisible(cancelButton);
}

//==============================================================================
// 预览更新
//==============================================================================
void PresetEditorPanel::updatePreview()
{
    juce::String roman = romanEditor.getText();
    if (roman.isEmpty()) { previewLabel.setText("", juce::dontSendNotification); return; }

    PitchClass tonic = getCurrentTonic();
    ScaleType type = getCurrentScaleType();
    auto notes = ProgressionEngine::getChordNotes(roman, tonic, type);
    
    juce::String preview;
    for (auto& chord : notes)
    {
        if (!chord.empty())
            preview += juce::String(PitchNameHelper::getNameForPitch(static_cast<PitchClass>(chord[0] % 12), true)) + " ";
    }
    previewLabel.setText(LocalisedStrings::translate("auto_current_key_preview") + preview.trim(), juce::dontSendNotification);
}

//==============================================================================
// 收集标签
//==============================================================================
void PresetEditorPanel::collectTags(juce::StringArray& tags)
{
    for (auto* btn : tagButtons)
        if (btn->getToggleState())
            tags.add(btn->getButtonText());
}

//==============================================================================
// 验证
//==============================================================================
bool PresetEditorPanel::validate()
{
    if (nameEditor.getText().isEmpty() || romanEditor.getText().isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            LocalisedStrings::translate("auto_error"), LocalisedStrings::translate("auto_name_and_roman_numeral_sequence_cannot_be_empty"));
        return false;
    }
    return true;
}

//==============================================================================
// 绘制（半透明背景 + 封面预览）
//==============================================================================
void PresetEditorPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.85f));

    if (previewCoverImage.isValid())
        g.drawImage(previewCoverImage, coverPreviewRect.toFloat(), juce::RectanglePlacement::centred);
    else
    {
        juce::ColourGradient grad(juce::Colour(0xff9D6BFF), coverPreviewRect.getTopLeft().toFloat(),
                                  juce::Colour(0xff5E3BEE), coverPreviewRect.getBottomRight().toFloat(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(coverPreviewRect.toFloat(), 4.0f);
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.drawText(juce::String::fromUTF8(u8"♫"), coverPreviewRect, juce::Justification::centred);
    }
}

//==============================================================================
// 选择封面图片
//==============================================================================
void PresetEditorPanel::chooseCoverImage()
{
    chooser = std::make_unique<juce::FileChooser>(
        LocalisedStrings::translate("auto_choose_cover_image"),
        juce::File::getSpecialLocation(juce::File::userPicturesDirectory),
        "*.png;*.jpg;*.jpeg");

    constexpr auto flags = juce::FileBrowserComponent::openMode |
                           juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(flags, [this](const juce::FileChooser& fc)
    {
        auto result = fc.getResult();
        if (result.existsAsFile())
        {
            juce::Image img = juce::ImageFileFormat::loadFrom(result);
            if (img.isValid())
            {
                if (img.getWidth() > 100)
                    img = img.rescaled(100, juce::jmin(100, img.getHeight() * 100 / img.getWidth()));

                juce::MemoryOutputStream mos;
                juce::PNGImageFormat pngWriter;
                pngWriter.writeImageToStream(img, mos);
                coverBase64 = mos.getMemoryBlock().toBase64Encoding();
                previewCoverImage = img;
                repaint();
            }
        }
    });
}

//==============================================================================
// 布局
//==============================================================================
void PresetEditorPanel::resized()
{
    auto area = getLocalBounds().reduced(10);

    titleLabel.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);

    nameEditor.setBounds(area.removeFromTop(28));
    area.removeFromTop(8);

    romanEditor.setBounds(area.removeFromTop(28));
    area.removeFromTop(8);

    previewLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(8);

    // 封面预览 + 选择按钮
    auto coverRow = area.removeFromTop(56);
    coverPreviewRect = coverRow.removeFromLeft(56).reduced(4);
    selectCoverButton.setBounds(coverRow.reduced(4));

    // 标签滚动
    tagViewport.setBounds(area.removeFromTop(120));
    area.removeFromTop(8);

    descriptionEditor.setBounds(area.removeFromTop(60));
    area.removeFromTop(16);

    // 保存/取消按钮
    auto btnArea = area.removeFromBottom(30);
    saveButton.setBounds(btnArea.removeFromRight(100));
    btnArea.removeFromRight(10);
    cancelButton.setBounds(btnArea.removeFromRight(100));
}

//==============================================================================
// 构建标签面板（复用 ProgressionBrowser 的六类标签定义）
//==============================================================================
void PresetEditorPanel::buildTagPanel()
{
    tagContainer.removeAllChildren();
    tagButtons.clear();

    // 六个类别的标签（与 ProgressionBrowser 完全相同）
    juce::Array<juce::StringArray> allCategoryTags = TagDefinitions::getAllCategoryTags();

    // 合并所有标签
    juce::StringArray allTags;
    for (auto& catTags : allCategoryTags)
        allTags.addArray(catTags);

    int btnHeight = 22;
    int btnWidth = 130;
    int cols = 5;
    int gap = 4;

    for (int i = 0; i < allTags.size(); ++i)
    {
        auto* btn = new juce::ToggleButton(allTags[i]);
        btn->setBounds((i % cols) * (btnWidth + gap), (i / cols) * (btnHeight + gap), btnWidth, btnHeight);
        tagContainer.addAndMakeVisible(btn);
        tagButtons.add(btn);
    }
    tagContainer.setSize(cols * (btnWidth + gap), ((allTags.size() + cols - 1) / cols) * (btnHeight + gap));
    tagViewport.setViewPosition(0, 0);
}

//==============================================================================
// 编辑模式下填充数据
//==============================================================================
void PresetEditorPanel::setEntry(const ProgressionEntry& entry)
{
    isEditing = true;
    originalName = entry.name;
    nameEditor.setText(entry.name);
    romanEditor.setText(entry.romanSequence);
    descriptionEditor.setText(entry.description);
    coverBase64 = entry.coverImageBase64;

    // 解码预览图
    previewCoverImage = juce::Image();
    if (coverBase64.isNotEmpty())
    {
        juce::MemoryBlock mb;
        mb.fromBase64Encoding(coverBase64);
        if (mb.getSize() > 0)
        {
            juce::MemoryInputStream stream(mb, false);
            previewCoverImage = juce::ImageFileFormat::loadFrom(stream);
        }
    }

    for (auto* btn : tagButtons)
        btn->setToggleState(entry.tags.contains(btn->getButtonText()), juce::dontSendNotification);

    titleLabel.setText(LocalisedStrings::translate("auto_edit_chord_progression"), juce::dontSendNotification);
}