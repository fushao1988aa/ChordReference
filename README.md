# ChordReference – 和弦调性速查器

---

## 一、总体架构

ChordReference 是一个**和弦调性分析与创作辅助**插件，主要面向音乐制作人、作曲者。它既能**实时识别MIDI输入的和弦**，又能作为**智能和弦进行预设浏览器**，还能生成**上下文感知的分解琶音**，并正在开发**自动旋律生成**功能。
整体采用经典的 **JUCE AudioProcessor + GUI** 架构，辅以独立的数据层、音乐理论引擎和UI组件。

### 模块层次图（概念）

```
┌─────────────────────────────────────────────────────────────┐
│                      PluginProcessor                         │
│  - MIDI输入处理（和弦识别数据库）                             │
│  - 内置钢琴合成器（试听）                                     │
│  - 状态持久化                                                │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      PluginEditor                            │
│  - TopBar（工具栏）                                          │
│  - ScaledContentComponent（可缩放主内容区）                   │
│  - ProgressionBrowser（预设浏览器，模态弹出）                 │
│  - RegistrationOverlay（注册覆盖层）                         │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
        ┌──────────┐  ┌─────────────┐  ┌──────────────┐
        │TopBar    │  │ScaledContent│  │Progression   │
        │          │  │Component    │  │Browser       │
        └──────────┘  └─────────────┘  └──────────────┘
                            │                  │
            ┌───────────────┼───────────────┐  │
            ▼               ▼               ▼  ▼
      CircleOfFifths   ChordCard x3    MiniKeyboard  ProgressionCard
            │               │               │          (预设列表卡片)
            └───────────────┴───────────────┘
                              │
                    ┌─────────┴─────────┐
                    ▼                   ▼
            ScaleEngine          TSDBuilder
            ProgressionEngine    ChordNaming
            MoodMapper           ...
```

### 外部构建与依赖

- **JUCE 8.0.12**（音频/MIDI/UI基础）
  
- **BinaryData**：嵌入字体、钢琴采样、出厂预设ZIP、语言JSON
  
- **C++17** 编译器（Visual Studio 2022 / Xcode / GCC）使用 STL 容器和算法
  
