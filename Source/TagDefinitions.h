//==================================================================================================
//  TagDefinitions.h
//  项目：ChordReference (和弦调性速查器)
//  功能：预设浏览器与编辑器共享的标签分类定义
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_core/juce_core.h>

class TagDefinitions
{
public:
    // 返回所有六类标签，每个类别是一个 StringArray
    static juce::Array<juce::StringArray> getAllCategoryTags();
};