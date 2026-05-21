//==================================================================================================
//  ProgressionBrowser.cpp （前半部分）
//  项目：ChordReference (和弦调性速查器)
//  功能：和弦进行预设浏览器实现
//        - 左侧类别列表、中间标签过滤、中央卡片、右侧详情面板
//        - 播放控制、导入导出、拖拽 MIDI 文件
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "ProgressionBrowser.h"
#include "PluginProcessor.h"
#include "PitchNameHelper.h"
#include "ScaleEngine.h"
#include <algorithm>
#include "PresetEditorPanel.h"
#include "NegativeHarmony.h"
#include "ProgressionCard.h"
#include "PresetInspectorPanel.h"
#include "ModernPurpleLookAndFeel.h"
#include "TagDefinitions.h"
#include "LocalisedStrings.h"
#include "TagLocalizer.h"
#include "ArpeggioEngine.h"
#include "ArpeggioMatcher.h"

namespace
{
    constexpr int patternComboIdOffset = 100;
    constexpr int userAddedPatternStartIndex = 198; // Patterns appended after ArpeggioPatternLibrary.cpp line 4008.

    void addIfMissing(juce::StringArray& values, const juce::String& value)
    {
        if (!values.contains(value, true))
            values.add(value);
    }

    juce::String getDensityLabel(ArpeggioDensity density)
    {
        switch (density)
        {
            case ArpeggioDensity::Whole:     return juce::String::fromUTF8(u8"全音符（4拍）");
            case ArpeggioDensity::Half:      return juce::String::fromUTF8(u8"二分（2拍）");
            case ArpeggioDensity::Quarter:   return juce::String::fromUTF8(u8"四分");
            case ArpeggioDensity::Eighth:    return juce::String::fromUTF8(u8"八分");
            case ArpeggioDensity::Sixteenth: return juce::String::fromUTF8(u8"十六分");
            case ArpeggioDensity::Mixed:     return juce::String::fromUTF8(u8"混合");
        }

        return {};
    }

    juce::String getRecommendationReasonLabel(const juce::String& reason)
    {
        juce::StringArray labels;
        if (reason.containsIgnoreCase("style"))
            labels.add(juce::String::fromUTF8(u8"风格"));
        if (reason.containsIgnoreCase("mood"))
            labels.add(juce::String::fromUTF8(u8"情绪"));
        if (reason.containsIgnoreCase("region"))
            labels.add(juce::String::fromUTF8(u8"地域"));
        if (reason.containsIgnoreCase("function"))
            labels.add(juce::String::fromUTF8(u8"功能"));
        if (reason.containsIgnoreCase("form"))
            labels.add(juce::String::fromUTF8(u8"段落"));
        if (reason.containsIgnoreCase("family"))
            labels.add(juce::String::fromUTF8(u8"范式"));
        if (reason.containsIgnoreCase("groove"))
            labels.add(juce::String::fromUTF8(u8"律动"));
        if (reason.containsIgnoreCase("complexity"))
            labels.add(juce::String::fromUTF8(u8"复杂度"));
        if (reason.containsIgnoreCase("texture"))
            labels.add(juce::String::fromUTF8(u8"织体"));

        if (labels.isEmpty())
            labels.add(juce::String::fromUTF8(u8"速度/密度"));

        return labels.joinIntoString("/");
    }

    juce::String getArpeggioPatternCategory(const ArpeggioPattern& pattern)
    {
        const auto family = pattern.familyId;
        const auto text = (pattern.id + " " + pattern.nameCN + " " + pattern.nameEN + " "
                           + pattern.styleTags.joinIntoString(" ") + " "
                           + pattern.regionTags.joinIntoString(" ") + " "
                           + pattern.formFit.joinIntoString(" ")).toLowerCase();

        auto has = [&](const char* token) {
            return text.contains(juce::String::fromUTF8(token).toLowerCase());
        };

        if (pattern.id.startsWith("form_") || pattern.id.startsWith("tech_")
            || pattern.id.startsWith("mood_") || pattern.id.startsWith("text_")
            || pattern.id.startsWith("groove_"))
            return juce::String::fromUTF8(u8"段落/技法/情绪/织体");

        if (has("j-pop") || has("jpop") || has("anime") || has(u8"日系") || has(u8"二次元")
            || has("k-pop") || has("kpop") || has(u8"韩系") || has("edm") || has("electro")
            || has("cyber") || has("trap") || has(u8"赛博") || has(u8"电子舞曲") || has(u8"快闪"))
            return juce::String::fromUTF8(u8"J-Pop/K-Pop/电子");

        if (has("march") || has("military") || has("anthem") || has("hymn") || has("pirates")
            || has("cinematic") || has("film") || has("string") || has(u8"军乐") || has(u8"进行曲")
            || has(u8"仪仗") || has(u8"国歌") || has(u8"弦乐") || has(u8"影视") || has(u8"加勒比"))
            return juce::String::fromUTF8(u8"影视/军乐/弦乐");

        if (has("jazz") || has("blues") || has("r&b") || has("rnb") || has("neo-soul")
            || has("soul") || has("citypop") || has("city-pop") || has("latin") || has("bossa")
            || has("salsa") || has(u8"爵士") || has(u8"蓝调") || has(u8"布鲁斯") || has(u8"拉丁"))
            return juce::String::fromUTF8(u8"爵士/R&B/拉丁");

        if (has("dorian") || has("phrygian") || has("lydian") || has("mixolydian") || has("locrian")
            || has("harmonic") || has("maqam") || has("raga") || has("gamelan") || has("baroque")
            || has("classical") || has(u8"调式") || has(u8"和声小调") || has(u8"弗里吉亚")
            || has(u8"利底亚") || has(u8"洛克里亚") || has(u8"马卡姆") || has(u8"巴洛克") || has(u8"古典"))
            return juce::String::fromUTF8(u8"调式/世界/张力/古典");

        if (has("c-pop") || has("cpop") || has("mandopop") || has("jay") || has(u8"华语")
            || has(u8"周杰伦") || has(u8"周氏") || has(u8"闽南") || has(u8"台湾"))
            return juce::String::fromUTF8(u8"华语/流行钢琴");

        if (has("folk") || has("acoustic") || has("travis") || has("celtic") || has(u8"民谣")
            || has(u8"指弹") || has(u8"校园") || has(u8"英伦") || has(u8"凯尔特"))
            return juce::String::fromUTF8(u8"民谣/吉他指弹");

        if (has("guofeng") || has("chinese") || has(u8"国风") || has(u8"古风") || has(u8"中国")
            || has(u8"蒙古") || has(u8"藏") || has(u8"高山族") || has(u8"鄂伦春") || has(u8"哈尼")
            || has(u8"羌") || has(u8"古筝") || has(u8"古琴") || has(u8"琵琶") || has(u8"戏曲")
            || has(u8"黄梅") || has(u8"角调") || has(u8"清乐") || has(u8"甘肃") || has(u8"青海"))
            return juce::String::fromUTF8(u8"国风/民族器乐");

        if (has("pad") || has("drone") || has("ambient") || has("intro") || has("outro")
            || has("cadence") || has(u8"氛围") || has(u8"铺底") || has(u8"尾奏") || has(u8"终止"))
            return juce::String::fromUTF8(u8"氛围/铺底/终止");

        if (family == "CPopBalladPianoFamily" || family == "MandopopVerseFamily"
            || family == "MandopopPrechorusFamily" || family == "MandopopBridgeFamily"
            || family == "MandopopChorusFamily" || family == "CPopChorusDriveFamily")
            return juce::String::fromUTF8(u8"华语/流行钢琴");

        if (family == "GuitarPickingFamily" || family == "CampusFolkFamily")
            return juce::String::fromUTF8(u8"民谣/吉他指弹");

        if (family == "ChineseOpenFamily" || family == "GuofengFlowFamily" || family == "GuzhengRollFamily"
            || family == "PipaSweepFamily" || family == "GuqinSparseFamily" || family == "ChineseOperaOrnamentFamily")
            return juce::String::fromUTF8(u8"国风/民族器乐");

        if (family == "MarchPulseFamily" || family == "OrchestralStaccatoFamily" || family == "HybridEpicStringFamily"
            || family == "CinematicStringPadFamily" || family == "CinematicOstinatoFamily")
            return juce::String::fromUTF8(u8"影视/军乐/弦乐");

        if (family == "NeoSoulFamily" || family == "JazzWalkingFamily" || family == "LatinMontunoFamily")
            return juce::String::fromUTF8(u8"爵士/R&B/拉丁");

        return juce::String::fromUTF8(u8"基础/通用");
    }

    bool tagContains(const juce::String& tag, const char* text)
    {
        return tag.containsIgnoreCase(juce::String::fromUTF8(text));
    }

    void classifyArpeggioTag(const juce::String& rawTag, ArpeggioContext& context)
    {
        const auto tag = TagLocalizer::translateTag(rawTag);
        const juce::String combined = rawTag + " " + tag;

        // Keep canonical recommendation tags plus legacy lowercase aliases for existing pattern metadata.
        if (tagContains(combined, "pop") || tagContains(combined, u8"流行"))
        {
            addIfMissing(context.styleTags, "Pop");
            addIfMissing(context.styleTags, "pop");
        }
        if (tagContains(combined, "auto_c_pop") || tagContains(combined, "auto_mandopop")
            || tagContains(combined, "mandopop") || tagContains(combined, "c-pop")
            || tagContains(combined, u8"华语"))
        {
            addIfMissing(context.styleTags, "Pop");
            addIfMissing(context.styleTags, "pop");
            addIfMissing(context.styleTags, "mandopop");
            addIfMissing(context.styleTags, "c-pop");
            addIfMissing(context.styleTags, "Chinese");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_guofeng") || tagContains(combined, "auto_ancient_style")
            || tagContains(combined, "guofeng") || tagContains(combined, "ancient_style")
            || tagContains(combined, u8"国风") || tagContains(combined, u8"古风"))
        {
            addIfMissing(context.styleTags, "Chinese");
            addIfMissing(context.styleTags, "folk");
            addIfMissing(context.styleTags, "cinematic");
            addIfMissing(context.regionTags, "chinese");
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "PentatonicOpen");
        }
        if (tagContains(combined, "future") || tagContains(combined, u8"未来贝斯"))
        {
            addIfMissing(context.styleTags, "EDM");
            addIfMissing(context.styleTags, "future-bass");
        }
        if (tagContains(combined, "j-pop") || tagContains(combined, u8"日系") || tagContains(combined, u8"动漫"))
        {
            addIfMissing(context.styleTags, "Pop");
            addIfMissing(context.styleTags, "pop");
            addIfMissing(context.styleTags, "j-pop");
            addIfMissing(context.styleTags, "jpop");
            addIfMissing(context.regionTags, "japanese");
        }
        if (tagContains(combined, "k-pop") || tagContains(combined, "kpop") || tagContains(combined, u8"韩系") || tagContains(combined, u8"韩国"))
        {
            addIfMissing(context.styleTags, "Pop");
            addIfMissing(context.styleTags, "pop");
            addIfMissing(context.styleTags, "k-pop");
            addIfMissing(context.styleTags, "kpop");
            addIfMissing(context.styleTags, "electro");
            addIfMissing(context.styleTags, "synth");
            addIfMissing(context.regionTags, "korean");
        }
        if (tagContains(combined, "r&b") || tagContains(combined, u8"节奏布鲁斯") || tagContains(combined, "soul") || tagContains(combined, u8"灵魂"))
        {
            addIfMissing(context.styleTags, "R&B");
            addIfMissing(context.styleTags, "rnb");
            addIfMissing(context.styleTags, "neo-soul");
        }
        if (tagContains(combined, "jay_chou") || tagContains(combined, u8"周杰伦"))
        {
            addIfMissing(context.styleTags, "Pop");
            addIfMissing(context.styleTags, "pop");
            addIfMissing(context.styleTags, "rnb");
            addIfMissing(context.styleTags, "neo-soul");
            addIfMissing(context.styleTags, "Chinese");
            addIfMissing(context.regionTags, "chinese");
            addIfMissing(context.textureTags, "PentatonicOpen");
        }
        if (tagContains(combined, "neo") || tagContains(combined, u8"爵士") || tagContains(combined, "jazz"))
        {
            addIfMissing(context.styleTags, "Jazz");
            addIfMissing(context.styleTags, "jazz");
        }
        if (tagContains(combined, u8"古典") || tagContains(combined, "classical") || tagContains(combined, u8"巴洛克"))
        {
            addIfMissing(context.styleTags, "Classical");
            addIfMissing(context.styleTags, "classical");
        }
        if (tagContains(combined, u8"摇滚") || tagContains(combined, "rock") || tagContains(combined, "metal")
            || tagContains(combined, u8"金属"))
        {
            addIfMissing(context.styleTags, "Rock");
            addIfMissing(context.styleTags, "rock");
            addIfMissing(context.moodTags, "energetic");
        }
        if (tagContains(combined, u8"电影") || tagContains(combined, "cinematic") || tagContains(combined, "soundtrack") || tagContains(combined, u8"原声"))
        {
            addIfMissing(context.styleTags, "Cinematic");
            addIfMissing(context.styleTags, "cinematic");
            addIfMissing(context.styleTags, "soundtrack");
        }
        if (tagContains(combined, u8"民谣") || tagContains(combined, "folk"))
        {
            addIfMissing(context.styleTags, "Folk");
            addIfMissing(context.styleTags, "folk");
        }
        if (tagContains(combined, u8"电子") || tagContains(combined, "edm") || tagContains(combined, "electro"))
        {
            addIfMissing(context.styleTags, "EDM");
            addIfMissing(context.styleTags, "electro");
            addIfMissing(context.styleTags, "edm");
        }
        if (tagContains(combined, "lo-fi") || tagContains(combined, "lofi") || tagContains(combined, u8"低保真"))
            addIfMissing(context.styleTags, "lofi");
        if (tagContains(combined, "hip-hop") || tagContains(combined, u8"嘻哈"))
            addIfMissing(context.styleTags, "hip-hop");
        if (tagContains(combined, "bossa") || tagContains(combined, u8"巴萨诺瓦"))
        {
            addIfMissing(context.styleTags, "Latin");
            addIfMissing(context.styleTags, "bossa");
        }
        if (tagContains(combined, "salsa") || tagContains(combined, u8"萨尔萨"))
        {
            addIfMissing(context.styleTags, "Latin");
            addIfMissing(context.styleTags, "salsa");
        }
        if (tagContains(combined, u8"中国") || tagContains(combined, u8"华语") || tagContains(combined, u8"古风") || tagContains(combined, u8"国风"))
        {
            addIfMissing(context.styleTags, "Chinese");
            addIfMissing(context.textureTags, "ChineseFolk");
        }
        if (tagContains(combined, u8"东北"))
        {
            addIfMissing(context.regionTags, "chinese");
            addIfMissing(context.styleTags, "folk");
            addIfMissing(context.textureTags, "EthnicFolk");
        }

