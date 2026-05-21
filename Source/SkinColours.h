//==================================================================================================
//  SkinColours.h
//  项目：ChordReference (和弦调性速查器)
//  功能：四套皮肤主题的集中配色定义
//        主题：现代紫 (FluidPurpleOrange)、未来蓝 (FluidLightShadow)、
//              象牙米 (IvorySatin)、尊黑金 (RoyalBlackGold)
//        每个主题包含：背景、信息面板、顶部栏、和弦卡片、
//                      迷你键盘、五度圈、预设浏览器的完整配色
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once

#include <juce_graphics/juce_graphics.h>
#include "MusicTypes.h"
#include <functional>

//==============================================================================
// 皮肤主题结构体（包含所有 UI 元素的配色方案）
//==============================================================================
struct SkinTheme
{
    std::function<juce::ColourGradient(float, float)> backgroundGradient;

    juce::Colour infoPanelBackground;
    juce::Colour infoPanelAccent;
    juce::Colour infoTextColour;

    juce::Colour topBarTextCol;
    juce::Colour topBarSubTextCol;
    juce::Colour topBarSeparatorCol;
    juce::Colour topBarButtonTextCol;
    juce::Colour topBarButtonBgCol;
    juce::Colour topBarToggleTextCol;
    juce::Colour topBarToggleTickCol;
    juce::Colour topBarComboTextCol;
    juce::Colour topBarComboBgCol;

    struct CardTheme
    {
        juce::Colour background;
        float moodBlendAlpha = 0.08f;
        juce::Colour borderColour;
        float borderThickness = 1.5f;
        juce::Colour leftBarT, leftBarS, leftBarD;
        juce::Colour mainTextCol;
        juce::Colour subTextCol;
        juce::Colour moodTextCol;
        juce::Colour buttonActiveFill;
        juce::Colour buttonInactiveFill;
        juce::Colour buttonInactiveText;
        juce::Colour buttonActiveText = juce::Colours::white;
        bool useShadow = false;
        juce::Colour shadowColour;
        int shadowRadius = 0;
        juce::Point<int> shadowOffset;
    } card;

    struct KeyboardTheme
    {
        juce::Colour whiteKeyBase;
        juce::Colour whiteKeyFrame;
        juce::Colour whiteKeyHighlightFill;
        juce::Colour whiteKeyHighlightBorder;
        juce::Colour blackKeyBase;
        juce::Colour blackKeyFrame;
        juce::Colour blackKeyHighlightFill;
        juce::Colour blackKeyHighlightBorder;
        juce::Colour noteText;
        juce::Colour c4MarkerColour;
    } keyboard;

    struct CircleTheme
    {
        juce::Colour majorColour;
        juce::Colour minorColour;
        juce::Colour outerRingLine;
        juce::Colour innerRingLine;
        juce::Colour textColour;
        juce::Colour inactiveText;
        juce::Colour highlight;
        juce::Colour keyBorderColour;
        juce::Colour highlightBorder; // 高亮描边
        juce::Colour modalHighlight;  // Modal 模式高亮
    } circle;

    struct ProgressionBrowserTheme
    {
        juce::Colour background;
        juce::Colour titleTextColour;
        juce::Colour tagButtonBg;
        juce::Colour tagButtonText;
        juce::Colour tagButtonBgActive;
        juce::Colour tagButtonTextActive;
        juce::Colour listBackground;
        juce::Colour listItemText;
        juce::Colour listItemSubText;
        juce::Colour listItemHighlight;
        juce::Colour bottomBarBackground;
        juce::Colour bottomBarText;
        juce::Colour buttonBg;
        juce::Colour buttonText;
        juce::Colour borderColour;
    } progressionBrowser;
};

//==============================================================================
// 各皮肤主题独立构建函数
//==============================================================================