- [VST3 SDK](https://steinbergmedia.github.io/vst3_dev_portal/)（需自行下载，放置于 `./VST_SDK/` 目录）
  

- ### 构建步骤
  

1. 克隆本仓库
  
2. 下载 VST3 SDK 并设置项目中的 SDK 路径
  
3. 打开 `ChordReference.jucer`（或对应的 IDE 解决方案文件）
  
4. 使用 Projucer 导出工程并编译
  
5. 生成的 VST3 插件位于 `Builds/` 目录下
  
  > 注意：由于许可证限制，本仓库不包含 VST3 SDK 和 JUCE 二进制文件，请自行下载并配置。
  

## 二、核心功能模块详解

### 1. 和弦识别引擎 (`PluginProcessor`)

#### 数据结构

- `KnownChord`：存储一个和弦的调性、调式、功能名、音高集合、低音（用于转位匹配）。
  
- `chordDatabaseBySize` 和 `chordDatabaseByPitchSet`：两个索引，分别按音符数量和音高集合查找。
  

#### 构建过程

`buildChordDatabase()` 为**15个大调主音**（包括等音）构建：

- 大调/小调下：T/S/D 三和弦（含转位）
  
- 大调/小调下：7/9/11/13 和弦（含转位）
  
- 为每个属七和弦添加引申音变体：`b9`, `#9`, `#11`, `b13`, `b9#11`, `#9#11`, `b9b13`
  
- 额外特殊和弦：`Iaug7`, `V7sus4`, `iminMaj9`, `v7sus4`
  

数据库非常庞大，覆盖了绝大多数常见和弦及转位。

#### 实时识别流程

1. **MIDI输入回调**：记录按下的音符、力度、延音踏板。
  
2. **防抖机制**：音符集合稳定`75ms`后才触发识别。
  
3. **匹配策略**（优先级从高到低）：
  
  - 当前调性 + 当前低音 + 音高集完全匹配
    
  - 当前调性 + 音高集匹配（忽略低音）
    
  - 全局匹配（任何调性） + 低音优先
    
  - 全局匹配（任何调性）
    
4. **输出结果**：`MidiChordResult` 包含调性、调式、功能名（如 `T7`）、MIDI音符、力度等。
  
5. **触发回调**：`newResultAvailable` 通知 UI 线程刷新。
  

#### 试听系统

- 内置钢琴采样器（7层，C0-C6），带混响、高通/峰值/高架滤波、软限幅。
  
- 支持两种试听模式：**内部钢琴** 和 **外部MIDI输出**。
  
- `triggerAudition()` 可同时触发多个音符。
  

### 2. UI 核心组件

#### `ScaledContentComponent`（可缩放主区域）

- **FlexBox 布局**：自适应窗口大小，缩放比例由 `getCurrentScale()` 决定。
  
- **包含子组件**：
  
  - `CircleOfFifths`（五度圈）
    
  - 三张 `ChordCard`（T/S/D 和弦卡片）
    
  - `MiniKeyboard`（迷你键盘）
    
  - 底部信息栏（显示调性、MIDI力度/踏板、智能生成提示）
    
- **状态同步**：
  
  - 当用户点击五度圈或MIDI识别改变调性时，调用 `setKey()` 更新所有卡片和五度圈。
    
  - 当卡片手动修改（转位、类型、质量）时，设置 `manualOverride = true` 阻止 MIDI 自动覆盖，并刷新键盘高亮。
    
- **皮肤/语言**：通过 `setSkin()` / `setLanguage()` 传递给所有子组件。
  
- **智能生成**：点击五度圈中心骰子触发 `SmartGenerator::generate()`，生成新的 T/S/D 序列并应用到三张卡片，同时记录到 Processor 用于状态恢复。
  

#### `ChordCard`（和弦卡片）

- **显示**：和弦符号（如 `Cmaj7`）、中文名、罗马数字、构成音、情绪文字。
  
- **交互**：
  
  - 点击卡片主体：试听该和弦（通过 Processor）。
    
  - 底部按钮：转位循环、节奏选择（用于拖拽MIDI时的时值）、拖拽MIDI按钮。
    
  - 底部质量栏：点击弹出质量菜单（大三/小三/属七/减/增/sus2/sus4等），对属七还有引申音子菜单（b9/#9/#11等）。
    
  - 数字按钮（3/7/9/11/13）：切换和弦扩展类型，并根据当前质量判断是否允许（例如减和弦只能到7）。
    
- **拖拽MIDI**：生成 `.mid` 临时文件，调用 `performExternalDragDropOfFiles` 拖入宿主。
  
- **负和声**：当Processor开启负和声时，卡片显示的和弦会被翻转（`NegativeHarmony::flipChord`），符号后添加 "NH"。
  

#### `CircleOfFifths`（五度圈）

- 绘制12个扇形切片（外圈大调，内圈关系小调）。
  
- **点击扇区**：切换调性（大调或小调，取决于点击外圈还是内圈）。
  
- **点击中心骰子**：触发智能生成（调用 `SmartGenerator`），并播放旋转动画。
  
- **右键骰子**：弹出风格菜单（通用/流行/爵士/古风），影响智能生成的转移概率表。
  

#### `ProgressionBrowser`（预设浏览器）

这是项目中**代码量最大**的UI组件（约3000行），实现了完整的预设管理系统。

#### 主要功能

- **左侧分类列表**：
  
  - 全部预设
    
  - 6个智能分类（体裁/地域/情绪/复杂度/曲式段落/中国风标签）
    
  - 收藏夹
    
  - 常用（Frequent）
    
  - 用户预设
    
  - 用户自定义文件夹（可创建/重命名/删除，支持移动预设）
    
- **中间标签过滤区**：根据左侧分类动态显示该分类下的所有标签，点击标签筛选预设。
  
- **搜索框**：实时按预设名称、描述、标签过滤。
  
- **中央卡片列表**：每个预设显示为 `ProgressionCard`，支持单选、多选（Shift+点击）、收藏、编辑（仅用户预设）、删除（仅用户预设）。
  
- **右侧详情面板**：显示封面（Base64解码）、名称、标签、和弦进行符号（根据当前调性实时计算）、罗马序列、描述，并提供两个拖拽按钮：
  
  - **拖拽预设到轨道**：生成整个和弦进行的MIDI（使用 `ProgressionRenderer`）。
    
  - **智能分解MIDI**：根据当前琶音设置，生成分解琶音的MIDI（使用 `ArpeggioEngine`）。
    
- **底部播放栏**：播放/停止当前选中预设的试听（按小节循环播放和弦进行）。
  
- **导入/导出**：支持单JSON、多JSON、ZIP包导入导出用户预设，自动重名处理。
  
- **预设编辑器**：`PresetEditorPanel` 允许用户新建/编辑预设，包括名称、罗马序列、描述、标签（六类多选）、封面图片（Base64编码存储）。
  

#### 数据层 (`ProgressionData`)

- 出厂预设从嵌入的 `FactoryPresets.zip` 解压到 `AppData/ChordReference/FactoryPresets`，仅首次启动或版本变化时解压。
  
- 用户预设保存在 `AppData/ChordReference/UserPresets`，每个预设一个JSON文件。
  
- 收藏和常用列表分别保存在 `favorites.json` 和 `frequent.json`。
  
- 用户文件夹定义保存在 `userFolders.json`。
  
- 所有操作（增删改）都会同步到文件。
  

### 3. 琶音智能生成系统 (`ArpeggioEngine` 全家桶)

#### `ArpeggioPatternLibrary`

定义了 **200+ 个内置琶音范式**，每个范式包含：

- 元数据：`id`, `nameCN`, `nameEN`, `familyId`, `grooveAffinity`, `complexity`, `energy` 等。
  
- 风格标签：`styleTags`, `moodTags`, `regionTags`, `functionFit`, `formFit`。
  
- 节奏步骤：`vector<ArpeggioStep>`，每个步骤有 `beatOffset`, `durationBeats`, `chordToneIndex`, `octaveOffset`, `velocityOffset`, `isBassAnchor`。
  

范式覆盖极广：古典阿尔贝蒂低音、现代流行分解、R&B错位、电影铺底、Future Bass、Lo-Fi摇摆、国风开五度、马卡姆装饰、甘美兰交错、拉丁蒙图诺、爵士低音行走、拉格持续音、中国少数民族（蒙古、藏族、侗族、彝族等）、欧洲民谣（凯尔特、法朵、弗拉门戈）、日本都节/琉球、K-Pop/J-Pop/Anime、赛博朋克、影视科幻、新闻媒体、Vlog、游戏8-bit、迪士尼魔法、企业宣传片、历史纪录片……以及大量针对具体预设名称的精准范式（如“周杰伦钢琴”“加勒比海盗”“黄梅戏”等）。

#### `ArpeggioMatcher`

核心匹配引擎，根据 `ArpeggioContext` 对每个范式进行**多维度打分**：

- **风格匹配**：`styleTags` 命中 +18分/个
  
- **情绪匹配**：`moodTags` 命中 +12分/个
  
- **地域匹配**：`regionTags` 命中 +12分/个
  
- **功能匹配**：`functionTags` 命中 +12分/个
  
- **段落匹配**：`formTags` 命中 +18分/个，另有 `scoreFormDynamics` 根据段落（intro/verse/chorus/bridge/outro）调整分数。
  
- **家族匹配**：`scoreFamilyFit` 针对大量硬编码的规则，例如中国风标签对 "ChineseOpenFamily" 加分，拉丁标签对 "LatinMontunoFamily" 加分等。
  
- **律动匹配**：`scoreGrooveFit` 根据风格和BPM匹配 grooveAffinity (Straight/Late/Tight/Shuffle)。
  
- **复杂度匹配**：`scoreComplexityFit` 根据和弦扩展程度匹配 pattern.complexity。
  
- **织体匹配**：`scoreTextureFit` 针对中国戏曲、民族器乐、马卡姆、拉格、甘美兰、踏板持续音、半音张力等特定纹理加分。
  
- **密度匹配**：`scoreDensity` 根据用户选择的密度和pattern.density。
  
- **速度匹配**：`scoreTempo` 根据BPM对密度偏好调整。
  
- **预设名称深度匹配**：`nameMatchBonus` 部分最为庞大，对预设名称中出现的“周杰伦”“加勒比”“黄梅戏”“黑田”“王道”“军乐”“蒙古”“人工智能”“赛博朋克”等数百个关键词进行定向加分拦截，甚至能识别“2-5-1爵士”“12小节蓝调”等乐理词汇。
  
- **通用底座保底**：`gen_univ_` 系列范式获得 +25 基础分，确保总有合适的推荐。
  

最终返回按分数排序的 `ArpeggioRankedPattern` 列表。

#### `ArpeggioEngine` & `ArpeggioMidiRenderer`

- `buildMidiChords`：将罗马序列和弦（音高类）转换为 MIDI 音符，处理声部连接（平滑八度分配）。
  
- `renderToSequence`：根据选中的 pattern 和上下文，生成 `MidiMessageSequence`。支持：
  
  - 低音加倍、左手根音/五音分解、右手上层色彩音（高彩层）。
    
  - 踏板控制。
    
  - 基于和声功能的力度调整（Dominant 和弦加重，Tonic 稍轻）。
    
  - 人性化：时间偏移、力度随机、时值缩放。
    
  - 律动偏移：根据 pattern 的 grooveAffinity 对后半拍或切分位置做微量偏移。
    
- `writeMidiFile`：导出 MIDI 文件，用于拖拽或保存。
  

#### `ProgressionBrowser` 中的琶音控制栏

用户可以在浏览器底部调整：

- **智能/手动**：智能模式下自动推荐排名第一的范式，手动模式从分类菜单选择。
  
- **密度**：Whole/Half/Quarter/Eighth/Sixteenth/Mixed。
  
- **高彩层**、**左根分解**、**倍低** 三个开关。
  
- **人性化滑块**：0-100。
  
- **选择范式按钮**：弹出按类别分类的菜单（华语/流行钢琴、J-Pop/电子、民谣指弹、国风器乐、影视军乐、爵士拉丁、调式世界、氛围铺底、基础通用）。
  

底部状态栏会实时显示当前选中的预设名称、推荐模式、范式名称、密度、附加层、人性化百分比。

### 4. 音乐理论引擎

#### `ScaleEngine`

- 生成7种调式的音阶（Major, NaturalMinor, Dorian, Phrygian, Lydian, Mixolydian, Locrian）。
  
- 根据音阶、根音级数和音符数量（3-7）构建三度堆叠和弦，支持 `ChordQuality` 修正（大三/小三/属七/减/增/sus2/sus4 等）。
  

#### `ProgressionEngine`

- 解析罗马数字序列（如 "I V vi IV"），支持：
  
  - 升降号前缀（bIII, #IV）
    
  - 和弦质量后缀（maj7, m7, dim7, sus2, sus4, 9, 11, 13, add9, 6, ...）
    
  - 斜杠低音（V/IV, I/III）
    
  - 副属和弦（V/ii, V/V 等）
    
- `getChordNotes`：将解析后的序列转换为每个和弦的音高类集合（0-11），供渲染器使用。
  

#### `TSDBuilder`

- 根据调性和调式返回 T/S/D 三和弦家族（大调 I/IV/V，小调 i/iv/V，中古调式对应调整）。
  
- 对 Locrian 调式的 V 级自动升半音（变成大三和弦）。
  
- 小调的 V 级使用和声小调（升高 VII 级）。
  

#### `ChordNaming`

- 根据根音、质量、扩展数字生成标准符号（如 `Cm7b5`）。
  
- 根据功能名（T/S/D7/t9）生成符号（如 D9 → G9）。
  
- 提供中文化名称（如 "Cmaj7" → "C大七"）。
  

#### `MoodMapper`

- 根据主音、功能名（T/S/D/t/s）和调式返回情绪描述文字（如“稳定平静”“过渡性”“紧张”）和氛围颜色。
  
- 数据为硬编码映射表。
  

#### `NegativeHarmony`

- 计算给定主音的负和声轴（属音上方小三度），翻转单个音高或整个和弦。

### 5. 智能生成器 (`SmartGenerator`)

- 基于概率转移表生成 T/S/D 功能序列（长度为3，对应三张卡片）。
  
- 支持四种风格：通用、流行、爵士、古风。
  
- 可附加七和弦后缀（`T7`, `S7` 等）。
  
- 首尾倾向于 T，避免无效进行。
  

### 6. 辅助模块

#### `RegistrationManager` & `RegistrationOverlay`

- 30天试用期，基于系统时间天数。
  
- 硬编码注册码列表（暂定，后期版本可更新更复杂的安全机制）。
  
- 保存用户偏好：窗口大小、皮肤索引、语言。
  
- 过期后显示覆盖层，阻止使用。
  

#### `LocalisedStrings`

- 从嵌入的 `strings_zh.json` / `strings_en.json` 加载键值对。
  
- 所有UI文字通过 `TR("key")` 调用，支持中英文切换。
  

#### `SkinColours`

- 四套完整配色：现代紫、未来蓝、象牙米、黑金。
  
- 每个主题定义背景渐变、信息栏、卡片、键盘、五度圈、预设浏览器的颜色。
  

#### `TagDefinitions` & `TagLocalizer`

- 定义六类标签（调式、地域、情感、复杂度、曲式、中国风）。
  
- `TagLocalizer::translateTag()` 将内部英文标签（如 "auto_major"）转为用户可见文字，同时支持反向映射（`大调` → `auto_major`）。
  

#### `BinaryData`

- CMake 的 `juce_add_binary_data` 嵌入了：
  
  - 鸿蒙中文字体
    
  - 7个钢琴采样 WAV
    
  - 出厂预设 ZIP
    
  - 中英文语言 JSON
    

#### 单元测试 (`UnitTests.cpp`)

- 测试 `ScaleEngine::getChord` 和 `TSDBuilder` 的基本正确性。
  
- 通过 JUCE 的 `UnitTest` 框架注册。
  

---

## 三、数据流与关键交互
### 实时MIDI识别流程

```实时MIDI识别流程
外部MIDI键盘/DAW
       │
       ▼
PluginProcessor::handleIncomingMidiMessage
       │
       ▼ (noteLock)
更新 activeNotes、activeNoteVelocities、sustainPedalDown
       │
       ▼ (每 processBlock)
稳定检测 → tryMatchChord()
       │
       ▼ (resultLock)
更新 lastResult，设置 newResultAvailable = true
       │
       ▼ (UI Timer 每100ms)
PluginEditor::timerCallback() 读取 getLastMidiChordResult()
       │
       ▼
ScaledContentComponent::setKeyAndChord() 或 setChordOnly()
       │
       ├─► 更新 CircleOfFifths 高亮
       ├─► 更新三张 ChordCard 的和弦数据
       └─► 刷新 MiniKeyboard 高亮
```

### 用户交互流程（示例：点击五度圈切换调性）
```用户交互流程（示例：点击五度圈切换调性）
用户点击五度圈扇区
       │
       ▼
CircleOfFifths::mouseDown → onKeySelected(tonic, type)
       │
       ▼
ScaledContentComponent::setKey(tonic, type)
       │
       ├─► 更新 currentTonic/currentType
       ├─► 更新 CircleOfFifths 高亮
       ├─► 调用 updateDisplay() (重建 T/S/D 和弦)
       └─► 调用 onKeyChanged 通知 Processor 保存状态
       │
       ▼
ChordCard::setChord() 更新卡片显示
       │
       ▼
刷新 MiniKeyboard 高亮
```

### 预设浏览器使用智能琶音流程
```预设浏览器使用智能琶音流程
用户在 ProgressionBrowser 中选择一个预设卡片
       │
       ▼
ProgressionCard::onClick → inspector.setEntry()
       │
       ▼
底部栏显示预设名称和当前琶音推荐
       │
用户点击“智能分解 MIDI”按钮
       │
       ▼
mouseDrag (从 dragArpeggioToTrackButton 发起)
       │
       ├─► 获取选中的 ProgressionEntry
       ├─► 创建 ArpeggioContext（融合预设名称、标签、描述、罗马序列、当前调性等）
       ├─► 根据 UI 设置（智能/手动、密度、人性化等）生成 ArpeggioGenerationSettings
       ├─► 调用 ArpeggioEngine::generateMidiFile()
       │       ├─► ArpeggioMatcher::rankPatterns (若智能模式)
       │       ├─► ArpeggioEngine::generate (若有首选 pattern)
       │       └─► ArpeggioMidiRenderer::writeMidiFile
       └─► performExternalDragDropOfFiles 将 .mid 文件拖入宿主
```


---


## 四、技术亮点代码质量

1. **模块化**：每个功能域独立成类/文件，职责单一。
  
2. **性能优化**：和弦识别有防抖；标签统计有缓存；MIDI设备列表按需刷新；UI缩放避免频繁重绘。
  
3. **可扩展性**：添加新琶音范式只需在 `ArpeggioPatternLibrary` 末尾追加，匹配器自动参与评分。
  
4. **鲁棒性**：大量 try-catch 保护和弦构建失败；文件操作有错误检查；空指针判断。
  
5. **用户体验**：情绪文字、封面图片、拖拽生成、实时反馈、多语言、多皮肤。
  
6. **跨平台考虑**：使用 JUCE 抽象层，路径处理、字体降级均有 Windows/macOS/Linux 分支。
  
7. **高性能实时识别****：基于预计算和弦数据库，匹配速度快，CPU 占用极低
  
8. **现代 C++ 与 JUCE 框架****：代码结构清晰，易于维护和扩展
  
9. **跨平台支持****：Windows / macOS（AU 待定），VST3 格式
  
10. **完全的 MIDI 拖拽支持****：生成的 MIDI 文件可直接拖入任何 DAW
  
11. **强大和弦进行分解支持****：生成的 和弦进行分解MIDI 文件可直接拖入任何 DAW
  

---
---

## 版本历史

### v1.8.0（当前版本）

- **用户文件夹管理**：可创建自定义文件夹，将用户预设分类存储
- **情绪映射双行显示**：修复情绪描述显示不全的问题，主情绪与附加情绪分行展示
- **负和声污染 Bug 修复**：关闭负和声后不再残留影响卡片状态
- **五度圈右键风格菜单**：右键骰子直接切换生成风格，无需进入额外菜单
- **界面响应式优化**：修复窗口缩放时部分元素错位
- 若干性能优化和内存泄漏修复

<details>
<summary>查看更早版本</summary>

### v1.7.0

- 完整双语支持：中英文界面切换，所有文本外部化
- 新增“未来蓝”、“象牙米”、“尊黑金”皮肤
- MIDI 输出模式（将识别到的和弦发送到外部 MIDI 设备或 DAW 其他轨道）
- 试听模式选择（内置/外部 MIDI）

### v1.6.0

- 用户自定义预设：添加、编辑、删除、导入/导出（JSON / ZIP）
- 封面图片支持（Base64 存储）
- 收藏和常用标记
- 批量删除/导出用户预设

### v1.5.0

- 扩展和弦支持：7/9/11/13 和弦及转位
- 属七和弦引申音子菜单（b9, #9, #11, b13 等）
- 数据库重构，识别准确率大幅提升
- 情绪映射细化，增加氛围色混合

### v1.4.0

- 智能生成功能：中心骰子随机生成 T/S/D 进行
- 支持四种生成风格（通用、流行、爵士、古风）
- 负和声模式（实验性）
- 皮肤系统（两种主题）

### v1.3.0

- 中古调式支持（多利亚、弗里吉亚、利底亚、混合利底亚、洛克里亚）
- 五度圈支持小调内圈点击
- 锁调模式（只更新和弦功能，不改变调性）

### v1.2.0

- **重大更新：预设浏览器**（内置 50 个出厂预设，分类筛选，拖拽生成 MIDI）
- 情绪色彩映射

### v1.1.0

- 和声小调属和弦支持（V7 升高七级）
- 和弦质量切换（增三、减三、挂留）
- 转位显示与切换
- 修复 MIDI 识别防抖问题

### v1.0.0

- 首个正式测试版本（C 大调 / A 小调下的 T/S/D 三和弦显示）
- 基础五度圈交互
- 基础 MIDI 输入识别（大三/小三和弦）
- 内置简易采样钢琴试听

</details>

---

## 许可证

本项目采用 **GPLv3** 许可证。  
请注意，VST3 SDK 和 JUCE 框架分别遵循其各自的许可证，使用时请遵守相关条款。

---

## 作者

开发维护：fushao1988aa  
欢迎提 Issue 或 Pull Request。

---

## 截图预览

> 和弦调性主界面.png
> <img width="2556" height="1375" alt="和弦调性主界面" src="https://github.com/user-attachments/assets/173b3a55-93b9-4db1-b699-219beba0d21e" />

> 和弦进行预设系统.png
> <img width="2560" height="1368" alt="和弦进行预设系统" src="https://github.com/user-attachments/assets/d0cf1543-e4ea-4d86-942f-e5408368fe14" />

> 分解范式分类库.
> <img width="1165" height="1613" alt="分解范式分类库" src="https://github.com/user-attachments/assets/31598bbf-db33-4463-a664-c1975cd4f1cb" />