        if (tagContains(combined, u8"温暖") || tagContains(combined, "warm") || tagContains(combined, u8"治愈"))
        {
            addIfMissing(context.moodTags, "Warm");
            addIfMissing(context.moodTags, "warm");
        }
        if (tagContains(combined, u8"伤感") || tagContains(combined, "sad") || tagContains(combined, u8"忧郁"))
        {
            addIfMissing(context.moodTags, "Sad");
            addIfMissing(context.moodTags, "sad");
        }
        if (tagContains(combined, u8"明亮") || tagContains(combined, u8"快乐") || tagContains(combined, "bright") || tagContains(combined, "happy"))
        {
            addIfMissing(context.moodTags, "Bright");
            addIfMissing(context.moodTags, "bright");
        }
        if (tagContains(combined, u8"轻快") || tagContains(combined, u8"俏皮") || tagContains(combined, "playful"))
            addIfMissing(context.moodTags, "playful");
        if (tagContains(combined, u8"希望") || tagContains(combined, "hopeful"))
            addIfMissing(context.moodTags, "hopeful");
        if (tagContains(combined, u8"浪漫") || tagContains(combined, "romantic"))
            addIfMissing(context.moodTags, "warm");
        if (tagContains(combined, u8"情绪") || tagContains(combined, u8"动情") || tagContains(combined, "emotional"))
            addIfMissing(context.moodTags, "warm");
        if (tagContains(combined, u8"蓄力") || tagContains(combined, u8"提升") || tagContains(combined, "build")
            || tagContains(combined, "building") || tagContains(combined, "climax"))
        {
            addIfMissing(context.moodTags, "tension");
            addIfMissing(context.formTags, "bridge");
        }
        if (tagContains(combined, u8"燃") || tagContains(combined, u8"热血") || tagContains(combined, "uplifting"))
        {
            addIfMissing(context.moodTags, "bright");
            addIfMissing(context.moodTags, "energetic");
            addIfMissing(context.moodTags, "epic");
        }
        if (tagContains(combined, u8"紧张") || tagContains(combined, u8"悬疑") || tagContains(combined, "tension") || tagContains(combined, "tense"))
        {
            addIfMissing(context.moodTags, "Tension");
            addIfMissing(context.moodTags, "tension");
        }
        if (tagContains(combined, u8"史诗") || tagContains(combined, "epic"))
        {
            addIfMissing(context.styleTags, "Epic");
            addIfMissing(context.styleTags, "epic");
            addIfMissing(context.moodTags, "Epic");
            addIfMissing(context.moodTags, "epic");
        }
        if (tagContains(combined, u8"梦幻") || tagContains(combined, u8"空灵") || tagContains(combined, "dreamy"))
        {
            addIfMissing(context.moodTags, "Dreamy");
            addIfMissing(context.moodTags, "dreamy");
        }
        if (tagContains(combined, u8"仙侠") || tagContains(combined, "ethereal"))
        {
            addIfMissing(context.moodTags, "Dreamy");
            addIfMissing(context.moodTags, "dreamy");
            addIfMissing(context.moodTags, "wide");
        }
        if (tagContains(combined, u8"平静") || tagContains(combined, u8"安静") || tagContains(combined, "calm"))
        {
            addIfMissing(context.moodTags, "Calm");
            addIfMissing(context.moodTags, "calm");
        }
        if (tagContains(combined, u8"开阔") || tagContains(combined, u8"空灵") || tagContains(combined, "wide"))
            addIfMissing(context.moodTags, "wide");
        if (tagContains(combined, u8"舞蹈") || tagContains(combined, "dance"))
        {
            addIfMissing(context.moodTags, "playful");
            addIfMissing(context.styleTags, "dance");
        }
        if (tagContains(combined, "3/4") || tagContains(combined, "6/8") || tagContains(combined, "waltz")
            || tagContains(combined, u8"三拍") || tagContains(combined, u8"华尔兹"))
        {
            addIfMissing(context.styleTags, "triple-meter");
            addIfMissing(context.textureTags, "TripleMeter");
            context.beatsPerBar = 3;
            context.beatsPerChord = 3.0;
        }
        if (tagContains(combined, "12/8"))
        {
            addIfMissing(context.styleTags, "compound-meter");
            addIfMissing(context.textureTags, "CompoundMeter");
            context.beatsPerBar = 4;
            context.beatsPerChord = 4.0;
        }

        if (tagContains(combined, u8"中国") || tagContains(combined, u8"华语") || tagContains(combined, u8"古风") || tagContains(combined, u8"国风")
            || tagContains(combined, "auto_c_pop") || tagContains(combined, "auto_mandopop") || tagContains(combined, "auto_guofeng")
            || tagContains(combined, u8"五声"))
            addIfMissing(context.regionTags, "chinese");
        if (tagContains(combined, u8"日本") || tagContains(combined, u8"日系") || tagContains(combined, "j-pop"))
            addIfMissing(context.regionTags, "japanese");
        if (tagContains(combined, "auto_nepal") || tagContains(combined, u8"尼泊尔"))
            addIfMissing(context.regionTags, "himalayan");
        if (tagContains(combined, "latin") || tagContains(combined, u8"拉丁"))
        {
            addIfMissing(context.styleTags, "Latin");
            addIfMissing(context.styleTags, "latin");
            addIfMissing(context.regionTags, "latin");
        }

        if (tagContains(combined, u8"主歌") || tagContains(combined, "verse"))
        {
            addIfMissing(context.formTags, "Verse");
            addIfMissing(context.formTags, "verse");
        }
        if (tagContains(combined, u8"副歌") || tagContains(combined, "chorus"))
        {
            addIfMissing(context.formTags, "Chorus");
            addIfMissing(context.formTags, "chorus");
        }
        if (tagContains(combined, u8"桥段") || tagContains(combined, "bridge"))
        {
            addIfMissing(context.formTags, "Bridge");
            addIfMissing(context.formTags, "bridge");
        }
        if (tagContains(combined, u8"过渡") || tagContains(combined, "transition"))
        {
            addIfMissing(context.formTags, "Bridge");
            addIfMissing(context.formTags, "bridge");
            addIfMissing(context.formTags, "prechorus");
            addIfMissing(context.moodTags, "tension");
        }
        if (tagContains(combined, u8"前奏") || tagContains(combined, "intro"))
        {
            addIfMissing(context.formTags, "Intro");
            addIfMissing(context.formTags, "intro");
        }
        if (tagContains(combined, u8"预副歌") || tagContains(combined, u8"主歌过渡") || tagContains(combined, "prechorus")
            || tagContains(combined, "pre-chorus"))
        {
            addIfMissing(context.formTags, "Bridge");
            addIfMissing(context.formTags, "bridge");
        }
        if (tagContains(combined, u8"间奏") || tagContains(combined, "interlude"))
        {
            addIfMissing(context.formTags, "Interlude");
            addIfMissing(context.formTags, "interlude");
            addIfMissing(context.formTags, "bridge");
        }
        if (tagContains(combined, u8"尾奏") || tagContains(combined, "outro"))
        {
            addIfMissing(context.formTags, "Outro");
            addIfMissing(context.formTags, "outro");
        }
        if (tagContains(combined, u8"终止") || tagContains(combined, u8"收束") || tagContains(combined, "cadence"))
        {
            addIfMissing(context.formTags, "Cadence");
            addIfMissing(context.formTags, "cadence");
        }

        if (tagContains(combined, u8"三和弦") || tagContains(combined, "triad"))
            addIfMissing(context.complexityTags, "Triad");
        if (tagContains(combined, u8"七和弦") || tagContains(combined, "seventh") || tagContains(combined, "7th"))
            addIfMissing(context.complexityTags, "Seventh");
        if (tagContains(combined, u8"九和弦") || tagContains(combined, "ninth") || tagContains(combined, "9th"))
            addIfMissing(context.complexityTags, "Ninth");
        if (tagContains(combined, u8"扩展") || tagContains(combined, "extended") || tagContains(combined, "11th") || tagContains(combined, "13th"))
            addIfMissing(context.complexityTags, "Extended");
        if (tagContains(combined, u8"调式") || tagContains(combined, "modal"))
            addIfMissing(context.complexityTags, "Modal");
        if (tagContains(combined, u8"半音") || tagContains(combined, "chromatic"))
            addIfMissing(context.complexityTags, "Chromatic");
        if (tagContains(combined, u8"和声小调") || tagContains(combined, "harmonic_minor"))
        {
            addIfMissing(context.styleTags, "rock");
            addIfMissing(context.styleTags, "classical");
            addIfMissing(context.complexityTags, "Modal");
            addIfMissing(context.complexityTags, "Chromatic");
            addIfMissing(context.textureTags, "ChromaticTension");
        }

