//==================================================================================================
//  TagDefinitions.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：标签分类定义实现（从 ProgressionBrowser / PresetEditorPanel 提取）
//        - 新增从 presets.json 中提取的缺失标签（2026-05-17）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "TagDefinitions.h"
#include "LocalisedStrings.h"

juce::Array<juce::StringArray> TagDefinitions::getAllCategoryTags()
{
    // ========== 1. 调式 / Mode ==========
    juce::StringArray modeTags = {
        LocalisedStrings::translate("auto_major"),
        LocalisedStrings::translate("auto_natural_minor"),
        LocalisedStrings::translate("auto_harmonic_minor"),
        LocalisedStrings::translate("auto_melodic_minor"),
        LocalisedStrings::translate("auto_dorian"),
        LocalisedStrings::translate("auto_phrygian"),
        LocalisedStrings::translate("auto_lydian"),
        LocalisedStrings::translate("auto_mixolydian"),
        LocalisedStrings::translate("auto_locrian"),
        LocalisedStrings::translate("auto_pentatonicgong_mode"),
        LocalisedStrings::translate("auto_pentatonicyu_mode"),
        LocalisedStrings::translate("auto_english_translation_pending_2"), // 商调
        LocalisedStrings::translate("auto_english_translation_pending_12"), // 角调
        LocalisedStrings::translate("auto_english_translation_pending_11"), // 徵调
        LocalisedStrings::translate("auto_japanese_miyako_bushi"),
        LocalisedStrings::translate("auto_japanese_ryukyu"),
        LocalisedStrings::translate("auto_blues_scale"),
        LocalisedStrings::translate("auto_diatonic"),
        LocalisedStrings::translate("auto_chromatic"),
        LocalisedStrings::translate("auto_diminished_scale"),
        LocalisedStrings::translate("auto_augmented_scale"),
        LocalisedStrings::translate("auto_indian_raga"),
        LocalisedStrings::translate("auto_middle_eastern_maqam")
    };

    // ========== 2. 地域 / 流派 ==========
    juce::StringArray regionTags = {
        // 原有
        LocalisedStrings::translate("auto_c_pop"),
        LocalisedStrings::translate("auto_j_pop"),
        LocalisedStrings::translate("auto_k_pop"),
        LocalisedStrings::translate("auto_anime"),
        LocalisedStrings::translate("auto_rock"),
        LocalisedStrings::translate("auto_r_b"),
        LocalisedStrings::translate("auto_blues"),
        LocalisedStrings::translate("auto_jazz"),
        LocalisedStrings::translate("auto_edm"),
        LocalisedStrings::translate("auto_lo_fi"),
        LocalisedStrings::translate("auto_hip_hop"),
        LocalisedStrings::translate("auto_country"),
        LocalisedStrings::translate("auto_folk"),
        LocalisedStrings::translate("auto_latin"),
        LocalisedStrings::translate("auto_bossa_nova"),
        LocalisedStrings::translate("auto_salsa"),
        LocalisedStrings::translate("auto_tango"),
        LocalisedStrings::translate("auto_reggae"),
        LocalisedStrings::translate("auto_funk"),
        LocalisedStrings::translate("auto_soul"),
        LocalisedStrings::translate("auto_gospel"),
        LocalisedStrings::translate("auto_classical"),
        LocalisedStrings::translate("auto_baroque"),
        LocalisedStrings::translate("auto_romantic"),
        LocalisedStrings::translate("auto_impressionist"),
        LocalisedStrings::translate("auto_celtic"),
        LocalisedStrings::translate("auto_flamenco"),
        LocalisedStrings::translate("auto_indian"),
        LocalisedStrings::translate("auto_arabic"),
        LocalisedStrings::translate("auto_african"),

        // 新增（从 presets.json 提取）
        LocalisedStrings::translate("tag_80s"),
        LocalisedStrings::translate("tag_choro"),
        LocalisedStrings::translate("tag_cumbia"),
        LocalisedStrings::translate("tag_disco"),
        LocalisedStrings::translate("tag_drill"),
        LocalisedStrings::translate("tag_drum_and_bass"),
        LocalisedStrings::translate("tag_dub"),
        LocalisedStrings::translate("tag_dubstep"),
        LocalisedStrings::translate("tag_emo"),
        LocalisedStrings::translate("tag_future_bass"),
        LocalisedStrings::translate("tag_future_garage"),
        LocalisedStrings::translate("tag_g_funk"),
        LocalisedStrings::translate("tag_gypsy"),
        LocalisedStrings::translate("tag_hardstyle"),
        LocalisedStrings::translate("tag_house"),
        LocalisedStrings::translate("tag_jersey_club"),
        LocalisedStrings::translate("tag_mariachi"),
        LocalisedStrings::translate("tag_memphis_rap"),
        LocalisedStrings::translate("tag_motown"),
        LocalisedStrings::translate("tag_neo_soul"),
        LocalisedStrings::translate("tag_new_age"),
        LocalisedStrings::translate("tag_nordic"),
        LocalisedStrings::translate("tag_piano_rock"),
        LocalisedStrings::translate("tag_post_rock"),
        LocalisedStrings::translate("tag_punk"),
        LocalisedStrings::translate("tag_samba"),
        LocalisedStrings::translate("tag_synthwave"),
        LocalisedStrings::translate("tag_techno"),
        LocalisedStrings::translate("tag_trance"),
        LocalisedStrings::translate("tag_trap"),
        LocalisedStrings::translate("tag_trip_hop"),
        LocalisedStrings::translate("tag_vaporwave"),
        LocalisedStrings::translate("tag_visual_kei"),
        LocalisedStrings::translate("tag_world_music"),
        LocalisedStrings::translate("tag_y2k"),
        LocalisedStrings::translate("tag_yodel"),
        LocalisedStrings::translate("tag_zeuhl")
    };

    // ========== 3. 情感色彩 ==========
    juce::StringArray moodTags = {
        LocalisedStrings::translate("auto_romantic_2"),
        LocalisedStrings::translate("auto_sad"),
        LocalisedStrings::translate("auto_healing"),
        LocalisedStrings::translate("auto_uplifting"),
        LocalisedStrings::translate("auto_epic"),
        LocalisedStrings::translate("auto_dreamy"),
        LocalisedStrings::translate("auto_tense_2"),
        LocalisedStrings::translate("auto_suspenseful"),
        LocalisedStrings::translate("auto_happy"),
        LocalisedStrings::translate("auto_melancholy"),
        LocalisedStrings::translate("auto_dark"),
        LocalisedStrings::translate("auto_warm"),
        LocalisedStrings::translate("auto_cold"),
        LocalisedStrings::translate("auto_ethereal"),
        LocalisedStrings::translate("auto_nostalgic"),
        LocalisedStrings::translate("auto_hopeful"),
        LocalisedStrings::translate("auto_solemn"),
        LocalisedStrings::translate("auto_playful"),
        LocalisedStrings::translate("auto_sensual"),
        // 新增
        LocalisedStrings::translate("tag_chill"),
        LocalisedStrings::translate("tag_mysterious")
    };

    // ========== 4. 复杂度 ==========
    juce::StringArray complexTags = {
        LocalisedStrings::translate("auto_triad"),
        LocalisedStrings::translate("auto_seventh_chord"),
        LocalisedStrings::translate("auto_ninth_chord"),
        LocalisedStrings::translate("auto_extended_chord"),
        LocalisedStrings::translate("auto_chromaticism"),
        LocalisedStrings::translate("auto_chromatic"),
        LocalisedStrings::translate("auto_suspension"),
        LocalisedStrings::translate("auto_modulation"),
        LocalisedStrings::translate("auto_modal_interchange"),
        LocalisedStrings::translate("auto_secondary_dominant"),
        LocalisedStrings::translate("auto_diminished_chord"),
        LocalisedStrings::translate("auto_augmented_chord"),
        LocalisedStrings::translate("auto_pedal_tone"),
        LocalisedStrings::translate("auto_ostinato"),
        // 新增
        LocalisedStrings::translate("tag_altered"),
        LocalisedStrings::translate("tag_half_diminished"),
        LocalisedStrings::translate("tag_power_chord")
    };

    // ========== 5. 曲式功能 ==========
    juce::StringArray funcTags = {
        LocalisedStrings::translate("auto_verse"),
        LocalisedStrings::translate("auto_chorus"),
        LocalisedStrings::translate("auto_bridge"),
        LocalisedStrings::translate("auto_intro"),
        LocalisedStrings::translate("auto_outro"),
        LocalisedStrings::translate("auto_interlude"),
        LocalisedStrings::translate("auto_transition"),
        LocalisedStrings::translate("auto_cadence"),
        LocalisedStrings::translate("auto_loop_progression"),
        LocalisedStrings::translate("auto_sequence")
    };

    // ========== 6. 中国风特别标签 ==========
    juce::StringArray chineseTags = {
        LocalisedStrings::translate("auto_gong_mode"),
        LocalisedStrings::translate("auto_yu_mode"),
        LocalisedStrings::translate("auto_pentatonic"),
        LocalisedStrings::translate("auto_hexatonic"),
        LocalisedStrings::translate("auto_heptatonic"),
        LocalisedStrings::translate("auto_yayue"),
        LocalisedStrings::translate("auto_yanyue"),
        LocalisedStrings::translate("auto_qingyue"),
        LocalisedStrings::translate("auto_ancient_style"),
        LocalisedStrings::translate("auto_chinese_opera"),
        LocalisedStrings::translate("auto_peking_opera"),
        LocalisedStrings::translate("auto_kunqu"),
        LocalisedStrings::translate("auto_huangmei_opera"),
        LocalisedStrings::translate("auto_northern_shaanxi_folk_song"),
        LocalisedStrings::translate("auto_jiangnan_sizhu"),
        LocalisedStrings::translate("auto_guangdong_music"),
        LocalisedStrings::translate("auto_suzhou_pingtan"),
        LocalisedStrings::translate("auto_jingyun_dagu"),
        LocalisedStrings::translate("auto_chinese_rock"),
        LocalisedStrings::translate("auto_northwest_wind"),
        LocalisedStrings::translate("auto_mongolian_long_song"),
        LocalisedStrings::translate("auto_tibetan"),
        LocalisedStrings::translate("auto_xinjiang_folk_song"),
        LocalisedStrings::translate("auto_taiwan_folk_song"),
        LocalisedStrings::translate("auto_hong_kong_pop")
    };

    juce::Array<juce::StringArray> result;
    result.add(modeTags);
    result.add(regionTags);
    result.add(moodTags);
    result.add(complexTags);
    result.add(funcTags);
    result.add(chineseTags);
    return result;
}