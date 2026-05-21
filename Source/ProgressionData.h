//==================================================================================================
//  ProgressionData.h
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦进行预设数据层声明
//        - 预设条目结构体（名称、罗马序列、标签、封面、来源标记）
//        - 出厂预设解压与用户预设管理
//        - 标签筛选、收藏管理
//        - JSON 序列化支持
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_core/juce_core.h>

//==============================================================================
// 单条和弦进行预设条目
//==============================================================================
struct ProgressionEntry
{
    juce::String name;               // 预设名称
    juce::String romanSequence;      // 罗马数字序列，如 "IV V iii vi ii V I"
    juce::StringArray tags;          // 标签数组
    juce::String scaleHint;          // 调式提示
    juce::String description;        // 说明文字
    juce::String sourceFileName;      // 内部来源文件名，用于乱码预设的语义识别，不写回 JSON
    bool isUserPreset = false;       // 是否为用户预设（false = 出厂预设）
    juce::String coverImageBase64;   // 封面图片的 Base64 编码
    juce::String folderId;           // 用户自定义文件夹 ID（仅用户预设使用）

    // JSON 序列化
    static ProgressionEntry fromJSON(const juce::var& json);
    juce::var toJSON() const;
};

struct UserPresetFolder
{
    juce::String id;
    juce::String name;
};

//==============================================================================
// 预设库数据管理器
//==============================================================================
class ProgressionData
{
public:
    ProgressionData();

    //---- 获取所有预设（source: 0=全部, 1=厂家, 2=用户）----
    juce::Array<ProgressionEntry> getAllEntries(int source = 0) const;

    //---- 按标签筛选（多标签必须全部匹配）----
    juce::Array<ProgressionEntry> filterByTags(const juce::StringArray& selectedTags) const;

    //---- 用户预设管理 ----
    bool addUserPreset(const ProgressionEntry& entry);
    bool removeUserPreset(const juce::String& name);
    bool updateUserPreset(const juce::String& oldName, const ProgressionEntry& newEntry);

    //---- 收藏管理 ----
    bool isFavorite(const juce::String& name) const;
    void setFavorite(const juce::String& name, bool shouldBeFavorite);

    //---- 常用管理 ---- 
    bool isFrequent(const juce::String& name) const;
    void setFrequent(const juce::String& name, bool shouldBeFrequent);

    //---- 获取所有标签（去重、排序）----
    juce::StringArray getAllTags() const;

    //---- 重新加载所有文件（用于新增/删除后刷新）----
    void reload();

    //---- 获取目录路径（调试用）----
    juce::File getFactoryDir() const { return factoryDir; }
    juce::File getUserDir() const    { return userDir; }

    //---- 获取用户预设列表 ----
    juce::Array<ProgressionEntry> getUserEntries() const { return userEntries; }

    //---- 用户文件夹 ----
    juce::Array<UserPresetFolder> getUserFolders() const { return userFolders; }
    bool createUserFolder(const juce::String& name);
    bool renameUserFolder(const juce::String& id, const juce::String& newName);
    bool deleteUserFolder(const juce::String& id);
    bool moveUserPresetToFolder(const juce::String& presetName, const juce::String& folderId);
    juce::Array<ProgressionEntry> getUserEntriesInFolder(const juce::String& folderId) const;
    int getUserFolderEntryCount(const juce::String& folderId) const;

private:
    // 内存中的预设列表
    juce::Array<ProgressionEntry> factoryEntries;
    juce::Array<ProgressionEntry> userEntries;

    juce::File factoryDir;
    juce::File userDir;
    juce::File userFoldersFile;
    juce::Array<UserPresetFolder> userFolders;

    // 首次启动释放出厂预设
    void initialiseFactoryPresets();

    // 从目录加载所有 .json 文件
    static juce::Array<ProgressionEntry> loadFromDirectory(const juce::File& dir);

    // 保存单条预设到文件
    static bool saveEntryToFile(const ProgressionEntry& entry, const juce::File& dir);

    // 删除对应的 .json 文件
    static bool deleteEntryFile(const juce::String& name, const juce::File& dir);

    // 收藏数据
    juce::StringArray favoriteNames;
    juce::File favoritesFile;

    // 最近常用数据
    juce::StringArray frequentNames;
    juce::File frequentFile;

    void loadFavorites();
    void saveFavorites();
    void loadFrequent();
    void saveFrequent();
    void loadUserFolders();
    void saveUserFolders();
};