        if (tagContains(combined, "auto_pentatonic") || tagContains(combined, u8"五声")
            || tagContains(combined, "auto_gong_mode") || tagContains(combined, "auto_yu_mode")
            || tagContains(combined, "auto_jue_mode") || tagContains(combined, "auto_zhi_mode"))
        {
            addIfMissing(context.textureTags, "PentatonicOpen");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_chinese_opera") || tagContains(combined, "auto_peking_opera")
            || tagContains(combined, "auto_kunqu") || tagContains(combined, "auto_huangmei_opera")
            || tagContains(combined, u8"戏曲") || tagContains(combined, u8"京剧") || tagContains(combined, u8"昆曲"))
        {
            addIfMissing(context.textureTags, "ChineseOpera");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_folk_song") || tagContains(combined, "auto_folk")
            || tagContains(combined, u8"民歌") || tagContains(combined, u8"民谣"))
            addIfMissing(context.textureTags, "EthnicFolk");
        if (tagContains(combined, "auto_erhu") || tagContains(combined, u8"二胡")
            || tagContains(combined, "auto_dizi") || tagContains(combined, u8"笛子") || tagContains(combined, u8"竹笛"))
        {
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "PentatonicOpen");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_yangqin") || tagContains(combined, u8"扬琴")
            || tagContains(combined, "auto_zheng") || tagContains(combined, u8"古筝")
            || tagContains(combined, "auto_pipa") || tagContains(combined, u8"琵琶"))
        {
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "PentatonicOpen");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_guqin") || tagContains(combined, u8"古琴")
            || tagContains(combined, "auto_xiao") || tagContains(combined, u8"箫"))
        {
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "PentatonicOpen");
            addIfMissing(context.textureTags, "PedalDrone");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_suona") || tagContains(combined, u8"唢呐"))
        {
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "MarchPulse");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_sheng") || tagContains(combined, u8"笙")
            || tagContains(combined, "auto_zhongruan") || tagContains(combined, u8"中阮"))
        {
            addIfMissing(context.textureTags, "ChineseFolk");
            addIfMissing(context.textureTags, "PentatonicOpen");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_yi") || tagContains(combined, "auto_tibetan") || tagContains(combined, "auto_mongol")
            || tagContains(combined, "auto_uyghur") || tagContains(combined, "auto_dong") || tagContains(combined, "auto_miao")
            || tagContains(combined, "auto_zhuang") || tagContains(combined, "auto_qiang") || tagContains(combined, "auto_hui")
            || tagContains(combined, "auto_kazakh") || tagContains(combined, "auto_hani") || tagContains(combined, "auto_naxi")
            || tagContains(combined, "auto_bai") || tagContains(combined, "auto_lisu") || tagContains(combined, "auto_lahu")
            || tagContains(combined, u8"族"))
        {
            addIfMissing(context.textureTags, "EthnicFolk");
            addIfMissing(context.regionTags, "chinese");
        }
        if (tagContains(combined, "auto_middle_eastern_maqam") || tagContains(combined, "auto_muqam")
            || tagContains(combined, "auto_arabic") || tagContains(combined, "auto_persia")
            || tagContains(combined, u8"马卡姆") || tagContains(combined, u8"木卡姆"))
            addIfMissing(context.textureTags, "MaqamOrnament");
        if (tagContains(combined, "auto_indian_raga") || tagContains(combined, "auto_indian")
            || tagContains(combined, u8"拉格") || tagContains(combined, u8"印度"))
            addIfMissing(context.textureTags, "RagaDrone");
        if (tagContains(combined, "gamelan") || tagContains(combined, u8"甘美兰")
            || tagContains(combined, "auto_indonesia") || tagContains(combined, "auto_bali"))
            addIfMissing(context.textureTags, "GamelanOstinato");
        if (tagContains(combined, "auto_pedal_tone") || tagContains(combined, "auto_pedal")
            || tagContains(combined, "auto_ostinato") || tagContains(combined, "drone")
            || tagContains(combined, u8"持续音") || tagContains(combined, u8"踏板") || tagContains(combined, u8"固定音型"))
            addIfMissing(context.textureTags, "PedalDrone");
        if (tagContains(combined, "auto_chromatic") || tagContains(combined, "auto_chromaticism")
            || tagContains(combined, "auto_tritone_substitution") || tagContains(combined, "auto_augmented_second")
            || tagContains(combined, u8"半音") || tagContains(combined, u8"三全音"))
            addIfMissing(context.textureTags, "ChromaticTension");
        if (tagContains(combined, "auto_diminished") || tagContains(combined, u8"减"))
            addIfMissing(context.textureTags, "DiminishedPassing");
        if (tagContains(combined, "auto_cadence") || tagContains(combined, u8"终止"))
            addIfMissing(context.textureTags, "CadentialBreath");
        if (tagContains(combined, "auto_march") || tagContains(combined, "auto_military")
            || tagContains(combined, u8"进行曲") || tagContains(combined, u8"军乐") || tagContains(combined, u8"解放军"))
        {
            addIfMissing(context.textureTags, "MarchPulse");
            addIfMissing(context.styleTags, "march");
            addIfMissing(context.moodTags, "epic");
            addIfMissing(context.moodTags, "energetic");
        }
        if (tagContains(combined, "auto_bossa") || tagContains(combined, "auto_salsa") || tagContains(combined, "auto_tango")
            || tagContains(combined, "auto_samba") || tagContains(combined, "auto_cumbia") || tagContains(combined, "auto_flamenco")
            || tagContains(combined, "montuno"))
            addIfMissing(context.textureTags, "LatinMontuno");
    }

    void addFunctionalHints(const juce::String& romanSequence, ArpeggioContext& context)
    {
        auto tokens = juce::StringArray::fromTokens(romanSequence, " -,\t\r\n", "\"");
        for (auto token : tokens)
        {
            token = token.trim();
            if (token.isEmpty())
                continue;

            while (token.startsWithChar('b') || token.startsWithChar('#'))
                token = token.substring(1);

            const auto upper = token.toUpperCase();
            if (upper.startsWith("VII") || upper.startsWith("V"))
                addIfMissing(context.functionTags, "D");
            else if (upper.startsWith("IV") || upper.startsWith("II"))
                addIfMissing(context.functionTags, token.startsWithChar('i') ? "s" : "S");
            else if (upper.startsWith("VI") || upper.startsWith("I"))
                addIfMissing(context.functionTags, token.startsWithChar('i') ? "t" : "T");
        }
    }

    void addProgressionTextureHints(const juce::String& romanSequence, ArpeggioContext& context)
    {
        auto tokens = juce::StringArray::fromTokens(romanSequence, " -,\t\r\n", "\"");
        tokens.removeEmptyStrings();
        if (tokens.isEmpty())
            return;

        int secondarySlashCount = 0;
        int flatTwoCount = 0;
        int flatSevenCount = 0;
        int diminishedCount = 0;
        int chromaticAlterationCount = 0;
        int fifthMotionCount = 0;

        juce::String previousRoot;
        for (auto token : tokens)
        {
            token = token.trim();
            if (token.isEmpty())
                continue;

            if (token.containsChar('/'))
                ++secondarySlashCount;
            if (token.containsIgnoreCase("dim") || token.containsIgnoreCase("ø") || token.containsIgnoreCase("o7"))
                ++diminishedCount;
            if (token.startsWithChar('b') || token.startsWithChar('#'))
                ++chromaticAlterationCount;

            auto root = token;
            if (root.containsChar('/'))
                root = root.fromFirstOccurrenceOf("/", false, false);
            while (root.startsWithChar('b') || root.startsWithChar('#'))
                root = root.substring(1);
            root = root.retainCharacters("ivIV");
            const auto upperRoot = root.toUpperCase();

            if (token.startsWith("bII") || token.startsWith("bii"))
                ++flatTwoCount;
            if (token.startsWith("bVII") || token.startsWith("bvii"))
                ++flatSevenCount;

            if (previousRoot.isNotEmpty()
                && ((previousRoot == "I" && upperRoot == "V") || (previousRoot == "V" && upperRoot == "I")
                    || (previousRoot == "II" && upperRoot == "V") || (previousRoot == "V" && upperRoot == "I")
                    || (previousRoot == "VI" && upperRoot == "II") || (previousRoot == "III" && upperRoot == "VI")))
            {
                ++fifthMotionCount;
            }

            if (upperRoot.isNotEmpty())
                previousRoot = upperRoot;
        }

        if (tokens.size() >= 2)
        {
            int sameNeighbourCount = 0;
            for (int i = 1; i < tokens.size(); ++i)
                if (tokens[i].equalsIgnoreCase(tokens[i - 1]))
                    ++sameNeighbourCount;

            if (sameNeighbourCount >= tokens.size() / 2 || tokens.size() <= 2)
                addIfMissing(context.textureTags, "SingleChordPulse");
        }

        if (secondarySlashCount >= 2)
            addIfMissing(context.textureTags, "PedalDrone");
        if (flatTwoCount > 0)
            addIfMissing(context.textureTags, "MaqamOrnament");
        if (flatSevenCount >= 2)
            addIfMissing(context.textureTags, "PentatonicOpen");
        if (diminishedCount > 0)
            addIfMissing(context.textureTags, "DiminishedPassing");
        if (chromaticAlterationCount >= 2)
            addIfMissing(context.textureTags, "ChromaticTension");
        if (fifthMotionCount >= 2)
            addIfMissing(context.textureTags, "CircleMotion");
    }

    ArpeggioContext makeArpeggioContextForEntry(const ProgressionEntry& entry,
                                                PitchClass tonic,
                                                ScaleType scaleType,
                                                double bpm)
    {
        ArpeggioContext context;
        context.presetName = entry.name;
        context.presetDescription = entry.description;
        context.progressionName = entry.name;
        context.romanSequence = entry.romanSequence;
        context.tonic = tonic;
        context.scaleType = scaleType;
        context.bpm = bpm;
        context.beatsPerBar = 4;
        context.beatsPerChord = 4.0;

        for (const auto& tag : entry.tags)
            classifyArpeggioTag(tag, context);

        classifyArpeggioTag(entry.sourceFileName, context);
        classifyArpeggioTag(entry.name, context);
        classifyArpeggioTag(entry.description, context);
        classifyArpeggioTag(entry.scaleHint, context);

        addFunctionalHints(entry.romanSequence, context);
        addProgressionTextureHints(entry.romanSequence, context);
        return context;
    }
}

//==============================================================================
// 构造函数
//==============================================================================
ProgressionBrowser::ProgressionBrowser(ChordReferenceAudioProcessor& proc,
                                       ProgressionData& data,
                                       std::function<PitchClass()> getTonic,
                                       std::function<ScaleType()> getScaleType)
    : processor(proc)
    , progressionData(data)
    , getCurrentTonic(getTonic)
    , getCurrentScaleType(getScaleType)
{
    setLookAndFeel(&modernLAF);
    setWantsKeyboardFocus(true);

    // ---------- 工具栏按钮 ----------
    clearTagsButton.setButtonText(LocalisedStrings::translate("preset_clear_tags"));
    clearTagsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    clearTagsButton.onClick = [this] { deselectAllTags(); };
    addAndMakeVisible(clearTagsButton);

    // ---- 批量操作按钮（初始隐藏） ----
    batchDeleteButton.setButtonText(LocalisedStrings::translate("preset_batch_delete"));
    batchDeleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    batchDeleteButton.onClick = [this] { batchDeleteSelected(); };
    batchDeleteButton.setVisible(false);
    addAndMakeVisible(batchDeleteButton);  // 注意：这里用 addAndMakeVisible 以便后续控制显示

    batchExportButton.setButtonText(LocalisedStrings::translate("preset_batch_export"));
    batchExportButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    batchExportButton.onClick = [this] { batchExportSelected(); };
    batchExportButton.setVisible(false);
    addAndMakeVisible(batchExportButton);

    addPresetButton.setButtonText(LocalisedStrings::translate("preset_add_custom"));
    addPresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addPresetButton.onClick = [this] { showPresetEditor(); };
    addAndMakeVisible(addPresetButton);

    importButton.setButtonText(LocalisedStrings::translate("preset_import"));
    importButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    importButton.onClick = [this] { showImportMenu(); };
    addAndMakeVisible(importButton);

    exportButton.setButtonText(LocalisedStrings::translate("preset_export"));
    exportButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    exportButton.onClick = [this] { showExportMenu(); };
    addAndMakeVisible(exportButton);

    closeButton.setButtonText(juce::String::fromUTF8(u8"✕"));
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    closeButton.onClick = [this] { if (onClose) onClose(); };
    addAndMakeVisible(closeButton);

    // 搜索框
    addAndMakeVisible(searchBox);
    searchBox.setMultiLine(false);
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    searchBox.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack); // 焦点轮廓透明
    searchBox.setName("SearchBox");
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colour(0xFF513DEB));
    searchBox.setFont(juce::Font(16.0f));
    searchBox.setHasFocusOutline(false);  // 明确禁用焦点绘制
    searchBox.setBorder(juce::BorderSize<int>(0)); // 无内边距线
    searchBox.setTextToShowWhenEmpty(LocalisedStrings::translate("preset_search_placeholder"), juce::Colour(0xFF513DEB));
    searchBox.onTextChange = [this] {
        currentSearchText = searchBox.getText();
        updateFilteredList();
    };

    // ---------- 底部播放栏 ----------
    bottomInfoLabel.setText(LocalisedStrings::translate("preset_ready"), juce::dontSendNotification);
    bottomInfoLabel.setFont(14.0f);
    addAndMakeVisible(bottomInfoLabel);

    playPauseButton.setButtonText(juce::String::fromUTF8(u8"▶"));
    addAndMakeVisible(playPauseButton);

    prevPresetButton.setButtonText(juce::String::fromUTF8(u8"<<"));
    addAndMakeVisible(prevPresetButton);

    nextPresetButton.setButtonText(juce::String::fromUTF8(u8">>"));
    addAndMakeVisible(nextPresetButton);

    initialiseArpeggioControls();

    // ---------- 中间区域标题 ----------
    categoryTitleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    addAndMakeVisible(categoryTitleLabel);

    matchCountLabel.setFont(13.0f);
    addAndMakeVisible(matchCountLabel);

    sortComboBox.addItem(LocalisedStrings::translate("sort_default"), 1);
    sortComboBox.addItem(LocalisedStrings::translate("sort_name"), 2);
    sortComboBox.addItem(LocalisedStrings::translate("sort_recent"), 3);
    sortComboBox.setSelectedId(1);
    sortComboBox.onChange = [this] { updateFilteredList(); };
    addAndMakeVisible(sortComboBox);

    // ---------- 左侧类别列表 ----------
    categoryList.setModel(&categoryModel);
    categoryList.setRowHeight(28);
    categoryList.selectRow(0);
    addAndMakeVisible(categoryList);

    // ---------- 标签区域 ----------
    addAndMakeVisible(tagViewport);
    tagViewport.setViewedComponent(&tagContainer, false);

    // ---------- 中央卡片列表 ----------
    addAndMakeVisible(cardViewport);
    cardViewport.setViewedComponent(&cardContainer, false);

    // ---------- 右侧详情面板 ----------
    addAndMakeVisible(inspector);
    inspector.dragToTrackButton.addMouseListener(this, false);
    inspector.dragArpeggioToTrackButton.addMouseListener(this, false);

    // ---------- 当前调性标签 ----------
    PitchClass initTonic = getCurrentTonic();
    ScaleType initType = getCurrentScaleType();
    keyLabel.setText(LocalisedStrings::translate("preset_key_prefix") +
                     PitchNameHelper::getNameForPitch(initTonic, true) + " " +
                     getScaleTypeName(initType), juce::dontSendNotification);
    keyLabel.setFont(16.0f);
    keyLabel.setVisible(true);
    addChildComponent(keyLabel);

    // ---------- 播放控制连接 ----------
    playPauseButton.onClick = [this] {
        if (currentAuditionStep >= 0) stopAudition();
        else startAudition();
    };

    prevPresetButton.onClick = [this] {
        if (cycleSmartArpeggioRecommendation(-1))
            return;

        if (cards.size() == 0) return;
        int idx = 0;
        for (int i = 0; i < cards.size(); ++i)
            if (cards[i]->getSelected()) { idx = i; break; }
        idx = (idx - 1 + cards.size()) % cards.size();
        cards[idx]->onClick();
    };

    nextPresetButton.onClick = [this] {
        if (cycleSmartArpeggioRecommendation(1))
            return;

        if (cards.size() == 0) return;
        int idx = 0;
        for (int i = 0; i < cards.size(); ++i)
            if (cards[i]->getSelected()) { idx = i; break; }
        idx = (idx + 1) % cards.size();
        cards[idx]->onClick();
    };

    buildTagPanel();
    updateFilteredList();
}

ProgressionBrowser::~ProgressionBrowser()
{
    stopAudition();
    stopTimer();

    onClose = nullptr;
    importChooser.reset();
    exportChooser.reset();

    inspector.dragToTrackButton.removeMouseListener(this);
    inspector.dragArpeggioToTrackButton.removeMouseListener(this);
    setLookAndFeel(nullptr);
}

bool ProgressionBrowser::keyPressed(const juce::KeyPress& key)
{
    return handleShortcut(key);
}

bool ProgressionBrowser::handleShortcut(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        if (onClose) onClose();
        return true;
    }

    if (key == juce::KeyPress::spaceKey)
    {
        if (currentAuditionStep >= 0) stopAudition();
        else startAudition();
        return true;
    }

    if (key == juce::KeyPress::leftKey)
    {
        prevPresetButton.triggerClick();
        return true;
    }

    if (key == juce::KeyPress::rightKey)
    {
        nextPresetButton.triggerClick();
        return true;
    }

    return false;
}

