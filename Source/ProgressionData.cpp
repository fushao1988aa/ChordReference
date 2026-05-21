//==================================================================================================
//  ProgressionData.cpp
//  椤圭洰锛欳hordReference (鍜屽鸡璋冩€ч€熸煡鍣?
//  鍔熻兘锛氶璁炬暟鎹鐞嗗疄鐜?//        - 鍑哄巶棰勮瑙ｅ帇涓庣敤鎴风洰褰曞垵濮嬪寲
//        - JSON 鏂囦欢鍔犺浇涓庝繚瀛?//        - 鏀惰棌淇℃伅鐨勮鍐?//  娉ㄦ剰锛氶渶淇濆瓨涓?UTF-8 with BOM
//==================================================================================================

#include "ProgressionData.h"
#include "BinaryData.h"

//==============================================================================
// ProgressionEntry JSON 搴忓垪鍖?//==============================================================================
juce::var ProgressionEntry::toJSON() const
{
    juce::DynamicObject* obj = new juce::DynamicObject();
    obj->setProperty("name", name);
    obj->setProperty("roman", romanSequence);
    obj->setProperty("tags", juce::var(tags));
    obj->setProperty("scaleHint", scaleHint);
    obj->setProperty("description", description);
    obj->setProperty("isUserPreset", isUserPreset);
    obj->setProperty("coverImage", coverImageBase64);
    obj->setProperty("folderId", folderId);
    return juce::var(obj);
}

ProgressionEntry ProgressionEntry::fromJSON(const juce::var& json)
{
    ProgressionEntry entry;
    if (auto* obj = json.getDynamicObject())
    {
        entry.name          = obj->getProperty("name");
        entry.romanSequence = obj->getProperty("roman");
        entry.scaleHint     = obj->getProperty("scaleHint");
        entry.description   = obj->getProperty("description");
        entry.isUserPreset  = obj->getProperty("isUserPreset");
        entry.coverImageBase64 = obj->getProperty("coverImage").toString();
        entry.folderId = obj->getProperty("folderId").toString();

        if (auto* tagsArray = obj->getProperty("tags").getArray())
            for (auto& tag : *tagsArray)
                entry.tags.add(tag.toString());
    }
    return entry;
}

//==============================================================================
// 鏋勯€犲嚱鏁帮細鍒濆鍖栫洰褰曘€佸嚭鍘傞璁句笌鏀惰棌
//==============================================================================
ProgressionData::ProgressionData()
{
    auto appDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                      .getChildFile("ChordReference");

    factoryDir = appDir.getChildFile("FactoryPresets");
    userDir    = appDir.getChildFile("UserPresets");
    favoritesFile = appDir.getChildFile("favorites.json");
    frequentFile   = appDir.getChildFile("frequent.json");
    userFoldersFile = appDir.getChildFile("userFolders.json");

    loadFavorites();
    loadFrequent();
    loadUserFolders();
    initialiseFactoryPresets();

    if (!userDir.exists())
        userDir.createDirectory();

    reload();
}

//==============================================================================
// 鍑哄巶棰勮鍒濆鍖栵紙浠?ZIP 瑙ｅ帇锛?//==============================================================================
void ProgressionData::initialiseFactoryPresets()
{
    auto zipData = BinaryData::FactoryPresets_zip;
    auto zipSize = BinaryData::FactoryPresets_zipSize;
    juce::MemoryInputStream zipStream(zipData, zipSize, false);
    juce::ZipFile zip(zipStream);

    int expectedJsonCount = 0;
    for (int i = 0; i < zip.getNumEntries(); ++i)
        if (auto* entry = zip.getEntry(i))
            if (entry->filename.endsWith(".json") && entry->uncompressedSize > 0)
                ++expectedJsonCount;

    const auto markerFile = factoryDir.getChildFile(".factoryPresetsVersion");
    const auto expectedMarker = juce::String(zipSize) + ":" + juce::String(expectedJsonCount);

    if (factoryDir.exists()
        && markerFile.existsAsFile()
        && markerFile.loadFileAsString().trim() == expectedMarker
        && factoryDir.findChildFiles(juce::File::findFiles, true, "*.json").size() >= expectedJsonCount)
    {
        return;
    }

    if (factoryDir.exists())
        factoryDir.deleteRecursively();

    factoryDir.createDirectory();

    int count = 0;
    for (int i = 0; i < zip.getNumEntries(); ++i)
    {
        if (auto* entry = zip.getEntry(i))
        {
            if (entry->filename.endsWith(".json") && entry->uncompressedSize > 0)
            {
                juce::File destFile = factoryDir.getChildFile(entry->filename);
                if (zip.uncompressEntry(i, destFile))
                    ++count;
            }
        }
    }

    if (count == expectedJsonCount)
        markerFile.replaceWithText(expectedMarker);
}

