//==================================================================================================
//  CircleOfFifths.cpp
//  项目：ChordReference (和弦调性速查器)
//  功能：五度圈组件实现
//        - 扇形切片风格大小调内外双圈
//        - 极坐标点击检测
//        - 中心骰子动画与风格选择菜单
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#include "CircleOfFifths.h"
#include "LocalisedStrings.h"
#include "ScaleEngine.h"
#include "PitchNameHelper.h"
#include "SkinColours.h"

//==============================================================================
// 构造函数：按五度顺序初始化大调按钮
//==============================================================================
CircleOfFifths::CircleOfFifths()
{
    static const PitchClass order[] = {
        PitchClass::C, PitchClass::G, PitchClass::D, PitchClass::A,
        PitchClass::E, PitchClass::B, PitchClass::F_Sharp,
        PitchClass::C_Sharp, PitchClass::A_Flat, PitchClass::E_Flat,
        PitchClass::B_Flat, PitchClass::F
    };
    for (int i = 0; i < 12; ++i)
    {
        KeyButton btn;
        btn.tonic = order[i];
        // 0 度 = 3 点方向，-halfPi 调整至 12 点方向（C 音）
        btn.angle = i * (juce::MathConstants<float>::twoPi / 12.0f)
                    - juce::MathConstants<float>::halfPi;
        majorKeys.push_back(btn);
    }
}

//==============================================================================
// 尺寸变化时重算布局
//==============================================================================
void CircleOfFifths::resized()
{
    auto currentBounds = getLocalBounds();
    if (currentBounds != lastBounds)
    {
        lastBounds = currentBounds;
        calculateLayout();
    }
}

//==============================================================================
// 计算小调数据（角度与大调严格对齐）
//==============================================================================
void CircleOfFifths::calculateLayout()
{
    minorKeys.clear();
    for (int i = 0; i < 12; ++i)
    {
        KeyButton mBtn;
        mBtn.tonic = ScaleEngine::getRelativeMinor(majorKeys[i].tonic);
        mBtn.angle = majorKeys[i].angle;            // 与大调角度严格一致
        minorKeys.push_back(mBtn);
    }
}