//==============================================================================
// 标签面板构建
//==============================================================================
void ProgressionBrowser::buildTagPanel()
{
    rebuildTagCountCache();

    // 六类标签定义
    juce::Array<juce::StringArray> allCategoryTags = TagDefinitions::getAllCategoryTags();

    if (selectedCategoryIndex < 0)
    {
        juce::StringArray allTags;
        for (auto& catTags : allCategoryTags)
            allTags.addArray(catTags);
        allTags.removeDuplicates(true);
        currentCategoryTags = allTags;
    }
    else
    {
        currentCategoryTags = allCategoryTags[selectedCategoryIndex];
    }

    tagContainer.removeAllChildren();
    tagButtons.clear();

    int btnHeight = 22;
    int btnWidth = 130;
    int cols = 5;
    int gap = 4;
    int y = 0;
    for (int i = 0; i < currentCategoryTags.size(); ++i)
    {
        auto tag = currentCategoryTags[i];
        int count = countTagMatches(tag);
        juce::String displayText = tag + " (" + juce::String(count) + ")";

        auto* btn = new juce::ToggleButton(displayText);
        bool isActive = activeTags.contains(tag);
        btn->setToggleState(isActive, juce::dontSendNotification);
        btn->onClick = [this, tag] {
            if (activeTags.contains(tag)) activeTags.removeString(tag);
            else activeTags.add(tag);
            updateFilteredList();
            buildTagPanel();
            categoryList.updateContent();
        };

        int col = i % cols;
        int row = i / cols;
        int x = col * (btnWidth + gap);
        y = row * (btnHeight + gap);

        btn->setBounds(x, y, btnWidth, btnHeight);
        tagContainer.addAndMakeVisible(btn);
        tagButtons.add(btn);
    }
    tagContainer.setSize(cols * (btnWidth + gap), y + btnHeight + gap);
    tagViewport.setViewPosition(0, 0);
    applySkinColours();
    resized();

    categoryTagCounts.clear();
    for (auto& catTags : allCategoryTags)
    {
        int catTotal = 0;
        for (auto& tag : catTags)
            catTotal += countTagMatches(tag);
        categoryTagCounts.add(catTotal);
    }
    categoryList.updateContent();
}

//==============================================================================
// 布局
//==============================================================================
void ProgressionBrowser::resized()
{
    const int outerMargin = 20;
    const int columnGap = 16;
    const int internalPadding = 12;

    auto area = getLocalBounds().reduced(outerMargin);

    // 顶部工具栏
    auto topBar = area.removeFromTop(32);
    topBarRect = topBar;
    int buttonHeight = 25;
    int buttonY = topBar.getY() + (topBar.getHeight() - buttonHeight) / 2;
    const int leftStartX = topBar.getX() + 20;
    importButton.setBounds(leftStartX, buttonY, 90, buttonHeight);
    exportButton.setBounds(leftStartX + 95, buttonY, 90, buttonHeight);
    addPresetButton.setBounds(leftStartX + 190, buttonY, 90, buttonHeight);
    clearTagsButton.setBounds(leftStartX + 285, buttonY, 80, buttonHeight);
    // 批量操作按钮（显示在清除标签按钮右侧）
    batchDeleteButton.setBounds(clearTagsButton.getRight() + 10, buttonY, 100, buttonHeight);
    batchExportButton.setBounds(batchDeleteButton.getRight() + 5, buttonY, 100, buttonHeight);
    closeButton.setBounds(topBar.getRight() - 32, buttonY, 24, 24);
    // 搜索框右侧对齐详情面板宽度，紧贴关闭按钮左侧
    const int searchWidth = 250;
    const int searchRightX = closeButton.getX() - 10;
    searchBox.setBounds(searchRightX - searchWidth, buttonY, searchWidth, buttonHeight);
    area.removeFromTop(10);

    // 底部播放栏
    auto bottomBar = area.removeFromBottom(56);
    bottomBarRect = bottomBar;
    {
        auto bottomLayout = bottomBar;
        auto leftInfoArea = bottomLayout.removeFromLeft(270);
        keyLabel.setBounds(leftInfoArea.removeFromLeft(150).reduced(0, 4));
        bottomInfoLabel.setBounds(leftInfoArea.withTrimmedLeft(2));

        int controlY = bottomBar.getY() + (bottomBar.getHeight() - 28) / 2;
        arpeggioDensityComboBox.setBounds(bottomLayout.removeFromLeft(126).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(22);

        int capsuleHeight = 36;
        int capsuleY = bottomBar.getY() + (bottomBar.getHeight() - capsuleHeight) / 2;
        int btnSize = 28;
        int gap = 8;
        int btnY = capsuleY + (capsuleHeight - btnSize) / 2;
        prevPresetButton.setBounds(bottomLayout.removeFromLeft(btnSize).withY(btnY).withHeight(btnSize));
        bottomLayout.removeFromLeft(gap);
        playPauseButton.setBounds(bottomLayout.removeFromLeft(btnSize).withY(btnY).withHeight(btnSize));
        bottomLayout.removeFromLeft(gap);
        nextPresetButton.setBounds(bottomLayout.removeFromLeft(btnSize).withY(btnY).withHeight(btnSize));
        bottomLayout.removeFromLeft(16);

        arpeggioSmartButton.setBounds(bottomLayout.removeFromLeft(70).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(8);
        arpeggioPatternButton.setBounds(bottomLayout.removeFromLeft(160).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(8);
        arpeggioColourLayerButton.setBounds(bottomLayout.removeFromLeft(46).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(5);
        arpeggioLeftRootButton.setBounds(bottomLayout.removeFromLeft(46).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(5);
        arpeggioBassDoubleButton.setBounds(bottomLayout.removeFromLeft(46).withY(controlY).withHeight(28));
        bottomLayout.removeFromLeft(8);
        arpeggioHumanizeLabel.setBounds(bottomLayout.removeFromLeft(54).withY(controlY).withHeight(28));
        arpeggioHumanizeSlider.setBounds(bottomLayout.removeFromLeft(96).withY(controlY).withHeight(28));
    }
    area.removeFromBottom(12);

    // 三栏布局
    auto leftBar = area.removeFromLeft(200);
    leftBarRect = leftBar;
    categoryList.setBounds(leftBar.reduced(internalPadding));

    area.removeFromLeft(columnGap);
    auto rightBar = area.removeFromRight(300);
    inspector.setBounds(rightBar);
    area.removeFromRight(columnGap);

    // 中央区域
    auto centerArea = area;
    auto contentArea = centerArea.reduced(internalPadding, 0);

    categoryTitleLabel.setBounds(contentArea.removeFromTop(26));
    contentArea.removeFromTop(4);
    tagViewport.setBounds(contentArea.removeFromTop(100));
    contentArea.removeFromTop(6);

    auto infoBar = contentArea.removeFromTop(28);
    matchCountLabel.setBounds(infoBar.removeFromLeft(170));
    sortComboBox.setBounds(infoBar.removeFromRight(120));

    cardViewport.setBounds(contentArea);

    int y = 0;
    const int cardHeight = 50;
    const int cardGap = 4;
    const int listWidth = cardViewport.getWidth() - 8;
    for (auto* card : cards)
    {
        card->setBounds(4, y, listWidth, cardHeight);
        y += (cardHeight + cardGap);
    }
    cardContainer.setSize(cardViewport.getWidth(), y);
}

//==============================================================================
// 过滤列表刷新
//==============================================================================
void ProgressionBrowser::updateFilteredList()
{
    juce::Array<ProgressionEntry> all;

    switch (currentFilterMode)
    {
        case All:
            all = progressionData.getAllEntries();
            if (!activeTags.isEmpty())
            {
                juce::Array<ProgressionEntry> filtered;
                for (auto& entry : all)
                {
                    bool matchesAll = true;
                    for (auto& tag : activeTags)
                    {
                        if (!entryHasDisplayTag(entry, tag))
                        {
                            matchesAll = false;
                            break;
                        }
                    }
                    if (matchesAll)
                        filtered.add(entry);
                }
                all = filtered;
            }
            break;
        case Favorites:
            for (auto& entry : progressionData.getAllEntries())
                if (progressionData.isFavorite(entry.name))
                    all.add(entry);
            break;
        case UserPresets:
            for (auto& entry : progressionData.getAllEntries())
                if (entry.isUserPreset)
                    all.add(entry);
            break;
        case UserFolder:
            all = progressionData.getUserEntriesInFolder(selectedUserFolderId);
            break;
        case Category:
            all = progressionData.getAllEntries();
            if (!activeTags.isEmpty())
            {
                juce::Array<ProgressionEntry> filtered;
                for (auto& entry : all)
                {
                    bool matchesAll = true;
                    for (auto& tag : activeTags)
                    {
                        if (!entryHasDisplayTag(entry, tag))
                        {
                            matchesAll = false;
                            break;
                        }
                    }
                    if (matchesAll)
                        filtered.add(entry);
                }
                all = filtered;
            }
            else
            {
                juce::Array<ProgressionEntry> filtered;
                for (auto& entry : all)
                {
                    bool hasTag = false;
                    for (auto& tag : entry.tags)
                        if (currentCategoryTags.contains(tag) || currentCategoryTags.contains(TagLocalizer::translateTag(tag))) { hasTag = true; break; }
                    if (hasTag) filtered.add(entry);
                }
                all = filtered;
            }
            break;
        case FrequentUsed:   // 常用过滤
            for (auto& entry : progressionData.getAllEntries())
                if (progressionData.isFrequent(entry.name))
                    all.add(entry);
            break;
    }

    applySorting(all);
    applySearchFilter(all);
    matchCountLabel.setText(LocalisedStrings::translate("preset_match_prefix") + juce::String(all.size()) + LocalisedStrings::translate("preset_match_suffix"),
                            juce::dontSendNotification);

    cardContainer.removeAllChildren();
    cards.clear();
    PitchClass tonic = getCurrentTonic();
    ScaleType type = getCurrentScaleType();

    for (auto& entry : all)
    {
        auto* card = new ProgressionCard();
        card->setEntry(entry, tonic, type);
        card->setFavourite(progressionData.isFavorite(entry.name));
        card->setFrequent(progressionData.isFrequent(entry.name));  // 设置常用状态标记

        // ---- 点击回调（支持 Shift 多选） ----
        card->onClick = [this, card, entry]() {
            auto mods = juce::ModifierKeys::getCurrentModifiers();

            if (mods.isShiftDown())
            {
                // Shift + 点击：切换该卡片的多选状态（不影响当前单选详情）
                card->setMultiSelected(!card->isMultiSelected);
            }
            else
            {
                // 普通点击：清除所有卡片的多选状态，设置为单选当前卡片
                for (auto* c : cards)
                {
                    c->setMultiSelected(false);
                    c->setSelected(false);
                }
                card->setMultiSelected(true);
                card->setSelected(true);
                inspector.setEntry(entry, getCurrentTonic(), getCurrentScaleType());
                arpeggioState.smartRecommendationIndex = 0;
                updateArpeggioStatusText();
            }
            updateBatchButtonVisibility();
        };

        // ---- 收藏切换回调 ----
        card->onFavouriteToggle = [this, entry]() {
            bool fav = progressionData.isFavorite(entry.name);
            progressionData.setFavorite(entry.name, !fav);
            updateFilteredList();
        };

        // ---- 常用切换回调 ----
        card->onFrequentToggle = [this, entry]() {
            bool freq = progressionData.isFrequent(entry.name);
            progressionData.setFrequent(entry.name, !freq);
            updateFilteredList();
        };

        // ---- 编辑与删除回调（用户预设） ----
        card->onEdit = [this, entry]() { showPresetEditor(entry); };
        card->onDelete = [this, entry]() {
            progressionData.removeUserPreset(entry.name);
            progressionData.reload();
            invalidatePresetBrowserCaches();
            updateFilteredList();
        };
        card->onBuildExtraMenu = [this, entry](juce::PopupMenu& menu) {
            addMoveToFolderMenu(menu, entry);
        };

        cardContainer.addAndMakeVisible(card);
        cards.add(card);
    }

    updateBatchButtonVisibility();  // 批量按钮可见性
    categoryList.updateContent();
    resized();
}

//==============================================================================
// 辅助：标签匹配数、排序、皮肤应用等
//==============================================================================
int ProgressionBrowser::countTagMatches(const juce::String& tag) const
{
    const auto it = tagCountCache.find(tag.toStdString());
    return it != tagCountCache.end() ? it->second : 0;
}

bool ProgressionBrowser::entryHasDisplayTag(const ProgressionEntry& entry, const juce::String& displayTag) const
{
    for (const auto& tag : entry.tags)
        if (tag == displayTag || TagLocalizer::translateTag(tag) == displayTag)
            return true;
    return false;
}

void ProgressionBrowser::rebuildTagCountCache()
{
    const int currentLanguage = LocalisedStrings::getCurrentLanguage();
    const auto entries = progressionData.getAllEntries();

    if (tagCountCacheLanguage == currentLanguage
        && tagCountCacheEntryCount == entries.size()
        && !tagCountCache.empty())
        return;

    tagCountCache.clear();
    tagCountCacheLanguage = currentLanguage;
    tagCountCacheEntryCount = entries.size();

    for (const auto& entry : entries)
    {
        juce::StringArray countedTags;

        for (const auto& tag : entry.tags)
        {
            countedTags.addIfNotAlreadyThere(tag);
            countedTags.addIfNotAlreadyThere(TagLocalizer::translateTag(tag));
        }

        for (const auto& tag : countedTags)
            ++tagCountCache[tag.toStdString()];
    }
}

void ProgressionBrowser::invalidatePresetBrowserCaches()
{
    tagCountCache.clear();
    tagCountCacheLanguage = -1;
    tagCountCacheEntryCount = -1;
}

void ProgressionBrowser::applySorting(juce::Array<ProgressionEntry>& entries)
{
    int sortId = sortComboBox.getSelectedId();
    if (sortId == 2)
    {
        std::sort(entries.begin(), entries.end(),
            [](const ProgressionEntry& a, const ProgressionEntry& b) {
                return a.name.compareIgnoreCase(b.name) < 0;
            });
    }
    else if (sortId == 3)
    {
        std::sort(entries.begin(), entries.end(),
            [](const ProgressionEntry& a, const ProgressionEntry& b) {
                if (a.isUserPreset != b.isUserPreset)
                    return a.isUserPreset > b.isUserPreset;
                return false;
            });
    }
}



void ProgressionBrowser::deselectAllTags() { activeTags.clear(); updateFilteredList(); buildTagPanel(); }

//搜索实现
void ProgressionBrowser::applySearchFilter(juce::Array<ProgressionEntry>& entries)
{
    if (currentSearchText.isEmpty()) return;
    
    juce::Array<ProgressionEntry> filtered;
    juce::String lowerSearch = currentSearchText.toLowerCase();
    
    for (auto& entry : entries)
    {
        bool matched = entry.name.toLowerCase().contains(lowerSearch)
            || entry.description.toLowerCase().contains(lowerSearch);
        if (!matched)
        {
            for (auto& tag : entry.tags)
                if (tag.toLowerCase().contains(lowerSearch))
                {
                    matched = true;
                    break;
                }
        }
        if (matched)
            filtered.add(entry);
    }
    entries = filtered;
}

//批量删除实现
void ProgressionBrowser::updateBatchButtonVisibility()
{
    int count = 0;
    bool onlyUserPresets = true;

    for (auto* c : cards)
    {
        if (c->isMultiSelected)
        {
            count++;
            if (!c->getEntry().isUserPreset)
                onlyUserPresets = false;
        }
    }

    // 仅当多选数量 >=2 且所有选中卡都是用户预设时，显示批量按钮
    bool shouldShow = (count >= 2 && onlyUserPresets);
    batchDeleteButton.setVisible(shouldShow);
    batchExportButton.setVisible(shouldShow);
}
//批量删除导出过滤厂家预设
void ProgressionBrowser::batchExportSelected()
{
    juce::Array<ProgressionEntry> toExport;
    for (auto* c : cards)
        if (c->isMultiSelected && c->getEntry().name.isNotEmpty() && c->getEntry().isUserPreset)
            toExport.add(c->getEntry());

    if (toExport.isEmpty()) return;
    exportEntries(toExport, true);
}

void ProgressionBrowser::applySkinColours()
{
    keyLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    bottomInfoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    arpeggioHumanizeLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.82f));

    sortComboBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    sortComboBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha(0.3f));
    sortComboBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    sortComboBox.setColour(juce::ComboBox::arrowColourId, juce::Colours::white);

    auto styleArpCombo = [](juce::ComboBox& combo) {
        combo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff100A20));
        combo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff7D5CFF).withAlpha(0.75f));
        combo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        combo.setColour(juce::ComboBox::arrowColourId, juce::Colours::white.withAlpha(0.92f));
    };
    styleArpCombo(arpeggioDensityComboBox);
    arpeggioPatternButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff100A20));
    arpeggioPatternButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    arpeggioPatternButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff6D48FF));

    auto styleArpToggle = [](juce::TextButton& button) {
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff100A20));
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff6D48FF));
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.82f));
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    };
    styleArpToggle(arpeggioColourLayerButton);
    styleArpToggle(arpeggioLeftRootButton);
    styleArpToggle(arpeggioBassDoubleButton);

    arpeggioSmartButton.setColour(juce::TextButton::buttonColourId,
                                  arpeggioState.smartMode ? juce::Colour(0xff6D48FF)
                                                          : juce::Colour(0x0019162D));
    arpeggioSmartButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff6D48FF));
    arpeggioSmartButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    arpeggioSmartButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

    arpeggioHumanizeSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff7D5CFF));
    arpeggioHumanizeSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffD9CEFF));
    arpeggioHumanizeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    arpeggioHumanizeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff100A20));
    arpeggioHumanizeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff7D5CFF).withAlpha(0.6f));

    for (auto* btn : tagButtons)
    {
        btn->setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        btn->setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
    }
    categoryTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    matchCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    searchBox.setColour(juce::TextEditor::textColourId, juce::Colour(0xFF513DEB));
    searchBox.applyColourToAllText(juce::Colour(0xFF513DEB));
}