//==============================================================================
// 浠庣洰褰曞姞杞芥墍鏈?JSON 鏂囦欢
//==============================================================================
juce::Array<ProgressionEntry> ProgressionData::loadFromDirectory(const juce::File& dir)
{
    juce::Array<ProgressionEntry> entries;
    if (!dir.exists()) return entries;

    auto files = dir.findChildFiles(juce::File::findFiles, true, "*.json");
    for (auto& file : files)
    {
        juce::FileInputStream inStream(file);
        if (inStream.openedOk())
        {
            auto json = juce::JSON::parse(inStream);
            if (auto* array = json.getArray())
            {
                for (auto& item : *array)
                {
                    auto entry = ProgressionEntry::fromJSON(item);
                    entry.sourceFileName = file.getFileNameWithoutExtension();
                    entries.add(entry);
                }
            }
            else if (json.isObject())
            {
                auto entry = ProgressionEntry::fromJSON(json);
                entry.sourceFileName = file.getFileNameWithoutExtension();
                entries.add(entry);
            }
        }
    }
    return entries;
}

//==============================================================================
// 閲嶆柊鍔犺浇鎵€鏈夐璁?//==============================================================================
void ProgressionData::reload()
{
    loadUserFolders();
    factoryEntries = loadFromDirectory(factoryDir);
    userEntries    = loadFromDirectory(userDir);
}

//==============================================================================
// 鑾峰彇鍏ㄩ儴/绛涢€夋潯鐩?//==============================================================================
juce::Array<ProgressionEntry> ProgressionData::getAllEntries(int source) const
{
    juce::Array<ProgressionEntry> result;
    if (source == 0 || source == 1) result.addArray(factoryEntries);
    if (source == 0 || source == 2) result.addArray(userEntries);
    return result;
}

juce::Array<ProgressionEntry> ProgressionData::filterByTags(const juce::StringArray& selectedTags) const
{
    if (selectedTags.isEmpty())
        return getAllEntries(0);

    auto allEntries = getAllEntries(0);
    juce::Array<ProgressionEntry> result;
    for (auto& entry : allEntries)
    {
        int matched = 0;
        for (auto& tag : selectedTags)
            if (entry.tags.contains(tag))
                matched++;
        if (matched == selectedTags.size())
            result.add(entry);
    }
    return result;
}

//==============================================================================
// 鑾峰彇鎵€鏈夋爣绛撅紙鍘婚噸鎺掑簭锛?//==============================================================================
juce::StringArray ProgressionData::getAllTags() const
{
    juce::StringArray allTags;
    auto allEntries = getAllEntries(0);
    for (auto& entry : allEntries)
        for (auto& tag : entry.tags)
            allTags.addIfNotAlreadyThere(tag);
    allTags.sortNatural();
    return allTags;
}

//==============================================================================
// 鐢ㄦ埛棰勮绠＄悊
//==============================================================================
bool ProgressionData::addUserPreset(const ProgressionEntry& entry)
{
    if (entry.name.isEmpty() || entry.romanSequence.isEmpty())
        return false;

    for (auto& existing : userEntries)
    {
        if (existing.name.compareIgnoreCase(entry.name) == 0)
            return false;
    }

    ProgressionEntry newEntry = entry;
    newEntry.isUserPreset = true;
    userEntries.add(newEntry);
    return saveEntryToFile(newEntry, userDir);
}

