//==================================================================================================
//  SmartGenerator.h
//  项目：ChordReference (和弦调性速查器)
//  功能：智能 TSD 进行生成器
//        - 基于概率权重随机生成功能序列（T/S/D/vi）
//        - 支持四种风格：通用/古典、流行/摇滚、爵士/离调、古风/中国风
//        - 首尾倾向 T，中间按转移概率选择
//        - 可附加七和弦后缀（T7等）
//  注意：需保存为 UTF-8 with BOM
//==================================================================================================

#pragma once
#include <juce_core/juce_core.h>
#include <map>
#include <random>

class SmartGenerator
{
public:
    enum class Style
    {
        General,    // 通用/古典
        Pop,        // 流行/摇滚
        Jazz,       // 爵士/离调
        Ancient     // 古风/中国风
    };

    // 生成功能序列（返回 3 个功能名，如 {"T", "S", "T"}）
    static juce::StringArray generate(Style style = Style::General, bool allowSeventh = false)
    {
        juce::StringArray functions;

        // 首位：60% 概率强制为主和弦 T
        juce::String firstFunc;
        if (randomInt(0, 9) < 6)
            firstFunc = "T";
        else
        {
            static const juce::StringArray starts = {"T", "D", "S"};
            firstFunc = starts[randomInt(0, 2)];
        }
        functions.add(firstFunc);
        juce::String current = firstFunc;

        // 生成第二、三位
        for (int i = 1; i < 3; ++i)
        {
            juce::String func = pickNext(current, style);

            if (allowSeventh && (func == "T" || func == "S"))
                func += "7";
            if (allowSeventh && func == "vi")
                func += "7";

            functions.add(func);
            current = func;
        }

        // 末位：70% 概率强制为主和弦 T
        if (functions.size() >= 3 && randomInt(0, 9) < 7)
            functions.set(2, "T");

        return functions;
    }

private:
    // 根据当前功能和风格选择下一个功能
    static juce::String pickNext(const juce::String& current, Style style)
    {
        auto& rules = getRules(style);
        auto it = rules.find(current);
        if (it == rules.end())
            return "T";

        int totalWeight = 0;
        for (auto& [func, weight] : it->second)
            totalWeight += weight;

        int roll = randomInt(0, totalWeight - 1);
        int cumulative = 0;
        for (auto& [func, weight] : it->second)
        {
            cumulative += weight;
            if (roll < cumulative)
                return func;
        }

        return "T"; // 兜底
    }

    // 获取风格对应的转移概率表
    static std::map<juce::String, std::map<juce::String, int>>& getRules(Style style)
    {
        static std::map<juce::String, std::map<juce::String, int>> generalRules = {
            { "T",  { {"S", 60}, {"D", 40} } },
            { "S",  { {"T", 70}, {"D", 30} } },
            { "D",  { {"T", 90}, {"S", 10} } },
            { "vi", { {"S", 50}, {"D", 50} } }
        };

        static std::map<juce::String, std::map<juce::String, int>> popRules = {
            { "T",  { {"S", 40}, {"D", 40}, {"vi", 20} } },
            { "S",  { {"T", 50}, {"D", 40}, {"vi", 10} } },
            { "D",  { {"T", 70}, {"vi", 30} } },
            { "vi", { {"S", 50}, {"D", 50} } }
        };

        static std::map<juce::String, std::map<juce::String, int>> jazzRules = {
            { "T",  { {"D", 50}, {"vi", 30}, {"S", 20} } },
            { "S",  { {"T", 40}, {"vi", 40}, {"D", 20} } },
            { "D",  { {"T", 60}, {"vi", 40} } },
            { "vi", { {"S", 40}, {"D", 40}, {"T", 20} } }
        };

        static std::map<juce::String, std::map<juce::String, int>> ancientRules = {
            { "T",  { {"S", 70}, {"vi", 30} } },
            { "S",  { {"T", 50}, {"vi", 50} } },
            { "D",  { {"T", 100} } },
            { "vi", { {"S", 50}, {"D", 50} } }
        };

        switch (style)
        {
            case Style::Pop:    return popRules;
            case Style::Jazz:   return jazzRules;
            case Style::Ancient: return ancientRules;
            default:            return generalRules;
        }
    }

    // 线程安全的随机数生成器
    static int randomInt(int min, int max)
    {
        static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
};