void ProgressionBrowser::initialiseArpeggioControls()
{
    arpeggioPatterns = ArpeggioPatternLibrary::getBuiltInPatterns();
    arpeggioState.smartMode = true;
    arpeggioState.enabled = true;
    arpeggioState.humanizeEnabled = true;
    arpeggioState.humanizeAmount = 50;
    arpeggioState.density = ArpeggioDensity::Eighth;
    arpeggioState.includeUpperColourLayer = false;
    arpeggioState.includeLeftRootBreakdown = false;
    arpeggioState.includeBassDoubling = false;
    arpeggioState.selectedPatternId = arpeggioPatterns.empty() ? juce::String{} : arpeggioPatterns.front().id;

    for (size_t i = 0; i < arpeggioPatterns.size(); ++i)
        if (arpeggioPatterns[i].id == "up_8")
            arpeggioState.selectedPatternId = arpeggioPatterns[i].id;

    arpeggioSmartButton.setButtonText(juce::String::fromUTF8(u8"智能"));
    arpeggioSmartButton.setClickingTogglesState(true);
    arpeggioSmartButton.onClick = [this] {
        arpeggioState.smartMode = arpeggioSmartButton.getToggleState();
        arpeggioState.smartRecommendationIndex = 0;
        updateArpeggioControlsFromState();
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioSmartButton);

    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Whole), 1);
    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Half), 2);
    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Quarter), 3);
    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Eighth), 4);
    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Sixteenth), 5);
    arpeggioDensityComboBox.addItem(getDensityLabel(ArpeggioDensity::Mixed), 6);
    arpeggioDensityComboBox.onChange = [this] {
        switch (arpeggioDensityComboBox.getSelectedId())
        {
            case 1: arpeggioState.density = ArpeggioDensity::Whole; break;
            case 2: arpeggioState.density = ArpeggioDensity::Half; break;
            case 3: arpeggioState.density = ArpeggioDensity::Quarter; break;
            case 5: arpeggioState.density = ArpeggioDensity::Sixteenth; break;
            case 6: arpeggioState.density = ArpeggioDensity::Mixed; break;
            case 4:
            default: arpeggioState.density = ArpeggioDensity::Eighth; break;
        }
        arpeggioState.smartRecommendationIndex = 0;
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioDensityComboBox);

    arpeggioPatternButton.setButtonText(juce::String::fromUTF8(u8"选择范式"));
    arpeggioPatternButton.setTooltip(juce::String::fromUTF8(u8"按类别选择分解范式：")
                                     + juce::String(arpeggioPatterns.size())
                                     + juce::String::fromUTF8(u8" 个，其中新增 ")
                                     + juce::String(juce::jmax(0, static_cast<int>(arpeggioPatterns.size()) - userAddedPatternStartIndex))
                                     + juce::String::fromUTF8(u8" 个"));

    arpeggioColourLayerButton.setButtonText(juce::String::fromUTF8(u8"高彩"));
    arpeggioColourLayerButton.setTooltip(juce::String::fromUTF8(u8"额外添加右手高区色彩音"));
    arpeggioColourLayerButton.setClickingTogglesState(true);
    arpeggioColourLayerButton.onClick = [this] {
        arpeggioState.includeUpperColourLayer = arpeggioColourLayerButton.getToggleState();
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioColourLayerButton);

    arpeggioLeftRootButton.setButtonText(juce::String::fromUTF8(u8"左根"));
    arpeggioLeftRootButton.setTooltip(juce::String::fromUTF8(u8"额外添加左手根音/五度分解"));
    arpeggioLeftRootButton.setClickingTogglesState(true);
    arpeggioLeftRootButton.onClick = [this] {
        arpeggioState.includeLeftRootBreakdown = arpeggioLeftRootButton.getToggleState();
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioLeftRootButton);

    arpeggioBassDoubleButton.setButtonText(juce::String::fromUTF8(u8"倍低"));
    arpeggioBassDoubleButton.setTooltip(juce::String::fromUTF8(u8"额外添加低八度长根音"));
    arpeggioBassDoubleButton.setClickingTogglesState(true);
    arpeggioBassDoubleButton.onClick = [this] {
        arpeggioState.includeBassDoubling = arpeggioBassDoubleButton.getToggleState();
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioBassDoubleButton);

    arpeggioPatternButton.onClick = [this] { showArpeggioPatternMenu(); };
    addAndMakeVisible(arpeggioPatternButton);

    arpeggioHumanizeLabel.setText(juce::String::fromUTF8(u8"人性化"), juce::dontSendNotification);
    arpeggioHumanizeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(arpeggioHumanizeLabel);

    arpeggioHumanizeSlider.setRange(0.0, 100.0, 1.0);
    arpeggioHumanizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    arpeggioHumanizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 38, 20);
    arpeggioHumanizeSlider.onValueChange = [this] {
        arpeggioState.humanizeAmount = static_cast<int>(arpeggioHumanizeSlider.getValue());
        arpeggioState.humanizeEnabled = arpeggioState.humanizeAmount > 0;
        updateArpeggioStatusText();
    };
    addAndMakeVisible(arpeggioHumanizeSlider);

    updateArpeggioControlsFromState();
}

void ProgressionBrowser::updateArpeggioControlsFromState()
{
    arpeggioSmartButton.setToggleState(arpeggioState.smartMode, juce::dontSendNotification);

    int densityId = 4;
    switch (arpeggioState.density)
    {
        case ArpeggioDensity::Whole: densityId = 1; break;
        case ArpeggioDensity::Half: densityId = 2; break;
        case ArpeggioDensity::Quarter: densityId = 3; break;
        case ArpeggioDensity::Eighth: densityId = 4; break;
        case ArpeggioDensity::Sixteenth: densityId = 5; break;
        case ArpeggioDensity::Mixed: densityId = 6; break;
    }
    arpeggioDensityComboBox.setSelectedId(densityId, juce::dontSendNotification);
    arpeggioColourLayerButton.setToggleState(arpeggioState.includeUpperColourLayer, juce::dontSendNotification);
    arpeggioLeftRootButton.setToggleState(arpeggioState.includeLeftRootBreakdown, juce::dontSendNotification);
    arpeggioBassDoubleButton.setToggleState(arpeggioState.includeBassDoubling, juce::dontSendNotification);



    for (int i = 0; i < static_cast<int>(arpeggioPatterns.size()); ++i)
    {
        if (arpeggioPatterns[static_cast<size_t>(i)].id == arpeggioState.selectedPatternId)
        {
            arpeggioState.selectedPatternIndex = i;
            arpeggioPatternButton.setButtonText(arpeggioPatterns[static_cast<size_t>(i)].nameCN);
            break;
        }
    }

    arpeggioHumanizeSlider.setValue(arpeggioState.humanizeAmount, juce::dontSendNotification);
    applySkinColours();
}

ArpeggioGenerationSettings ProgressionBrowser::createArpeggioSettingsFromState() const
{
    ArpeggioGenerationSettings settings;
    settings.smartMode = arpeggioState.smartMode;
    settings.preferredPatternId = arpeggioState.selectedPatternId;
    settings.manualDirection = arpeggioState.direction;
    settings.density = arpeggioState.density;
    settings.humanize.enabled = arpeggioState.humanizeEnabled && arpeggioState.humanizeAmount > 0;
    settings.humanize.amount = arpeggioState.humanizeAmount;
    settings.humanize.velocityRange = juce::roundToInt(2.0 + arpeggioState.humanizeAmount * 0.16);
    settings.humanize.timingRangeMs = 2.0 + arpeggioState.humanizeAmount * 0.20;
    settings.includePedal = true;
    settings.includeUpperColourLayer = arpeggioState.includeUpperColourLayer;
    settings.includeLeftRootBreakdown = arpeggioState.includeLeftRootBreakdown;
    settings.includeBassDoubling = arpeggioState.includeBassDoubling;
    settings.includeBassAnchor = settings.includeLeftRootBreakdown || settings.includeBassDoubling;
    return settings;
}

void ProgressionBrowser::selectArpeggioPatternIndex(int index)
{
    if (index < 0 || index >= static_cast<int>(arpeggioPatterns.size()))
        return;

    arpeggioState.selectedPatternIndex = index;
    arpeggioState.selectedPatternId = arpeggioPatterns[static_cast<size_t>(index)].id;
    arpeggioState.direction = arpeggioPatterns[static_cast<size_t>(index)].direction;
    arpeggioState.smartMode = false;
    arpeggioState.smartRecommendationIndex = 0;

    updateArpeggioControlsFromState();
    updateArpeggioStatusText();
}

void ProgressionBrowser::showArpeggioPatternMenu()
{
    struct CategoryMenu
    {
        juce::String name;
        juce::PopupMenu menu;
    };

    std::vector<CategoryMenu> categories = {
        { juce::String::fromUTF8(u8"段落/技法/情绪/织体"), {} },
        { juce::String::fromUTF8(u8"华语/流行钢琴"), {} },
        { juce::String::fromUTF8(u8"J-Pop/K-Pop/电子"), {} },
        { juce::String::fromUTF8(u8"民谣/吉他指弹"), {} },
        { juce::String::fromUTF8(u8"国风/民族器乐"), {} },
        { juce::String::fromUTF8(u8"影视/军乐/弦乐"), {} },
        { juce::String::fromUTF8(u8"爵士/R&B/拉丁"), {} },
        { juce::String::fromUTF8(u8"调式/世界/张力/古典"), {} },
        { juce::String::fromUTF8(u8"氛围/铺底/终止"), {} },
        { juce::String::fromUTF8(u8"基础/通用"), {} }
    };

    auto addToCategory = [&](int index) {
        const auto& pattern = arpeggioPatterns[static_cast<size_t>(index)];
        const auto categoryName = getArpeggioPatternCategory(pattern);
        auto category = std::find_if(categories.begin(), categories.end(), [&](const CategoryMenu& c) {
            return c.name == categoryName;
        });

        if (category == categories.end())
            category = std::prev(categories.end());

        auto label = pattern.nameCN;
        if (index >= userAddedPatternStartIndex)
            label = juce::String::fromUTF8(u8"★ ") + label;

        category->menu.addItem(patternComboIdOffset + index,
                               label,
                               true,
                               pattern.id == arpeggioState.selectedPatternId);
    };

    for (int i = userAddedPatternStartIndex; i < static_cast<int>(arpeggioPatterns.size()); ++i)
        addToCategory(i);
    for (int i = 0; i < juce::jmin(userAddedPatternStartIndex, static_cast<int>(arpeggioPatterns.size())); ++i)
        addToCategory(i);

    juce::PopupMenu menu;
    menu.addSectionHeader(juce::String::fromUTF8(u8"分解范式分类选择"));
    for (auto& category : categories)
        if (category.menu.getNumItems() > 0)
            menu.addSubMenu(category.name, category.menu);

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(arpeggioPatternButton),
                       [this](int result) {
                           const int index = result - patternComboIdOffset;
                           if (index >= 0)
                               selectArpeggioPatternIndex(index);
                       });
}