bool ProgressionData::removeUserPreset(const juce::String& name)
{
    userEntries.removeIf([&](const ProgressionEntry& e) { return e.name == name; });
    juce::String safeName = name.replaceCharacters("\\/:*?\"<>|", "_");
    juce::File target = userDir.getChildFile(safeName + ".json");
    return target.deleteFile();
}

bool ProgressionData::updateUserPreset(const juce::String& oldName, const ProgressionEntry& newEntry)
{
    if (newEntry.name.isEmpty() || newEntry.romanSequence.isEmpty())
        return false;

    if (oldName != newEntry.name)
    {
        for (auto& existing : userEntries)
            if (existing.name.compareIgnoreCase(newEntry.name) == 0)
                return false;
    }

    removeUserPreset(oldName);
    return addUserPreset(newEntry);
}

bool ProgressionData::createUserFolder(const juce::String& name)
{
    auto trimmedName = name.trim();
    if (trimmedName.isEmpty())
        return false;

    for (const auto& folder : userFolders)
        if (folder.name.compareIgnoreCase(trimmedName) == 0)
            return false;

    UserPresetFolder folder;
    folder.id = "folder_" + juce::String::toHexString(juce::Time::currentTimeMillis());
    folder.name = trimmedName;
    userFolders.add(folder);
    saveUserFolders();
    return true;
}

bool ProgressionData::renameUserFolder(const juce::String& id, const juce::String& newName)
{
    auto trimmedName = newName.trim();
    if (id.isEmpty() || trimmedName.isEmpty())
        return false;

    for (const auto& folder : userFolders)
        if (folder.id != id && folder.name.compareIgnoreCase(trimmedName) == 0)
            return false;

    for (auto& folder : userFolders)
    {
        if (folder.id == id)
        {
            folder.name = trimmedName;
            saveUserFolders();
            return true;
        }
    }
    return false;
}

bool ProgressionData::deleteUserFolder(const juce::String& id)
{
    if (id.isEmpty() || getUserFolderEntryCount(id) > 0)
        return false;

    userFolders.removeIf([&](const UserPresetFolder& folder) { return folder.id == id; });
    saveUserFolders();
    return true;
}

bool ProgressionData::moveUserPresetToFolder(const juce::String& presetName, const juce::String& folderId)
{
    for (auto& entry : userEntries)
    {
        if (entry.name == presetName)
        {
            entry.folderId = folderId;
            return saveEntryToFile(entry, userDir);
        }
    }
    return false;
}

juce::Array<ProgressionEntry> ProgressionData::getUserEntriesInFolder(const juce::String& folderId) const
{
    juce::Array<ProgressionEntry> result;
    for (const auto& entry : userEntries)
        if (entry.folderId == folderId)
            result.add(entry);
    return result;
}

int ProgressionData::getUserFolderEntryCount(const juce::String& folderId) const
{
    int count = 0;
    for (const auto& entry : userEntries)
        if (entry.folderId == folderId)
            ++count;
    return count;
}

//==============================================================================
// 淇濆瓨/鍒犻櫎鏂囦欢杈呭姪
//==============================================================================
bool ProgressionData::saveEntryToFile(const ProgressionEntry& entry, const juce::File& dir)
{
    if (!dir.exists()) dir.createDirectory();
    juce::String safeName = entry.name.replaceCharacters("\\/:*?\"<>|", "_");
    juce::File outFile = dir.getChildFile(safeName + ".json");
    juce::Array<juce::var> array;
    array.add(entry.toJSON());
    juce::FileOutputStream outStream(outFile);
    if (outStream.openedOk())
    {
        outStream.setPosition(0);
        outStream.truncate();
        juce::JSON::writeToStream(outStream, juce::var(array));
        return true;
    }
    return false;
}