static SkinTheme createFluidPurpleOrangeTheme()
{
    SkinTheme t;
    auto accent = juce::Colour(0xFFB06CFF);
    auto accentHot = juce::Colour(0xFFFF66CC);
    auto bgBase = juce::Colour(0xFF12081F);
    auto bgMid  = juce::Colour(0xFF1B1035);
    auto panelBg = juce::Colour(0x8825163F);
    auto cardBg = juce::Colour(0x882B184D);
    auto white = juce::Colours::white;

    t.backgroundGradient = [bgBase, bgMid](float w, float h) {
        juce::ColourGradient g(bgBase, 0, 0, bgMid, w, h, false);
        g.addColour(0.45, juce::Colour(0xFF41206B));
        g.addColour(0.75, juce::Colour(0xFF6A2FA8));
        return g;
    };
    t.infoPanelBackground = panelBg.withAlpha(0.92f);
    t.infoPanelAccent = accent.withAlpha(0.5f);
    t.infoTextColour = white.withAlpha(0.9f);
    t.topBarTextCol = white;
    t.topBarSubTextCol = white.withAlpha(0.65f);
    t.topBarSeparatorCol = white.withAlpha(0.1f);
    t.topBarButtonTextCol = white;
    t.topBarButtonBgCol = juce::Colour(0x33FFFFFF);
    t.topBarToggleTextCol = white;
    t.topBarToggleTickCol = accent;
    t.topBarComboTextCol = white;
    t.topBarComboBgCol = juce::Colour(0x22FFFFFF);

    t.card.background = cardBg;
    t.card.borderColour = accent.withAlpha(0.18f);
    t.card.borderThickness = 1.0f;
    t.card.leftBarT = accent.withAlpha(0.35f);
    t.card.leftBarS = accent.withAlpha(0.55f);
    t.card.leftBarD = accentHot.withAlpha(0.7f);
    t.card.mainTextCol = white;
    t.card.subTextCol = white.withAlpha(0.55f);
    t.card.moodTextCol = juce::Colour::fromRGB(196, 181, 253);
    t.card.buttonActiveFill = accentHot.withMultipliedSaturation(0.7f);
    t.card.buttonInactiveFill = juce::Colour(0x22FFFFFF);
    t.card.buttonInactiveText = white.withAlpha(0.65f);
    t.card.useShadow = true;
    t.card.shadowColour = juce::Colour(0x22000000);
    t.card.shadowRadius = 10;
    t.card.shadowOffset = juce::Point<int>(0, 4);

    t.keyboard.whiteKeyBase = juce::Colour(0x05FFFFFF);
    t.keyboard.whiteKeyFrame = accent.withAlpha(0.08f);
    t.keyboard.whiteKeyHighlightFill = juce::Colour(0xFF793CBE);
    t.keyboard.blackKeyHighlightFill = juce::Colour(0xFF6C2DB4);
    t.keyboard.blackKeyBase = juce::Colour(0xFF382951);
    t.keyboard.whiteKeyHighlightBorder = accent.withAlpha(0.35f);
    t.keyboard.blackKeyFrame = accent.withAlpha(0.2f);
    t.keyboard.blackKeyHighlightBorder = accent.withAlpha(0.4f);
    t.keyboard.noteText = white.withAlpha(0.55f);
    t.keyboard.c4MarkerColour = accent.withAlpha(0.4f);

    t.circle.majorColour = accent.withAlpha(0.12f);
    t.circle.minorColour = accent.withAlpha(0.08f);
    t.circle.outerRingLine = accent.withAlpha(0.2f);
    t.circle.innerRingLine = accent.withAlpha(0.15f);
    t.circle.textColour = white;
    t.circle.inactiveText = white.withAlpha(0.35f);
    t.circle.highlight = accent.withAlpha(0.3f);
    t.circle.highlightBorder = accent.withAlpha(0.25f);
    t.circle.keyBorderColour = accent.withAlpha(0.4f);
    t.circle.modalHighlight = accent.withMultipliedSaturation(0.6f).withAlpha(0.35f);

    t.progressionBrowser.background = juce::Colour(0xEE111827);
    t.progressionBrowser.titleTextColour = white;
    t.progressionBrowser.tagButtonBg = juce::Colour(0x33FFFFFF);
    t.progressionBrowser.tagButtonText = white.withAlpha(0.8f);
    t.progressionBrowser.tagButtonBgActive = accent;
    t.progressionBrowser.tagButtonTextActive = white;
    t.progressionBrowser.listBackground = juce::Colour(0xCC1A1F2E);
    t.progressionBrowser.listItemText = white;
    t.progressionBrowser.listItemSubText = white.withAlpha(0.55f);
    t.progressionBrowser.listItemHighlight = accent.withAlpha(0.45f);
    t.progressionBrowser.bottomBarBackground = juce::Colour(0xEE1A1F2E);
    t.progressionBrowser.bottomBarText = white.withAlpha(0.7f);
    t.progressionBrowser.buttonBg = accent;
    t.progressionBrowser.buttonText = white;
    t.progressionBrowser.borderColour = accent.withAlpha(0.22f);

    return t;
}