std::vector<ArpeggioRankedPattern> ProgressionBrowser::getCurrentArpeggioRecommendations() const
{
    auto entry = getSelectedEntry();
    if (entry.name.isEmpty())
        return {};

    auto settings = createArpeggioSettingsFromState();
    settings.preferredPatternId = {};

    auto context = makeArpeggioContextForEntry(entry, getCurrentTonic(), getCurrentScaleType(), 120.0);
    auto ranked = ArpeggioMatcher::rankPatterns(context, settings, arpeggioPatterns);
    if (ranked.size() > 8)
        ranked.resize(8);
    return ranked;
}

bool ProgressionBrowser::cycleSmartArpeggioRecommendation(int delta)
{
    if (!arpeggioState.smartMode || currentAuditionStep >= 0)
        return false;

    auto ranked = getCurrentArpeggioRecommendations();
    if (ranked.size() <= 1)
        return false;

    const int count = static_cast<int>(ranked.size());
    arpeggioState.smartRecommendationIndex = (arpeggioState.smartRecommendationIndex + delta + count) % count;
    arpeggioState.selectedPatternId = ranked[static_cast<size_t>(arpeggioState.smartRecommendationIndex)].pattern.id;
    arpeggioState.direction = ranked[static_cast<size_t>(arpeggioState.smartRecommendationIndex)].pattern.direction;
    updateArpeggioControlsFromState();
    updateArpeggioStatusText();
    return true;
}

juce::String ProgressionBrowser::getSelectedArpeggioPatternName() const
{
    for (const auto& pattern : arpeggioPatterns)
        if (pattern.id == arpeggioState.selectedPatternId)
            return pattern.nameCN;

    return juce::String::fromUTF8(u8"上行八分");
}

void ProgressionBrowser::updateArpeggioStatusText()
{
    if (currentAuditionStep >= 0)
        return;

    auto entry = getSelectedEntry();
    if (entry.name.isEmpty())
    {
        bottomInfoLabel.setText(LocalisedStrings::translate("preset_ready"), juce::dontSendNotification);
        return;
    }

    auto settings = createArpeggioSettingsFromState();
    auto context = makeArpeggioContextForEntry(entry, getCurrentTonic(), getCurrentScaleType(), 120.0);

    juce::String modeName = getSelectedArpeggioPatternName();
    juce::String modePrefix = juce::String::fromUTF8(u8"手动分解：");

    if (arpeggioState.smartMode)
    {
        auto ranked = getCurrentArpeggioRecommendations();
        if (!ranked.empty())
        {
            const int index = juce::jlimit(0, static_cast<int>(ranked.size()) - 1, arpeggioState.smartRecommendationIndex);
            const auto& pick = ranked[static_cast<size_t>(index)];
            modeName = pick.pattern.nameCN
                       + " " + juce::String(index + 1) + "/" + juce::String(ranked.size())
                       + " (" + getRecommendationReasonLabel(pick.reason) + ")";
            arpeggioState.selectedPatternId = pick.pattern.id;
            arpeggioState.direction = pick.pattern.direction;
        }
        modePrefix = juce::String::fromUTF8(u8"智能推荐：");
    }

    const auto humanizeText = arpeggioState.humanizeEnabled
        ? juce::String(arpeggioState.humanizeAmount)
        : juce::String::fromUTF8(u8"关闭");

    juce::StringArray layerFlags;
    if (arpeggioState.includeUpperColourLayer) layerFlags.add(juce::String::fromUTF8(u8"高彩"));
    if (arpeggioState.includeLeftRootBreakdown) layerFlags.add(juce::String::fromUTF8(u8"左根"));
    if (arpeggioState.includeBassDoubling) layerFlags.add(juce::String::fromUTF8(u8"倍低"));
    const auto layerText = layerFlags.isEmpty() ? juce::String::fromUTF8(u8"纯范式") : layerFlags.joinIntoString("+");



    bottomInfoLabel.setText(entry.name + " | " + modePrefix + modeName
                            + " / " + getDensityLabel(arpeggioState.density)
                            + " / " + layerText
                            + " / " + juce::String::fromUTF8(u8"人性化 ") + humanizeText,
                            juce::dontSendNotification);
}

//==============================================================================
// 试听播放
//==============================================================================
void ProgressionBrowser::startAudition()
{
    ProgressionEntry entry;
    for (auto& card : cards)
        if (card->getSelected()) { entry = card->getEntry(); break; }
    if (entry.name.isEmpty()) return;

    PitchClass tonic = getCurrentTonic();
    ScaleType type = getCurrentScaleType();
    lastTonic = tonic; lastType = type;

    double bpm = 120.0;
    if (auto* playHead = processor.getPlayHead())
    {
        auto posInfo = playHead->getPosition();
        if (posInfo.hasValue()) bpm = posInfo->getBpm().orFallback(120.0);
    }

    RenderSettings settings;
    settings.bpm = bpm;
    settings.noteLength = 4.0;
    settings.bassDouble = true;

    auto rawChords = ProgressionEngine::getChordNotes(entry.romanSequence, tonic, type);
    auditionChordNotes.clear();
    auditionVelocities.clear();
    int chordIndex = 0;
    for (auto& chord : rawChords)
    {
        if (chord.empty()) continue;
        int rootPc = chord[0];
        int offset = 60 - (rootPc % 12);
        std::vector<int> midiNotes;
        for (int pc : chord) midiNotes.push_back(pc + offset);
        auditionChordNotes.push_back(midiNotes);

        std::vector<int> velocities;
        velocities.reserve(midiNotes.size());
        for (int i = 0; i < static_cast<int>(midiNotes.size()); ++i)
        {
            const int accent = (i == 0 ? 100 : 86 - juce::jmin(i, 5) * 3);
            const int phraseLift = (chordIndex % 4 == 0 ? 4 : 0);
            const int drift = juce::Random::getSystemRandom().nextInt(9) - 4;
            velocities.push_back(juce::jlimit(58, 108, accent + phraseLift + drift));
        }
        auditionVelocities.push_back(velocities);
        ++chordIndex;
    }

    if (processor.isNegativeHarmonyEnabled())
        for (auto& chord : auditionChordNotes) chord = NegativeHarmony::flipChord(chord, tonic);

    auditionBpm = bpm;
    auditionChordIndex = 0;
    currentAuditionStep = 0;

    keyLabel.setText(LocalisedStrings::translate("preset_key_prefix") +
        PitchNameHelper::getNameForPitch(tonic, true) + " " +
        getScaleTypeName(type), juce::dontSendNotification);

    if (!auditionChordNotes.empty())
        processor.triggerAudition(auditionChordNotes[0], auditionVelocities.empty() ? std::vector<int>{} : auditionVelocities[0]);

    startTimer(static_cast<int>(60000.0 / bpm * settings.noteLength));
    playPauseButton.setButtonText(juce::String::fromUTF8(u8"||"));
    bottomInfoLabel.setText(LocalisedStrings::translate("preset_playing_prefix") + entry.name, juce::dontSendNotification);
}

void ProgressionBrowser::stopAudition()
{
    stopTimer();
    processor.releaseAudition();
    currentAuditionStep = -1;
    auditionChordIndex = 0;
    auditionVelocities.clear();

    playPauseButton.setButtonText(juce::String::fromUTF8(u8"▶"));

    ProgressionEntry selectedEntry;
    bool hasSelection = false;
    for (auto* card : cards) {
        if (card->getSelected()) {
            selectedEntry = card->getEntry();
            hasSelection = true;
            break;
        }
    }

    if (!hasSelection)
        bottomInfoLabel.setText(LocalisedStrings::translate("preset_stopped"), juce::dontSendNotification);

    keyLabel.setText(LocalisedStrings::translate("preset_key_prefix") +
                     PitchNameHelper::getNameForPitch(getCurrentTonic(), true) + " " +
                     getScaleTypeName(getCurrentScaleType()),
                     juce::dontSendNotification);

    if (hasSelection)
        updateArpeggioStatusText();
}

void ProgressionBrowser::timerCallback()
{
    processor.releaseAudition();

    if (currentAuditionStep < 0 || auditionChordNotes.empty())
    {
        stopAudition();
        return;
    }

    auditionChordIndex = (auditionChordIndex + 1) % auditionChordNotes.size();
    processor.triggerAudition(auditionChordNotes[auditionChordIndex],
                              auditionChordIndex < static_cast<int>(auditionVelocities.size())
                                  ? auditionVelocities[static_cast<size_t>(auditionChordIndex)]
                                  : std::vector<int>{});

    ProgressionEntry selectedEntry;
    bool found = false;
    for (auto* card : cards)
    {
        if (card->getSelected())
        {
            selectedEntry = card->getEntry();
            found = true;
            break;
        }
    }

    if (found)
    {
        juce::String symbols = getChordSymbolsForEntry(selectedEntry);
        juce::StringArray tokens;
        tokens.addTokens(symbols, " ", "\"");

        if (auditionChordIndex < tokens.size())
        {
            juce::String currentChord = tokens[auditionChordIndex];
            bottomInfoLabel.setText(selectedEntry.name + " -> " + currentChord, juce::dontSendNotification);
        }
        else
        {
            bottomInfoLabel.setText(selectedEntry.name, juce::dontSendNotification);
        }
    }
}

//==============================================================================
// 拖拽 MIDI 文件到轨道（通过右侧按钮触发）
//==============================================================================
void ProgressionBrowser::mouseDown(const juce::MouseEvent& e)
{
    if (e.eventComponent == &inspector.dragToTrackButton)
    {
        dragButtonPressed = true;
        midiDragMode = MidiDragMode::Progression;
        dragStartPos = e.getScreenPosition().toInt();
        repaint();
        return;
    }

    if (e.eventComponent == &inspector.dragArpeggioToTrackButton)
    {
        dragButtonPressed = true;
        midiDragMode = MidiDragMode::Arpeggio;
        dragStartPos = e.getScreenPosition().toInt();
        repaint();
        return;
    }
}

void ProgressionBrowser::mouseDrag(const juce::MouseEvent& e)
{
    if (!dragButtonPressed) return;
    if (e.getDistanceFromDragStart() < 15.0f) return;

    ProgressionEntry entry;
    for (auto& card : cards)
        if (card->getSelected()) { entry = card->getEntry(); break; }
    if (entry.name.isEmpty())
    {
        dragButtonPressed = false;
        midiDragMode = MidiDragMode::None;
        return;
    }

    PitchClass tonic = getCurrentTonic();
    ScaleType type = getCurrentScaleType();

    double bpm = 120.0;
    if (auto* playHead = processor.getPlayHead())
    {
        auto pos = playHead->getPosition();
        if (pos.hasValue()) bpm = pos->getBpm().orFallback(120.0);
    }

    juce::File midiFile;
    if (midiDragMode == MidiDragMode::Arpeggio)
    {
        auto settings = createArpeggioSettingsFromState();

        auto context = makeArpeggioContextForEntry(entry, tonic, type, bpm);
        if (arpeggioState.smartMode)
        {
            auto rankingSettings = settings;
            rankingSettings.preferredPatternId = {};
            auto ranked = ArpeggioMatcher::rankPatterns(context, rankingSettings, arpeggioPatterns);
            if (!ranked.empty())
            {
                const int index = juce::jlimit(0, static_cast<int>(ranked.size()) - 1, arpeggioState.smartRecommendationIndex);
                settings.preferredPatternId = ranked[static_cast<size_t>(index)].pattern.id;
            }
        }
        midiFile = ArpeggioEngine::generateMidiFile(context, settings);
    }
    else
    {
        RenderSettings settings;
        settings.bpm = bpm;
        settings.noteLength = 2.0;
        settings.bassDouble = true;
        settings.humanize = true;
        settings.humanizeVelocityRange = 8;
        settings.humanizeTimeRangeMs = 12;

        midiFile = ProgressionRenderer::generateMidiFile(entry.romanSequence, tonic, type, settings);
    }

    if (midiFile.existsAsFile())
    {
        juce::StringArray files;
        files.add(midiFile.getFullPathName());
        performExternalDragDropOfFiles(files, false);
    }
    dragButtonPressed = false;
    midiDragMode = MidiDragMode::None;
    resetDragButtonColor();
    repaint();
}

void ProgressionBrowser::mouseUp(const juce::MouseEvent&)
{
    if (dragButtonPressed)
    {
        dragButtonPressed = false;
        midiDragMode = MidiDragMode::None;
        repaint();
    }
}

void ProgressionBrowser::resetDragButtonColor()
{
    // 拖拽按钮实际无颜色变化，保留空实现
}

//==============================================================================
// 获取当前选中预设（空则返回空）
//==============================================================================
ProgressionEntry ProgressionBrowser::getSelectedEntry() const
{
    for (auto* card : cards)
        if (card->getSelected())
            return card->getEntry();
    return {};
}