bool ProgressionData::deleteEntryFile(const juce::String& name, const juce::File& dir)
{
    juce::String safeName = name.replaceCharacters("\\/:*?\"<>|", "_");
    juce::File target = dir.getChildFile(safeName + ".json");
    return target.deleteFile();
}

//==============================================================================
// 鏀惰棌绠＄悊
//==============================================================================
void ProgressionData::loadFavorites()
{
    favoriteNames.clear();
    if (favoritesFile.existsAsFile())
    {
        juce::FileInputStream inStream(favoritesFile);
        if (inStream.openedOk())
        {
            auto json = juce::JSON::parse(inStream);
            if (auto* arr = json.getArray())
                for (auto& item : *arr)
                    favoriteNames.add(item.toString());
        }
    }
}

void ProgressionData::saveFavorites()
{
    juce::Array<juce::var> arr;
    for (auto& name : favoriteNames)
        arr.add(juce::var(name));
    juce::FileOutputStream outStream(favoritesFile);
    if (outStream.openedOk())
        juce::JSON::writeToStream(outStream, juce::var(arr));
}

bool ProgressionData::isFavorite(const juce::String& name) const
{
    return favoriteNames.contains(name);
}

void ProgressionData::setFavorite(const juce::String& name, bool shouldBeFavorite)
{
    if (shouldBeFavorite)
        favoriteNames.addIfNotAlreadyThere(name);
    else
        favoriteNames.removeString(name);
    saveFavorites();
}
//==============================================================================
// 甯哥敤绠＄悊瀹炵幇
//==============================================================================
void ProgressionData::loadFrequent()
{
    frequentNames.clear();
    if (frequentFile.existsAsFile())
    {
        juce::FileInputStream inStream(frequentFile);
        if (inStream.openedOk())
        {
            auto json = juce::JSON::parse(inStream);
            if (auto* arr = json.getArray())
                for (auto& item : *arr)
                    frequentNames.add(item.toString());
        }
    }
}

void ProgressionData::saveFrequent()
{
    juce::Array<juce::var> arr;
    for (auto& name : frequentNames)
        arr.add(juce::var(name));
    juce::FileOutputStream outStream(frequentFile);
    if (outStream.openedOk())
        juce::JSON::writeToStream(outStream, juce::var(arr));
}

bool ProgressionData::isFrequent(const juce::String& name) const
{
    return frequentNames.contains(name);
}

void ProgressionData::setFrequent(const juce::String& name, bool shouldBeFrequent)
{
    if (shouldBeFrequent)
        frequentNames.addIfNotAlreadyThere(name);
    else
        frequentNames.removeString(name);
    saveFrequent();
}

void ProgressionData::loadUserFolders()
{
    userFolders.clear();
    if (!userFoldersFile.existsAsFile())
        return;

    juce::FileInputStream inStream(userFoldersFile);
    if (!inStream.openedOk())
        return;

    auto json = juce::JSON::parse(inStream);
    auto* obj = json.getDynamicObject();
    if (obj == nullptr)
        return;

    if (auto* folders = obj->getProperty("folders").getArray())
    {
        for (const auto& item : *folders)
        {
            if (auto* folderObj = item.getDynamicObject())
            {
                UserPresetFolder folder;
                folder.id = folderObj->getProperty("id").toString();
                folder.name = folderObj->getProperty("name").toString();
                if (folder.id.isNotEmpty() && folder.name.isNotEmpty())
                    userFolders.add(folder);
            }
        }
    }
}

void ProgressionData::saveUserFolders()
{
    juce::Array<juce::var> folders;
    for (const auto& folder : userFolders)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", folder.id);
        obj->setProperty("name", folder.name);
        folders.add(juce::var(obj));
    }

    auto* root = new juce::DynamicObject();
    root->setProperty("folders", juce::var(folders));

    juce::FileOutputStream outStream(userFoldersFile);
    if (outStream.openedOk())
    {
        outStream.setPosition(0);
        outStream.truncate();
        juce::JSON::writeToStream(outStream, juce::var(root));
    }
}