static SkinTheme createFluidLightShadowTheme()
{
    SkinTheme t;
    auto accent = juce::Colour(0xFF47A8FF);
    auto accentPurple = juce::Colour(0xFF7A5CFF);
    auto accentWarm = juce::Colour(0xFFFFA94D);
    auto bgTop = juce::Colour(0xFF071133);
    auto bgBottom = juce::Colour(0xFF0A2160);
    auto panel = juce::Colour(0x88102A52);
    auto white = juce::Colours::white;

    t.backgroundGradient = [bgTop, bgBottom](float w, float h) {
        juce::ColourGradient g(bgTop, 0, 0, bgBottom, w, h, false);
        g.addColour(0.35, juce::Colour(0xFF3A2FC8));
        g.addColour(0.65, juce::Colour(0xFF5F4DFF));
        g.addColour(0.88, juce::Colour(0x3329D8FF));
        return g;
    };
    t.infoPanelBackground = panel.withAlpha(0.85f);
    t.infoPanelAccent = accent.withAlpha(0.4f);
    t.infoTextColour = white.withAlpha(0.9f);
    t.topBarTextCol = white;
    t.topBarSubTextCol = white.withAlpha(0.65f);
    t.topBarSeparatorCol = white.withAlpha(0.08f);
    t.topBarButtonTextCol = white;
    t.topBarButtonBgCol = juce::Colour(0x33FFFFFF);
    t.topBarToggleTextCol = white;
    t.topBarToggleTickCol = accent;
    t.topBarComboTextCol = white;
    t.topBarComboBgCol = juce::Colour(0x22FFFFFF);

    t.card.background = panel;
    t.card.borderColour = accent.withAlpha(0.2f);
    t.card.borderThickness = 1.0f;
    t.card.leftBarT = accent.withAlpha(0.35f);
    t.card.leftBarS = accentPurple.withAlpha(0.55f);
    t.card.leftBarD = accentWarm.withAlpha(0.75f);
    t.card.mainTextCol = juce::Colour(0xFFEAF4FF);
    t.card.subTextCol = juce::Colour(0x99D8EAFF);
    t.card.moodTextCol = juce::Colour::fromRGB(14, 165, 233);
    t.card.buttonActiveFill = accentPurple.withMultipliedSaturation(0.75f);
    t.card.buttonInactiveFill = juce::Colour(0x22FFFFFF);
    t.card.buttonInactiveText = white.withAlpha(0.65f);
    t.card.useShadow = true;
    t.card.shadowColour = juce::Colour(0x33000000);
    t.card.shadowRadius = 12;
    t.card.shadowOffset = juce::Point<int>(0, 4);

    t.keyboard.whiteKeyBase = juce::Colour(0x05FFFFFF);
    t.keyboard.whiteKeyFrame = accent.withAlpha(0.08f);
    t.keyboard.whiteKeyHighlightFill = juce::Colour(0xFF568DFB);
    t.keyboard.blackKeyHighlightFill = juce::Colour(0xFF4C7CDD);
    t.keyboard.blackKeyBase = juce::Colour(0xFF192D7D);
    t.keyboard.whiteKeyHighlightBorder = accent.withAlpha(0.3f);
    t.keyboard.blackKeyFrame = accent.withAlpha(0.15f);
    t.keyboard.blackKeyHighlightBorder = juce::Colour(0xFF568DFB).withAlpha(0.35f);
    t.keyboard.noteText = white.withAlpha(0.5f);
    t.keyboard.c4MarkerColour = accent.withAlpha(0.35f);

    t.circle.majorColour = accent.withAlpha(0.12f);
    t.circle.minorColour = accent.withAlpha(0.08f);
    t.circle.outerRingLine = accent.withAlpha(0.25f);
    t.circle.innerRingLine = accent.withAlpha(0.18f);
    t.circle.textColour = white;
    t.circle.inactiveText = white.withAlpha(0.35f);
    t.circle.highlight = accent.withAlpha(0.3f);
    t.circle.highlightBorder = accent.withAlpha(0.2f);
    t.circle.keyBorderColour = accent.withAlpha(0.45f);
    t.circle.modalHighlight = accent.withMultipliedSaturation(0.6f).withAlpha(0.35f);

    t.progressionBrowser.background = juce::Colour(0xEE111827);
    t.progressionBrowser.titleTextColour = white;
    t.progressionBrowser.tagButtonBg = juce::Colour(0x33FFFFFF);
    t.progressionBrowser.tagButtonText = white.withAlpha(0.8f);
    t.progressionBrowser.tagButtonBgActive = accent;
    t.progressionBrowser.tagButtonTextActive = white;
    t.progressionBrowser.listBackground = juce::Colour(0xCC1A1F2E);
    t.progressionBrowser.listItemText = white;
    t.progressionBrowser.listItemSubText = white.withAlpha(0.55f);
    t.progressionBrowser.listItemHighlight = accent.withAlpha(0.45f);
    t.progressionBrowser.bottomBarBackground = juce::Colour(0xEE1A1F2E);
    t.progressionBrowser.bottomBarText = white.withAlpha(0.7f);
    t.progressionBrowser.buttonBg = accent;
    t.progressionBrowser.buttonText = white;
    t.progressionBrowser.borderColour = accent.withAlpha(0.22f);

    return t;
}