//==============================================================================
// 和弦符号生成辅助
//==============================================================================
juce::String ProgressionBrowser::getChordSymbolsForEntry(const ProgressionEntry& entry) const
{
    PitchClass tonic = getCurrentTonic();
    ScaleType type   = getCurrentScaleType();

    auto parsed = ProgressionEngine::parseProgression(entry.romanSequence);
    juce::String result;
    for (auto& pc : parsed)
    {
        int rootPitch = ProgressionEngine::getScaleDegreeInterval(pc.scaleDegree, type)
                        + static_cast<int>(tonic) + pc.alteration;
        int pitch = (rootPitch % 12 + 12) % 12;

        juce::String name = PitchNameHelper::getNameForPitch(static_cast<PitchClass>(pitch), true);

        if (pc.quality == "min" || pc.quality == "minor")      name += "m";
        else if (pc.quality == "dom7")                         name += "7";
        else if (pc.quality == "maj7")                         name += "maj7";
        else if (pc.quality == "dim")                          name += "dim";
        else if (pc.quality == "aug")                          name += "aug";
        else if (pc.quality == "sus2")                         name += "sus2";
        else if (pc.quality == "sus4")                         name += "sus4";

        result += name + " ";
    }
    return result.trim();
}

//==============================================================================
// 导入/导出 菜单
//==============================================================================
void ProgressionBrowser::showImportMenu()
{
    juce::PopupMenu menu;
    menu.addItem(LocalisedStrings::translate("auto_import_single_json_preset"), [this] {
        importChooser = std::make_unique<juce::FileChooser>(
            LocalisedStrings::translate("auto_choose_the_json_file_to_import"),
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "*.json",
            true,
            getTopLevelComponent());

        importChooser->launchAsync(juce::FileBrowserComponent::openMode |
                                   juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser) {
                auto file = chooser.getResult();
                if (file.existsAsFile())
                    importSingleJson(file);
            });
    });

    menu.addItem(LocalisedStrings::translate("auto_import_zip_preset_pack"), [this] {
        importChooser = std::make_unique<juce::FileChooser>(
            LocalisedStrings::translate("auto_choose_the_zip_file_to_import"),
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "*.zip",
            true,
            getTopLevelComponent());

        importChooser->launchAsync(juce::FileBrowserComponent::openMode |
                                   juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser) {
                auto file = chooser.getResult();
                if (file.existsAsFile())
                    importZipPackage(file);
            });
    });

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(importButton));
}

void ProgressionBrowser::showExportMenu()
{
    juce::PopupMenu menu;
    menu.addItem(LocalisedStrings::translate("auto_export_selected_preset_as_json"), [this] {
        ProgressionEntry entry = getSelectedEntry();
        if (entry.name.isEmpty())
        {
            juce::AlertWindow::showAsync(
                juce::MessageBoxOptions()
                    .withTitle(LocalisedStrings::translate("auto_notice"))
                    .withMessage(LocalisedStrings::translate("auto_no_preset_selected"))
                    .withIconType(juce::AlertWindow::WarningIcon),
                nullptr);
            return;
        }
        juce::Array<ProgressionEntry> list;
        list.add(entry);
        exportEntries(list, true);
    });

    menu.addSeparator();

    menu.addItem(LocalisedStrings::translate("auto_export_all_presets_as_one_json_file"), [this] {
        exportEntries(progressionData.getAllEntries(), true);
    });

    menu.addItem(LocalisedStrings::translate("auto_export_all_presets_as_separate_json_files"), [this] {
        exportEntries(progressionData.getAllEntries(), false);
    });

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(exportButton));
}

//==============================================================================
// 导入 JSON / ZIP 实现
//==============================================================================
void ProgressionBrowser::importSingleJson(const juce::File& file)
{
    juce::String fileContent = file.loadFileAsString();
    if (fileContent.isEmpty()) {
        showMessageDialog(LocalisedStrings::translate("auto_error"),
                          LocalisedStrings::translate("auto_unable_to_read_the_file_or_the_file_is_empty"),
                          juce::AlertWindow::WarningIcon);
        return;
    }

    auto json = juce::JSON::parse(fileContent);
    if (json.isVoid()) {
        showMessageDialog(LocalisedStrings::translate("auto_error"),
                          LocalisedStrings::translate("auto_invalid_json_format_unable_to_parse"),
                          juce::AlertWindow::WarningIcon);
        return;
    }

    juce::Array<ProgressionEntry> imported;

    if (auto* array = json.getArray())
    {
        for (auto& item : *array)
            if (item.isObject())
                imported.add(ProgressionEntry::fromJSON(item));
    }
    else if (json.isObject())
    {
        imported.add(ProgressionEntry::fromJSON(json));
    }

    if (imported.isEmpty())
    {
        showMessageDialog(LocalisedStrings::translate("auto_import_failed"),
                          LocalisedStrings::translate("auto_no_valid_preset_data_found"),
                          juce::AlertWindow::WarningIcon);
        return;
    }

    addImportedEntries(imported);
}

void ProgressionBrowser::importZipPackage(const juce::File& zipFile)
{
    juce::MemoryBlock zipData;
    {
        juce::FileInputStream fis(zipFile);
        if (!fis.openedOk()) {
            showMessageDialog(LocalisedStrings::translate("auto_error"), LocalisedStrings::translate("auto_unable_to_read_the_zip_file"), juce::AlertWindow::WarningIcon);
            return;
        }
        fis.readIntoMemoryBlock(zipData);
    }

    juce::MemoryInputStream mis(zipData, false);
    juce::ZipFile zip(mis);

    juce::Array<ProgressionEntry> imported;

    for (int i = 0; i < zip.getNumEntries(); ++i)
    {
        if (auto* entry = zip.getEntry(i))
        {
            if (entry->filename.endsWith(".json") && entry->uncompressedSize > 0)
            {
                std::unique_ptr<juce::InputStream> stream(zip.createStreamForEntry(i));
                if (stream != nullptr)
                {
                    juce::String jsonStr = stream->readEntireStreamAsString();
                    auto json = juce::JSON::parse(jsonStr);

                    if (json.isVoid()) continue;

                    if (auto* arr = json.getArray()) {
                        for (auto& item : *arr) {
                            if (item.isObject()) imported.add(ProgressionEntry::fromJSON(item));
                        }
                    }
                    else if (json.isObject()) {
                        imported.add(ProgressionEntry::fromJSON(json));
                    }
                }
            }
        }
    }

    if (imported.isEmpty())
    {
        showMessageDialog(LocalisedStrings::translate("auto_import_failed"), LocalisedStrings::translate("auto_no_valid_json_presets_found_in_the_zip_file"), juce::AlertWindow::WarningIcon);
        return;
    }

    addImportedEntries(imported);
}

void ProgressionBrowser::addImportedEntries(const juce::Array<ProgressionEntry>& entries)
{
    int addedCount = 0;
    int skippedCount = 0;

    auto existingEntries = progressionData.getAllEntries();

    for (auto& entry : entries)
    {
        if (entry.name.isEmpty() || entry.romanSequence.isEmpty())
            continue;

        ProgressionEntry newEntry = entry;
        newEntry.isUserPreset = true;

        juce::String originalName = newEntry.name;
        int counter = 1;

        while (std::any_of(existingEntries.begin(), existingEntries.end(),
                           [&](const ProgressionEntry& e) { return e.name == newEntry.name; }))
        {
            newEntry.name = originalName + " (" + juce::String(counter) + ")";
            counter++;
        }

        try {
            if (progressionData.addUserPreset(newEntry))
            {
                existingEntries.add(newEntry);
                addedCount++;
            }
            else
            {
                skippedCount++;
            }
        }
        catch (...) {
            skippedCount++;
        }
    }

    if (addedCount > 0)
    {
        progressionData.reload();
        invalidatePresetBrowserCaches();
        updateFilteredList();
    }

    juce::String msg = LocalisedStrings::translate("auto_successfully_imported") + juce::String(addedCount) + LocalisedStrings::translate("auto_presets");
    if (skippedCount > 0)
        msg += "\n" + LocalisedStrings::translate("auto_skipped") + juce::String(skippedCount) + LocalisedStrings::translate("auto_invalid_presets");

    showMessageDialog(LocalisedStrings::translate("auto_import_complete"), msg, juce::AlertWindow::InfoIcon);
}

void ProgressionBrowser::exportEntries(const juce::Array<ProgressionEntry>& entries, bool singleJson)
{
    if (entries.isEmpty())
        return;

    if (singleJson)
    {
        juce::String defaultName = "Presets.json";
        if (entries.size() == 1)
            defaultName = entries[0].name.replaceCharacters("\\/:*?\"<>|", "_") + ".json";

        exportChooser = std::make_unique<juce::FileChooser>(
            LocalisedStrings::translate("auto_save_preset_json_file"),
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile(defaultName),
            "*.json",
            true,
            getTopLevelComponent());
    }
    else
    {
        exportChooser = std::make_unique<juce::FileChooser>(
            LocalisedStrings::translate("auto_choose_export_folder"),
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "",
            true,
            getTopLevelComponent());
    }

    auto flags = singleJson
        ? juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles
        : juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;

    exportChooser->launchAsync(flags,
        [this, entries, singleJson](const juce::FileChooser& chooser)
        {
            juce::File result = chooser.getResult();

            if (result == juce::File())
            {
                result = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                             .getChildFile("ChordReference Presets");
                result.createDirectory();
                if (singleJson)
                {
                    juce::String backupName = (entries.size() == 1)
                        ? entries[0].name.replaceCharacters("\\/:*?\"<>|", "_") + ".json"
                        : "AllPresets.json";
                    result = result.getChildFile(backupName);
                }
            }

            if (singleJson)
            {
                if (result.getFileExtension() != ".json")
                    result = result.withFileExtension(".json");

                juce::Array<juce::var> arr;
                for (auto& e : entries)
                    arr.add(e.toJSON());

                juce::FileOutputStream fos(result);
                if (fos.openedOk())
                {
                    juce::JSON::writeToStream(fos, juce::var(arr));
                    showSuccessMessage(true, static_cast<int>(entries.size()), result);
                }
                else
                {
                    showMessageDialog(LocalisedStrings::translate("auto_error"),
                                      LocalisedStrings::translate("auto_unable_to_write_the_file_please_check_disk_permi"),
                                      juce::AlertWindow::WarningIcon);
                }
            }
            else
            {
                int success = 0;
                for (auto& e : entries)
                {
                    juce::String safeName = e.name.replaceCharacters("\\/:*?\"<>|", "_") + ".json";
                    juce::File outFile = result.getChildFile(safeName);
                    juce::FileOutputStream fos(outFile);
                    if (fos.openedOk())
                    {
                        juce::Array<juce::var> arr;
                        arr.add(e.toJSON());
                        juce::JSON::writeToStream(fos, juce::var(arr));
                        ++success;
                    }
                    else
                    {
                        outFile.deleteFile();
                    }
                }
                showSuccessMessage(false, success, result);
            }
        });
}

void ProgressionBrowser::showSuccessMessage(bool singleFile, int count, const juce::File& path)
{
    juce::String msg = singleFile
        ? LocalisedStrings::translate("auto_exported_to") + path.getFullPathName()
        : LocalisedStrings::translate("auto_successfully_exported") + juce::String(count) + LocalisedStrings::translate("auto_files_to") + path.getFullPathName();

    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withTitle(LocalisedStrings::translate("auto_export_successful"))
            .withMessage(msg)
            .withIconType(juce::AlertWindow::InfoIcon)
            .withButton(LocalisedStrings::translate("auto_ok")),
        nullptr);
}

void ProgressionBrowser::showMessageDialog(const juce::String& title, const juce::String& message,
                                           juce::AlertWindow::AlertIconType iconType) const
{
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withTitle(title)
            .withMessage(message)
            .withIconType(iconType)
            .withButton(LocalisedStrings::translate("auto_ok")),
        nullptr);
}

void ProgressionBrowser::showFolderMenu(int rowNumber, juce::Point<int> screenPos)
{
    const int catCount = categoryNames.size();
    const int folderIndex = rowNumber - (catCount + 5);
    auto folders = progressionData.getUserFolders();

    juce::PopupMenu menu;
    menu.addItem(LocalisedStrings::translate("preset_new_folder"), [this] { promptCreateFolder(); });

    if (folderIndex >= 0 && folderIndex < folders.size())
    {
        const auto folder = folders[folderIndex];
        menu.addSeparator();
        menu.addItem(LocalisedStrings::translate("preset_rename_folder"),
                     [this, folder] { promptRenameFolder(folder.id, folder.name); });
        menu.addItem(LocalisedStrings::translate("preset_delete_folder"),
                     progressionData.getUserFolderEntryCount(folder.id) == 0,
                     false,
                     [this, folder] {
                         progressionData.deleteUserFolder(folder.id);
                         categoryList.updateContent();
                         if (selectedUserFolderId == folder.id)
                         {
                             currentFilterMode = UserPresets;
                             selectedUserFolderId.clear();
                             categoryTitleLabel.setText(LocalisedStrings::translate("preset_user_presets"), juce::dontSendNotification);
                             updateFilteredList();
                         }
                     });
    }

    menu.showMenuAsync(juce::PopupMenu::Options()
                           .withTargetScreenArea({ screenPos.x, screenPos.y, 1, 1 }));
}

void ProgressionBrowser::promptCreateFolder()
{
    auto* alert = new juce::AlertWindow(LocalisedStrings::translate("preset_new_folder"),
                                        LocalisedStrings::translate("preset_folder_name"),
                                        juce::AlertWindow::NoIcon,
                                        this);
    alert->addTextEditor("name", "", LocalisedStrings::translate("preset_folder_name"));
    alert->addButton(LocalisedStrings::translate("auto_cancel"), 0);
    alert->addButton(LocalisedStrings::translate("auto_ok"), 1);
    alert->enterModalState(true, juce::ModalCallbackFunction::create([this, alert](int result) {
        if (result == 1 && progressionData.createUserFolder(alert->getTextEditorContents("name")))
            categoryList.updateContent();
    }), true);
}