//==============================================================================
// 绘制：内外扇形、文字、中心骰子
//==============================================================================
void CircleOfFifths::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    float minSide = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    float outerRadius  = minSide * 0.48f;           // 大调外边缘
    float middleRadius = outerRadius * 0.65f;       // 大调内边缘 / 小调外边缘
    float innerRadius  = outerRadius * 0.35f;       // 小调内边缘 / 中心留空

    float lineThickness = juce::jlimit(0.8f, 2.5f, outerRadius * 0.01f);

    auto& theme = getSkinTheme(currentSkin);
    const auto& circ = theme.circle;

    float majorFontHeight = juce::jlimit(11.0f, 24.0f, outerRadius * 0.12f);
    float minorFontHeight = juce::jlimit(9.0f, 20.0f, outerRadius * 0.10f);

    float angleStep = juce::MathConstants<float>::twoPi / 12.0f;

    //---- 辅助：亮度微调 ----
    auto adjustBrightness = [](juce::Colour col, float factor) -> juce::Colour {
        return col.withMultipliedBrightness(factor);
    };

    //---- 绘制 12 个大调/小调扇区 ----
    for (int i = 0; i < 12; ++i)
    {
        auto& majBtn = majorKeys[i];
        auto& minBtn = minorKeys[i];

        float startAngleMath = majBtn.angle - angleStep / 2.0f;
        float endAngleMath   = majBtn.angle + angleStep / 2.0f;
        float startPieAngle = startAngleMath + juce::MathConstants<float>::halfPi;
        float endPieAngle   = endAngleMath + juce::MathConstants<float>::halfPi;

        bool isAlt = (i % 2 != 0);
        float brightnessAdjust = isAlt ? 1.05f : 1.0f;

        // -- 大调外圈 --
        juce::Path majorSector;
        majorSector.addPieSegment(center.x - outerRadius, center.y - outerRadius,
                                  outerRadius * 2, outerRadius * 2,
                                  startPieAngle, endPieAngle, middleRadius / outerRadius);
        bool isStrictMajor = (currentType == ScaleType::Major);
        bool isStrictMinor = (currentType == ScaleType::NaturalMinor);
        bool majHighlight = false;
        // ---- 大调外圈高亮 ----
        juce::Colour majFill;
        if (isStrictMajor)
        {
            majHighlight = (majBtn.tonic == currentTonic);
            majFill = majHighlight ? circ.highlight : adjustBrightness(circ.majorColour, brightnessAdjust);
                }
        else if (isStrictMinor)
        {
            majFill = adjustBrightness(circ.majorColour, brightnessAdjust);
        }
        else // Modal
        {
            majHighlight = (majBtn.tonic == currentTonic);
            majFill = majHighlight ? circ.modalHighlight : adjustBrightness(circ.majorColour, brightnessAdjust);
        }
        g.setColour(majFill);
        g.fillPath(majorSector);
        if (majHighlight) {
            g.setColour(circ.highlightBorder);  // 替代原来的 circ.highlight.brighter(0.3f)
            g.strokePath(majorSector, juce::PathStrokeType(lineThickness * 1.8f));
        }
        g.setColour(circ.keyBorderColour);
        g.strokePath(majorSector, juce::PathStrokeType(lineThickness));
        // 大调文字
        g.setColour(circ.textColour);
        g.setFont(majorFontHeight);
        juce::Point<float> majTextPos = getPosition(majBtn.angle,
            (outerRadius + middleRadius) / 2.0f, center);
        juce::Rectangle<float> majTextBounds(0, 0, outerRadius * 0.4f, majorFontHeight * 2.0f);
        majTextBounds.setCentre(majTextPos);
        g.drawText(PitchNameHelper::getNameForPitch(majBtn.tonic,
                   PitchNameHelper::isSharpKeyForTonic(majBtn.tonic)),
                   majTextBounds, juce::Justification::centred);

        // -- 小调内圈 --
        juce::Path minorSector;
        minorSector.addPieSegment(center.x - middleRadius, center.y - middleRadius,
                                  middleRadius * 2, middleRadius * 2,
                                  startPieAngle, endPieAngle, innerRadius / middleRadius);

        // ---- 绘制小调扇区 ----
        bool minHighlight = false;
        juce::Colour minFill;
        if (isStrictMinor)
        {
            minHighlight = (minBtn.tonic == currentTonic);
            minFill = minHighlight ? circ.highlight : adjustBrightness(circ.minorColour, brightnessAdjust);
        }
        else if (isStrictMajor)
        {
            minFill = adjustBrightness(circ.minorColour, brightnessAdjust);
        }
        else // Modal
        {
            minHighlight = (minBtn.tonic == currentTonic);
            minFill = minHighlight ? circ.modalHighlight : adjustBrightness(circ.minorColour, brightnessAdjust);
        }
        g.setColour(minFill);
        g.fillPath(minorSector);
        if (minHighlight) {
            g.setColour(circ.highlightBorder);
            g.strokePath(minorSector, juce::PathStrokeType(lineThickness * 1.8f));
        }
        g.setColour(circ.keyBorderColour);
        g.strokePath(minorSector, juce::PathStrokeType(lineThickness));

        // 小调文字
        g.setColour(circ.textColour);
        g.setFont(minorFontHeight);
        juce::Point<float> minTextPos = getPosition(minBtn.angle,
            (middleRadius + innerRadius) / 2.0f, center);
        juce::Rectangle<float> minTextBounds(0, 0, middleRadius * 0.4f, minorFontHeight * 2.0f);
        minTextBounds.setCentre(minTextPos);
        bool sharpKey = PitchNameHelper::isSharpKeyForTonic(
            ScaleEngine::getRelativeMajor(minBtn.tonic));
        g.drawText(PitchNameHelper::getNameForPitch(minBtn.tonic, sharpKey),
                   minTextBounds, juce::Justification::centred);
    }

    //---- 中心骰子 ----
    float diceSize = innerRadius * 1.25f;
    juce::Rectangle<float> diceBounds(diceSize, diceSize);
    diceBounds.setCentre(center);

    if (diceIsRolling)
    {
        g.saveState();
        g.addTransform(juce::AffineTransform::rotation(diceRotationAngle, center.x, center.y));
    }

    g.setColour(circ.majorColour.withAlpha(0.4f));
    g.fillRoundedRectangle(diceBounds, diceBounds.getWidth() * 0.2f);
    g.setColour(circ.keyBorderColour);
    g.drawRoundedRectangle(diceBounds, diceBounds.getWidth() * 0.2f, lineThickness);

    // 骰子上的三个点
    float dotSize = diceBounds.getWidth() * 0.12f;
    g.setColour(circ.textColour);
    auto drawDot = [&](float relX, float relY)
    {
        juce::Rectangle<float> dot(0, 0, dotSize, dotSize);
        dot.setCentre(diceBounds.getX() + diceBounds.getWidth() * relX,
                      diceBounds.getY() + diceBounds.getHeight() * relY);
        g.fillEllipse(dot);
    };
    drawDot(0.25f, 0.25f);
    drawDot(0.5f,  0.5f);
    drawDot(0.75f, 0.75f);

    if (diceIsRolling)
        g.restoreState();
}