static SkinTheme createIvorySatinTheme()
{
    SkinTheme t;
    auto accent = juce::Colour(0xFFC5A46D);
    auto bg = juce::Colour(0xFFF5F1E8);
    auto bg2 = juce::Colour(0xFFEDE5D8);
    auto panel = juce::Colour(0xE0FFFDF8);
    auto textCol = juce::Colour(0xFF4A3A2A);
    auto white = juce::Colours::white;

    t.backgroundGradient = [bg, bg2](float w, float h) {
        juce::ColourGradient g(bg, 0, 0, bg2, w, h, false);
        g.addColour(0.7, juce::Colour(0xFFF7EFE0));
        return g;
    };
    t.infoPanelBackground = bg.withAlpha(0.85f);
    t.infoPanelAccent = accent.withAlpha(0.5f);
    t.infoTextColour = textCol;
    t.topBarTextCol = textCol;
    t.topBarSubTextCol = textCol.withAlpha(0.6f);
    t.topBarSeparatorCol = textCol.withAlpha(0.12f);
    t.topBarButtonTextCol = textCol;
    t.topBarButtonBgCol = juce::Colour(0x33FFFFFF);
    t.topBarToggleTextCol = textCol;
    t.topBarToggleTickCol = accent;
    t.topBarComboTextCol = textCol;
    t.topBarComboBgCol = juce::Colour(0x22FFFFFF);

    t.card.background = panel;
    t.card.borderColour = accent.withAlpha(0.15f);
    t.card.borderThickness = 1.0f;
    t.card.leftBarT = accent.withAlpha(0.5f);
    t.card.leftBarS = accent.withAlpha(0.65f);
    t.card.leftBarD = juce::Colour::fromRGB(255, 160, 0).withAlpha(0.65f);
    t.card.mainTextCol = textCol;
    t.card.subTextCol = textCol.withAlpha(0.55f);
    t.card.moodTextCol = textCol;
    t.card.buttonActiveFill = juce::Colour(0xFFD8B57A).withMultipliedSaturation(0.9f);
    t.card.buttonInactiveFill = juce::Colour(0x11000000);
    t.card.buttonInactiveText = textCol.withAlpha(0.65f);
    t.card.buttonActiveText = white;
    t.card.useShadow = true;
    t.card.shadowColour = juce::Colour(0x33200000);
    t.card.shadowRadius = 8;
    t.card.shadowOffset = juce::Point<int>(0, 2);

    t.keyboard.whiteKeyBase = juce::Colour(0x24FFFFFF);
    t.keyboard.whiteKeyFrame = accent.withAlpha(0.18f);
    t.keyboard.whiteKeyHighlightFill = juce::Colour(0xFFE3D4C0);
    t.keyboard.blackKeyHighlightFill = juce::Colour(0xFFBCAB95);
    t.keyboard.blackKeyBase = juce::Colour(0xFF423D3A);
    t.keyboard.whiteKeyHighlightBorder = juce::Colour(0xFFC5A46D).withAlpha(0.35f);
    t.keyboard.blackKeyFrame = accent.withAlpha(0.25f);
    t.keyboard.blackKeyHighlightBorder = juce::Colour(0xFFE3D4C0).withAlpha(0.45f);
    t.keyboard.noteText = textCol.withAlpha(0.55f);
    t.keyboard.c4MarkerColour = accent.withAlpha(0.4f);

    t.circle.majorColour = accent.withAlpha(0.15f);
    t.circle.minorColour = accent.withAlpha(0.1f);
    t.circle.outerRingLine = accent.withAlpha(0.25f);
    t.circle.innerRingLine = accent.withAlpha(0.18f);
    t.circle.textColour = textCol;
    t.circle.inactiveText = textCol.withAlpha(0.25f);
    t.circle.highlight = juce::Colour::fromRGB(218, 165, 32).withAlpha(0.35f);
    t.circle.highlightBorder = accent.withAlpha(0.25f);
    t.circle.keyBorderColour = accent.withAlpha(0.45f);
    t.circle.modalHighlight = accent.withMultipliedSaturation(0.6f).withAlpha(0.35f);

    t.progressionBrowser.background = juce::Colour(0xEE111827);
    t.progressionBrowser.titleTextColour = white;
    t.progressionBrowser.tagButtonBg = juce::Colour(0x33FFFFFF);
    t.progressionBrowser.tagButtonText = white.withAlpha(0.8f);
    t.progressionBrowser.tagButtonBgActive = accent;
    t.progressionBrowser.tagButtonTextActive = white;
    t.progressionBrowser.listBackground = juce::Colour(0xCC1A1F2E);
    t.progressionBrowser.listItemText = white;
    t.progressionBrowser.listItemSubText = white.withAlpha(0.55f);
    t.progressionBrowser.listItemHighlight = accent.withAlpha(0.45f);
    t.progressionBrowser.bottomBarBackground = juce::Colour(0xEE1A1F2E);
    t.progressionBrowser.bottomBarText = white.withAlpha(0.7f);
    t.progressionBrowser.buttonBg = accent;
    t.progressionBrowser.buttonText = white;
    t.progressionBrowser.borderColour = accent.withAlpha(0.22f);

    return t;
}