void ProgressionBrowser::promptRenameFolder(const juce::String& folderId, const juce::String& oldName)
{
    auto* alert = new juce::AlertWindow(LocalisedStrings::translate("preset_rename_folder"),
                                        LocalisedStrings::translate("preset_folder_name"),
                                        juce::AlertWindow::NoIcon,
                                        this);
    alert->addTextEditor("name", oldName, LocalisedStrings::translate("preset_folder_name"));
    alert->addButton(LocalisedStrings::translate("auto_cancel"), 0);
    alert->addButton(LocalisedStrings::translate("auto_ok"), 1);
    alert->enterModalState(true, juce::ModalCallbackFunction::create([this, alert, folderId](int result) {
        if (result == 1 && progressionData.renameUserFolder(folderId, alert->getTextEditorContents("name")))
        {
            categoryList.updateContent();
            if (selectedUserFolderId == folderId)
            {
                for (const auto& folder : progressionData.getUserFolders())
                    if (folder.id == folderId)
                        categoryTitleLabel.setText(folder.name, juce::dontSendNotification);
            }
        }
    }), true);
}

void ProgressionBrowser::addMoveToFolderMenu(juce::PopupMenu& menu, const ProgressionEntry& entry)
{
    if (!entry.isUserPreset)
        return;

    juce::PopupMenu folderMenu;
    folderMenu.addItem(LocalisedStrings::translate("preset_no_folder"),
                       entry.folderId.isNotEmpty(),
                       false,
                       [this, entry] {
                           progressionData.moveUserPresetToFolder(entry.name, {});
                           progressionData.reload();
                           invalidatePresetBrowserCaches();
                           updateFilteredList();
                           categoryList.updateContent();
                       });

    for (const auto& folder : progressionData.getUserFolders())
    {
        folderMenu.addItem(folder.name,
                           entry.folderId != folder.id,
                           false,
                           [this, entry, folder] {
                               progressionData.moveUserPresetToFolder(entry.name, folder.id);
                               progressionData.reload();
                               invalidatePresetBrowserCaches();
                               updateFilteredList();
                               categoryList.updateContent();
                           });
    }

    if (folderMenu.getNumItems() > 0)
        menu.addSubMenu(LocalisedStrings::translate("preset_move_to_folder"), folderMenu);
}

//==============================================================================
// 类别列表模型实现
//==============================================================================
int ProgressionBrowser::CategoryListModel::getNumRows()
{
    return owner.categoryNames.size() + 5 + owner.progressionData.getUserFolders().size();
}

void ProgressionBrowser::CategoryListModel::paintListBoxItem(
    int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    juce::Colour listItemTextColour = juce::Colours::white;
    juce::Colour listItemHighlight = juce::Colour(0x555E3BEE).withAlpha(0.55f);
    juce::Colour separatorColour = juce::Colours::white.withAlpha(0.15f);
    juce::Colour categoryCountColour = juce::Colours::white.withAlpha(0.45f);

    if (rowNumber == 0)
    {
        int totalCount = owner.progressionData.getAllEntries().size();
        juce::String text = LocalisedStrings::translate("preset_show_all_prefix") + juce::String(totalCount) + ")";

        if (rowIsSelected) g.fillAll(listItemHighlight);
        g.setColour(listItemTextColour);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);
        g.setColour(separatorColour);
        g.drawHorizontalLine(height - 1, 6, width - 6);
        return;
    }

    int catCount = owner.categoryNames.size();

    if (rowNumber == catCount + 4)
    {
        if (rowIsSelected) g.fillAll(listItemHighlight);
        g.setColour(separatorColour);
        g.drawHorizontalLine(0, 6, width - 6);
        g.setColour(listItemTextColour.withAlpha(0.8f));
        g.setFont(juce::Font(13.0f, juce::Font::bold));
        g.drawText(LocalisedStrings::translate("preset_my_folders"), 6, 0, width - 12, height,
                   juce::Justification::centredLeft, true);
        return;
    }

    if (rowNumber > catCount + 4)
    {
        const int folderIndex = rowNumber - (catCount + 5);
        auto folders = owner.progressionData.getUserFolders();
        if (folderIndex >= 0 && folderIndex < folders.size())
        {
            const auto folder = folders[folderIndex];
            if (rowIsSelected) g.fillAll(listItemHighlight);
            g.setColour(listItemTextColour);
            g.setFont(juce::Font(14.0f));
            g.drawText("  " + folder.name, 6, 0, width - 70, height,
                       juce::Justification::centredLeft, true);

            juce::String countText = "  (" + juce::String(owner.progressionData.getUserFolderEntryCount(folder.id)) + ")";
            g.setColour(categoryCountColour);
            g.drawText(countText, width - 70, 0, 60, height, juce::Justification::centredRight, true);
        }
        return;
    }

    if (rowNumber >= catCount + 1)
    {
        if (rowNumber == catCount + 1)
        {
            // 收藏行
            juce::String text = LocalisedStrings::translate("preset_favorites");
            if (rowIsSelected) g.fillAll(listItemHighlight);
            g.setColour(listItemTextColour);
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);
            g.setColour(separatorColour);
            g.drawHorizontalLine(0, 6, width - 6);
            return;
        }
        else if (rowNumber == catCount + 2)
        {
            // 常用行
            juce::String text = LocalisedStrings::translate("preset_frequent");
            if (rowIsSelected) g.fillAll(listItemHighlight);
            g.setColour(listItemTextColour);
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);
            return;
        }
        else if (rowNumber == catCount + 3)
        {
            // 用户预设行
            juce::String text = LocalisedStrings::translate("preset_user_presets");
            if (rowIsSelected) g.fillAll(listItemHighlight);
            g.setColour(listItemTextColour);
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);
            return;
        }
        else
        {
            // 未来自定义文件夹占位（暂时空）
            if (rowIsSelected) g.fillAll(listItemHighlight);
            g.setColour(separatorColour);
            g.drawHorizontalLine(height / 2, 6, width - 6);
            return;
        }
    }

    int catIndex = rowNumber - 1;
    if (rowIsSelected) g.fillAll(listItemHighlight);

    g.setFont(juce::Font(15.0f, juce::Font::bold));

    int categoryCount = (catIndex < owner.categoryTagCounts.size()) ? owner.categoryTagCounts[catIndex] : 0;
    juce::String text = owner.categoryNames[catIndex];
    juce::String countText = "  (" + juce::String(categoryCount) + ")";

    g.setColour(listItemTextColour);
    g.drawText(text, 6, 0, width - 70, height, juce::Justification::centredLeft, true);
    g.setColour(categoryCountColour);
    g.drawText(countText, width - 70, 0, 60, height, juce::Justification::centredRight, true);
}

void ProgressionBrowser::CategoryListModel::selectedRowsChanged(int lastRowSelected)
{
    int catCount = owner.categoryNames.size();

    if (lastRowSelected == 0)
    {
        owner.currentFilterMode = ProgressionBrowser::All;
        owner.activeTags.clear();
        owner.selectedCategoryIndex = -1;
        owner.categoryTitleLabel.setText(LocalisedStrings::translate("preset_all_title"), juce::dontSendNotification);
        owner.buildTagPanel();
        owner.updateFilteredList();
        return;
    }

    if (lastRowSelected == catCount + 1)
    {
        owner.currentFilterMode = ProgressionBrowser::Favorites;
        owner.activeTags.clear();
        owner.selectedCategoryIndex = -1;
        owner.categoryTitleLabel.setText(LocalisedStrings::translate("preset_favorites"), juce::dontSendNotification);
        owner.buildTagPanel();
        owner.updateFilteredList();
        return;
    }

    if (lastRowSelected == catCount + 2)
    {
        owner.currentFilterMode = ProgressionBrowser::FrequentUsed;
        owner.activeTags.clear();
        owner.selectedCategoryIndex = -1;
        owner.categoryTitleLabel.setText(LocalisedStrings::translate("preset_frequent"), juce::dontSendNotification);
        owner.buildTagPanel();
        owner.updateFilteredList();
        return;
    }

    if (lastRowSelected == catCount + 3)
    {
        owner.currentFilterMode = ProgressionBrowser::UserPresets;
        owner.activeTags.clear();
        owner.selectedCategoryIndex = -1;
        owner.categoryTitleLabel.setText(LocalisedStrings::translate("preset_user_presets"), juce::dontSendNotification);
        owner.buildTagPanel();
        owner.updateFilteredList();
        return;
    }

    if (lastRowSelected == catCount + 4)
    {
        owner.showFolderMenu(lastRowSelected, owner.categoryList.getScreenBounds().getCentre());
        return;
    }

    if (lastRowSelected > catCount + 4)
    {
        const int folderIndex = lastRowSelected - (catCount + 5);
        auto folders = owner.progressionData.getUserFolders();
        if (folderIndex >= 0 && folderIndex < folders.size())
        {
            owner.currentFilterMode = ProgressionBrowser::UserFolder;
            owner.activeTags.clear();
            owner.selectedCategoryIndex = -1;
            owner.selectedUserFolderId = folders[folderIndex].id;
            owner.categoryTitleLabel.setText(folders[folderIndex].name, juce::dontSendNotification);
            owner.buildTagPanel();
            owner.updateFilteredList();
        }
        return;
    }

    owner.currentFilterMode = ProgressionBrowser::Category;
    owner.selectedCategoryIndex = lastRowSelected - 1;
    owner.categoryTitleLabel.setText(owner.categoryNames[lastRowSelected - 1], juce::dontSendNotification);
    owner.buildTagPanel();
    owner.updateFilteredList();
}

void ProgressionBrowser::CategoryListModel::listBoxItemClicked(int rowNumber, const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
        owner.showFolderMenu(rowNumber, e.getScreenPosition().toInt());
}

//==============================================================================
// 绘制背景与分割线
//==============================================================================
void ProgressionBrowser::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0D091A));

    g.setColour(juce::Colour(0xff1A142F).withAlpha(0.8f));
    if (leftBarRect.getHeight() > 0)
        g.fillRoundedRectangle(leftBarRect.toFloat(), 12.0f);

    auto centerRect = categoryTitleLabel.getBounds()
                        .getUnion(tagViewport.getBounds())
                        .getUnion(matchCountLabel.getBounds())
                        .getUnion(cardViewport.getBounds())
                        .toFloat().expanded(4.0f);
    g.fillRoundedRectangle(centerRect, 12.0f);

    g.fillRoundedRectangle(inspector.getBounds().toFloat(), 12.0f);

    auto capsuleArea = prevPresetButton.getBounds()
                           .getUnion(playPauseButton.getBounds())
                           .getUnion(nextPresetButton.getBounds())
                           .expanded(10, 4)
                           .toFloat();
    if (!capsuleArea.isEmpty())
    {
        g.setColour(juce::Colour(0xff5E3BEE).withAlpha(0.15f));
        g.fillRoundedRectangle(capsuleArea, capsuleArea.getHeight() / 2);
        g.setColour(juce::Colour(0xff9D6BFF).withAlpha(0.4f));
        g.drawRoundedRectangle(capsuleArea, capsuleArea.getHeight() / 2, 1.0f);
    }

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawHorizontalLine(topBarRect.getBottom(), topBarRect.getX(), topBarRect.getRight());
    g.drawHorizontalLine(bottomBarRect.getY() - 1, bottomBarRect.getX(), bottomBarRect.getRight());

    // 绘制搜索框下划线
    auto searchBounds = searchBox.getBounds().toFloat();
    g.setColour(juce::Colour(0xFF513DEB).withAlpha(0.8f));
    g.drawLine(juce::Line<float>(searchBounds.getBottomLeft(), searchBounds.getBottomRight()), 1.0f);
}

//==============================================================================
// 预设编辑器显示
//==============================================================================
void ProgressionBrowser::showPresetEditor()
{
    auto* editor = new PresetEditorPanel(progressionData, getCurrentTonic, getCurrentScaleType);
    editor->onSave = [this, editor](const ProgressionEntry& entry) {
        if (entry.romanSequence.isEmpty() || entry.name.isEmpty()) return;
        progressionData.addUserPreset(entry);
        progressionData.reload();
        invalidatePresetBrowserCaches();
        updateFilteredList();
        delete editor;
    };
    editor->onCancel = [editor] { delete editor; };
    addAndMakeVisible(editor);
    editor->setBounds(getLocalBounds());
}

void ProgressionBrowser::showPresetEditor(const ProgressionEntry& entry)
{
    auto* editor = new PresetEditorPanel(progressionData, getCurrentTonic, getCurrentScaleType);
    editor->setEntry(entry);
    editor->onSave = [this, editor](const ProgressionEntry& newEntry) {
        if (newEntry.romanSequence.isEmpty() || newEntry.name.isEmpty()) return;
        progressionData.updateUserPreset(editor->getOriginalName(), newEntry);
        progressionData.reload();
        invalidatePresetBrowserCaches();
        updateFilteredList();
        delete editor;
    };
    editor->onCancel = [editor] { delete editor; };
    addAndMakeVisible(editor);
    editor->setBounds(getLocalBounds());
}

//==============================================================================
// 皮肤
//==============================================================================
void ProgressionBrowser::setSkin(SkinType newSkin)
{
    if (currentSkin != newSkin)
    {
        currentSkin = newSkin;
        applySkinColours();
        repaint();
    }
}
//==============================================================================
// 批量删除操作实现
//==============================================================================
void ProgressionBrowser::batchDeleteSelected()
{
    juce::Array<juce::String> toDelete;
    for (auto* c : cards)
        if (c->isMultiSelected && c->getEntry().name.isNotEmpty() && c->getEntry().isUserPreset)
            toDelete.add(c->getEntry().name);

    if (toDelete.isEmpty()) return;

    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withTitle(LocalisedStrings::translate("auto_batch_delete"))
            .withMessage(LocalisedStrings::translate("auto_delete_2") + juce::String(toDelete.size()) + LocalisedStrings::translate("auto_presets_2"))
            .withIconType(juce::AlertWindow::WarningIcon)
            .withButton(LocalisedStrings::translate("auto_ok"))
            .withButton(LocalisedStrings::translate("auto_cancel")),
        [this, toDelete](int result) {
            if (result == 1) { // 确定按钮
                for (auto& name : toDelete)
                    progressionData.removeUserPreset(name);
                progressionData.reload();
                invalidatePresetBrowserCaches();
                updateFilteredList();
            }
        });
}