//==============================================================================
// 鼠标点击：极坐标判定扇区 / 骰子
//==============================================================================
void CircleOfFifths::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.position;
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    float minSide = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    float outerRadius  = minSide * 0.48f;
    float middleRadius = outerRadius * 0.65f;
    float innerRadius  = outerRadius * 0.35f;

    float dx = pos.x - center.x;
    float dy = pos.y - center.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    //---- 中心骰子区域 ----
    if (dist <= innerRadius)
    {
        if (e.mods.isRightButtonDown())
        {
            juce::PopupMenu styleMenu;
            styleMenu.addItem(LocalisedStrings::translate("auto_general_classical"), [this] { if (onStyleChanged) onStyleChanged(0); });
            styleMenu.addItem(LocalisedStrings::translate("auto_pop_rock"), [this] { if (onStyleChanged) onStyleChanged(1); });
            styleMenu.addItem(LocalisedStrings::translate("auto_jazz_chromatic"), [this] { if (onStyleChanged) onStyleChanged(2); });
            styleMenu.addItem(LocalisedStrings::translate("auto_chinese_style"), [this] { if (onStyleChanged) onStyleChanged(3); });
            
            float diceSize = innerRadius * 1.25f;
            auto diceScreenBounds = juce::Rectangle<int>(
                getScreenX() + (int)(center.x - diceSize/2),
                getScreenY() + (int)(center.y - diceSize/2),
                (int)diceSize, (int)diceSize);
            styleMenu.showMenuAsync(juce::PopupMenu::Options().withTargetScreenArea(diceScreenBounds));
            return;
        }
        diceRotationAngle = 0.0f;
        diceIsRolling = true;
        generateButtonFlashed = true;
        startTimerHz(60);
        if (onGenerateRequested) onGenerateRequested();
        return;
    }

    //---- 外圈（大调）或内圈（小调）扇区 ----
    if (dist > innerRadius && dist <= outerRadius)
    {
        float clickAngle = std::atan2(dy, dx);
        
        int bestIndex = 0;
        float minAngleDiff = juce::MathConstants<float>::twoPi;
        
        for (int i = 0; i < 12; ++i)
        {
            float diff = std::fmod(std::abs(majorKeys[i].angle - clickAngle),
                                   juce::MathConstants<float>::twoPi);
            if (diff > juce::MathConstants<float>::pi)
                diff = juce::MathConstants<float>::twoPi - diff;
                
            if (diff < minAngleDiff)
            {
                minAngleDiff = diff;
                bestIndex = i;
            }
        }

        if (dist > middleRadius)
        {
            // 外圈点击
            if (currentType == ScaleType::Major || currentType == ScaleType::NaturalMinor)
                onKeySelected(majorKeys[bestIndex].tonic, ScaleType::Major);
            else
                onKeySelected(majorKeys[bestIndex].tonic, currentType);   // Modal：保持当前调式
        }
        else
        {
            // 内圈点击
            if (currentType == ScaleType::Major || currentType == ScaleType::NaturalMinor)
                onKeySelected(minorKeys[bestIndex].tonic, ScaleType::NaturalMinor);
            else
                onKeySelected(minorKeys[bestIndex].tonic, currentType);   // Modal：保持当前调式
        }
    }
}

//==============================================================================
// 设置当前调性（高亮对应扇区）
//==============================================================================
void CircleOfFifths::setCurrentKey(PitchClass tonic, ScaleType type)
{
    currentTonic = tonic;
    currentType = type;
    repaint();
}

//==============================================================================
// 极坐标转笛卡尔坐标
//==============================================================================
juce::Point<float> CircleOfFifths::getPosition(float angle, float radius,
                                               juce::Point<float> center) const
{
    return { center.x + radius * std::cos(angle), center.y + radius * std::sin(angle) };
}