static SkinTheme createRoyalBlackGoldTheme()
{
    SkinTheme t;
    auto gold = juce::Colour(0xFFD6B36A);
    auto goldSoft = juce::Colour(0xFF8C6A2D);
    auto bg = juce::Colour(0xFF050505);
    auto bg2 = juce::Colour(0xFF0C0C0F);
    auto panel = juce::Colour(0xAA101014);
    auto white = juce::Colours::white;

    t.backgroundGradient = [bg, bg2](float w, float h) {
        juce::ColourGradient g(bg, 0, 0, bg2, w, h, false);
        g.addColour(0.8, juce::Colour(0xFF17171C));
        return g;
    };
    t.infoPanelBackground = bg.withAlpha(0.85f);
    t.infoPanelAccent = gold.withAlpha(0.5f);
    t.infoTextColour = gold;
    t.topBarTextCol = gold;
    t.topBarSubTextCol = gold.withAlpha(0.6f);
    t.topBarSeparatorCol = gold.withAlpha(0.1f);
    t.topBarButtonTextCol = gold;
    t.topBarButtonBgCol = juce::Colour(0x33FFFFFF);
    t.topBarToggleTextCol = gold;
    t.topBarToggleTickCol = gold;
    t.topBarComboTextCol = gold;
    t.topBarComboBgCol = juce::Colour(0x22FFFFFF);

    t.card.background = panel;
    t.card.borderColour = gold.withAlpha(0.18f);
    t.card.borderThickness = 1.2f;
    t.card.leftBarT = gold.withAlpha(0.75f);
    t.card.leftBarS = juce::Colour::fromRGB(205, 127, 50).withAlpha(0.75f);
    t.card.leftBarD = juce::Colour::fromRGB(255, 69, 0).withAlpha(0.75f);
    t.card.mainTextCol = gold;
    t.card.subTextCol = gold.withAlpha(0.55f);
    t.card.moodTextCol = juce::Colour::fromRGB(180, 140, 40);
    t.card.buttonActiveFill = goldSoft.withMultipliedSaturation(0.8f);
    t.card.buttonInactiveFill = juce::Colour(0x22FFFFFF);
    t.card.buttonInactiveText = gold.withAlpha(0.7f);
    t.card.buttonActiveText = juce::Colours::black;
    t.card.useShadow = true;
    t.card.shadowColour = juce::Colour(0x44000000);
    t.card.shadowRadius = 14;
    t.card.shadowOffset = juce::Point<int>(0, 4);

    t.keyboard.whiteKeyBase = juce::Colour(0x05000000);
    t.keyboard.whiteKeyFrame = gold.withAlpha(0.08f);
    t.keyboard.whiteKeyHighlightFill = juce::Colour(0xFF9A8566);
    t.keyboard.blackKeyHighlightFill = juce::Colour(0xFF756852);
    t.keyboard.blackKeyBase = juce::Colour(0xFF191917);
    t.keyboard.whiteKeyHighlightBorder = gold.withAlpha(0.28f);
    t.keyboard.blackKeyFrame = gold.withAlpha(0.15f);
    t.keyboard.blackKeyHighlightBorder = gold.withAlpha(0.35f);
    t.keyboard.noteText = gold.withAlpha(0.5f);
    t.keyboard.c4MarkerColour = gold.withAlpha(0.35f);

    t.circle.majorColour = gold.withAlpha(0.12f);
    t.circle.minorColour = gold.withAlpha(0.07f);
    t.circle.outerRingLine = gold.withAlpha(0.25f);
    t.circle.innerRingLine = gold.withAlpha(0.18f);
    t.circle.textColour = gold;
    t.circle.inactiveText = white.withAlpha(0.25f);
    t.circle.highlight = gold.withAlpha(0.3f);
    t.circle.highlightBorder = gold.withAlpha(0.2f);
    t.circle.keyBorderColour = gold.withAlpha(0.45f);
    t.circle.modalHighlight = gold.withMultipliedSaturation(0.6f).withAlpha(0.35f);

    t.progressionBrowser.background = juce::Colour(0xEE111827);
    t.progressionBrowser.titleTextColour = white;
    t.progressionBrowser.tagButtonBg = juce::Colour(0x33FFFFFF);
    t.progressionBrowser.tagButtonText = white.withAlpha(0.8f);
    t.progressionBrowser.tagButtonBgActive = gold;
    t.progressionBrowser.tagButtonTextActive = white;
    t.progressionBrowser.listBackground = juce::Colour(0xCC1A1F2E);
    t.progressionBrowser.listItemText = white;
    t.progressionBrowser.listItemSubText = white.withAlpha(0.55f);
    t.progressionBrowser.listItemHighlight = gold.withAlpha(0.45f);
    t.progressionBrowser.bottomBarBackground = juce::Colour(0xEE1A1F2E);
    t.progressionBrowser.bottomBarText = white.withAlpha(0.7f);
    t.progressionBrowser.buttonBg = gold;
    t.progressionBrowser.buttonText = juce::Colours::black;
    t.progressionBrowser.borderColour = gold.withAlpha(0.22f);

    return t;
}

//==============================================================================
// 根据 SkinType 获取对应的皮肤主题引用
//==============================================================================
inline const SkinTheme& getSkinTheme(SkinType type)
{
    static const SkinTheme fluidPurpleOrange = createFluidPurpleOrangeTheme();
    static const SkinTheme fluidLightShadow  = createFluidLightShadowTheme();
    static const SkinTheme ivorySatin        = createIvorySatinTheme();
    static const SkinTheme royalBlackGold    = createRoyalBlackGoldTheme();

    switch (type)
    {
    case SkinType::FluidPurpleOrange: return fluidPurpleOrange;
    case SkinType::FluidLightShadow:  return fluidLightShadow;
    case SkinType::IvorySatin:        return ivorySatin;
    case SkinType::RoyalBlackGold:    return royalBlackGold;
    default:                          return fluidPurpleOrange;
    }
}