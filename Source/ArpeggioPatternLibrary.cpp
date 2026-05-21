//==================================================================================================
//  ArpeggioPatternLibrary.cpp
//  Project: ChordReference
//  Purpose: Built-in arpeggio pattern catalogue implementation.
//  Note: Save as UTF-8 with BOM.
//==================================================================================================

#include "ArpeggioPatternLibrary.h"

namespace
{
    juce::String cn(const char* text)
    {
        return juce::String::fromUTF8(text);
    }

    ArpeggioStep step(double offset, double length, int tone, int octave = 0, int vel = 0, bool bass = false)
    {
        ArpeggioStep s;
        s.beatOffset = offset;
        s.durationBeats = length;
        s.chordToneIndex = tone;
        s.octaveOffset = octave;
        s.velocityOffset = vel;
        s.isBassAnchor = bass;
        return s;
    }
}

ArpeggioPattern ArpeggioPatternLibrary::makePattern(const juce::String& id,
                                                    const juce::String& nameCN,
                                                    const juce::String& nameEN,
                                                    ArpeggioDirection direction,
                                                    ArpeggioDensity density,
                                                    double cycleBeats,
                                                    std::initializer_list<ArpeggioStep> steps)
{
    ArpeggioPattern pattern;
    pattern.id = id;
    pattern.nameCN = nameCN;
    pattern.nameEN = nameEN;
    pattern.direction = direction;
    pattern.density = density;
    pattern.cycleBeats = cycleBeats;
    pattern.steps.assign(steps.begin(), steps.end());
    return pattern;
}

std::vector<ArpeggioPattern> ArpeggioPatternLibrary::getBuiltInPatterns()
{
    std::vector<ArpeggioPattern> patterns;

    auto whole = makePattern("whole_pad", cn(u8"全音符铺底"), "Whole-note Pad",
                             ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                             { step(0.0, 3.85, 0), step(0.0, 3.85, 1), step(0.0, 3.85, 2), step(0.0, 3.85, 3) });
    whole.styleTags.addArray({ "pop", "cinematic", "ballad" });
    whole.moodTags.addArray({ "warm", "calm", "sad" });
    whole.functionFit.addArray({ "T", "t" });
    whole.formFit.addArray({ "intro", "verse", "outro", "cadence" });
    whole.familyId = "PadFamily";
    whole.grooveAffinity = "Straight";
    whole.complexity = 1;
    whole.energy = 1;
    whole.pedalFriendly = true;
    patterns.push_back(whole);

    auto up = makePattern("up_8", cn(u8"上行八分"), "Up 8ths",
                          ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                          { step(0.0, 0.46, 0, 0, 6), step(0.5, 0.46, 1), step(1.0, 0.46, 2), step(1.5, 0.46, 3, 1, 3) });
    up.styleTags.addArray({ "pop", "jpop", "folk" });
    up.moodTags.addArray({ "bright", "flowing", "hopeful" });
    up.functionFit.addArray({ "T", "S", "t", "s" });
    up.formFit.addArray({ "verse", "chorus", "intro" });
    up.familyId = "BrokenPianoFamily";
    up.grooveAffinity = "Straight";
    up.complexity = 2;
    up.energy = 3;
    patterns.push_back(up);

    auto down = makePattern("down_8", cn(u8"下行八分"), "Down 8ths",
                            ArpeggioDirection::Down, ArpeggioDensity::Eighth, 2.0,
                            { step(0.0, 0.46, 3, 1, 5), step(0.5, 0.46, 2), step(1.0, 0.46, 1), step(1.5, 0.46, 0) });
    down.styleTags.addArray({ "pop", "ballad", "cinematic" });
    down.moodTags.addArray({ "gentle", "sad", "warm" });
    down.functionFit.addArray({ "T", "S", "t", "s" });
    down.formFit.addArray({ "verse", "outro", "cadence" });
    down.familyId = "BrokenPianoFamily";
    down.grooveAffinity = "Straight";
    down.complexity = 2;
    down.energy = 2;
    patterns.push_back(down);

    auto upDown = makePattern("up_down_16", cn(u8"上下行十六分"), "Up-Down 16ths",
                              ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 2.0,
                              { step(0.0, 0.23, 0), step(0.25, 0.23, 1), step(0.5, 0.23, 2), step(0.75, 0.23, 3, 1),
                                step(1.0, 0.23, 2), step(1.25, 0.23, 1), step(1.5, 0.23, 0), step(1.75, 0.23, 1) });
    upDown.styleTags.addArray({ "pop", "electro", "jpop" });
    upDown.moodTags.addArray({ "energetic", "bright" });
    upDown.functionFit.addArray({ "S", "D", "s" });
    upDown.formFit.addArray({ "chorus", "bridge" });
    upDown.familyId = "EDMTranceFamily";
    upDown.grooveAffinity = "Tight";
    upDown.complexity = 3;
    upDown.energy = 4;
    patterns.push_back(upDown);

    auto alberti = makePattern("alberti", cn(u8"阿尔贝蒂低音"), "Alberti Bass",
                               ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                               { step(0.0, 0.45, 0, -1, 5, true), step(0.5, 0.45, 2), step(1.0, 0.45, 1), step(1.5, 0.45, 2) });
    alberti.styleTags.addArray({ "classical", "neoclassical", "children" });
    alberti.moodTags.addArray({ "clear", "light" });
    alberti.functionFit.addArray({ "T", "S", "D", "t", "s" });
    alberti.formFit.addArray({ "verse", "intro" });
    alberti.familyId = "BrokenPianoFamily";
    alberti.grooveAffinity = "Straight";
    alberti.bassAnchored = true;
    alberti.complexity = 2;
    alberti.energy = 2;
    patterns.push_back(alberti);

    auto popBroken = makePattern("modern_pop", cn(u8"现代流行分解"), "Modern Pop Broken",
                                 ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                 { step(0.0, 0.92, 0, -1, 8, true), step(0.75, 0.42, 1), step(1.25, 0.42, 2),
                                   step(1.75, 0.70, 3, 1, 4), step(2.5, 0.42, 2), step(3.0, 0.72, 1) });
    popBroken.styleTags.addArray({ "pop", "ballad", "modern" });
    popBroken.moodTags.addArray({ "warm", "flowing", "emotional" });
    popBroken.functionFit.addArray({ "T", "S", "D", "t", "s" });
    popBroken.formFit.addArray({ "verse", "chorus" });
    popBroken.familyId = "BrokenPianoFamily";
    popBroken.grooveAffinity = "Straight";
    popBroken.bassAnchored = true;
    popBroken.complexity = 3;
    popBroken.energy = 3;
    patterns.push_back(popBroken);

    auto rnb = makePattern("rnb_late", cn(u8"R&B错位"), "R&B Late Chords",
                           ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                           { step(0.0, 0.65, 0, -1, 5, true), step(0.65, 0.35, 2), step(1.10, 0.55, 1),
                             step(1.85, 0.40, 3, 1, 2), step(2.55, 0.35, 1), step(3.15, 0.55, 2) });
    rnb.styleTags.addArray({ "rnb", "neo-soul", "jazz" });
    rnb.moodTags.addArray({ "smooth", "warm", "tension" });
    rnb.functionFit.addArray({ "S", "D", "s" });
    rnb.formFit.addArray({ "verse", "bridge" });
    rnb.familyId = "NeoSoulFamily";
    rnb.grooveAffinity = "Late";
    rnb.complexity = 4;
    rnb.energy = 2;
    patterns.push_back(rnb);

    auto cinematic = makePattern("cinematic_slow", cn(u8"电影感铺底"), "Cinematic Slow",
                                 ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                 { step(0.0, 3.75, 0, -1, 8, true), step(0.0, 1.8, 1), step(1.5, 1.8, 2),
                                   step(3.0, 0.9, 3, 1, 4) });
    cinematic.styleTags.addArray({ "cinematic", "ambient", "soundtrack" });
    cinematic.moodTags.addArray({ "wide", "sad", "warm", "epic" });
    cinematic.functionFit.addArray({ "T", "S", "D", "t", "s" });
    cinematic.formFit.addArray({ "intro", "bridge", "outro", "cadence" });
    cinematic.familyId = "CinematicOstinatoFamily";
    cinematic.grooveAffinity = "Straight";
    cinematic.bassAnchored = true;
    cinematic.complexity = 2;
    cinematic.energy = 2;
    patterns.push_back(cinematic);

    auto futureBass = makePattern("future_bass_gate", cn(u8"Future Bass切分"), "Future Bass Gate",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                  { step(0.0, 0.18, 0, -1, 8, true), step(0.25, 0.18, 2, 0, -3), step(0.75, 0.18, 3, 1, 5),
                                    step(1.0, 0.18, 1, 0, -2), step(1.25, 0.18, 2, 1, 3), step(1.75, 0.18, 3, 1, 6) });
    futureBass.styleTags.addArray({ "electro", "edm", "future-bass", "synth" });
    futureBass.moodTags.addArray({ "bright", "energetic", "wide" });
    futureBass.functionFit.addArray({ "S", "D" });
    futureBass.formFit.addArray({ "chorus", "bridge" });
    futureBass.familyId = "SyncopatedElectronicFamily";
    futureBass.grooveAffinity = "Tight";
    futureBass.complexity = 4;
    futureBass.energy = 5;
    futureBass.supportsExtensions = true;
    patterns.push_back(futureBass);

    auto lofi = makePattern("lofi_sway", cn(u8"Lo-Fi摇摆"), "Lo-Fi Sway",
                            ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                            { step(0.0, 0.55, 0, -1, 4, true), step(0.85, 0.38, 2, 0, -7), step(1.45, 0.45, 1, 0, -5),
                              step(2.2, 0.42, 3, 1, -2), step(2.95, 0.5, 2, 0, -6), step(3.55, 0.34, 1, 0, -8) });
    lofi.styleTags.addArray({ "lofi", "hip-hop", "jazz", "neo-soul" });
    lofi.moodTags.addArray({ "warm", "smooth", "calm", "sad" });
    lofi.functionFit.addArray({ "T", "S", "D", "t", "s" });
    lofi.formFit.addArray({ "intro", "verse", "bridge" });
    lofi.familyId = "NeoSoulFamily";
    lofi.grooveAffinity = "Shuffle";
    lofi.bassAnchored = true;
    lofi.complexity = 3;
    lofi.energy = 2;
    patterns.push_back(lofi);

    auto folk = makePattern("folk_pick", cn(u8"民谣指弹"), "Folk Picking",
                            ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 4.0,
                            { step(0.0, 0.42, 0, -1, 7, true), step(0.5, 0.42, 2), step(1.0, 0.42, 1), step(1.5, 0.42, 2),
                              step(2.0, 0.42, 0, -1, 5, true), step(2.5, 0.42, 3, 1, 3), step(3.0, 0.42, 1), step(3.5, 0.42, 2) });
    folk.styleTags.addArray({ "folk", "country", "celtic", "pop" });
    folk.moodTags.addArray({ "hopeful", "bright", "gentle" });
    folk.functionFit.addArray({ "T", "S", "t", "s" });
    folk.formFit.addArray({ "intro", "verse", "chorus" });
    folk.familyId = "GuitarPickingFamily";
    folk.grooveAffinity = "Straight";
    folk.bassAnchored = true;
    folk.complexity = 2;
    folk.energy = 3;
    patterns.push_back(folk);

    auto latin = makePattern("latin_push", cn(u8"拉丁前推"), "Latin Push",
                             ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                             { step(0.0, 0.32, 0, -1, 7, true), step(0.5, 0.28, 2), step(0.75, 0.24, 1),
                               step(1.25, 0.28, 3, 1, 3), step(1.5, 0.32, 2), step(1.75, 0.22, 1, 0, -4) });
    latin.styleTags.addArray({ "latin", "bossa", "salsa", "pop" });
    latin.moodTags.addArray({ "bright", "playful", "energetic" });
    latin.regionTags.addArray({ "latin" });
    latin.functionFit.addArray({ "S", "D" });
    latin.formFit.addArray({ "verse", "chorus", "bridge" });
    latin.familyId = "GuitarPickingFamily";
    latin.grooveAffinity = "Tight";
    latin.bassAnchored = true;
    latin.complexity = 3;
    latin.energy = 4;
    patterns.push_back(latin);

    auto chinese = makePattern("chinese_open_fifth", cn(u8"国风开五度"), "Chinese Open Fifth",
                               ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                               { step(0.0, 1.6, 0, -1, 7, true), step(0.5, 1.1, 2, 0, 1), step(1.75, 0.8, 3, 1, 4),
                                 step(2.5, 1.2, 2, 1, -2), step(3.25, 0.65, 0, 0, -4) });
    chinese.styleTags.addArray({ "cinematic", "folk", "pop" });
    chinese.moodTags.addArray({ "wide", "solemn", "calm", "epic" });
    chinese.regionTags.addArray({ "chinese" });
    chinese.functionFit.addArray({ "T", "S", "t", "s" });
    chinese.formFit.addArray({ "intro", "verse", "outro", "cadence" });
    chinese.familyId = "ChineseOpenFamily";
    chinese.grooveAffinity = "Straight";
    chinese.bassAnchored = true;
    chinese.complexity = 2;
    chinese.energy = 2;
    patterns.push_back(chinese);

    auto pedalDrone = makePattern("pedal_drone_pulse", cn(u8"踏板持续脉冲"), "Pedal Drone Pulse",
                                  ArpeggioDirection::BlockPulse, ArpeggioDensity::Mixed, 4.0,
                                  { step(0.0, 3.75, 0, -1, 8, true), step(0.0, 0.75, 2, 0, -4),
                                    step(1.0, 0.70, 0, 0, -7), step(2.0, 0.85, 2, 0, -3),
                                    step(3.0, 0.50, 3, 1, 2), step(3.5, 0.34, 2, 0, -6) });
    pedalDrone.styleTags.addArray({ "cinematic", "ambient", "folk", "modal" });
    pedalDrone.moodTags.addArray({ "calm", "wide", "solemn", "hypnotic" });
    pedalDrone.functionFit.addArray({ "T", "t", "S", "s" });
    pedalDrone.formFit.addArray({ "intro", "verse", "bridge", "outro" });
    pedalDrone.familyId = "PedalDroneFamily";
    pedalDrone.grooveAffinity = "Straight";
    pedalDrone.bassAnchored = true;
    pedalDrone.pedalFriendly = true;
    pedalDrone.complexity = 2;
    pedalDrone.energy = 2;
    patterns.push_back(pedalDrone);

    auto maqam = makePattern("maqam_turn_ornament", cn(u8"马卡姆回转装饰"), "Maqam Turn Ornament",
                             ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                             { step(0.0, 0.48, 0, -1, 6, true), step(0.5, 0.20, 1, 0, -6),
                               step(0.75, 0.22, 0, 0, 1), step(1.0, 0.34, 2, 0, 4),
                               step(1.35, 0.18, 1, 0, -5), step(1.60, 0.36, 3, 1, 2) });
    maqam.styleTags.addArray({ "folk", "cinematic", "modal", "middle-eastern" });
    maqam.moodTags.addArray({ "tension", "solemn", "mysterious", "ornamental" });
    maqam.regionTags.addArray({ "middle-east", "central-asia" });
    maqam.functionFit.addArray({ "T", "S", "D", "t", "s" });
    maqam.formFit.addArray({ "intro", "verse", "bridge" });
    maqam.familyId = "MaqamOrnamentFamily";
    maqam.grooveAffinity = "Straight";
    maqam.bassAnchored = true;
    maqam.pedalFriendly = true;
    maqam.supportsExtensions = true;
    maqam.complexity = 4;
    maqam.energy = 3;
    patterns.push_back(maqam);

    auto gamelan = makePattern("gamelan_interlock", cn(u8"甘美兰交错固定音型"), "Gamelan Interlock",
                               ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 2.0,
                               { step(0.0, 0.16, 0, 0, 4), step(0.25, 0.14, 2, 1, -2),
                                 step(0.50, 0.16, 1, 0, 1), step(0.75, 0.14, 3, 1, 5),
                                 step(1.0, 0.16, 2, 0, -3), step(1.25, 0.14, 0, 1, 2),
                                 step(1.50, 0.16, 3, 0, 4), step(1.75, 0.14, 1, 1, -1) });
    gamelan.styleTags.addArray({ "folk", "cinematic", "electro", "ostinato" });
    gamelan.moodTags.addArray({ "bright", "hypnotic", "ritual", "energetic" });
    gamelan.regionTags.addArray({ "southeast-asia" });
    gamelan.functionFit.addArray({ "T", "S", "t", "s" });
    gamelan.formFit.addArray({ "intro", "verse", "chorus", "bridge" });
    gamelan.familyId = "GamelanOstinatoFamily";
    gamelan.grooveAffinity = "Tight";
    gamelan.bassAnchored = false;
    gamelan.pedalFriendly = false;
    gamelan.supportsExtensions = false;
    gamelan.complexity = 4;
    gamelan.energy = 4;
    patterns.push_back(gamelan);

    auto latinMontuno = makePattern("latin_montuno", cn(u8"拉丁蒙图诺"), "Latin Montuno",
                                    ArpeggioDirection::Alternating, ArpeggioDensity::Mixed, 2.0,
                                    { step(0.0, 0.26, 0, -1, 7, true), step(0.25, 0.18, 2, 0, -2),
                                      step(0.75, 0.22, 1, 0, 4), step(1.0, 0.24, 3, 1, 6),
                                      step(1.25, 0.18, 2, 0, -3), step(1.75, 0.20, 1, 0, 1) });
    latinMontuno.styleTags.addArray({ "latin", "salsa", "son", "bossa" });
    latinMontuno.moodTags.addArray({ "bright", "playful", "energetic" });
    latinMontuno.regionTags.addArray({ "latin" });
    latinMontuno.functionFit.addArray({ "T", "S", "D" });
    latinMontuno.formFit.addArray({ "verse", "chorus", "bridge" });
    latinMontuno.familyId = "LatinMontunoFamily";
    latinMontuno.grooveAffinity = "Tight";
    latinMontuno.bassAnchored = true;
    latinMontuno.pedalFriendly = false;
    latinMontuno.supportsExtensions = true;
    latinMontuno.complexity = 3;
    latinMontuno.energy = 4;
    patterns.push_back(latinMontuno);

    auto marchPulse = makePattern("march_octave_pulse", cn(u8"进行曲八度脉冲"), "March Octave Pulse",
                                  ArpeggioDirection::BlockPulse, ArpeggioDensity::Quarter, 2.0,
                                  { step(0.0, 0.34, 0, -1, 9, true), step(0.0, 0.30, 0, 0, 2),
                                    step(0.5, 0.22, 2, 0, -3), step(1.0, 0.34, 0, -1, 7, true),
                                    step(1.0, 0.30, 3, 0, 4), step(1.5, 0.22, 2, 0, -4) });
    marchPulse.styleTags.addArray({ "cinematic", "march", "military", "folk" });
    marchPulse.moodTags.addArray({ "epic", "bright", "solemn", "energetic" });
    marchPulse.functionFit.addArray({ "T", "S", "D" });
    marchPulse.formFit.addArray({ "intro", "chorus", "bridge" });
    marchPulse.familyId = "MarchPulseFamily";
    marchPulse.grooveAffinity = "Straight";
    marchPulse.bassAnchored = true;
    marchPulse.pedalFriendly = false;
    marchPulse.supportsExtensions = false;
    marchPulse.complexity = 2;
    marchPulse.energy = 4;
    patterns.push_back(marchPulse);

    auto cadenceBreath = makePattern("cadence_breath", cn(u8"终止呼吸收束"), "Cadence Breath",
                                     ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                     { step(0.0, 1.25, 3, 1, 4), step(1.25, 1.0, 2, 0, -1),
                                       step(2.25, 0.85, 1, 0, -4), step(3.15, 0.70, 0, -1, -2, true) });
    cadenceBreath.styleTags.addArray({ "pop", "cinematic", "classical", "ballad" });
    cadenceBreath.moodTags.addArray({ "calm", "sad", "warm", "resolved" });
    cadenceBreath.functionFit.addArray({ "T", "D", "t" });
    cadenceBreath.formFit.addArray({ "outro", "cadence", "verse" });
    cadenceBreath.familyId = "CadentialBreathFamily";
    cadenceBreath.grooveAffinity = "Straight";
    cadenceBreath.bassAnchored = true;
    cadenceBreath.pedalFriendly = true;
    cadenceBreath.supportsExtensions = true;
    cadenceBreath.complexity = 2;
    cadenceBreath.energy = 1;
    patterns.push_back(cadenceBreath);


    // =========================================================================
    // 扩展范式库（新增 10 个 pattern）
    // =========================================================================

    auto jazzWalking = makePattern("jazz_walking", cn(u8"爵士低音行走"), "Jazz Walking Bass",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Quarter, 2.0,
                                   { step(0.0, 0.45, 0, -1, 4, true),   // 根音
                                     step(0.5, 0.45, 0, -1, 3, true),   // 重复根音（模拟趋近）
                                     step(1.0, 0.45, 4, -1, 5, true),   // 五音
                                     step(1.5, 0.45, 5, -1, 4, true) }); // 六音（半音趋近）
    jazzWalking.styleTags.addArray({ "jazz", "swing", "bebop" });
    jazzWalking.moodTags.addArray({ "cool", "groovy", "smooth" });
    jazzWalking.functionFit.addArray({ "T", "S", "D", "t", "s" });
    jazzWalking.formFit.addArray({ "verse", "bridge", "chorus" });
    jazzWalking.familyId = "JazzWalkingFamily";
    jazzWalking.grooveAffinity = "Shuffle";
    jazzWalking.bassAnchored = true;
    jazzWalking.pedalFriendly = false;
    jazzWalking.supportsExtensions = true;
    jazzWalking.complexity = 3;
    jazzWalking.energy = 3;
    patterns.push_back(jazzWalking);

    auto ragaDrone = makePattern("raga_drone", cn(u8"拉格持续音"), "Raga Drone",
                                 ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                                 { step(0.0, 3.9, 0, -1, 7, true),   // 根音持续
                                   step(0.0, 3.9, 4, -1, 5, true),   // 五音持续
                                   step(2.5, 0.45, 2, 0, -2),         // 偶尔装饰三音
                                   step(3.75, 0.2, 1, 0, -4) });      // 短装饰二音
    ragaDrone.styleTags.addArray({ "folk", "cinematic", "modal", "indian" });
    ragaDrone.moodTags.addArray({ "calm", "meditative", "hypnotic", "wide" });
    ragaDrone.regionTags.addArray({ "india", "south-asia" });
    ragaDrone.functionFit.addArray({ "T", "t", "S", "s" });
    ragaDrone.formFit.addArray({ "intro", "verse", "outro" });
    ragaDrone.familyId = "RagaDroneFamily";
    ragaDrone.grooveAffinity = "Straight";
    ragaDrone.bassAnchored = true;
    ragaDrone.pedalFriendly = true;
    ragaDrone.complexity = 2;
    ragaDrone.energy = 1;
    patterns.push_back(ragaDrone);

    auto chineseOrnament = makePattern("chinese_ornament", cn(u8"国风装饰"), "Chinese Ornament",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                       { step(0.0, 0.65, 0, -1, 5, true),  // 根音
                                         step(0.5, 0.35, 2, 0, -1),         // 三音
                                         step(0.75, 0.2, 3, 1, 4),          // 四音向上
                                         step(1.0, 0.5, 4, 1, 2),           // 五音
                                         step(1.5, 0.45, 2, 0, -3),         // 回三音
                                         step(1.75, 0.2, 5, 1, 6) });       // 短六音装饰
    chineseOrnament.styleTags.addArray({ "chinese", "cinematic", "folk" });
    chineseOrnament.moodTags.addArray({ "elegant", "calm", "wide" });
    chineseOrnament.regionTags.addArray({ "chinese" });
    chineseOrnament.functionFit.addArray({ "T", "S", "t", "s" });
    chineseOrnament.formFit.addArray({ "verse", "bridge", "outro" });
    chineseOrnament.familyId = "ChineseOpenFamily";
    chineseOrnament.grooveAffinity = "Straight";
    chineseOrnament.bassAnchored = true;
    chineseOrnament.pedalFriendly = true;
    chineseOrnament.supportsExtensions = false;
    chineseOrnament.complexity = 3;
    chineseOrnament.energy = 2;
    patterns.push_back(chineseOrnament);

    auto neoChords = makePattern("neo_chromatic", cn(u8"新灵魂色彩"), "Neo-Soul Chromatic",
                                 ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                 { step(0.0, 1.2, 0, -1, 6, true),   // 根音延展
                                   step(0.85, 0.45, 2, 0, -2),        // 三音推迟
                                   step(1.45, 0.7, 3, 1, 4),          // 十一音（4 音）
                                   step(2.2, 0.5, 1, 0, -1),          // 九音（2 音）
                                   step(2.9, 0.65, 3, 1, 2),          // 再次十一音
                                   step(3.55, 0.35, 2, 0, -5) });     // 短滑音
    neoChords.styleTags.addArray({ "rnb", "neo-soul", "jazz" });
    neoChords.moodTags.addArray({ "warm", "smooth", "tension", "dreamy" });
    neoChords.functionFit.addArray({ "S", "D", "s" });
    neoChords.formFit.addArray({ "verse", "bridge", "chorus" });
    neoChords.familyId = "NeoSoulFamily";
    neoChords.grooveAffinity = "Late";
    neoChords.bassAnchored = true;
    neoChords.supportsExtensions = true;
    neoChords.complexity = 4;
    neoChords.energy = 3;
    patterns.push_back(neoChords);

    auto edmGate = makePattern("edm_gate_sixteenth", cn(u8"EDM 门限十六分"), "EDM Gate 16ths",
                               ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 1.0,
                               { step(0.0, 0.18, 0, -1, 8, true),
                                 step(0.25, 0.1, 1, 0, -4),
                                 step(0.5, 0.18, 2, 0, 2),
                                 step(0.75, 0.08, 3, 1, 6),
                                 step(1.0, 0.18, 1, 0, -2),
                                 step(1.25, 0.1, 2, 0, -1),
                                 step(1.5, 0.18, 3, 1, 4),
                                 step(1.75, 0.08, 0, -1, 7) });
    edmGate.styleTags.addArray({ "edm", "electro", "trance", "future-bass" });
    edmGate.moodTags.addArray({ "energetic", "bright", "epic" });
    edmGate.functionFit.addArray({ "S", "D" });
    edmGate.formFit.addArray({ "chorus", "bridge", "build-up" });
    edmGate.familyId = "EDMTranceFamily";
    edmGate.grooveAffinity = "Tight";
    edmGate.bassAnchored = true;
    edmGate.supportsExtensions = true;
    edmGate.complexity = 3;
    edmGate.energy = 5;
    patterns.push_back(edmGate);

    auto cinematicOst = makePattern("cinematic_triplet", cn(u8"电影三连音"), "Cinematic Triplet",
                                    ArpeggioDirection::Up, ArpeggioDensity::Eighth, 3.0,
                                    { step(0.0, 0.9, 0, -1, 6, true),
                                      step(1.0, 0.7, 2, 0, 2),
                                      step(2.0, 1.2, 3, 1, 5),
                                      step(3.0, 0.8, 1, 0, -1) });
    cinematicOst.styleTags.addArray({ "cinematic", "ambient", "soundtrack" });
    cinematicOst.moodTags.addArray({ "wide", "epic", "tension", "hopeful" });
    cinematicOst.functionFit.addArray({ "T", "S", "D", "t", "s" });
    cinematicOst.formFit.addArray({ "intro", "bridge", "outro" });
    cinematicOst.familyId = "CinematicOstinatoFamily";
    cinematicOst.grooveAffinity = "Straight";
    cinematicOst.bassAnchored = true;
    cinematicOst.complexity = 2;
    cinematicOst.energy = 3;
    patterns.push_back(cinematicOst);

    auto guitarFinger = makePattern("guitar_travis", cn(u8"吉他特拉维斯拨弦"), "Guitar Travis Picking",
                                    ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                    { step(0.0, 0.42, 0, -1, 7, true),
                                      step(0.5, 0.38, 2, 0, -2),
                                      step(1.0, 0.42, 1, 0, 3),
                                      step(1.5, 0.38, 2, 0, -1) });
    guitarFinger.styleTags.addArray({ "folk", "country", "bluegrass" });
    guitarFinger.moodTags.addArray({ "hopeful", "bright", "gentle" });
    guitarFinger.functionFit.addArray({ "T", "S", "t", "s" });
    guitarFinger.formFit.addArray({ "verse", "chorus" });
    guitarFinger.familyId = "GuitarPickingFamily";
    guitarFinger.grooveAffinity = "Straight";
    guitarFinger.bassAnchored = true;
    guitarFinger.complexity = 2;
    guitarFinger.energy = 3;
    patterns.push_back(guitarFinger);

    auto padSwell = makePattern("pad_swell", cn(u8"铺底渐强"), "Pad Swell",
                                ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                                { step(0.0, 3.9, 0, -1, 5, true),
                                  step(0.5, 3.4, 1, 0, 2),
                                  step(1.0, 2.9, 2, 0, 4),
                                  step(2.0, 1.9, 3, 1, 3) });
    padSwell.styleTags.addArray({ "cinematic", "ambient", "pop", "ballad" });
    padSwell.moodTags.addArray({ "warm", "calm", "wide", "sad" });
    padSwell.functionFit.addArray({ "T", "S", "t", "s" });
    padSwell.formFit.addArray({ "intro", "verse", "outro" });
    padSwell.familyId = "PadFamily";
    padSwell.grooveAffinity = "Straight";
    padSwell.pedalFriendly = true;
    padSwell.complexity = 1;
    padSwell.energy = 1;
    patterns.push_back(padSwell);

    auto syncopated = makePattern("syncopated_rhythm", cn(u8"切分电子节奏"), "Syncopated Rhythm",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                  { step(0.0, 0.25, 0, -1, 7, true),
                                    step(0.375, 0.2, 2, 0, -3),
                                    step(0.875, 0.3, 1, 0, 1),
                                    step(1.125, 0.18, 3, 1, 6),
                                    step(1.5, 0.25, 2, 0, -2),
                                    step(1.875, 0.2, 0, -1, 5) });
    syncopated.styleTags.addArray({ "edm", "future-bass", "electro", "synth" });
    syncopated.moodTags.addArray({ "energetic", "bright", "tension" });
    syncopated.functionFit.addArray({ "S", "D" });
    syncopated.formFit.addArray({ "chorus", "bridge" });
    syncopated.familyId = "SyncopatedElectronicFamily";
    syncopated.grooveAffinity = "Tight";
    syncopated.bassAnchored = true;
    syncopated.supportsExtensions = true;
    syncopated.complexity = 4;
    syncopated.energy = 4;
    patterns.push_back(syncopated);

    auto brokenMoonlight = makePattern("moonlight_broken", cn(u8"月光分解"), "Moonlight Broken",
                                       ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 3.0,
                                       { step(0.0, 0.7, 0, -1, 6, true),
                                         step(1.0, 0.5, 2, 0, 2),
                                         step(1.5, 0.4, 1, 0, -1),
                                         step(2.0, 0.7, 3, 1, 4),
                                         step(2.5, 0.5, 2, 0, 1),
                                         step(3.0, 0.4, 1, 0, -2) });
    brokenMoonlight.styleTags.addArray({ "classical", "neoclassical", "cinematic" });
    brokenMoonlight.moodTags.addArray({ "sad", "calm", "dreamy", "wide" });
    brokenMoonlight.functionFit.addArray({ "T", "S", "t", "s" });
    brokenMoonlight.formFit.addArray({ "intro", "verse", "outro" });
    brokenMoonlight.familyId = "BrokenPianoFamily";
    brokenMoonlight.grooveAffinity = "Straight";
    brokenMoonlight.bassAnchored = true;
    brokenMoonlight.complexity = 3;
    brokenMoonlight.energy = 2;
    patterns.push_back(brokenMoonlight);


    // ========================================================================
    // 中国少数民族及特殊风格 (Chinese Minority & Special)
    // ========================================================================

    // 藏族风格: 长音 drone + 偶尔的六度跳跃 (模拟扎木聂/宫廷音乐)
    auto tibetanDrone = makePattern("tibetan_drone", cn(u8"藏族持续"), "Tibetan Drone",
                                    ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 4.0,
                                    { step(0.0, 3.8, 0, -1, 8, true),     // 根音持续
                                      step(0.0, 3.8, 4, -1, 7, true),     // 五音持续
                                      step(2.0, 0.6, 5, 1, 3),            // 六度跳跃
                                      step(3.0, 0.4, 2, 0, -2) });        // 偶尔三音装饰
    tibetanDrone.styleTags.addArray({ "cinematic", "folk", "ambient" });
    tibetanDrone.moodTags.addArray({ "wide", "calm", "solemn", "sacred" });
    tibetanDrone.regionTags.addArray({ "chinese", "tibetan" });
    tibetanDrone.functionFit.addArray({ "T", "t" });
    tibetanDrone.formFit.addArray({ "intro", "verse", "outro" });
    tibetanDrone.familyId = "PedalDroneFamily";
    tibetanDrone.grooveAffinity = "Straight";
    tibetanDrone.bassAnchored = true;
    tibetanDrone.pedalFriendly = true;
    tibetanDrone.complexity = 2;
    tibetanDrone.energy = 1;
    patterns.push_back(tibetanDrone);

    // 蒙古族风格: 马头琴式五度跳 + 短促装饰 (模拟呼麦低音)
    auto mongolianHoomii = makePattern("mongolian_hoomii", cn(u8"蒙古呼麦低音"), "Mongolian Hoomii Bass",
                                       ArpeggioDirection::Alternating, ArpeggioDensity::Quarter, 2.0,
                                       { step(0.0, 0.45, 0, -1, 7, true),
                                         step(0.5, 0.45, 4, -1, 6, true),
                                         step(1.0, 0.3, 5, 0, 3),
                                         step(1.5, 0.2, 2, 0, -2) });
    mongolianHoomii.styleTags.addArray({ "folk", "cinematic", "world" });
    mongolianHoomii.moodTags.addArray({ "wide", "epic", "solemn" });
    mongolianHoomii.regionTags.addArray({ "chinese", "mongolia" });
    mongolianHoomii.functionFit.addArray({ "T", "S", "t" });
    mongolianHoomii.formFit.addArray({ "intro", "verse", "bridge" });
    mongolianHoomii.familyId = "PedalDroneFamily";
    mongolianHoomii.grooveAffinity = "Straight";
    mongolianHoomii.bassAnchored = true;
    mongolianHoomii.complexity = 2;
    mongolianHoomii.energy = 2;
    patterns.push_back(mongolianHoomii);

    // 维吾尔/哈萨克: 冬不拉风格快速交替根/三/五音 + 偶尔的邻音
    auto dombraPick = makePattern("dombra_pick", cn(u8"冬不拉弹拨"), "Dombra Picking",
                                  ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                  { step(0.0, 0.18, 0, -1, 6, true),
                                    step(0.25, 0.16, 2, 0, -3),
                                    step(0.5, 0.18, 4, 0, 2),
                                    step(0.75, 0.14, 1, 0, -1),
                                    step(1.0, 0.18, 0, -1, 5, true),
                                    step(1.25, 0.16, 3, 1, 4),
                                    step(1.5, 0.18, 2, 0, 1),
                                    step(1.75, 0.14, 4, 0, -2) });
    dombraPick.styleTags.addArray({ "folk", "world", "central-asia" });
    dombraPick.moodTags.addArray({ "bright", "playful", "energetic" });
    dombraPick.regionTags.addArray({ "central-asia", "xinjiang", "kazakh", "uyghur" });
    dombraPick.functionFit.addArray({ "T", "S", "t" });
    dombraPick.formFit.addArray({ "verse", "chorus" });
    dombraPick.familyId = "GuitarPickingFamily";
    dombraPick.grooveAffinity = "Tight";
    dombraPick.bassAnchored = true;
    dombraPick.complexity = 3;
    dombraPick.energy = 4;
    patterns.push_back(dombraPick);

    // 彝族/苗族: 山歌风格五度+八度跳跃 (模拟口弦/芦笙)
    auto miaoMountain = makePattern("miao_mountain", cn(u8"苗族芦笙"), "Miao Mountain Song",
                                    ArpeggioDirection::UpDown, ArpeggioDensity::Mixed, 2.0,
                                    { step(0.0, 0.45, 0, -1, 8, true),
                                      step(0.75, 0.3, 4, 0, 4),
                                      step(1.25, 0.4, 0, 0, 6),
                                      step(1.75, 0.25, 2, 1, 3) });
    miaoMountain.styleTags.addArray({ "folk", "chinese", "ethnic" });
    miaoMountain.moodTags.addArray({ "bright", "hopeful", "playful" });
    miaoMountain.regionTags.addArray({ "chinese", "miao", "yi" });
    miaoMountain.functionFit.addArray({ "T", "S", "t" });
    miaoMountain.formFit.addArray({ "verse", "chorus" });
    miaoMountain.familyId = "ChineseOpenFamily";
    miaoMountain.grooveAffinity = "Straight";
    miaoMountain.bassAnchored = true;
    miaoMountain.complexity = 2;
    miaoMountain.energy = 3;
    patterns.push_back(miaoMountain);

    // 朝鲜族: 伽倻琴风格散调 (长音 + 短促推弦感)
    auto gayageum = makePattern("gayageum", cn(u8"伽倻琴散调"), "Gayageum Sanjo",
                                ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                { step(0.0, 1.2, 0, -1, 7, true),
                                  step(1.2, 0.4, 2, 0, -2),
                                  step(2.0, 0.9, 4, 0, 4),
                                  step(2.9, 0.3, 3, 1, 6),
                                  step(3.5, 0.5, 1, 0, -1) });
    gayageum.styleTags.addArray({ "folk", "korean", "traditional" });
    gayageum.moodTags.addArray({ "calm", "sad", "elegant" });
    gayageum.regionTags.addArray({ "korean", "chinese" });
    gayageum.functionFit.addArray({ "T", "S", "t" });
    gayageum.formFit.addArray({ "intro", "verse", "outro" });
    gayageum.familyId = "ChineseOpenFamily";
    gayageum.grooveAffinity = "Straight";
    gayageum.bassAnchored = true;
    gayageum.complexity = 3;
    gayageum.energy = 2;
    patterns.push_back(gayageum);

    // ========================================================================
    // 中亚/西亚风格 (Middle East / Central Asia)
    // ========================================================================

    // 波斯/阿拉伯木卡姆: 多装饰音 + 半音邻音
    auto persianOrnament = makePattern("persian_ornament", cn(u8"波斯装饰"), "Persian Ornament",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                       { step(0.0, 0.5, 0, -1, 6, true),
                                         step(0.5, 0.2, 1, 0, -4),
                                         step(0.7, 0.15, 0, 0, 2),
                                         step(1.0, 0.45, 2, 0, 3),
                                         step(1.45, 0.2, 3, 1, 5),
                                         step(1.7, 0.15, 2, 0, -2) });
    persianOrnament.styleTags.addArray({ "folk", "cinematic", "middle-eastern" });
    persianOrnament.moodTags.addArray({ "mysterious", "tension", "ornamental" });
    persianOrnament.regionTags.addArray({ "middle-east", "persia", "arabic" });
    persianOrnament.functionFit.addArray({ "T", "S", "D", "t", "s" });
    persianOrnament.formFit.addArray({ "intro", "verse", "bridge" });
    persianOrnament.familyId = "MaqamOrnamentFamily";
    persianOrnament.grooveAffinity = "Straight";
    persianOrnament.bassAnchored = true;
    persianOrnament.pedalFriendly = true;
    persianOrnament.supportsExtensions = true;
    persianOrnament.complexity = 4;
    persianOrnament.energy = 3;
    patterns.push_back(persianOrnament);

    // 土耳其/阿拉伯: 循环固定音型 (模拟乌德琴)
    auto oudOstinato = makePattern("oud_ostinato", cn(u8"乌德琴固定音型"), "Oud Ostinato",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                   { step(0.0, 0.45, 0, -1, 7, true),
                                     step(0.5, 0.4, 4, 0, 1),
                                     step(1.0, 0.45, 2, 0, -3),
                                     step(1.5, 0.4, 4, 0, 2) });
    oudOstinato.styleTags.addArray({ "folk", "world", "arabic" });
    oudOstinato.moodTags.addArray({ "hypnotic", "dance", "bright" });
    oudOstinato.regionTags.addArray({ "middle-east", "turkey", "arabic" });
    oudOstinato.functionFit.addArray({ "T", "S", "t" });
    oudOstinato.formFit.addArray({ "verse", "chorus" });
    oudOstinato.familyId = "PedalDroneFamily";
    oudOstinato.grooveAffinity = "Tight";
    oudOstinato.bassAnchored = true;
    oudOstinato.pedalFriendly = true;
    oudOstinato.complexity = 2;
    oudOstinato.energy = 3;
    patterns.push_back(oudOstinato);

    // ========================================================================
    // 南亚风格 (South Asia)
    // ========================================================================

    // 印度拉格: 西塔琴式 drone + 慢速滑音 (模拟 Meend)
    auto sitarMeend = makePattern("sitar_meend", cn(u8"西塔琴滑音"), "Sitar Meend",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                  { step(0.0, 3.5, 0, -1, 8, true),
                                    step(0.5, 0.8, 1, 0, -4),
                                    step(1.5, 0.6, 2, 0, 3),
                                    step(2.5, 1.0, 4, 1, 5),
                                    step(3.5, 0.4, 3, 0, -2) });
    sitarMeend.styleTags.addArray({ "folk", "cinematic", "indian" });
    sitarMeend.moodTags.addArray({ "meditative", "calm", "wide" });
    sitarMeend.regionTags.addArray({ "india", "south-asia" });
    sitarMeend.functionFit.addArray({ "T", "t" });
    sitarMeend.formFit.addArray({ "intro", "outro" });
    sitarMeend.familyId = "RagaDroneFamily";
    sitarMeend.grooveAffinity = "Straight";
    sitarMeend.bassAnchored = true;
    sitarMeend.pedalFriendly = true;
    sitarMeend.complexity = 2;
    sitarMeend.energy = 1;
    patterns.push_back(sitarMeend);

    // 宝莱坞舞曲: 切分节奏 + 短促和弦音
    auto bollywoodDance = makePattern("bollywood_dance", cn(u8"宝莱坞舞曲"), "Bollywood Dance",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                      { step(0.0, 0.25, 0, -1, 7, true),
                                        step(0.5, 0.2, 2, 0, 2),
                                        step(0.75, 0.18, 1, 0, -3),
                                        step(1.25, 0.3, 3, 1, 6),
                                        step(1.5, 0.2, 2, 0, -1),
                                        step(1.75, 0.18, 0, -1, 5, true) });
    bollywoodDance.styleTags.addArray({ "pop", "dance", "world" });
    bollywoodDance.moodTags.addArray({ "bright", "energetic", "playful" });
    bollywoodDance.regionTags.addArray({ "india", "bollywood" });
    bollywoodDance.functionFit.addArray({ "S", "D" });
    bollywoodDance.formFit.addArray({ "chorus", "bridge" });
    bollywoodDance.familyId = "SyncopatedElectronicFamily";
    bollywoodDance.grooveAffinity = "Tight";
    bollywoodDance.bassAnchored = true;
    bollywoodDance.complexity = 3;
    bollywoodDance.energy = 4;
    patterns.push_back(bollywoodDance);

    // ========================================================================
    // 东南亚风格 (Southeast Asia)
    // ========================================================================

    // 印尼甘美兰: 快速交错短音 (已在 GamelanOstinatoFamily 有一个，增加变体)
    auto gamelanFast = makePattern("gamelan_fast", cn(u8"甘美兰快速交错"), "Gamelan Fast Interlock",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.12, 0, 0, 5),
                                     step(0.125, 0.1, 2, 1, -3),
                                     step(0.25, 0.12, 1, 0, 2),
                                     step(0.375, 0.1, 3, 1, 6),
                                     step(0.5, 0.12, 2, 0, -2),
                                     step(0.625, 0.1, 0, 1, 4),
                                     step(0.75, 0.12, 3, 0, 1),
                                     step(0.875, 0.1, 1, 1, -1) });
    gamelanFast.styleTags.addArray({ "folk", "cinematic", "ostinato" });
    gamelanFast.moodTags.addArray({ "bright", "energetic", "ritual" });
    gamelanFast.regionTags.addArray({ "indonesia", "bali", "southeast-asia" });
    gamelanFast.functionFit.addArray({ "T", "S", "t" });
    gamelanFast.formFit.addArray({ "verse", "chorus", "bridge" });
    gamelanFast.familyId = "GamelanOstinatoFamily";
    gamelanFast.grooveAffinity = "Tight";
    gamelanFast.complexity = 4;
    gamelanFast.energy = 5;
    patterns.push_back(gamelanFast);

    // 菲律宾/泰国: 竹排琴风格 (长延音 + 交替)
    auto kulintang = makePattern("kulintang", cn(u8"库林当竹排"), "Kulintang",
                                 ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                 { step(0.0, 0.8, 0, -1, 6, true),
                                   step(1.0, 0.7, 2, 0, 2),
                                   step(2.0, 0.8, 4, 1, 5),
                                   step(3.0, 0.6, 1, 0, -2) });
    kulintang.styleTags.addArray({ "folk", "world", "southeast-asia" });
    kulintang.moodTags.addArray({ "calm", "hypnotic", "wide" });
    kulintang.regionTags.addArray({ "philippines", "thailand", "indonesia" });
    kulintang.functionFit.addArray({ "T", "S", "t" });
    kulintang.formFit.addArray({ "intro", "verse" });
    kulintang.familyId = "PedalDroneFamily";
    kulintang.grooveAffinity = "Straight";
    kulintang.bassAnchored = true;
    kulintang.complexity = 2;
    kulintang.energy = 2;
    patterns.push_back(kulintang);

    // ========================================================================
    // 欧洲民间风格 (European Folk)
    // ========================================================================

    // 凯尔特: 竖琴式分解 (交替八度)
    auto celticHarp = makePattern("celtic_harp", cn(u8"凯尔特竖琴"), "Celtic Harp",
                                  ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.45, 0, -1, 7, true),
                                    step(0.5, 0.4, 2, 0, 2),
                                    step(1.0, 0.45, 1, 0, -1),
                                    step(1.5, 0.4, 2, 0, 3) });
    celticHarp.styleTags.addArray({ "folk", "celtic", "world" });
    celticHarp.moodTags.addArray({ "hopeful", "bright", "gentle" });
    celticHarp.regionTags.addArray({ "celtic", "ireland", "scotland" });
    celticHarp.functionFit.addArray({ "T", "S", "t" });
    celticHarp.formFit.addArray({ "verse", "chorus" });
    celticHarp.familyId = "GuitarPickingFamily";
    celticHarp.grooveAffinity = "Straight";
    celticHarp.bassAnchored = true;
    celticHarp.complexity = 2;
    celticHarp.energy = 3;
    patterns.push_back(celticHarp);

    // 吉普赛 (匈牙利/罗马尼亚): 切分节奏 + 增二度跳跃感
    auto gypsyCimbalom = makePattern("gypsy_cimbalom", cn(u8"吉普赛钦巴龙"), "Gypsy Cimbalom",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                     { step(0.0, 0.35, 0, -1, 8, true),
                                       step(0.5, 0.3, 3, 0, -4),
                                       step(0.875, 0.25, 1, 0, 2),
                                       step(1.125, 0.3, 4, 1, 6),
                                       step(1.5, 0.35, 2, 0, -2) });
    gypsyCimbalom.styleTags.addArray({ "folk", "world", "gypsy" });
    gypsyCimbalom.moodTags.addArray({ "bright", "playful", "energetic" });
    gypsyCimbalom.regionTags.addArray({ "hungary", "romania", "balkan" });
    gypsyCimbalom.functionFit.addArray({ "S", "D" });
    gypsyCimbalom.formFit.addArray({ "chorus", "bridge" });
    gypsyCimbalom.familyId = "SyncopatedElectronicFamily";
    gypsyCimbalom.grooveAffinity = "Shuffle";
    gypsyCimbalom.bassAnchored = true;
    gypsyCimbalom.complexity = 3;
    gypsyCimbalom.energy = 4;
    patterns.push_back(gypsyCimbalom);

    // 法朵 (葡萄牙): 低沉半音趋近 + 长尾音
    auto fadoLamento = makePattern("fado_lamento", cn(u8"法朵哀歌"), "Fado Lamento",
                                   ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                   { step(0.0, 1.2, 0, -1, 6, true),
                                     step(1.2, 0.8, 1, 0, -3),
                                     step(2.0, 1.0, 4, 0, 2),
                                     step(3.0, 0.9, 5, 1, 5) });
    fadoLamento.styleTags.addArray({ "folk", "world", "fado" });
    fadoLamento.moodTags.addArray({ "sad", "warm", "nostalgic" });
    fadoLamento.regionTags.addArray({ "portugal", "iberia" });
    fadoLamento.functionFit.addArray({ "T", "t", "D" });
    fadoLamento.formFit.addArray({ "verse", "outro" });
    fadoLamento.familyId = "BrokenPianoFamily";
    fadoLamento.grooveAffinity = "Straight";
    fadoLamento.bassAnchored = true;
    fadoLamento.complexity = 2;
    fadoLamento.energy = 2;
    patterns.push_back(fadoLamento);

    // 弗拉门戈: 扫弦感 + 快速上行
    auto flamencoRasgueo = makePattern("flamenco_rasgueo", cn(u8"弗拉门戈扫弦"), "Flamenco Rasgueo",
                                       ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 1.0,
                                       { step(0.0, 0.12, 0, -1, 8, true),
                                         step(0.125, 0.1, 2, 0, 3),
                                         step(0.25, 0.12, 1, 0, -2),
                                         step(0.375, 0.1, 3, 1, 6),
                                         step(0.5, 0.12, 2, 0, 1),
                                         step(0.625, 0.1, 4, 1, -3),
                                         step(0.75, 0.12, 3, 1, 4),
                                         step(0.875, 0.1, 0, -1, 7, true) });
    flamencoRasgueo.styleTags.addArray({ "folk", "latin", "flamenco" });
    flamencoRasgueo.moodTags.addArray({ "bright", "energetic", "passionate" });
    flamencoRasgueo.regionTags.addArray({ "spain", "latin" });
    flamencoRasgueo.functionFit.addArray({ "S", "D" });
    flamencoRasgueo.formFit.addArray({ "chorus", "bridge" });
    flamencoRasgueo.familyId = "LatinMontunoFamily";
    flamencoRasgueo.grooveAffinity = "Tight";
    flamencoRasgueo.bassAnchored = true;
    flamencoRasgueo.complexity = 4;
    flamencoRasgueo.energy = 5;
    patterns.push_back(flamencoRasgueo);

    // ========================================================================
    // 拉丁美洲/非洲风格 (Latin America & Africa)
    // ========================================================================

    // 安第斯排箫: 五度+八度跳跃 + 长持续
    auto andeanSiku = makePattern("andean_siku", cn(u8"安第斯排箫"), "Andean Siku",
                                  ArpeggioDirection::UpDown, ArpeggioDensity::Half, 4.0,
                                  { step(0.0, 1.8, 0, -1, 7, true),
                                    step(1.0, 1.2, 4, 0, 4),
                                    step(2.0, 1.8, 0, 0, 6),
                                    step(3.0, 0.9, 2, 1, 2) });
    andeanSiku.styleTags.addArray({ "folk", "world", "andean" });
    andeanSiku.moodTags.addArray({ "wide", "calm", "hopeful" });
    andeanSiku.regionTags.addArray({ "andean", "peru", "bolivia" });
    andeanSiku.functionFit.addArray({ "T", "S", "t" });
    andeanSiku.formFit.addArray({ "intro", "verse" });
    andeanSiku.familyId = "PedalDroneFamily";
    andeanSiku.grooveAffinity = "Straight";
    andeanSiku.bassAnchored = true;
    andeanSiku.complexity = 2;
    andeanSiku.energy = 2;
    patterns.push_back(andeanSiku);

    // 非洲鼓乐: 短促节奏型 + 切分
    auto africanDjembe = makePattern("african_djembe", cn(u8"非洲金贝鼓"), "African Djembe",
                                     ArpeggioDirection::BlockPulse, ArpeggioDensity::Mixed, 2.0,
                                     { step(0.0, 0.25, 0, -1, 8, true),
                                       step(0.5, 0.2, 4, 0, 2),
                                       step(1.0, 0.3, 0, -1, 6, true),
                                       step(1.5, 0.2, 4, 0, 3) });
    africanDjembe.styleTags.addArray({ "folk", "world", "african" });
    africanDjembe.moodTags.addArray({ "energetic", "playful", "dance" });
    africanDjembe.regionTags.addArray({ "africa", "west-africa" });
    africanDjembe.functionFit.addArray({ "T", "S", "t" });
    africanDjembe.formFit.addArray({ "chorus", "bridge" });
    africanDjembe.familyId = "MarchPulseFamily";
    africanDjembe.grooveAffinity = "Tight";
    africanDjembe.bassAnchored = true;
    africanDjembe.complexity = 2;
    africanDjembe.energy = 4;
    patterns.push_back(africanDjembe);

    // ========================================================================
    // 特殊调式/古风 (Modal / Ancient)
    // ========================================================================

    // 多利亚调式: 平稳步进 + 六度强调
    auto dorianWalk = makePattern("dorian_walk", cn(u8"多利亚步行"), "Dorian Walk",
                                  ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.45, 0, -1, 7, true),
                                    step(0.5, 0.45, 2, 0, -2),
                                    step(1.0, 0.45, 3, 0, 4),
                                    step(1.5, 0.45, 4, 0, 2) });
    dorianWalk.styleTags.addArray({ "folk", "modal", "dorian" });
    dorianWalk.moodTags.addArray({ "calm", "bright", "hopeful" });
    dorianWalk.functionFit.addArray({ "T", "S", "t" });
    dorianWalk.formFit.addArray({ "verse", "chorus" });
    dorianWalk.familyId = "BrokenPianoFamily";
    dorianWalk.grooveAffinity = "Straight";
    dorianWalk.bassAnchored = true;
    dorianWalk.complexity = 2;
    dorianWalk.energy = 2;
    patterns.push_back(dorianWalk);

    // 弗里吉亚调式: 小二度下行 + 紧张感
    auto phrygianDescent = makePattern("phrygian_descent", cn(u8"弗里吉亚下行"), "Phrygian Descent",
                                       ArpeggioDirection::Down, ArpeggioDensity::Mixed, 2.0,
                                       { step(0.0, 0.5, 0, -1, 8, true),
                                         step(0.75, 0.35, 1, 0, -3),
                                         step(1.25, 0.45, 2, 0, 2),
                                         step(1.75, 0.3, 1, 0, -4) });
    phrygianDescent.styleTags.addArray({ "modal", "phrygian", "tension" });
    phrygianDescent.moodTags.addArray({ "dark", "tense", "mysterious" });
    phrygianDescent.functionFit.addArray({ "D", "s" });
    phrygianDescent.formFit.addArray({ "bridge", "chorus" });
    phrygianDescent.familyId = "MaqamOrnamentFamily";
    phrygianDescent.grooveAffinity = "Straight";
    phrygianDescent.bassAnchored = true;
    phrygianDescent.complexity = 3;
    phrygianDescent.energy = 3;
    patterns.push_back(phrygianDescent);

    // 混合利底亚: 降七度跳跃 + 开放感
    auto mixolydianJump = makePattern("mixolydian_jump", cn(u8"混合利底亚跳跃"), "Mixolydian Jump",
                                      ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.45, 0, -1, 7, true),
                                        step(0.5, 0.45, 4, 0, 2),
                                        step(1.0, 0.45, 3, 0, 5),
                                        step(1.5, 0.45, 2, 0, -2) });
    mixolydianJump.styleTags.addArray({ "modal", "mixolydian", "blues" });
    mixolydianJump.moodTags.addArray({ "bright", "playful", "hopeful" });
    mixolydianJump.functionFit.addArray({ "D", "S" });
    mixolydianJump.formFit.addArray({ "chorus", "bridge" });
    mixolydianJump.familyId = "GuitarPickingFamily";
    mixolydianJump.grooveAffinity = "Straight";
    mixolydianJump.bassAnchored = true;
    mixolydianJump.complexity = 2;
    mixolydianJump.energy = 3;
    patterns.push_back(mixolydianJump);


        // ========================================================================
    // 中国流行音乐 (Chinese Pop)
    // ========================================================================

    // 华语流行抒情 (经典八分分解，温暖柔和)
    auto cpopBallad = makePattern("cpop_ballad", cn(u8"华语流行抒情"), "C-Pop Ballad",
                                  ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.65, 0, -1, 6, true),   // 根音偏长
                                    step(0.75, 0.45, 2, 0, -2),         // 三音稍晚
                                    step(1.25, 0.5, 1, 0, 1),           // 五音
                                    step(1.75, 0.45, 2, 0, -1) });      // 回到三音
    cpopBallad.styleTags.addArray({ "pop", "c-pop", "ballad", "chinese" });
    cpopBallad.moodTags.addArray({ "warm", "sad", "romantic", "calm" });
    cpopBallad.regionTags.addArray({ "chinese" });
    cpopBallad.functionFit.addArray({ "T", "S", "t", "s" });
    cpopBallad.formFit.addArray({ "verse", "chorus", "intro" });
    cpopBallad.familyId = "BrokenPianoFamily";
    cpopBallad.grooveAffinity = "Straight";
    cpopBallad.bassAnchored = true;
    cpopBallad.complexity = 2;
    cpopBallad.energy = 2;
    patterns.push_back(cpopBallad);

    // 周杰伦风格 (切分 + 五声性跳跃，钢琴伴唱)
    auto jayChouStyle = makePattern("jay_chou", cn(u8"周杰伦钢琴"), "Jay Chou Piano",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                    { step(0.0, 1.2, 0, -1, 7, true),
                                      step(1.2, 0.45, 2, 0, -4),
                                      step(1.9, 0.6, 4, 0, 3),
                                      step(2.5, 0.5, 1, 0, -1),
                                      step(3.2, 0.7, 3, 1, 5) });
    jayChouStyle.styleTags.addArray({ "pop", "c-pop", "jay_chou", "piano" });
    jayChouStyle.moodTags.addArray({ "nostalgic", "romantic", "hopeful" });
    jayChouStyle.regionTags.addArray({ "chinese", "taiwan" });
    jayChouStyle.functionFit.addArray({ "T", "S", "D", "t", "s" });
    jayChouStyle.formFit.addArray({ "verse", "chorus", "bridge" });
    jayChouStyle.familyId = "BrokenPianoFamily";
    jayChouStyle.grooveAffinity = "Late";
    jayChouStyle.bassAnchored = true;
    jayChouStyle.complexity = 3;
    jayChouStyle.energy = 3;
    patterns.push_back(jayChouStyle);

    // ========================================================================
    // 中国古风 (Chinese Ancient / Gufeng)
    // ========================================================================

    // 古筝轮指风格 (快速交替，五声音阶)
    auto guzhengTremolo = makePattern("guzheng_tremolo", cn(u8"古筝轮指"), "Guzheng Tremolo",
                                      ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                      { step(0.0, 0.12, 0, -1, 5, true),
                                        step(0.125, 0.1, 2, 0, -2),
                                        step(0.25, 0.12, 1, 0, 1),
                                        step(0.375, 0.1, 2, 0, -1),
                                        step(0.5, 0.12, 0, -1, 4, true),
                                        step(0.625, 0.1, 3, 1, 3),
                                        step(0.75, 0.12, 2, 0, 0),
                                        step(0.875, 0.1, 1, 0, -3) });
    guzhengTremolo.styleTags.addArray({ "chinese", "ancient_style", "guzheng" });
    guzhengTremolo.moodTags.addArray({ "elegant", "calm", "flowing" });
    guzhengTremolo.regionTags.addArray({ "chinese" });
    guzhengTremolo.functionFit.addArray({ "T", "S", "t", "s" });
    guzhengTremolo.formFit.addArray({ "intro", "verse", "bridge" });
    guzhengTremolo.familyId = "ChineseOpenFamily";
    guzhengTremolo.grooveAffinity = "Tight";
    guzhengTremolo.bassAnchored = true;
    guzhengTremolo.complexity = 4;
    guzhengTremolo.energy = 3;
    patterns.push_back(guzhengTremolo);

    // 古琴散板风格 (极慢速，留白，空五度)
    auto guqinFlowing = makePattern("guqin_flowing", cn(u8"古琴泛音"), "Guqin Flowing",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                    { step(0.0, 3.5, 0, -1, 6, true),
                                      step(1.8, 1.2, 4, -1, 4, true),
                                      step(3.2, 0.7, 2, 0, -2) });
    guqinFlowing.styleTags.addArray({ "chinese", "ancient_style", "guqin", "traditional" });
    guqinFlowing.moodTags.addArray({ "calm", "wide", "meditative", "solemn" });
    guqinFlowing.regionTags.addArray({ "chinese" });
    guqinFlowing.functionFit.addArray({ "T", "t" });
    guqinFlowing.formFit.addArray({ "intro", "outro", "cadence" });
    guqinFlowing.familyId = "PedalDroneFamily";
    guqinFlowing.grooveAffinity = "Straight";
    guqinFlowing.bassAnchored = true;
    guqinFlowing.pedalFriendly = true;
    guqinFlowing.complexity = 1;
    guqinFlowing.energy = 1;
    patterns.push_back(guqinFlowing);

    // 琵琶扫弦风格 (短促上行，模拟轮扫)
    auto pipaSweep = makePattern("pipa_sweep", cn(u8"琵琶扫弦"), "Pipa Sweep",
                                 ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 1.0,
                                 { step(0.0, 0.08, 0, -1, 8, true),
                                   step(0.125, 0.08, 2, 0, 2),
                                   step(0.25, 0.08, 1, 0, -1),
                                   step(0.375, 0.08, 3, 1, 5),
                                   step(0.5, 0.08, 2, 0, 1),
                                   step(0.625, 0.08, 4, 1, -2),
                                   step(0.75, 0.08, 3, 1, 3),
                                   step(0.875, 0.08, 0, -1, 7, true) });
    pipaSweep.styleTags.addArray({ "chinese", "ancient_style", "pipa", "folk" });
    pipaSweep.moodTags.addArray({ "bright", "energetic", "playful" });
    pipaSweep.regionTags.addArray({ "chinese" });
    pipaSweep.functionFit.addArray({ "S", "D" });
    pipaSweep.formFit.addArray({ "chorus", "bridge" });
    pipaSweep.familyId = "GuitarPickingFamily";
    pipaSweep.grooveAffinity = "Tight";
    pipaSweep.bassAnchored = true;
    pipaSweep.complexity = 3;
    pipaSweep.energy = 4;
    patterns.push_back(pipaSweep);

    // ========================================================================
    // 弦乐合奏团织体 (String Ensemble)
    // ========================================================================

    // 弦乐拨奏 (Pizzicato) 风格: 跳跃 + 短音
    auto stringPizz = makePattern("string_pizz", cn(u8"弦乐拨奏"), "String Pizzicato",
                                  ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.35, 0, -1, 6, true),
                                    step(0.5, 0.3, 2, 0, 1),
                                    step(1.0, 0.35, 4, 0, -2),
                                    step(1.5, 0.3, 1, 0, 3) });
    stringPizz.styleTags.addArray({ "cinematic", "classical", "string", "pizzicato" });
    stringPizz.moodTags.addArray({ "playful", "bright", "light" });
    stringPizz.functionFit.addArray({ "T", "S", "t" });
    stringPizz.formFit.addArray({ "verse", "bridge" });
    stringPizz.familyId = "GuitarPickingFamily";
    stringPizz.grooveAffinity = "Straight";
    stringPizz.bassAnchored = true;
    stringPizz.complexity = 2;
    stringPizz.energy = 3;
    patterns.push_back(stringPizz);

    // 弦乐颤音 (Tremolo): 快速重复同音
    auto stringTremolo = makePattern("string_tremolo", cn(u8"弦乐颤音"), "String Tremolo",
                                     ArpeggioDirection::BlockPulse, ArpeggioDensity::Sixteenth, 1.0,
                                     { step(0.0, 0.1, 0, -1, 7, true),
                                       step(0.125, 0.1, 0, -1, 6, true),
                                       step(0.25, 0.1, 0, -1, 5, true),
                                       step(0.375, 0.1, 0, -1, 4, true),
                                       step(0.5, 0.1, 0, -1, 7, true),
                                       step(0.625, 0.1, 0, -1, 6, true),
                                       step(0.75, 0.1, 0, -1, 5, true),
                                       step(0.875, 0.1, 0, -1, 4, true) });
    stringTremolo.styleTags.addArray({ "cinematic", "classical", "string", "tremolo" });
    stringTremolo.moodTags.addArray({ "tension", "intense", "dramatic" });
    stringTremolo.functionFit.addArray({ "D", "s" });
    stringTremolo.formFit.addArray({ "bridge", "build-up", "chorus" });
    stringTremolo.familyId = "PadFamily";
    stringTremolo.grooveAffinity = "Tight";
    stringTremolo.bassAnchored = true;
    stringTremolo.complexity = 2;
    stringTremolo.energy = 4;
    patterns.push_back(stringTremolo);

    // 弦乐长弓 (Legato): 宽广跨八度
    auto stringLegato = makePattern("string_legato", cn(u8"弦乐长弓"), "String Legato",
                                    ArpeggioDirection::UpDown, ArpeggioDensity::Half, 4.0,
                                    { step(0.0, 3.5, 0, -1, 6, true),
                                      step(1.2, 2.2, 2, 0, 3),
                                      step(2.5, 1.5, 4, 1, 5) });
    stringLegato.styleTags.addArray({ "cinematic", "classical", "string", "legato" });
    stringLegato.moodTags.addArray({ "wide", "epic", "sad", "warm" });
    stringLegato.functionFit.addArray({ "T", "S", "t", "s" });
    stringLegato.formFit.addArray({ "intro", "verse", "outro" });
    stringLegato.familyId = "CinematicOstinatoFamily";
    stringLegato.grooveAffinity = "Straight";
    stringLegato.bassAnchored = true;
    stringLegato.complexity = 2;
    stringLegato.energy = 2;
    patterns.push_back(stringLegato);

    // ========================================================================
    // 电影 & 史诗 (Cinematic & Epic)
    // ========================================================================

    // 史诗战斗: 低音脉冲 + 上行音阶 (号角感)
    auto epicBattle = makePattern("epic_battle", cn(u8"史诗战斗"), "Epic Battle",
                                  ArpeggioDirection::Up, ArpeggioDensity::Quarter, 2.0,
                                  { step(0.0, 0.45, 0, -1, 9, true),
                                    step(0.5, 0.4, 1, 0, -2),
                                    step(1.0, 0.45, 2, 0, 4),
                                    step(1.5, 0.4, 3, 1, 7) });
    epicBattle.styleTags.addArray({ "cinematic", "epic", "war", "march" });
    epicBattle.moodTags.addArray({ "epic", "tension", "powerful", "energetic" });
    epicBattle.functionFit.addArray({ "D", "S" });
    epicBattle.formFit.addArray({ "chorus", "bridge", "build-up" });
    epicBattle.familyId = "MarchPulseFamily";
    epicBattle.grooveAffinity = "Straight";
    epicBattle.bassAnchored = true;
    epicBattle.complexity = 2;
    epicBattle.energy = 5;
    patterns.push_back(epicBattle);

    // 电影悬念: 半音交替 + 不协和音程
    auto cinematicSuspense = makePattern("cinematic_suspense", cn(u8"电影悬念"), "Cinematic Suspense",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                         { step(0.0, 0.55, 0, -1, 6, true),
                                           step(0.55, 0.45, 1, 0, -3),   // 半音向上
                                           step(1.0, 0.5, 0, 0, 2),       // 回到根音
                                           step(1.5, 0.4, 1, 0, -4) });
    cinematicSuspense.styleTags.addArray({ "cinematic", "soundtrack", "suspense", "horror" });
    cinematicSuspense.moodTags.addArray({ "tension", "dark", "mysterious" });
    cinematicSuspense.functionFit.addArray({ "D", "s" });
    cinematicSuspense.formFit.addArray({ "bridge", "intro", "outro" });
    cinematicSuspense.familyId = "MaqamOrnamentFamily";
    cinematicSuspense.grooveAffinity = "Straight";
    cinematicSuspense.bassAnchored = true;
    cinematicSuspense.supportsExtensions = true;
    cinematicSuspense.complexity = 3;
    cinematicSuspense.energy = 2;
    patterns.push_back(cinematicSuspense);

    // 电影希望/升华: 缓慢上行的开放和声
    auto cinematicHope = makePattern("cinematic_hope", cn(u8"电影希望"), "Cinematic Hope",
                                     ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                     { step(0.0, 1.8, 0, -1, 5, true),
                                       step(1.0, 1.2, 2, 0, 2),
                                       step(2.0, 1.4, 4, 1, 6),
                                       step(3.0, 0.9, 1, 0, -1) });
    cinematicHope.styleTags.addArray({ "cinematic", "soundtrack", "hopeful", "epic" });
    cinematicHope.moodTags.addArray({ "hopeful", "wide", "bright", "epic" });
    cinematicHope.functionFit.addArray({ "T", "S", "t" });
    cinematicHope.formFit.addArray({ "chorus", "outro", "cadence" });
    cinematicHope.familyId = "CinematicOstinatoFamily";
    cinematicHope.grooveAffinity = "Straight";
    cinematicHope.bassAnchored = true;
    cinematicHope.complexity = 2;
    cinematicHope.energy = 3;
    patterns.push_back(cinematicHope);

    // ========================================================================
    // 中国西北风格 (Northwest China)
    // ========================================================================

    // 陕北民歌: 四度跳进 + 高亢呐喊感
    auto shanbeiFolk = makePattern("shanbei_folk", cn(u8"陕北民歌"), "Shanbei Folk",
                                   ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                   { step(0.0, 0.5, 0, -1, 8, true),
                                     step(0.75, 0.4, 4, 0, 4),     // 四度跳
                                     step(1.25, 0.5, 1, 0, -2),
                                     step(1.75, 0.4, 4, 0, 3) });
    shanbeiFolk.styleTags.addArray({ "folk", "chinese", "northwest_wind", "shanbei" });
    shanbeiFolk.moodTags.addArray({ "bright", "hopeful", "energetic", "solemn" });
    shanbeiFolk.regionTags.addArray({ "chinese", "northwest" });
    shanbeiFolk.functionFit.addArray({ "T", "S", "t" });
    shanbeiFolk.formFit.addArray({ "verse", "chorus" });
    shanbeiFolk.familyId = "ChineseOpenFamily";
    shanbeiFolk.grooveAffinity = "Straight";
    shanbeiFolk.bassAnchored = true;
    shanbeiFolk.complexity = 2;
    shanbeiFolk.energy = 4;
    patterns.push_back(shanbeiFolk);

    // 西北风 (信天游风格): 自由延长 + 大跳
    auto xintianyou = makePattern("xintianyou", cn(u8"信天游"), "Xintianyou",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                  { step(0.0, 2.5, 0, -1, 7, true),
                                    step(1.5, 0.9, 4, 0, 5),
                                    step(2.5, 1.2, 1, 0, -3),
                                    step(3.5, 0.5, 4, 0, 4) });
    xintianyou.styleTags.addArray({ "folk", "chinese", "northwest_wind", "xintianyou" });
    xintianyou.moodTags.addArray({ "wide", "solemn", "nostalgic", "bright" });
    xintianyou.regionTags.addArray({ "chinese", "northwest" });
    xintianyou.functionFit.addArray({ "T", "S", "t" });
    xintianyou.formFit.addArray({ "intro", "verse", "outro" });
    xintianyou.familyId = "PedalDroneFamily";
    xintianyou.grooveAffinity = "Straight";
    xintianyou.bassAnchored = true;
    xintianyou.pedalFriendly = true;
    xintianyou.complexity = 2;
    xintianyou.energy = 3;
    patterns.push_back(xintianyou);

    // 西北鼓点式 (腰鼓节奏): 短促低音脉冲 + 交替五音
    auto waistDrum = makePattern("waist_drum", cn(u8"腰鼓节奏"), "Waist Drum Pulse",
                                 ArpeggioDirection::BlockPulse, ArpeggioDensity::Quarter, 2.0,
                                 { step(0.0, 0.35, 0, -1, 8, true),
                                   step(0.5, 0.3, 4, -1, 6, true),
                                   step(1.0, 0.35, 0, -1, 8, true),
                                   step(1.5, 0.3, 4, -1, 5, true) });
    waistDrum.styleTags.addArray({ "folk", "chinese", "northwest_wind", "dance" });
    waistDrum.moodTags.addArray({ "energetic", "playful", "bright" });
    waistDrum.regionTags.addArray({ "chinese", "northwest" });
    waistDrum.functionFit.addArray({ "T", "S", "t" });
    waistDrum.formFit.addArray({ "chorus", "bridge" });
    waistDrum.familyId = "MarchPulseFamily";
    waistDrum.grooveAffinity = "Tight";
    waistDrum.bassAnchored = true;
    waistDrum.complexity = 1;
    waistDrum.energy = 4;
    patterns.push_back(waistDrum);

        // ========================================================================
    // 更多中国少数民族 (More Chinese Minorities)
    // ========================================================================

    // 侗族大歌: 三度叠置 + 平行进行
    auto dongDage = makePattern("dong_dage", cn(u8"侗族大歌"), "Dong Grand Song",
                                ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                { step(0.0, 1.8, 0, -1, 6, true),
                                  step(1.0, 1.2, 2, 0, 2),
                                  step(2.0, 1.5, 1, 0, 4),
                                  step(3.0, 0.9, 2, 0, -1) });
    dongDage.styleTags.addArray({ "folk", "chinese", "dong", "choir" });
    dongDage.moodTags.addArray({ "wide", "calm", "harmonious", "hopeful" });
    dongDage.regionTags.addArray({ "chinese", "dong" });
    dongDage.functionFit.addArray({ "T", "S", "t" });
    dongDage.formFit.addArray({ "verse", "chorus" });
    dongDage.familyId = "ChineseOpenFamily";
    dongDage.grooveAffinity = "Straight";
    dongDage.bassAnchored = true;
    dongDage.complexity = 2;
    dongDage.energy = 2;
    patterns.push_back(dongDage);

    // 羌族多声部: 平行四五度 + 持续低音
    auto qiangPoly = makePattern("qiang_poly", cn(u8"羌族多声部"), "Qiang Polyphony",
                                 ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 4.0,
                                 { step(0.0, 3.5, 0, -1, 7, true),
                                   step(0.0, 3.5, 4, -1, 5, true),
                                   step(2.0, 1.2, 2, 0, -2) });
    qiangPoly.styleTags.addArray({ "folk", "chinese", "qiang", "polyphony" });
    qiangPoly.moodTags.addArray({ "wide", "solemn", "calm" });
    qiangPoly.regionTags.addArray({ "chinese", "qiang" });
    qiangPoly.functionFit.addArray({ "T", "t" });
    qiangPoly.formFit.addArray({ "intro", "verse", "outro" });
    qiangPoly.familyId = "PedalDroneFamily";
    qiangPoly.grooveAffinity = "Straight";
    qiangPoly.bassAnchored = true;
    qiangPoly.pedalFriendly = true;
    qiangPoly.complexity = 2;
    qiangPoly.energy = 2;
    patterns.push_back(qiangPoly);

    // 苗族芦笙舞: 跳跃感 + 短促重复
    auto miaoLusheng = makePattern("miao_lusheng", cn(u8"苗族芦笙"), "Miao Lusheng",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                   { step(0.0, 0.4, 0, -1, 8, true),
                                     step(0.5, 0.35, 4, 0, 3),
                                     step(1.0, 0.4, 0, -1, 7, true),
                                     step(1.5, 0.35, 4, 0, 2) });
    miaoLusheng.styleTags.addArray({ "folk", "chinese", "miao", "dance" });
    miaoLusheng.moodTags.addArray({ "bright", "playful", "energetic" });
    miaoLusheng.regionTags.addArray({ "chinese", "miao" });
    miaoLusheng.functionFit.addArray({ "T", "S", "t" });
    miaoLusheng.formFit.addArray({ "chorus", "dance" });
    miaoLusheng.familyId = "MarchPulseFamily";
    miaoLusheng.grooveAffinity = "Tight";
    miaoLusheng.bassAnchored = true;
    miaoLusheng.complexity = 2;
    miaoLusheng.energy = 4;
    patterns.push_back(miaoLusheng);

    // 彝族阿细跳月: 五度跳 + 切分节奏
    auto axiTiaoyue = makePattern("axi_tiaoyue", cn(u8"彝族阿细跳月"), "Axi Tiaoyue",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                  { step(0.0, 0.35, 0, -1, 8, true),
                                    step(0.75, 0.3, 4, 0, 5),
                                    step(1.25, 0.35, 1, 0, -2),
                                    step(1.75, 0.3, 4, 0, 4) });
    axiTiaoyue.styleTags.addArray({ "folk", "chinese", "yi", "dance" });
    axiTiaoyue.moodTags.addArray({ "bright", "playful", "hopeful" });
    axiTiaoyue.regionTags.addArray({ "chinese", "yi" });
    axiTiaoyue.functionFit.addArray({ "T", "S", "t" });
    axiTiaoyue.formFit.addArray({ "chorus", "bridge" });
    axiTiaoyue.familyId = "SyncopatedElectronicFamily";
    axiTiaoyue.grooveAffinity = "Shuffle";
    axiTiaoyue.bassAnchored = true;
    axiTiaoyue.complexity = 3;
    axiTiaoyue.energy = 4;
    patterns.push_back(axiTiaoyue);

    // 藏族酒歌/弦子: 六度跳跃 + 慢速旋转
    auto xianzi = makePattern("xianzi", cn(u8"藏族弦子"), "Tibetan Xianzi",
                              ArpeggioDirection::UpDown, ArpeggioDensity::Half, 4.0,
                              { step(0.0, 1.5, 0, -1, 6, true),
                                step(1.5, 1.0, 5, 0, 4),  // 六度
                                step(2.5, 1.2, 2, 0, -2) });
    xianzi.styleTags.addArray({ "folk", "chinese", "tibetan", "dance" });
    xianzi.moodTags.addArray({ "wide", "hopeful", "warm" });
    xianzi.regionTags.addArray({ "chinese", "tibetan" });
    xianzi.functionFit.addArray({ "T", "S", "t" });
    xianzi.formFit.addArray({ "verse", "chorus" });
    xianzi.familyId = "ChineseOpenFamily";
    xianzi.grooveAffinity = "Straight";
    xianzi.bassAnchored = true;
    xianzi.complexity = 2;
    xianzi.energy = 3;
    patterns.push_back(xianzi);

    // ========================================================================
    // 日本传统 (Japanese Traditional)
    // ========================================================================

    // 都节音阶 (Miyako-bushi): 小二度下行 + 四度跳
    auto miyakobushi = makePattern("miyakobushi", cn(u8"都节音阶"), "Miyako-bushi",
                                   ArpeggioDirection::Down, ArpeggioDensity::Mixed, 2.0,
                                   { step(0.0, 0.6, 0, -1, 7, true),
                                     step(0.8, 0.4, 1, 0, -3),   // 小二度
                                     step(1.2, 0.5, 0, 0, 2),
                                     step(1.7, 0.4, 3, 1, 5) });
    miyakobushi.styleTags.addArray({ "japanese", "miyakobushi", "traditional" });
    miyakobushi.moodTags.addArray({ "calm", "sad", "elegant", "nostalgic" });
    miyakobushi.regionTags.addArray({ "japan", "okinawa" });
    miyakobushi.functionFit.addArray({ "T", "t", "S" });
    miyakobushi.formFit.addArray({ "intro", "verse", "outro" });
    miyakobushi.familyId = "MaqamOrnamentFamily";
    miyakobushi.grooveAffinity = "Straight";
    miyakobushi.bassAnchored = true;
    miyakobushi.complexity = 3;
    miyakobushi.energy = 2;
    patterns.push_back(miyakobushi);

    // 筝 (Koto) 风格: 拨弦 + 滑音感
    auto koto = makePattern("koto", cn(u8"日本筝"), "Koto",
                            ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                            { step(0.0, 0.45, 0, -1, 6, true),
                              step(0.5, 0.4, 2, 0, 1),
                              step(1.0, 0.45, 4, 0, 4),
                              step(1.5, 0.4, 1, 0, -3) });
    koto.styleTags.addArray({ "japanese", "koto", "traditional" });
    koto.moodTags.addArray({ "calm", "elegant", "flowing" });
    koto.regionTags.addArray({ "japan" });
    koto.functionFit.addArray({ "T", "S", "t" });
    koto.formFit.addArray({ "intro", "verse" });
    koto.familyId = "GuitarPickingFamily";
    koto.grooveAffinity = "Straight";
    koto.bassAnchored = true;
    koto.complexity = 2;
    koto.energy = 2;
    patterns.push_back(koto);

    // 能乐/雅乐 风格: 缓慢 drone + 单音装饰
    auto gagaku = makePattern("gagaku", cn(u8"雅乐"), "Gagaku",
                              ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                              { step(0.0, 3.8, 0, -1, 5, true),
                                step(1.5, 0.6, 2, 0, -2),
                                step(3.0, 0.8, 1, 0, 1) });
    gagaku.styleTags.addArray({ "japanese", "gagaku", "court", "traditional" });
    gagaku.moodTags.addArray({ "calm", "wide", "solemn", "meditative" });
    gagaku.regionTags.addArray({ "japan" });
    gagaku.functionFit.addArray({ "T", "t" });
    gagaku.formFit.addArray({ "intro", "outro" });
    gagaku.familyId = "PedalDroneFamily";
    gagaku.grooveAffinity = "Straight";
    gagaku.bassAnchored = true;
    gagaku.pedalFriendly = true;
    gagaku.complexity = 1;
    gagaku.energy = 1;
    patterns.push_back(gagaku);

    // ========================================================================
    // 巴尔干地区 (Balkan)
    // ========================================================================

    // 巴尔干 7/8 拍 (模拟不规则节奏，使用 cycleBeats 3.5 但 steps 用 beatOffset 实现)
    // 由于 cycleBeats 只能支持单一拍长，我们通过 steps 的 beatOffset 模拟 7/8 feel
    auto balkanSevens = makePattern("balkan_sevens", cn(u8"巴尔干 7/8"), "Balkan 7/8",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 3.5,
                                    { step(0.0, 0.45, 0, -1, 8, true),
                                      step(1.0, 0.4, 2, 0, 2),
                                      step(1.75, 0.35, 1, 0, -3),
                                      step(2.25, 0.5, 3, 1, 5),
                                      step(2.75, 0.4, 2, 0, -1) });
    balkanSevens.styleTags.addArray({ "folk", "balkan", "bulgaria", "romania", "odd_meter" });
    balkanSevens.moodTags.addArray({ "bright", "energetic", "playful", "dance" });
    balkanSevens.regionTags.addArray({ "balkan", "bulgaria", "romania", "greece" });
    balkanSevens.functionFit.addArray({ "S", "D", "t" });
    balkanSevens.formFit.addArray({ "verse", "chorus", "dance" });
    balkanSevens.familyId = "SyncopatedElectronicFamily";
    balkanSevens.grooveAffinity = "Tight";
    balkanSevens.bassAnchored = true;
    balkanSevens.complexity = 3;
    balkanSevens.energy = 4;
    patterns.push_back(balkanSevens);

    // 保加利亚女性合唱 (平行三度 + 持续)
    auto bulgarianChoir = makePattern("bulgarian_choir", cn(u8"保加利亚合唱"), "Bulgarian Choir",
                                      ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 4.0,
                                      { step(0.0, 3.2, 0, -1, 6, true),
                                        step(0.5, 2.5, 2, 0, 3),
                                        step(2.0, 1.8, 1, 0, 4) });
    bulgarianChoir.styleTags.addArray({ "folk", "balkan", "bulgaria", "choir" });
    bulgarianChoir.moodTags.addArray({ "wide", "calm", "harmonious", "mysterious" });
    bulgarianChoir.regionTags.addArray({ "balkan", "bulgaria" });
    bulgarianChoir.functionFit.addArray({ "T", "t", "S" });
    bulgarianChoir.formFit.addArray({ "intro", "verse", "outro" });
    bulgarianChoir.familyId = "PadFamily";
    bulgarianChoir.grooveAffinity = "Straight";
    bulgarianChoir.bassAnchored = true;
    bulgarianChoir.complexity = 2;
    bulgarianChoir.energy = 2;
    patterns.push_back(bulgarianChoir);

    // 罗马尼亚 Doina 风格: 自由装饰 + 下行哭泣感
    auto doina = makePattern("doina", cn(u8"罗马尼亚多依纳"), "Romanian Doina",
                             ArpeggioDirection::Down, ArpeggioDensity::Mixed, 4.0,
                             { step(0.0, 1.2, 0, -1, 7, true),
                               step(1.2, 0.6, 1, 0, -3),
                               step(1.8, 0.5, 2, 0, 2),
                               step(2.3, 0.4, 1, 0, -4),
                               step(2.7, 0.8, 0, 0, -1) });
    doina.styleTags.addArray({ "folk", "balkan", "romania", "doina" });
    doina.moodTags.addArray({ "sad", "nostalgic", "melancholy", "wide" });
    doina.regionTags.addArray({ "balkan", "romania" });
    doina.functionFit.addArray({ "T", "t", "S" });
    doina.formFit.addArray({ "intro", "verse", "outro" });
    doina.familyId = "MaqamOrnamentFamily";
    doina.grooveAffinity = "Straight";
    doina.bassAnchored = true;
    doina.complexity = 3;
    doina.energy = 2;
    patterns.push_back(doina);

    // ========================================================================
    // 其他世界音乐 (Other World Music)
    // ========================================================================

    // 非洲马里 Balafon 风格: 快速重复音型
    auto balafon = makePattern("balafon", cn(u8"非洲巴啦风"), "Balafon",
                               ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                               { step(0.0, 0.1, 0, -1, 7, true),
                                 step(0.125, 0.1, 2, 0, 1),
                                 step(0.25, 0.1, 1, 0, -2),
                                 step(0.375, 0.1, 3, 1, 4),
                                 step(0.5, 0.1, 2, 0, -1),
                                 step(0.625, 0.1, 4, 1, 6),
                                 step(0.75, 0.1, 3, 1, 2),
                                 step(0.875, 0.1, 0, -1, 5, true) });
    balafon.styleTags.addArray({ "folk", "african", "mali", "balafon" });
    balafon.moodTags.addArray({ "bright", "energetic", "playful" });
    balafon.regionTags.addArray({ "africa", "west-africa" });
    balafon.functionFit.addArray({ "T", "S", "t" });
    balafon.formFit.addArray({ "verse", "chorus" });
    balafon.familyId = "GamelanOstinatoFamily";
    balafon.grooveAffinity = "Tight";
    balafon.complexity = 3;
    balafon.energy = 4;
    patterns.push_back(balafon);

    // 蒙古长调风格 (已经有过呼麦低音，这里补充更旋律性的)
    auto mongolianLongSong = makePattern("mongolian_long", cn(u8"蒙古长调"), "Mongolian Long Song",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                         { step(0.0, 2.5, 0, -1, 6, true),
                                           step(1.0, 1.2, 2, 0, 2),
                                           step(2.0, 1.5, 4, 1, 5),
                                           step(3.0, 0.9, 1, 0, -2) });
    mongolianLongSong.styleTags.addArray({ "folk", "mongolian", "long_song", "traditional" });
    mongolianLongSong.moodTags.addArray({ "wide", "calm", "nostalgic", "epic" });
    mongolianLongSong.regionTags.addArray({ "mongolia", "chinese" });
    mongolianLongSong.functionFit.addArray({ "T", "t", "S" });
    mongolianLongSong.formFit.addArray({ "intro", "verse", "outro" });
    mongolianLongSong.familyId = "CinematicOstinatoFamily";
    mongolianLongSong.grooveAffinity = "Straight";
    mongolianLongSong.bassAnchored = true;
    mongolianLongSong.complexity = 2;
    mongolianLongSong.energy = 2;
    patterns.push_back(mongolianLongSong);

    // 爱尔兰 jig (6/8 拍，使用 cycleBeats 1.5 模拟)
    auto irishJig = makePattern("irish_jig", cn(u8"爱尔兰吉格"), "Irish Jig",
                                ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 1.5,
                                { step(0.0, 0.35, 0, -1, 7, true),
                                  step(0.5, 0.3, 2, 0, 2),
                                  step(1.0, 0.35, 1, 0, -1) });
    irishJig.styleTags.addArray({ "folk", "celtic", "irish", "jig" });
    irishJig.moodTags.addArray({ "bright", "playful", "energetic" });
    irishJig.regionTags.addArray({ "ireland", "celtic" });
    irishJig.functionFit.addArray({ "T", "S", "t" });
    irishJig.formFit.addArray({ "verse", "chorus", "dance" });
    irishJig.familyId = "GuitarPickingFamily";
    irishJig.grooveAffinity = "Straight";
    irishJig.bassAnchored = true;
    irishJig.complexity = 2;
    irishJig.energy = 3;
    patterns.push_back(irishJig);

    // 印度西塔琴装饰 (更丰富的 ornament)
    auto sitarGamak = makePattern("sitar_gamak", cn(u8"西塔琴装饰"), "Sitar Gamak",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                  { step(0.0, 1.2, 0, -1, 7, true),
                                    step(0.6, 0.3, 1, 0, -4),
                                    step(0.9, 0.2, 0, 0, 2),
                                    step(1.2, 0.8, 2, 0, 3),
                                    step(2.0, 1.0, 4, 1, 5),
                                    step(2.5, 0.4, 3, 1, -2),
                                    step(3.0, 0.6, 2, 0, 1) });
    sitarGamak.styleTags.addArray({ "indian", "raga", "sitar", "ornamental" });
    sitarGamak.moodTags.addArray({ "meditative", "mysterious", "ornamental" });
    sitarGamak.regionTags.addArray({ "india", "south-asia" });
    sitarGamak.functionFit.addArray({ "T", "t", "S", "D" });
    sitarGamak.formFit.addArray({ "intro", "verse", "bridge" });
    sitarGamak.familyId = "MaqamOrnamentFamily";
    sitarGamak.grooveAffinity = "Straight";
    sitarGamak.bassAnchored = true;
    sitarGamak.pedalFriendly = true;
    sitarGamak.supportsExtensions = true;
    sitarGamak.complexity = 4;
    sitarGamak.energy = 2;
    patterns.push_back(sitarGamak);

    // 犹太 Klezmer: 半音装饰 + 弗利吉亚色彩
    auto klezmer = makePattern("klezmer", cn(u8"克莱兹默"), "Klezmer",
                               ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                               { step(0.0, 0.5, 0, -1, 8, true),
                                 step(0.6, 0.3, 2, 0, -3),
                                 step(0.9, 0.2, 1, 0, 2),
                                 step(1.2, 0.5, 0, 0, 4),
                                 step(1.7, 0.4, 3, 1, 6) });
    klezmer.styleTags.addArray({ "folk", "jewish", "klezmer", "european" });
    klezmer.moodTags.addArray({ "playful", "sad", "nostalgic", "dance" });
    klezmer.regionTags.addArray({ "jewish", "eastern-europe" });
    klezmer.functionFit.addArray({ "T", "S", "D", "s" });
    klezmer.formFit.addArray({ "verse", "chorus", "bridge" });
    klezmer.familyId = "MaqamOrnamentFamily";
    klezmer.grooveAffinity = "Shuffle";
    klezmer.bassAnchored = true;
    klezmer.complexity = 3;
    klezmer.energy = 3;
    patterns.push_back(klezmer);

    // ========================================================================
    // 中国民谣 (Chinese Folk)
    // ========================================================================

    // 校园民谣风格: 简单清新的八分分解，以根音、五音为主
    auto campusFolk = makePattern("campus_folk", cn(u8"校园民谣"), "Campus Folk",
                                  ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.55, 0, -1, 6, true),
                                    step(0.75, 0.45, 4, 0, 1),
                                    step(1.25, 0.55, 1, 0, -2),
                                    step(1.75, 0.45, 4, 0, 2) });
    campusFolk.styleTags.addArray({ "folk", "chinese", "c-pop", "acoustic" });
    campusFolk.moodTags.addArray({ "hopeful", "bright", "nostalgic", "warm" });
    campusFolk.regionTags.addArray({ "chinese" });
    campusFolk.functionFit.addArray({ "T", "S", "t" });
    campusFolk.formFit.addArray({ "verse", "chorus" });
    campusFolk.familyId = "GuitarPickingFamily";
    campusFolk.grooveAffinity = "Straight";
    campusFolk.bassAnchored = true;
    campusFolk.complexity = 2;
    campusFolk.energy = 2;
    patterns.push_back(campusFolk);

    // 城市民谣 (如赵雷风格): 略带切分，强调根音与三音交替
    auto urbanFolk = makePattern("urban_folk", cn(u8"城市民谣"), "Urban Folk",
                                 ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                 { step(0.0, 0.7, 0, -1, 7, true),
                                   step(0.7, 0.45, 2, 0, -3),
                                   step(1.3, 0.6, 0, -1, 5, true),
                                   step(1.9, 0.4, 2, 0, -1) });
    urbanFolk.styleTags.addArray({ "folk", "chinese", "indie_folk" });
    urbanFolk.moodTags.addArray({ "sad", "nostalgic", "calm", "warm" });
    urbanFolk.regionTags.addArray({ "chinese" });
    urbanFolk.functionFit.addArray({ "T", "S", "t", "s" });
    urbanFolk.formFit.addArray({ "verse", "bridge" });
    urbanFolk.familyId = "BrokenPianoFamily";
    urbanFolk.grooveAffinity = "Late";
    urbanFolk.bassAnchored = true;
    urbanFolk.complexity = 2;
    urbanFolk.energy = 2;
    patterns.push_back(urbanFolk);

    // ========================================================================
    // 中国伤感情歌 (Chinese Sad Ballad)
    // ========================================================================

    // 慢速下行分解，强调小三度，情感浓烈
    auto sadBalladDown = makePattern("sad_ballad_down", cn(u8"伤感下行"), "Sad Ballad Down",
                                     ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                     { step(0.0, 1.2, 0, -1, 6, true),
                                       step(1.2, 1.0, 1, 0, -2),
                                       step(2.2, 0.8, 2, 0, 1),
                                       step(3.0, 0.9, 1, 0, -3) });
    sadBalladDown.styleTags.addArray({ "pop", "ballad", "c-pop", "chinese" });
    sadBalladDown.moodTags.addArray({ "sad", "emotional", "warm", "calm" });
    sadBalladDown.regionTags.addArray({ "chinese" });
    sadBalladDown.functionFit.addArray({ "T", "S", "t", "s" });
    sadBalladDown.formFit.addArray({ "verse", "outro", "cadence" });
    sadBalladDown.familyId = "BrokenPianoFamily";
    sadBalladDown.grooveAffinity = "Straight";
    sadBalladDown.bassAnchored = true;
    sadBalladDown.complexity = 2;
    sadBalladDown.energy = 1;
    patterns.push_back(sadBalladDown);

    // 钢琴琶音带延迟 (像《后来》风格)
    auto delayedArpeggio = makePattern("delayed_arpeggio", cn(u8"延迟琶音"), "Delayed Arpeggio",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                       { step(0.0, 1.0, 0, -1, 5, true),
                                         step(0.9, 0.5, 2, 0, -1),
                                         step(1.8, 0.7, 1, 0, 2),
                                         step(2.7, 0.6, 3, 1, 4),
                                         step(3.5, 0.5, 2, 0, -2) });
    delayedArpeggio.styleTags.addArray({ "pop", "ballad", "c-pop", "piano" });
    delayedArpeggio.moodTags.addArray({ "sad", "nostalgic", "dreamy", "emotional" });
    delayedArpeggio.regionTags.addArray({ "chinese" });
    delayedArpeggio.functionFit.addArray({ "T", "S", "t", "s" });
    delayedArpeggio.formFit.addArray({ "verse", "chorus" });
    delayedArpeggio.familyId = "BrokenPianoFamily";
    delayedArpeggio.grooveAffinity = "Late";
    delayedArpeggio.bassAnchored = true;
    delayedArpeggio.complexity = 3;
    delayedArpeggio.energy = 2;
    patterns.push_back(delayedArpeggio);

    // ========================================================================
    // 中国摇滚 (Chinese Rock)
    // ========================================================================

    // 硬摇滚强力和弦琶音 (五度为主，低音厚实)
    auto powerChordArp = makePattern("power_chord_arp", cn(u8"强力和弦琶音"), "Power Chord Arp",
                                     ArpeggioDirection::Up, ArpeggioDensity::Quarter, 2.0,
                                     { step(0.0, 0.35, 0, -1, 8, true),
                                       step(0.5, 0.3, 4, -1, 6, true),
                                       step(1.0, 0.35, 0, -1, 7, true),
                                       step(1.5, 0.3, 4, -1, 5, true) });
    powerChordArp.styleTags.addArray({ "rock", "chinese_rock", "hard_rock", "metal" });
    powerChordArp.moodTags.addArray({ "energetic", "powerful", "intense" });
    powerChordArp.regionTags.addArray({ "chinese" });
    powerChordArp.functionFit.addArray({ "S", "D" });
    powerChordArp.formFit.addArray({ "chorus", "bridge", "build-up" });
    powerChordArp.familyId = "MarchPulseFamily";
    powerChordArp.grooveAffinity = "Tight";
    powerChordArp.bassAnchored = true;
    powerChordArp.complexity = 2;
    powerChordArp.energy = 5;
    patterns.push_back(powerChordArp);

    // 流行摇滚 (如五月天风格): 八分音符分解加切分
    auto popRockArp = makePattern("pop_rock_arp", cn(u8"流行摇滚琶音"), "Pop Rock Arp",
                                  ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.4, 0, -1, 7, true),
                                    step(0.5, 0.35, 2, 0, -1),
                                    step(1.0, 0.4, 1, 0, 2),
                                    step(1.5, 0.35, 2, 0, -2) });
    popRockArp.styleTags.addArray({ "rock", "pop", "chinese_rock" });
    popRockArp.moodTags.addArray({ "bright", "hopeful", "energetic" });
    popRockArp.regionTags.addArray({ "chinese" });
    popRockArp.functionFit.addArray({ "T", "S", "D" });
    popRockArp.formFit.addArray({ "verse", "chorus" });
    popRockArp.familyId = "GuitarPickingFamily";
    popRockArp.grooveAffinity = "Straight";
    popRockArp.bassAnchored = true;
    popRockArp.complexity = 2;
    popRockArp.energy = 4;
    patterns.push_back(popRockArp);

    // ========================================================================
    // 中国戏曲 (Chinese Opera)
    // ========================================================================

    // 京剧过门风格: 高音区快速装饰，五声调式
    auto pekingOpera = makePattern("peking_opera", cn(u8"京剧过门"), "Peking Opera",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.12, 0, 1, 8, true),
                                     step(0.125, 0.1, 2, 1, 4),
                                     step(0.25, 0.12, 1, 1, -2),
                                     step(0.375, 0.1, 3, 2, 6),
                                     step(0.5, 0.12, 2, 1, 1),
                                     step(0.625, 0.1, 4, 2, -3),
                                     step(0.75, 0.12, 3, 2, 5),
                                     step(0.875, 0.1, 0, 1, 7, true) });
    pekingOpera.styleTags.addArray({ "chinese_opera", "peking_opera", "traditional" });
    pekingOpera.moodTags.addArray({ "bright", "playful", "dramatic" });
    pekingOpera.regionTags.addArray({ "chinese" });
    pekingOpera.functionFit.addArray({ "S", "D" });
    pekingOpera.formFit.addArray({ "intro", "bridge", "chorus" });
    pekingOpera.familyId = "ChineseOpenFamily";
    pekingOpera.grooveAffinity = "Tight";
    pekingOpera.bassAnchored = false;
    pekingOpera.complexity = 4;
    pekingOpera.energy = 4;
    patterns.push_back(pekingOpera);

    // 昆曲水磨腔风格: 悠长婉转，慢速上行
    auto kunqu = makePattern("kunqu", cn(u8"昆曲水磨"), "Kunqu",
                             ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                             { step(0.0, 2.0, 0, -1, 5, true),
                               step(1.2, 1.2, 2, 0, -1),
                               step(2.5, 1.0, 4, 1, 3),
                               step(3.5, 0.5, 1, 0, -2) });
    kunqu.styleTags.addArray({ "chinese_opera", "kunqu", "traditional" });
    kunqu.moodTags.addArray({ "calm", "elegant", "sad" });
    kunqu.regionTags.addArray({ "chinese" });
    kunqu.functionFit.addArray({ "T", "S", "t" });
    kunqu.formFit.addArray({ "intro", "verse", "outro" });
    kunqu.familyId = "PedalDroneFamily";
    kunqu.grooveAffinity = "Straight";
    kunqu.bassAnchored = true;
    kunqu.complexity = 2;
    kunqu.energy = 2;
    patterns.push_back(kunqu);

    // ========================================================================
    // K-Pop (韩流)
    // ========================================================================

    // 典型 K-Pop 合成器琶音: 十六分音符 gate，明亮，切分
    auto kpopGate = makePattern("kpop_gate", cn(u8"K-Pop 门限"), "K-Pop Gate",
                                ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 1.0,
                                { step(0.0, 0.12, 0, -1, 7, true),
                                  step(0.25, 0.1, 2, 0, 2),
                                  step(0.5, 0.12, 1, 0, -1),
                                  step(0.75, 0.1, 3, 1, 5),
                                  step(1.0, 0.12, 2, 0, 1),
                                  step(1.25, 0.1, 0, -1, 6, true),
                                  step(1.5, 0.12, 3, 1, 3),
                                  step(1.75, 0.1, 1, 0, -2) });
    kpopGate.styleTags.addArray({ "k-pop", "dance", "electro", "synth" });
    kpopGate.moodTags.addArray({ "bright", "energetic", "playful" });
    kpopGate.regionTags.addArray({ "korean" });
    kpopGate.functionFit.addArray({ "S", "D" });
    kpopGate.formFit.addArray({ "chorus", "bridge" });
    kpopGate.familyId = "SyncopatedElectronicFamily";
    kpopGate.grooveAffinity = "Tight";
    kpopGate.bassAnchored = true;
    kpopGate.complexity = 3;
    kpopGate.energy = 5;
    patterns.push_back(kpopGate);

    // K-Pop 抒情 (如 IU 风格): 温暖钢琴分解
    auto kpopBallad = makePattern("kpop_ballad", cn(u8"K-Pop 抒情"), "K-Pop Ballad",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.65, 0, -1, 5, true),
                                    step(0.65, 0.45, 2, 0, -2),
                                    step(1.3, 0.55, 1, 0, 1),
                                    step(1.9, 0.45, 2, 0, -1) });
    kpopBallad.styleTags.addArray({ "k-pop", "ballad", "pop" });
    kpopBallad.moodTags.addArray({ "warm", "sad", "nostalgic" });
    kpopBallad.regionTags.addArray({ "korean" });
    kpopBallad.functionFit.addArray({ "T", "S", "t", "s" });
    kpopBallad.formFit.addArray({ "verse", "chorus" });
    kpopBallad.familyId = "BrokenPianoFamily";
    kpopBallad.grooveAffinity = "Straight";
    kpopBallad.bassAnchored = true;
    kpopBallad.complexity = 2;
    kpopBallad.energy = 2;
    patterns.push_back(kpopBallad);

    // ========================================================================
    // J-Pop & Anime (日系流行/动漫)
    // ========================================================================

    // J-Pop 明亮上行 (如 Radwimps 风格)
    auto jpopUp = makePattern("jpop_up", cn(u8"J-Pop 上行"), "J-Pop Up",
                              ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                              { step(0.0, 0.45, 0, -1, 7, true),
                                step(0.5, 0.4, 1, 0, -1),
                                step(1.0, 0.45, 2, 0, 2),
                                step(1.5, 0.4, 3, 1, 5) });
    jpopUp.styleTags.addArray({ "j-pop", "anime", "pop" });
    jpopUp.moodTags.addArray({ "bright", "hopeful", "energetic" });
    jpopUp.regionTags.addArray({ "japanese" });
    jpopUp.functionFit.addArray({ "T", "S", "D" });
    jpopUp.formFit.addArray({ "chorus", "bridge" });
    jpopUp.familyId = "BrokenPianoFamily";
    jpopUp.grooveAffinity = "Straight";
    jpopUp.bassAnchored = true;
    jpopUp.complexity = 2;
    jpopUp.energy = 4;
    patterns.push_back(jpopUp);

    // J-Pop 切分节奏 (如 Official髭男dism 风格)
    auto jpopSync = makePattern("jpop_sync", cn(u8"J-Pop 切分"), "J-Pop Sync",
                                ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                { step(0.0, 0.3, 0, -1, 6, true),
                                  step(0.375, 0.25, 2, 0, -2),
                                  step(0.875, 0.35, 1, 0, 3),
                                  step(1.25, 0.28, 3, 1, 5),
                                  step(1.625, 0.3, 2, 0, -1) });
    jpopSync.styleTags.addArray({ "j-pop", "jpop", "anime" });
    jpopSync.moodTags.addArray({ "bright", "playful", "energetic" });
    jpopSync.regionTags.addArray({ "japanese" });
    jpopSync.functionFit.addArray({ "S", "D" });
    jpopSync.formFit.addArray({ "chorus", "bridge" });
    jpopSync.familyId = "SyncopatedElectronicFamily";
    jpopSync.grooveAffinity = "Tight";
    jpopSync.bassAnchored = true;
    jpopSync.complexity = 3;
    jpopSync.energy = 4;
    patterns.push_back(jpopSync);

    // 动漫片头曲风格: 快速八分 + 十六分混合，高能量
    auto animeOpen = makePattern("anime_opening", cn(u8"动漫片头"), "Anime Opening",
                                 ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 1.0,
                                 { step(0.0, 0.12, 0, -1, 8, true),
                                   step(0.125, 0.1, 2, 0, 2),
                                   step(0.25, 0.12, 3, 1, 4),
                                   step(0.375, 0.1, 1, 0, -2),
                                   step(0.5, 0.12, 2, 0, 1),
                                   step(0.625, 0.1, 0, -1, 6, true),
                                   step(0.75, 0.12, 3, 1, 5),
                                   step(0.875, 0.1, 2, 0, -1) });
    animeOpen.styleTags.addArray({ "anime", "j-pop", "rock" });
    animeOpen.moodTags.addArray({ "energetic", "bright", "hopeful", "epic" });
    animeOpen.regionTags.addArray({ "japanese" });
    animeOpen.functionFit.addArray({ "S", "D" });
    animeOpen.formFit.addArray({ "chorus", "intro" });
    animeOpen.familyId = "EDMTranceFamily";
    animeOpen.grooveAffinity = "Tight";
    animeOpen.bassAnchored = true;
    animeOpen.complexity = 4;
    animeOpen.energy = 5;
    patterns.push_back(animeOpen);

    // ========================================================================
    // 补充少数民族 (Extra Minorities)
    // ========================================================================

    // 侗族大歌风格: 平行三度/六度，长气息
    auto dongChorus = makePattern("dong_chorus", cn(u8"侗族大歌"), "Dong Chorus",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                  { step(0.0, 2.5, 0, -1, 6, true),
                                    step(0.0, 2.5, 2, -1, 5, true),   // 三度平行
                                    step(2.0, 1.5, 4, 0, 3),
                                    step(3.2, 0.8, 1, 0, -2) });
    dongChorus.styleTags.addArray({ "folk", "chinese", "ethnic", "dong" });
    dongChorus.moodTags.addArray({ "wide", "calm", "hypnotic", "harmonious" });
    dongChorus.regionTags.addArray({ "chinese", "dong" });
    dongChorus.functionFit.addArray({ "T", "t", "S" });
    dongChorus.formFit.addArray({ "intro", "verse" });
    dongChorus.familyId = "PedalDroneFamily";
    dongChorus.grooveAffinity = "Straight";
    dongChorus.bassAnchored = true;
    dongChorus.pedalFriendly = true;
    dongChorus.complexity = 2;
    dongChorus.energy = 2;
    patterns.push_back(dongChorus);

    // 傣族象脚鼓舞风格: 低音脉冲 + 快速五度跳
    auto daiDrum = makePattern("dai_drum", cn(u8"傣族象脚鼓"), "Dai Drum",
                               ArpeggioDirection::BlockPulse, ArpeggioDensity::Quarter, 2.0,
                               { step(0.0, 0.35, 0, -1, 8, true),
                                 step(0.5, 0.3, 4, 0, 4),
                                 step(1.0, 0.35, 0, -1, 7, true),
                                 step(1.5, 0.3, 4, 0, 5) });
    daiDrum.styleTags.addArray({ "folk", "chinese", "ethnic", "dai" });
    daiDrum.moodTags.addArray({ "energetic", "playful", "dance" });
    daiDrum.regionTags.addArray({ "chinese", "dai" });
    daiDrum.functionFit.addArray({ "T", "S" });
    daiDrum.formFit.addArray({ "chorus", "dance" });
    daiDrum.familyId = "MarchPulseFamily";
    daiDrum.grooveAffinity = "Tight";
    daiDrum.bassAnchored = true;
    daiDrum.complexity = 1;
    daiDrum.energy = 4;
    patterns.push_back(daiDrum);

    // 彝族火把节风格: 五声性跳进，热情
    auto yiTorch = makePattern("yi_torch", cn(u8"彝族火把节"), "Yi Torch Festival",
                               ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                               { step(0.0, 0.45, 0, -1, 8, true),
                                 step(0.5, 0.4, 2, 0, -2),
                                 step(1.0, 0.45, 4, 0, 4),
                                 step(1.5, 0.4, 2, 0, 1) });
    yiTorch.styleTags.addArray({ "folk", "chinese", "ethnic", "yi" });
    yiTorch.moodTags.addArray({ "bright", "energetic", "playful" });
    yiTorch.regionTags.addArray({ "chinese", "yi" });
    yiTorch.functionFit.addArray({ "T", "S", "t" });
    yiTorch.formFit.addArray({ "chorus", "bridge" });
    yiTorch.familyId = "ChineseOpenFamily";
    yiTorch.grooveAffinity = "Tight";
    yiTorch.bassAnchored = true;
    yiTorch.complexity = 2;
    yiTorch.energy = 4;
    patterns.push_back(yiTorch);

    // ========================================================================
    // 日本传统 (Japanese Traditional)
    // ========================================================================

    // 都节音阶风格 (Yonanuki): 强调小二度下行，如樱花
    auto miyakoBushi = makePattern("miyako_bushi", cn(u8"都节音阶"), "Miyako Bushi",
                                   ArpeggioDirection::Down, ArpeggioDensity::Eighth, 2.0,
                                   { step(0.0, 0.6, 0, -1, 6, true),
                                     step(0.6, 0.4, 1, 0, -3),   // 小二度
                                     step(1.0, 0.5, 0, 0, 2),
                                     step(1.6, 0.4, 1, 0, -2) });
    miyakoBushi.styleTags.addArray({ "japanese", "traditional", "miyako_bushi" });
    miyakoBushi.moodTags.addArray({ "sad", "calm", "nostalgic" });
    miyakoBushi.regionTags.addArray({ "japanese" });
    miyakoBushi.functionFit.addArray({ "T", "t", "S" });
    miyakoBushi.formFit.addArray({ "intro", "verse", "outro" });
    miyakoBushi.familyId = "MaqamOrnamentFamily";
    miyakoBushi.grooveAffinity = "Straight";
    miyakoBushi.bassAnchored = true;
    miyakoBushi.complexity = 2;
    miyakoBushi.energy = 2;
    patterns.push_back(miyakoBushi);

    // 琉球音阶风格 (冲绳): 明亮，多用三度
    auto ryukyu = makePattern("ryukyu", cn(u8"琉球音阶"), "Ryukyu Scale",
                              ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                              { step(0.0, 0.5, 0, -1, 7, true),
                                step(0.5, 0.45, 2, 0, -1),
                                step(1.0, 0.5, 3, 0, 3),
                                step(1.5, 0.45, 1, 0, -2) });
    ryukyu.styleTags.addArray({ "japanese", "traditional", "ryukyu", "okinawa" });
    ryukyu.moodTags.addArray({ "bright", "hopeful", "playful" });
    ryukyu.regionTags.addArray({ "japanese", "okinawa" });
    ryukyu.functionFit.addArray({ "T", "S" });
    ryukyu.formFit.addArray({ "verse", "chorus" });
    ryukyu.familyId = "GuitarPickingFamily";
    ryukyu.grooveAffinity = "Straight";
    ryukyu.bassAnchored = true;
    ryukyu.complexity = 2;
    ryukyu.energy = 3;
    patterns.push_back(ryukyu);

    // 日本能乐风格: 极慢，留白，单音持续
    auto nohgaku = makePattern("nohgaku", cn(u8"能乐"), "Nohgaku",
                               ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                               { step(0.0, 3.8, 0, -1, 5, true),
                                 step(2.0, 1.8, 4, -1, 4, true) });
    nohgaku.styleTags.addArray({ "japanese", "traditional", "noh" });
    nohgaku.moodTags.addArray({ "calm", "wide", "solemn", "meditative" });
    nohgaku.regionTags.addArray({ "japanese" });
    nohgaku.functionFit.addArray({ "T", "t" });
    nohgaku.formFit.addArray({ "intro", "outro" });
    nohgaku.familyId = "PedalDroneFamily";
    nohgaku.grooveAffinity = "Straight";
    nohgaku.bassAnchored = true;
    nohgaku.pedalFriendly = true;
    nohgaku.complexity = 1;
    nohgaku.energy = 1;
    patterns.push_back(nohgaku);

    // ========================================================================
    // 巴尔干 (Balkan) 风格
    // ========================================================================

    // 保加利亚女性合唱风格: 不协和二度，微差音 (用 step 模拟)
    auto bulgarianPoly = makePattern("bulgarian_poly", cn(u8"保加利亚复音"), "Bulgarian Polyphony",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                     { step(0.0, 1.8, 0, -1, 6, true),
                                       step(0.0, 1.8, 1, 0, 4, true),   // 二度叠置
                                       step(2.0, 1.2, 2, 0, 2),
                                       step(3.2, 0.8, 3, 1, 5) });
    bulgarianPoly.styleTags.addArray({ "folk", "balkan", "bulgaria" });
    bulgarianPoly.moodTags.addArray({ "mysterious", "wide", "intense" });
    bulgarianPoly.regionTags.addArray({ "balkan", "bulgaria" });
    bulgarianPoly.functionFit.addArray({ "T", "S", "t", "s" });
    bulgarianPoly.formFit.addArray({ "verse", "chorus" });
    bulgarianPoly.familyId = "PadFamily";
    bulgarianPoly.grooveAffinity = "Straight";
    bulgarianPoly.bassAnchored = true;
    bulgarianPoly.complexity = 3;
    bulgarianPoly.energy = 3;
    patterns.push_back(bulgarianPoly);

    // 罗马尼亚霍拉舞曲风格: 切分，快速上行
    auto hora = makePattern("hora", cn(u8"霍拉舞曲"), "Hora",
                            ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 1.0,
                            { step(0.0, 0.12, 0, -1, 7, true),
                              step(0.25, 0.1, 2, 0, 2),
                              step(0.5, 0.12, 1, 0, -1),
                              step(0.75, 0.1, 3, 1, 5),
                              step(1.0, 0.12, 2, 0, 1),
                              step(1.25, 0.1, 0, -1, 6, true),
                              step(1.5, 0.12, 3, 1, 4),
                              step(1.75, 0.1, 1, 0, -2) });
    hora.styleTags.addArray({ "folk", "balkan", "romania", "dance" });
    hora.moodTags.addArray({ "bright", "energetic", "playful" });
    hora.regionTags.addArray({ "balkan", "romania" });
    hora.functionFit.addArray({ "S", "D" });
    hora.formFit.addArray({ "chorus", "dance" });
    hora.familyId = "SyncopatedElectronicFamily";
    hora.grooveAffinity = "Tight";
    hora.bassAnchored = true;
    hora.complexity = 3;
    hora.energy = 5;
    patterns.push_back(hora);


    // ========================================================================
    // 中国古风 - 新细分家族 (Guofeng / Ancient Chinese)
    // ========================================================================

    // 古风抒情流动 (GuofengFlowFamily): 适合 intro/verse，八分混合，根音+五度+上方装饰
    auto guofengFlow = makePattern("guofeng_flow", cn(u8"古风流韵"), "Guofeng Flowing",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                   { step(0.0, 1.2, 0, -1, 5, true),
                                     step(0.8, 0.6, 4, 0, 2),
                                     step(1.8, 0.9, 2, 0, -1),
                                     step(2.8, 0.7, 5, 1, 4),
                                     step(3.5, 0.5, 1, 0, -2) });
    guofengFlow.styleTags.addArray({ "chinese", "ancient_style", "guofeng", "cinematic" });
    guofengFlow.moodTags.addArray({ "elegant", "calm", "flowing", "wide" });
    guofengFlow.regionTags.addArray({ "chinese" });
    guofengFlow.functionFit.addArray({ "T", "S", "t", "s" });
    guofengFlow.formFit.addArray({ "intro", "verse", "bridge" });
    guofengFlow.familyId = "GuofengFlowFamily";
    guofengFlow.grooveAffinity = "Straight";
    guofengFlow.bassAnchored = true;
    guofengFlow.pedalFriendly = true;
    guofengFlow.complexity = 3;
    guofengFlow.energy = 2;
    patterns.push_back(guofengFlow);

    auto guofengRising = makePattern("guofeng_rising", cn(u8"古风升华"), "Guofeng Rising",
                                     ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.6, 0, -1, 6, true),
                                       step(0.6, 0.5, 2, 0, -1),
                                       step(1.2, 0.6, 4, 1, 3),
                                       step(1.8, 0.5, 1, 0, -2) });
    guofengRising.styleTags.addArray({ "chinese", "ancient_style", "guofeng", "cinematic" });
    guofengRising.moodTags.addArray({ "hopeful", "bright", "epic" });
    guofengRising.regionTags.addArray({ "chinese" });
    guofengRising.functionFit.addArray({ "T", "S", "D" });
    guofengRising.formFit.addArray({ "chorus", "bridge" });
    guofengRising.familyId = "GuofengFlowFamily";
    guofengRising.grooveAffinity = "Straight";
    guofengRising.bassAnchored = true;
    guofengRising.complexity = 2;
    guofengRising.energy = 3;
    patterns.push_back(guofengRising);

    // 古筝轮指/刮奏 (GuzhengRollFamily): 短密度，同音重复，上方八度
    auto guzhengRoll = makePattern("guzheng_roll", cn(u8"古筝轮指"), "Guzheng Roll",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.12, 0, -1, 6, true),
                                     step(0.125, 0.1, 0, -1, 5, true),
                                     step(0.25, 0.12, 0, -1, 7, true),
                                     step(0.375, 0.1, 0, -1, 6, true),
                                     step(0.5, 0.12, 2, 0, 2),
                                     step(0.625, 0.1, 4, 1, 5),
                                     step(0.75, 0.12, 2, 0, -1),
                                     step(0.875, 0.1, 0, -1, 7, true) });
    guzhengRoll.styleTags.addArray({ "chinese", "ancient_style", "guzheng", "pipa" });
    guzhengRoll.moodTags.addArray({ "energetic", "bright", "playful" });
    guzhengRoll.regionTags.addArray({ "chinese" });
    guzhengRoll.functionFit.addArray({ "S", "D" });
    guzhengRoll.formFit.addArray({ "chorus", "bridge" });
    guzhengRoll.familyId = "GuzhengRollFamily";
    guzhengRoll.grooveAffinity = "Tight";
    guzhengRoll.bassAnchored = false;
    guzhengRoll.complexity = 4;
    guzhengRoll.energy = 4;
    patterns.push_back(guzhengRoll);

    auto guzhengGliss = makePattern("guzheng_gliss", cn(u8"古筝刮奏"), "Guzheng Glissando",
                                    ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 0.5,
                                    { step(0.0, 0.08, 0, 0, 5),
                                      step(0.0625, 0.08, 1, 0, 4),
                                      step(0.125, 0.08, 2, 0, 3),
                                      step(0.1875, 0.08, 3, 1, 2),
                                      step(0.25, 0.08, 4, 1, 1),
                                      step(0.3125, 0.08, 5, 2, 0),
                                      step(0.375, 0.08, 4, 2, -1),
                                      step(0.4375, 0.08, 3, 1, -2) });
    guzhengGliss.styleTags.addArray({ "chinese", "ancient_style", "guzheng", "ornament" });
    guzhengGliss.moodTags.addArray({ "bright", "playful", "dramatic" });
    guzhengGliss.regionTags.addArray({ "chinese" });
    guzhengGliss.functionFit.addArray({ "D", "S" });
    guzhengGliss.formFit.addArray({ "bridge", "chorus" });
    guzhengGliss.familyId = "GuzhengRollFamily";
    guzhengGliss.grooveAffinity = "Tight";
    guzhengGliss.bassAnchored = false;
    guzhengGliss.complexity = 5;
    guzhengGliss.energy = 5;
    patterns.push_back(guzhengGliss);

    // 琵琶扫弦/弹挑 (PipaSweepFamily): 强弱对比大，适合 chorus/bridge
    auto pipaSweepNew = makePattern("pipa_sweep_new", cn(u8"琵琶扫弦"), "Pipa Sweep",
                                    ArpeggioDirection::UpDown, ArpeggioDensity::Mixed, 2.0,
                                    { step(0.0, 0.18, 0, -1, 8, true),
                                      step(0.125, 0.1, 2, 0, -2),
                                      step(0.25, 0.18, 3, 1, 5),
                                      step(0.375, 0.1, 1, 0, -3),
                                      step(0.5, 0.18, 2, 0, 1),
                                      step(0.625, 0.1, 4, 1, 6),
                                      step(0.75, 0.18, 3, 1, 2),
                                      step(0.875, 0.1, 0, -1, 7, true) });
    pipaSweepNew.styleTags.addArray({ "chinese", "ancient_style", "pipa", "folk" });
    pipaSweepNew.moodTags.addArray({ "energetic", "bright", "dramatic" });
    pipaSweepNew.regionTags.addArray({ "chinese" });
    pipaSweepNew.functionFit.addArray({ "S", "D" });
    pipaSweepNew.formFit.addArray({ "chorus", "bridge" });
    pipaSweepNew.familyId = "PipaSweepFamily";
    pipaSweepNew.grooveAffinity = "Tight";
    pipaSweepNew.bassAnchored = true;
    pipaSweepNew.complexity = 4;
    pipaSweepNew.energy = 5;
    patterns.push_back(pipaSweepNew);

    auto pipaTremolo = makePattern("pipa_tremolo", cn(u8"琵琶轮指"), "Pipa Tremolo",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.12, 0, -1, 7, true),
                                     step(0.125, 0.1, 1, 0, -2),
                                     step(0.25, 0.12, 0, -1, 6, true),
                                     step(0.375, 0.1, 1, 0, -1),
                                     step(0.5, 0.12, 0, -1, 8, true),
                                     step(0.625, 0.1, 2, 0, 2),
                                     step(0.75, 0.12, 0, -1, 7, true),
                                     step(0.875, 0.1, 2, 0, 1) });
    pipaTremolo.styleTags.addArray({ "chinese", "ancient_style", "pipa" });
    pipaTremolo.moodTags.addArray({ "intense", "dramatic", "energetic" });
    pipaTremolo.regionTags.addArray({ "chinese" });
    pipaTremolo.functionFit.addArray({ "D", "S" });
    pipaTremolo.formFit.addArray({ "bridge", "build-up" });
    pipaTremolo.familyId = "PipaSweepFamily";
    pipaTremolo.grooveAffinity = "Tight";
    pipaTremolo.bassAnchored = false;
    pipaTremolo.complexity = 4;
    pipaTremolo.energy = 4;
    patterns.push_back(pipaTremolo);

    // 古琴留白 (GuqinSparseFamily): 低能量、长 duration、少音
    auto guqinSparse = makePattern("guqin_sparse", cn(u8"古琴留白"), "Guqin Sparse",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                   { step(0.0, 3.5, 0, -1, 4, true),
                                     step(2.0, 1.5, 4, -1, 3, true) });
    guqinSparse.styleTags.addArray({ "chinese", "ancient_style", "guqin", "traditional" });
    guqinSparse.moodTags.addArray({ "calm", "meditative", "wide", "solemn" });
    guqinSparse.regionTags.addArray({ "chinese" });
    guqinSparse.functionFit.addArray({ "T", "t" });
    guqinSparse.formFit.addArray({ "intro", "outro", "cadence" });
    guqinSparse.familyId = "GuqinSparseFamily";
    guqinSparse.grooveAffinity = "Straight";
    guqinSparse.bassAnchored = true;
    guqinSparse.pedalFriendly = true;
    guqinSparse.complexity = 1;
    guqinSparse.energy = 1;
    patterns.push_back(guqinSparse);

    auto guqinHarmonic = makePattern("guqin_harmonic", cn(u8"古琴泛音"), "Guqin Harmonic",
                                     ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 2.0,
                                     { step(0.0, 1.2, 0, 0, 5, true),
                                       step(1.0, 0.8, 4, 0, 4, true) });
    guqinHarmonic.styleTags.addArray({ "chinese", "ancient_style", "guqin", "harmonics" });
    guqinHarmonic.moodTags.addArray({ "ethereal", "calm", "wide" });
    guqinHarmonic.regionTags.addArray({ "chinese" });
    guqinHarmonic.functionFit.addArray({ "T", "t" });
    guqinHarmonic.formFit.addArray({ "intro", "outro" });
    guqinHarmonic.familyId = "GuqinSparseFamily";
    guqinHarmonic.grooveAffinity = "Straight";
    guqinHarmonic.bassAnchored = true;
    guqinHarmonic.pedalFriendly = true;
    guqinHarmonic.complexity = 2;
    guqinHarmonic.energy = 1;
    patterns.push_back(guqinHarmonic);

    // 戏曲过门 (ChineseOperaOrnamentFamily): 短装饰、高音区、Tight
    auto operaOrnament = makePattern("opera_ornament", cn(u8"戏曲过门"), "Opera Ornament",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 1.0,
                                     { step(0.0, 0.12, 0, 1, 8, true),
                                       step(0.125, 0.1, 2, 1, 3),
                                       step(0.25, 0.12, 3, 2, 6),
                                       step(0.375, 0.1, 1, 1, -1),
                                       step(0.5, 0.12, 2, 2, 2),
                                       step(0.625, 0.1, 0, 1, 7, true),
                                       step(0.75, 0.12, 3, 2, 5),
                                       step(0.875, 0.1, 2, 1, -2) });
    operaOrnament.styleTags.addArray({ "chinese_opera", "peking_opera", "kunqu", "traditional" });
    operaOrnament.moodTags.addArray({ "bright", "playful", "dramatic" });
    operaOrnament.regionTags.addArray({ "chinese" });
    operaOrnament.functionFit.addArray({ "S", "D" });
    operaOrnament.formFit.addArray({ "bridge", "chorus" });
    operaOrnament.familyId = "ChineseOperaOrnamentFamily";
    operaOrnament.grooveAffinity = "Tight";
    operaOrnament.bassAnchored = false;
    operaOrnament.complexity = 4;
    operaOrnament.energy = 4;
    patterns.push_back(operaOrnament);

    auto operaIntro = makePattern("opera_intro", cn(u8"戏曲引子"), "Opera Intro",
                                  ArpeggioDirection::Down, ArpeggioDensity::Eighth, 2.0,
                                  { step(0.0, 0.5, 0, 1, 6, true),
                                    step(0.5, 0.4, 2, 1, -2),
                                    step(1.0, 0.5, 1, 0, 1),
                                    step(1.5, 0.4, 3, 2, 4) });
    operaIntro.styleTags.addArray({ "chinese_opera", "peking_opera" });
    operaIntro.moodTags.addArray({ "dramatic", "mysterious", "tense" });
    operaIntro.regionTags.addArray({ "chinese" });
    operaIntro.functionFit.addArray({ "T", "S" });
    operaIntro.formFit.addArray({ "intro", "bridge" });
    operaIntro.familyId = "ChineseOperaOrnamentFamily";
    operaIntro.grooveAffinity = "Straight";
    operaIntro.bassAnchored = true;
    operaIntro.complexity = 2;
    operaIntro.energy = 2;
    patterns.push_back(operaIntro);

    // ========================================================================
    // 华语流行 - 新细分家族 (Chinese Pop)
    // ========================================================================

    // 华语抒情钢琴 (CPopBalladPianoFamily): 四拍循环，低音落点+中高音回应
    auto cpopBalladPiano = makePattern("cpop_ballad_piano", cn(u8"华语抒情钢琴"), "C-Pop Ballad Piano",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                       { step(0.0, 1.0, 0, -1, 5, true),
                                         step(1.0, 0.6, 2, 0, -1),
                                         step(2.0, 0.8, 1, 0, 2),
                                         step(3.0, 0.7, 3, 1, 4) });
    cpopBalladPiano.styleTags.addArray({ "pop", "c-pop", "ballad", "piano" });
    cpopBalladPiano.moodTags.addArray({ "sad", "warm", "romantic", "calm" });
    cpopBalladPiano.regionTags.addArray({ "chinese" });
    cpopBalladPiano.functionFit.addArray({ "T", "S", "t", "s" });
    cpopBalladPiano.formFit.addArray({ "verse", "chorus", "outro" });
    cpopBalladPiano.familyId = "CPopBalladPianoFamily";
    cpopBalladPiano.grooveAffinity = "Straight";
    cpopBalladPiano.bassAnchored = true;
    cpopBalladPiano.complexity = 2;
    cpopBalladPiano.energy = 2;
    patterns.push_back(cpopBalladPiano);

    auto cpopBalladWide = makePattern("cpop_ballad_wide", cn(u8"华语抒情宽幅"), "C-Pop Ballad Wide",
                                      ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 4.0,
                                      { step(0.0, 0.9, 0, -1, 6, true),
                                        step(0.8, 0.5, 2, 0, -2),
                                        step(1.6, 0.7, 4, 1, 3),
                                        step(2.5, 0.6, 1, 0, -1),
                                        step(3.2, 0.8, 3, 1, 5) });
    cpopBalladWide.styleTags.addArray({ "pop", "c-pop", "ballad", "piano" });
    cpopBalladWide.moodTags.addArray({ "emotional", "wide", "sad", "hopeful" });
    cpopBalladWide.regionTags.addArray({ "chinese" });
    cpopBalladWide.functionFit.addArray({ "T", "S", "t", "s" });
    cpopBalladWide.formFit.addArray({ "chorus", "bridge" });
    cpopBalladWide.familyId = "CPopBalladPianoFamily";
    cpopBalladWide.grooveAffinity = "Straight";
    cpopBalladWide.bassAnchored = true;
    cpopBalladWide.complexity = 3;
    cpopBalladWide.energy = 3;
    patterns.push_back(cpopBalladWide);

    // 主歌克制 (MandopopVerseFamily): 低能量、八分、少切分
    auto mandopopVerse = makePattern("mandopop_verse", cn(u8"华语主歌"), "Mandopop Verse",
                                     ArpeggioDirection::Down, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.5, 0, -1, 4, true),
                                       step(0.75, 0.4, 2, 0, -1),
                                       step(1.25, 0.5, 1, 0, 1) });
    mandopopVerse.styleTags.addArray({ "pop", "c-pop", "mandopop" });
    mandopopVerse.moodTags.addArray({ "calm", "sad", "warm", "nostalgic" });
    mandopopVerse.regionTags.addArray({ "chinese" });
    mandopopVerse.functionFit.addArray({ "T", "t" });
    mandopopVerse.formFit.addArray({ "verse", "intro" });
    mandopopVerse.familyId = "MandopopVerseFamily";
    mandopopVerse.grooveAffinity = "Straight";
    mandopopVerse.bassAnchored = true;
    mandopopVerse.complexity = 1;
    mandopopVerse.energy = 1;
    patterns.push_back(mandopopVerse);

    auto mandopopVersePick = makePattern("mandopop_verse_pick", cn(u8"主歌吉他分解"), "Mandopop Verse Picking",
                                         ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                         { step(0.0, 0.45, 0, -1, 5, true),
                                           step(0.5, 0.4, 2, 0, -2),
                                           step(1.0, 0.45, 1, 0, 1),
                                           step(1.5, 0.4, 2, 0, -1) });
    mandopopVersePick.styleTags.addArray({ "pop", "c-pop", "folk", "acoustic" });
    mandopopVersePick.moodTags.addArray({ "calm", "bright", "hopeful" });
    mandopopVersePick.regionTags.addArray({ "chinese" });
    mandopopVersePick.functionFit.addArray({ "T", "S" });
    mandopopVersePick.formFit.addArray({ "verse" });
    mandopopVersePick.familyId = "MandopopVerseFamily";
    mandopopVersePick.grooveAffinity = "Straight";
    mandopopVersePick.bassAnchored = true;
    mandopopVersePick.complexity = 2;
    mandopopVersePick.energy = 2;
    patterns.push_back(mandopopVersePick);

    // 副歌展开 (MandopopChorusFamily): 能量 4，宽音域，上行/上下行
    auto mandopopChorus = makePattern("mandopop_chorus", cn(u8"华语副歌"), "Mandopop Chorus",
                                      ArpeggioDirection::UpDown, ArpeggioDensity::Mixed, 4.0,
                                      { step(0.0, 0.8, 0, -1, 7, true),
                                        step(0.8, 0.5, 2, 0, -1),
                                        step(1.6, 0.7, 3, 1, 4),
                                        step(2.4, 0.6, 1, 0, -2),
                                        step(3.2, 0.8, 4, 1, 5) });
    mandopopChorus.styleTags.addArray({ "pop", "c-pop", "mandopop", "ballad" });
    mandopopChorus.moodTags.addArray({ "bright", "hopeful", "energetic", "emotional" });
    mandopopChorus.regionTags.addArray({ "chinese" });
    mandopopChorus.functionFit.addArray({ "T", "S", "D" });
    mandopopChorus.formFit.addArray({ "chorus", "bridge" });
    mandopopChorus.familyId = "MandopopChorusFamily";
    mandopopChorus.grooveAffinity = "Straight";
    mandopopChorus.bassAnchored = true;
    mandopopChorus.complexity = 3;
    mandopopChorus.energy = 4;
    patterns.push_back(mandopopChorus);

    auto mandopopChorusWide = makePattern("mandopop_chorus_wide", cn(u8"副歌宽幅"), "Mandopop Chorus Wide",
                                          ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                          { step(0.0, 0.6, 0, -1, 8, true),
                                            step(0.5, 0.5, 2, 0, 0),
                                            step(1.0, 0.6, 4, 1, 3),
                                            step(1.5, 0.5, 1, 0, -2) });
    mandopopChorusWide.styleTags.addArray({ "pop", "c-pop", "mandopop" });
    mandopopChorusWide.moodTags.addArray({ "epic", "bright", "hopeful" });
    mandopopChorusWide.regionTags.addArray({ "chinese" });
    mandopopChorusWide.functionFit.addArray({ "S", "D" });
    mandopopChorusWide.formFit.addArray({ "chorus" });
    mandopopChorusWide.familyId = "MandopopChorusFamily";
    mandopopChorusWide.grooveAffinity = "Straight";
    mandopopChorusWide.bassAnchored = true;
    mandopopChorusWide.complexity = 2;
    mandopopChorusWide.energy = 4;
    patterns.push_back(mandopopChorusWide);

    // 周杰伦式切分 (JayStyleSyncopationFamily): Late groove，更多 7/9 色彩
    auto jayStyle = makePattern("jay_style", cn(u8"周杰伦式切分"), "Jay Style Syncopation",
                                ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                { step(0.0, 1.2, 0, -1, 6, true),
                                  step(1.2, 0.6, 2, 0, -3),
                                  step(2.0, 0.9, 1, 0, 2),
                                  step(2.8, 0.7, 3, 1, 5),
                                  step(3.5, 0.5, 2, 0, -1) });
    jayStyle.styleTags.addArray({ "pop", "c-pop", "jay_chou", "piano" });
    jayStyle.moodTags.addArray({ "nostalgic", "emotional", "playful" });
    jayStyle.regionTags.addArray({ "chinese" });
    jayStyle.functionFit.addArray({ "T", "S", "D", "t", "s" });
    jayStyle.formFit.addArray({ "verse", "chorus", "bridge" });
    jayStyle.familyId = "JayStyleSyncopationFamily";
    jayStyle.grooveAffinity = "Late";
    jayStyle.bassAnchored = true;
    jayStyle.supportsExtensions = true;
    jayStyle.complexity = 4;
    jayStyle.energy = 3;
    patterns.push_back(jayStyle);

    auto jayStyleRnb = makePattern("jay_rnb", cn(u8"周氏 R&B 切分"), "Jay RnB Sync",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                   { step(0.0, 0.8, 0, -1, 5, true),
                                     step(0.8, 0.4, 2, 0, -2),
                                     step(1.4, 0.6, 3, 1, 4),
                                     step(2.2, 0.5, 1, 0, -1),
                                     step(3.0, 0.7, 2, 0, 2) });
    jayStyleRnb.styleTags.addArray({ "pop", "c-pop", "jay_chou", "rnb", "neo-soul" });
    jayStyleRnb.moodTags.addArray({ "smooth", "warm", "nostalgic" });
    jayStyleRnb.regionTags.addArray({ "chinese" });
    jayStyleRnb.functionFit.addArray({ "S", "D" });
    jayStyleRnb.formFit.addArray({ "verse", "bridge" });
    jayStyleRnb.familyId = "JayStyleSyncopationFamily";
    jayStyleRnb.grooveAffinity = "Late";
    jayStyleRnb.bassAnchored = true;
    jayStyleRnb.supportsExtensions = true;
    jayStyleRnb.complexity = 4;
    jayStyleRnb.energy = 2;
    patterns.push_back(jayStyleRnb);

    // 校园民谣 (CampusFolkFamily): 吉他/钢琴二合一，Straight，T/S 优先
    auto campusFolkNew = makePattern("campus_folk_new", cn(u8"校园民谣"), "Campus Folk",
                                     ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.55, 0, -1, 6, true),
                                       step(0.75, 0.45, 2, 0, -1),
                                       step(1.25, 0.55, 1, 0, 2),
                                       step(1.75, 0.45, 2, 0, -2) });
    campusFolkNew.styleTags.addArray({ "folk", "c-pop", "acoustic", "campus" });
    campusFolkNew.moodTags.addArray({ "hopeful", "bright", "nostalgic", "warm" });
    campusFolkNew.regionTags.addArray({ "chinese" });
    campusFolkNew.functionFit.addArray({ "T", "S", "t" });
    campusFolkNew.formFit.addArray({ "verse", "chorus" });
    campusFolkNew.familyId = "CampusFolkFamily";
    campusFolkNew.grooveAffinity = "Straight";
    campusFolkNew.bassAnchored = true;
    campusFolkNew.complexity = 2;
    campusFolkNew.energy = 2;
    patterns.push_back(campusFolkNew);

    auto campusFolkPick = makePattern("campus_folk_pick", cn(u8"校园吉他弹唱"), "Campus Guitar",
                                      ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.45, 0, -1, 7, true),
                                        step(0.5, 0.4, 2, 0, -2),
                                        step(1.0, 0.45, 4, 0, 2),
                                        step(1.5, 0.4, 2, 0, -1) });
    campusFolkPick.styleTags.addArray({ "folk", "c-pop", "acoustic", "guitar" });
    campusFolkPick.moodTags.addArray({ "bright", "hopeful", "playful" });
    campusFolkPick.regionTags.addArray({ "chinese" });
    campusFolkPick.functionFit.addArray({ "T", "S" });
    campusFolkPick.formFit.addArray({ "verse", "chorus" });
    campusFolkPick.familyId = "CampusFolkFamily";
    campusFolkPick.grooveAffinity = "Straight";
    campusFolkPick.bassAnchored = true;
    campusFolkPick.complexity = 2;
    campusFolkPick.energy = 3;
    patterns.push_back(campusFolkPick);

    // ========================================================================
    // 补充偏少家族 (CadentialBreathFamily, JazzWalkingFamily, LatinMontunoFamily, RagaDroneFamily)
    // ========================================================================

    // 终止呼吸式 (CadentialBreathFamily) 补充
    auto cadencePop = makePattern("cadence_pop", cn(u8"流行终止"), "Pop Cadence",
                                  ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                  { step(0.0, 1.2, 0, -1, 5, true),
                                    step(1.2, 1.0, 2, 0, -1),
                                    step(2.2, 0.8, 1, 0, 2),
                                    step(3.0, 0.9, 0, -1, 3, true) });
    cadencePop.styleTags.addArray({ "pop", "ballad", "c-pop" });
    cadencePop.moodTags.addArray({ "resolved", "calm", "sad" });
    cadencePop.regionTags.addArray({ "chinese" });
    cadencePop.functionFit.addArray({ "T", "D" });
    cadencePop.formFit.addArray({ "outro", "cadence" });
    cadencePop.familyId = "CadentialBreathFamily";
    cadencePop.grooveAffinity = "Straight";
    cadencePop.bassAnchored = true;
    cadencePop.complexity = 2;
    cadencePop.energy = 1;
    patterns.push_back(cadencePop);

    auto cadenceCinematic = makePattern("cadence_cinematic", cn(u8"电影终止"), "Cinematic Cadence",
                                        ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                        { step(0.0, 1.5, 0, -1, 6, true),
                                          step(1.5, 1.0, 3, 1, 4),
                                          step(2.5, 1.2, 1, 0, -2),
                                          step(3.5, 0.5, 0, -1, 2, true) });
    cadenceCinematic.styleTags.addArray({ "cinematic", "soundtrack" });
    cadenceCinematic.moodTags.addArray({ "epic", "resolved", "wide" });
    cadenceCinematic.functionFit.addArray({ "T", "D" });
    cadenceCinematic.formFit.addArray({ "outro", "cadence" });
    cadenceCinematic.familyId = "CadentialBreathFamily";
    cadenceCinematic.grooveAffinity = "Straight";
    cadenceCinematic.bassAnchored = true;
    cadenceCinematic.complexity = 2;
    cadenceCinematic.energy = 2;
    patterns.push_back(cadenceCinematic);

    // JazzWalkingFamily 补充
    auto jazzWalkingSwing = makePattern("jazz_walking_swing", cn(u8"爵士低音摇摆"), "Jazz Walking Swing",
                                        ArpeggioDirection::Alternating, ArpeggioDensity::Quarter, 2.0,
                                        { step(0.0, 0.45, 0, -1, 6, true),
                                          step(0.5, 0.45, 4, -1, 5, true),
                                          step(1.0, 0.45, 5, -1, 4, true),
                                          step(1.5, 0.45, 0, -1, 6, true) });
    jazzWalkingSwing.styleTags.addArray({ "jazz", "swing", "bebop" });
    jazzWalkingSwing.moodTags.addArray({ "groovy", "cool", "smooth" });
    jazzWalkingSwing.functionFit.addArray({ "T", "S", "D" });
    jazzWalkingSwing.formFit.addArray({ "verse", "bridge" });
    jazzWalkingSwing.familyId = "JazzWalkingFamily";
    jazzWalkingSwing.grooveAffinity = "Shuffle";
    jazzWalkingSwing.bassAnchored = true;
    jazzWalkingSwing.complexity = 2;
    jazzWalkingSwing.energy = 3;
    patterns.push_back(jazzWalkingSwing);

    // LatinMontunoFamily 补充
    auto latinMontunoFast = makePattern("latin_montuno_fast", cn(u8"快速蒙图诺"), "Fast Montuno",
                                        ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                        { step(0.0, 0.12, 0, -1, 8, true),
                                          step(0.25, 0.1, 2, 0, 2),
                                          step(0.5, 0.12, 1, 0, -1),
                                          step(0.75, 0.1, 3, 1, 6) });
    latinMontunoFast.styleTags.addArray({ "latin", "salsa", "son" });
    latinMontunoFast.moodTags.addArray({ "energetic", "bright", "dance" });
    latinMontunoFast.regionTags.addArray({ "latin" });
    latinMontunoFast.functionFit.addArray({ "S", "D" });
    latinMontunoFast.formFit.addArray({ "chorus", "bridge" });
    latinMontunoFast.familyId = "LatinMontunoFamily";
    latinMontunoFast.grooveAffinity = "Tight";
    latinMontunoFast.bassAnchored = true;
    latinMontunoFast.complexity = 3;
    latinMontunoFast.energy = 5;
    patterns.push_back(latinMontunoFast);

    // RagaDroneFamily 补充
    auto ragaDhrupad = makePattern("raga_dhrupad", cn(u8"拉格德鲁帕德"), "Dhrupad Alap",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                   { step(0.0, 3.8, 0, -1, 5, true),
                                     step(0.0, 3.8, 4, -1, 4, true),
                                     step(2.0, 1.5, 2, 0, -2) });
    ragaDhrupad.styleTags.addArray({ "indian", "raga", "traditional" });
    ragaDhrupad.moodTags.addArray({ "meditative", "calm", "wide", "solemn" });
    ragaDhrupad.regionTags.addArray({ "india" });
    ragaDhrupad.functionFit.addArray({ "T", "t" });
    ragaDhrupad.formFit.addArray({ "intro", "outro" });
    ragaDhrupad.familyId = "RagaDroneFamily";
    ragaDhrupad.grooveAffinity = "Straight";
    ragaDhrupad.bassAnchored = true;
    ragaDhrupad.pedalFriendly = true;
    ragaDhrupad.complexity = 2;
    ragaDhrupad.energy = 1;
    patterns.push_back(ragaDhrupad);

    // ========================================================================
    // 补充 Ryukyu 和 Kolo 的修正（避免重复）
    // ========================================================================
    auto ryukyuBright = makePattern("ryukyu_bright", cn(u8"琉球明快"), "Ryukyu Bright",
                                    ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                    { step(0.0, 0.5, 0, -1, 7, true),
                                      step(0.5, 0.45, 2, 0, -1),
                                      step(1.0, 0.5, 3, 0, 3),
                                      step(1.5, 0.45, 1, 0, -2) });
    ryukyuBright.styleTags.addArray({ "japanese", "ryukyu", "okinawa" });
    ryukyuBright.moodTags.addArray({ "bright", "hopeful", "playful" });
    ryukyuBright.regionTags.addArray({ "japanese", "okinawa" });
    ryukyuBright.functionFit.addArray({ "T", "S" });
    ryukyuBright.formFit.addArray({ "verse", "chorus" });
    ryukyuBright.familyId = "GuitarPickingFamily";
    ryukyuBright.grooveAffinity = "Straight";
    ryukyuBright.bassAnchored = true;
    ryukyuBright.complexity = 2;
    ryukyuBright.energy = 3;
    patterns.push_back(ryukyuBright);

    auto koloPulse = makePattern("kolo_pulse", cn(u8"科洛脉冲"), "Kolo Pulse",
                                 ArpeggioDirection::Alternating, ArpeggioDensity::Quarter, 3.0,
                                 { step(0.0, 0.5, 0, -1, 7, true),
                                   step(1.0, 0.45, 2, 0, 2),
                                   step(2.0, 0.5, 4, 0, 4) });
    koloPulse.styleTags.addArray({ "folk", "balkan", "serbia", "dance" });
    koloPulse.moodTags.addArray({ "bright", "playful", "energetic" });
    koloPulse.regionTags.addArray({ "balkan", "serbia" });
    koloPulse.functionFit.addArray({ "T", "S" });
    koloPulse.formFit.addArray({ "chorus", "dance" });
    koloPulse.familyId = "MarchPulseFamily";
    koloPulse.grooveAffinity = "Tight";
    koloPulse.bassAnchored = true;
    koloPulse.complexity = 2;
    koloPulse.energy = 4;
    patterns.push_back(koloPulse);


        // ========================================================================
    // 军乐 / 进行曲 (Military March)
    // ========================================================================

    // 标准军乐脉冲: 强低音+五音，四四拍，刚毅
    auto militaryStandard = makePattern("military_standard", cn(u8"标准军乐"), "Military March",
                                        ArpeggioDirection::BlockPulse, ArpeggioDensity::Quarter, 2.0,
                                        { step(0.0, 0.4, 0, -1, 8, true),
                                          step(0.5, 0.35, 4, -1, 6, true),
                                          step(1.0, 0.4, 0, -1, 9, true),
                                          step(1.5, 0.35, 4, -1, 7, true) });
    militaryStandard.styleTags.addArray({ "march", "military", "cinematic", "epic" });
    militaryStandard.moodTags.addArray({ "epic", "powerful", "solemn", "bright" });
    militaryStandard.functionFit.addArray({ "T", "D" });
    militaryStandard.formFit.addArray({ "intro", "chorus", "outro" });
    militaryStandard.familyId = "MarchPulseFamily";
    militaryStandard.grooveAffinity = "Straight";
    militaryStandard.bassAnchored = true;
    militaryStandard.pedalFriendly = false;
    militaryStandard.complexity = 1;
    militaryStandard.energy = 4;
    patterns.push_back(militaryStandard);

    // 军乐上行: 充满希望，上行音阶片段
    auto militaryRising = makePattern("military_rising", cn(u8"军乐上行"), "March Rising",
                                      ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.4, 0, -1, 7, true),
                                        step(0.5, 0.35, 1, 0, 1),
                                        step(1.0, 0.4, 2, 0, 3),
                                        step(1.5, 0.35, 3, 1, 5) });
    militaryRising.styleTags.addArray({ "march", "military", "cinematic", "epic" });
    militaryRising.moodTags.addArray({ "hopeful", "bright", "powerful" });
    militaryRising.functionFit.addArray({ "T", "S", "D" });
    militaryRising.formFit.addArray({ "chorus", "bridge", "build-up" });
    militaryRising.familyId = "MarchPulseFamily";
    militaryRising.grooveAffinity = "Straight";
    militaryRising.bassAnchored = true;
    militaryRising.complexity = 2;
    militaryRising.energy = 4;
    patterns.push_back(militaryRising);

    // 军乐哀悼 (慢速，下行)
    auto militaryFuneral = makePattern("military_funeral", cn(u8"军乐哀悼"), "Funeral March",
                                       ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                       { step(0.0, 1.2, 0, -1, 6, true),
                                         step(1.2, 0.8, 1, 0, -2),
                                         step(2.2, 1.0, 0, -1, 5, true),
                                         step(3.2, 0.7, 4, -1, 3) });
    militaryFuneral.styleTags.addArray({ "march", "military", "sad", "ceremony" });
    militaryFuneral.moodTags.addArray({ "sad", "solemn", "wide" });
    militaryFuneral.functionFit.addArray({ "T", "t" });
    militaryFuneral.formFit.addArray({ "outro", "cadence" });
    militaryFuneral.familyId = "MarchPulseFamily";
    militaryFuneral.grooveAffinity = "Straight";
    militaryFuneral.bassAnchored = true;
    militaryFuneral.complexity = 1;
    militaryFuneral.energy = 2;
    patterns.push_back(militaryFuneral);

    // ========================================================================
    // 电影配乐 (Film Score) - 补充更多场景
    // ========================================================================

    // 电影情感主题: 温暖上行，弦乐感
    auto filmEmotion = makePattern("film_emotion", cn(u8"电影情感"), "Film Emotion",
                                   ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0,
                                   { step(0.0, 1.0, 0, -1, 5, true),
                                     step(1.2, 0.8, 2, 0, -1),
                                     step(2.2, 0.9, 4, 1, 3),
                                     step(3.2, 0.7, 1, 0, -2) });
    filmEmotion.styleTags.addArray({ "cinematic", "soundtrack", "emotional" });
    filmEmotion.moodTags.addArray({ "warm", "sad", "hopeful", "wide" });
    filmEmotion.functionFit.addArray({ "T", "S", "t" });
    filmEmotion.formFit.addArray({ "verse", "chorus" });
    filmEmotion.familyId = "CinematicOstinatoFamily";
    filmEmotion.grooveAffinity = "Straight";
    filmEmotion.bassAnchored = true;
    filmEmotion.complexity = 2;
    filmEmotion.energy = 2;
    patterns.push_back(filmEmotion);

    // 电影动作追逐: 快速上下行，紧张
    auto filmAction = makePattern("film_action", cn(u8"电影动作"), "Film Action",
                                  ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 1.0,
                                  { step(0.0, 0.12, 0, -1, 8, true),
                                    step(0.125, 0.1, 2, 0, 2),
                                    step(0.25, 0.12, 1, 0, -1),
                                    step(0.375, 0.1, 3, 1, 5),
                                    step(0.5, 0.12, 2, 0, 1),
                                    step(0.625, 0.1, 0, -1, 7, true),
                                    step(0.75, 0.12, 3, 1, 4),
                                    step(0.875, 0.1, 1, 0, -2) });
    filmAction.styleTags.addArray({ "cinematic", "action", "tension" });
    filmAction.moodTags.addArray({ "energetic", "tense", "intense" });
    filmAction.functionFit.addArray({ "S", "D" });
    filmAction.formFit.addArray({ "bridge", "build-up" });
    filmAction.familyId = "SyncopatedElectronicFamily";
    filmAction.grooveAffinity = "Tight";
    filmAction.bassAnchored = true;
    filmAction.complexity = 3;
    filmAction.energy = 5;
    patterns.push_back(filmAction);

    // 电影悬念低音: 半音阶下行，慢速
    auto filmSuspense = makePattern("film_suspense", cn(u8"电影悬念低音"), "Film Suspense Bass",
                                    ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                    { step(0.0, 1.5, 0, -1, 5, true),
                                      step(1.2, 0.8, 1, -1, 4, true),
                                      step(2.2, 1.0, 2, -1, 3, true),
                                      step(3.2, 0.6, 1, -1, 4, true) });
    filmSuspense.styleTags.addArray({ "cinematic", "suspense", "horror" });
    filmSuspense.moodTags.addArray({ "tense", "dark", "mysterious" });
    filmSuspense.functionFit.addArray({ "D", "s" });
    filmSuspense.formFit.addArray({ "bridge", "intro" });
    filmSuspense.familyId = "PedalDroneFamily";
    filmSuspense.grooveAffinity = "Straight";
    filmSuspense.bassAnchored = true;
    filmSuspense.pedalFriendly = true;
    filmSuspense.complexity = 2;
    filmSuspense.energy = 1;
    patterns.push_back(filmSuspense);

    // 电影希望/升华 (替代版，更宽广)
    auto filmHope = makePattern("film_hope", cn(u8"电影希望"), "Film Hope",
                                ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0,
                                { step(0.0, 1.2, 0, -1, 4, true),
                                  step(1.0, 0.9, 2, 0, 1),
                                  step(2.0, 1.1, 4, 1, 5),
                                  step(3.0, 0.8, 1, 0, -2) });
    filmHope.styleTags.addArray({ "cinematic", "hopeful", "epic" });
    filmHope.moodTags.addArray({ "hopeful", "wide", "bright", "epic" });
    filmHope.functionFit.addArray({ "T", "S" });
    filmHope.formFit.addArray({ "chorus", "outro" });
    filmHope.familyId = "CinematicOstinatoFamily";
    filmHope.grooveAffinity = "Straight";
    filmHope.bassAnchored = true;
    filmHope.complexity = 2;
    filmHope.energy = 3;
    patterns.push_back(filmHope);

    // ========================================================================
    // 流行前奏 (Pop Intro)
    // ========================================================================

    // 流行前奏 - 稀疏钢琴单音 + 五度空灵
    auto popIntroSparse = makePattern("pop_intro_sparse", cn(u8"流行前奏稀疏"), "Pop Intro Sparse",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0,
                                      { step(0.0, 2.5, 0, -1, 4, true),
                                        step(2.0, 1.2, 4, 0, 2) });
    popIntroSparse.styleTags.addArray({ "pop", "ballad", "cinematic" });
    popIntroSparse.moodTags.addArray({ "calm", "wide", "mysterious", "hopeful" });
    popIntroSparse.functionFit.addArray({ "T", "t" });
    popIntroSparse.formFit.addArray({ "intro" });
    popIntroSparse.familyId = "PadFamily";
    popIntroSparse.grooveAffinity = "Straight";
    popIntroSparse.bassAnchored = true;
    popIntroSparse.pedalFriendly = true;
    popIntroSparse.complexity = 1;
    popIntroSparse.energy = 1;
    patterns.push_back(popIntroSparse);

    // 流行前奏 - 八度跳跃，引人入胜
    auto popIntroOctave = makePattern("pop_intro_octave", cn(u8"流行前奏八度"), "Pop Intro Octave",
                                      ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.6, 0, -1, 5, true),
                                        step(0.8, 0.5, 4, 0, 2),
                                        step(1.6, 0.6, 0, 0, 4, true) });
    popIntroOctave.styleTags.addArray({ "pop", "c-pop", "mandopop" });
    popIntroOctave.moodTags.addArray({ "hopeful", "bright", "calm" });
    popIntroOctave.functionFit.addArray({ "T", "S" });
    popIntroOctave.formFit.addArray({ "intro" });
    popIntroOctave.familyId = "BrokenPianoFamily";
    popIntroOctave.grooveAffinity = "Straight";
    popIntroOctave.bassAnchored = true;
    popIntroOctave.complexity = 2;
    popIntroOctave.energy = 2;
    patterns.push_back(popIntroOctave);

    // 流行前奏 - 吉他分解 (民谣感)
    auto popIntroGuitar = makePattern("pop_intro_guitar", cn(u8"流行前奏吉他"), "Pop Intro Guitar",
                                      ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.45, 0, -1, 5, true),
                                        step(0.75, 0.4, 2, 0, -1),
                                        step(1.25, 0.45, 4, 0, 2) });
    popIntroGuitar.styleTags.addArray({ "pop", "folk", "acoustic" });
    popIntroGuitar.moodTags.addArray({ "warm", "hopeful", "calm" });
    popIntroGuitar.functionFit.addArray({ "T", "S" });
    popIntroGuitar.formFit.addArray({ "intro" });
    popIntroGuitar.familyId = "GuitarPickingFamily";
    popIntroGuitar.grooveAffinity = "Straight";
    popIntroGuitar.bassAnchored = true;
    popIntroGuitar.complexity = 2;
    popIntroGuitar.energy = 2;
    patterns.push_back(popIntroGuitar);

    // ========================================================================
    // Pre-chorus / 间奏 (Pre-chorus / Interlude)
    // ========================================================================

    // Pre-chorus 上行张力: 半音级进，密度渐增
    auto preChorusRise = makePattern("prechorus_rise", cn(u8"Pre-chorus 上行"), "Pre-chorus Rise",
                                     ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.4, 0, -1, 6, true),
                                       step(0.5, 0.35, 1, 0, -1),
                                       step(1.0, 0.4, 2, 0, 3),
                                       step(1.5, 0.35, 3, 1, 5) });
    preChorusRise.styleTags.addArray({ "pop", "rock", "cinematic" });
    preChorusRise.moodTags.addArray({ "tension", "build-up", "hopeful" });
    preChorusRise.functionFit.addArray({ "S", "D" });
    preChorusRise.formFit.addArray({ "bridge", "prechorus" });
    preChorusRise.familyId = "BrokenPianoFamily";
    preChorusRise.grooveAffinity = "Straight";
    preChorusRise.bassAnchored = true;
    preChorusRise.complexity = 3;
    preChorusRise.energy = 3;
    patterns.push_back(preChorusRise);

    // Pre-chorus 节奏切分: 反拍强调，制造期待
    auto preChorusSync = makePattern("prechorus_sync", cn(u8"Pre-chorus 切分"), "Pre-chorus Sync",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                     { step(0.0, 0.35, 0, -1, 7, true),
                                       step(0.375, 0.28, 2, 0, -2),
                                       step(0.875, 0.35, 1, 0, 3),
                                       step(1.25, 0.3, 3, 1, 5) });
    preChorusSync.styleTags.addArray({ "pop", "r&b", "rock" });
    preChorusSync.moodTags.addArray({ "tension", "energetic", "build-up" });
    preChorusSync.functionFit.addArray({ "S", "D" });
    preChorusSync.formFit.addArray({ "bridge", "prechorus" });
    preChorusSync.familyId = "SyncopatedElectronicFamily";
    preChorusSync.grooveAffinity = "Late";
    preChorusSync.bassAnchored = true;
    preChorusSync.complexity = 3;
    preChorusSync.energy = 4;
    patterns.push_back(preChorusSync);

    // 间奏 (乐器独奏感): 中高音区，旋律性
    auto interludeMelody = makePattern("interlude_melody", cn(u8"间奏旋律"), "Interlude Melody",
                                       ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                       { step(0.0, 0.5, 0, 0, 7, true),
                                         step(0.6, 0.45, 2, 0, -1),
                                         step(1.2, 0.5, 1, 0, 3),
                                         step(1.8, 0.4, 3, 1, 5) });
    interludeMelody.styleTags.addArray({ "pop", "cinematic", "instrumental" });
    interludeMelody.moodTags.addArray({ "emotional", "flowing", "bright" });
    interludeMelody.functionFit.addArray({ "T", "S", "D" });
    interludeMelody.formFit.addArray({ "interlude", "bridge" });
    interludeMelody.familyId = "BrokenPianoFamily";
    interludeMelody.grooveAffinity = "Straight";
    interludeMelody.bassAnchored = false;
    interludeMelody.complexity = 3;
    interludeMelody.energy = 3;
    patterns.push_back(interludeMelody);

    // 间奏 - 弦乐长音 (铺垫)
    auto interludeString = makePattern("interlude_string", cn(u8"间奏弦乐"), "Interlude String",
                                       ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 4.0,
                                       { step(0.0, 3.5, 0, -1, 5, true),
                                         step(1.0, 2.0, 2, 0, 2),
                                         step(2.5, 1.5, 4, 1, 4) });
    interludeString.styleTags.addArray({ "cinematic", "pop", "string" });
    interludeString.moodTags.addArray({ "wide", "calm", "emotional" });
    interludeString.functionFit.addArray({ "T", "S" });
    interludeString.formFit.addArray({ "interlude", "bridge" });
    interludeString.familyId = "CinematicOstinatoFamily";
    interludeString.grooveAffinity = "Straight";
    interludeString.bassAnchored = true;
    interludeString.complexity = 2;
    interludeString.energy = 2;
    patterns.push_back(interludeString);

    // ========================================================================
    // 尾奏 / 收束 (Outro / Cadence)
    // ========================================================================

    // 尾奏 - 下行叹息，情绪回落
    auto outroSigh = makePattern("outro_sigh", cn(u8"尾奏叹息"), "Outro Sigh",
                                 ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                 { step(0.0, 1.2, 0, -1, 4, true),
                                   step(1.2, 0.8, 1, 0, -1),
                                   step(2.2, 1.0, 0, -1, 3, true) });
    outroSigh.styleTags.addArray({ "pop", "ballad", "cinematic" });
    outroSigh.moodTags.addArray({ "sad", "calm", "resolved" });
    outroSigh.functionFit.addArray({ "T", "t" });
    outroSigh.formFit.addArray({ "outro", "cadence" });
    outroSigh.familyId = "CadentialBreathFamily";
    outroSigh.grooveAffinity = "Straight";
    outroSigh.bassAnchored = true;
    outroSigh.complexity = 1;
    outroSigh.energy = 1;
    patterns.push_back(outroSigh);

    // 尾奏 - 根音持续 + 五度，渐弱
    auto outroDrone = makePattern("outro_drone", cn(u8"尾奏持续"), "Outro Drone",
                                  ArpeggioDirection::BlockPulse, ArpeggioDensity::Whole, 4.0,
                                  { step(0.0, 3.8, 0, -1, 3, true),
                                    step(0.0, 3.8, 4, -1, 2, true) });
    outroDrone.styleTags.addArray({ "pop", "cinematic", "ambient" });
    outroDrone.moodTags.addArray({ "calm", "wide", "resolved" });
    outroDrone.functionFit.addArray({ "T", "t" });
    outroDrone.formFit.addArray({ "outro", "cadence" });
    outroDrone.familyId = "PedalDroneFamily";
    outroDrone.grooveAffinity = "Straight";
    outroDrone.bassAnchored = true;
    outroDrone.pedalFriendly = true;
    outroDrone.complexity = 1;
    outroDrone.energy = 1;
    patterns.push_back(outroDrone);

    // 尾奏 - 上行开放收束 (希望感)
    auto outroOpen = makePattern("outro_open", cn(u8"尾奏开放"), "Outro Open",
                                 ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                 { step(0.0, 1.5, 0, -1, 5, true),
                                   step(1.0, 1.2, 2, 0, 1),
                                   step(2.0, 1.0, 4, 1, 4) });
    outroOpen.styleTags.addArray({ "pop", "cinematic", "hopeful" });
    outroOpen.moodTags.addArray({ "hopeful", "bright", "wide" });
    outroOpen.functionFit.addArray({ "T", "S" });
    outroOpen.formFit.addArray({ "outro", "cadence" });
    outroOpen.familyId = "CinematicOstinatoFamily";
    outroOpen.grooveAffinity = "Straight";
    outroOpen.bassAnchored = true;
    outroOpen.complexity = 2;
    outroOpen.energy = 2;
    patterns.push_back(outroOpen);

    // 尾奏 - 古典终止式 (V-I 强调)
    auto outroClassical = makePattern("outro_classical", cn(u8"古典终止"), "Classical Cadence",
                                      ArpeggioDirection::Down, ArpeggioDensity::Quarter, 2.0,
                                      { step(0.0, 0.5, 0, -1, 6, true),
                                        step(0.5, 0.4, 4, -1, 5, true),
                                        step(1.0, 0.5, 0, -1, 7, true) });
    outroClassical.styleTags.addArray({ "classical", "pop", "cinematic" });
    outroClassical.moodTags.addArray({ "resolved", "calm", "grand" });
    outroClassical.functionFit.addArray({ "T", "D" });
    outroClassical.formFit.addArray({ "outro", "cadence" });
    outroClassical.familyId = "CadentialBreathFamily";
    outroClassical.grooveAffinity = "Straight";
    outroClassical.bassAnchored = true;
    outroClassical.complexity = 2;
    outroClassical.energy = 2;
    patterns.push_back(outroClassical);


        // ========================================================================
    // 华语流行分段细化 (Prechorus / Bridge / Chorus Drive / Outro / Interlude)
    // ========================================================================

    // 华语主歌过渡 (MandopopPrechorusFamily - 新家族)
    auto mandopopPrechorus = makePattern("mandopop_prechorus", cn(u8"华语主歌过渡"), "Mandopop Prechorus",
                                         ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                         { step(0.0, 0.5, 0, -1, 6, true),
                                           step(0.5, 0.45, 2, 0, -1),
                                           step(1.0, 0.6, 4, 1, 4),
                                           step(1.6, 0.4, 1, 0, -2) });
    mandopopPrechorus.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopPrechorus.moodTags.addArray({ "building", "hopeful", "tension" });
    mandopopPrechorus.regionTags.addArray({ "chinese" });
    mandopopPrechorus.functionFit.addArray({ "S", "D" });
    mandopopPrechorus.formFit.addArray({ "prechorus", "bridge" });
    mandopopPrechorus.familyId = "MandopopPrechorusFamily";
    mandopopPrechorus.grooveAffinity = "Straight";
    mandopopPrechorus.bassAnchored = true;
    mandopopPrechorus.complexity = 2;
    mandopopPrechorus.energy = 3;
    patterns.push_back(mandopopPrechorus);

    // 华语桥段提升 (MandopopBridgeFamily)
    auto mandopopBridgeLift = makePattern("mandopop_bridge_lift", cn(u8"华语桥段提升"), "Mandopop Bridge Lift",
                                          ArpeggioDirection::UpDown, ArpeggioDensity::Mixed, 4.0,
                                          { step(0.0, 0.8, 0, -1, 7, true),
                                            step(0.8, 0.5, 2, 0, -2),
                                            step(1.6, 0.7, 3, 1, 5),
                                            step(2.4, 0.6, 1, 0, -1),
                                            step(3.2, 0.8, 4, 1, 6) });
    mandopopBridgeLift.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopBridgeLift.moodTags.addArray({ "climax", "emotional", "tension" });
    mandopopBridgeLift.regionTags.addArray({ "chinese" });
    mandopopBridgeLift.functionFit.addArray({ "D", "S" });
    mandopopBridgeLift.formFit.addArray({ "bridge" });
    mandopopBridgeLift.familyId = "MandopopBridgeFamily";
    mandopopBridgeLift.grooveAffinity = "Late";
    mandopopBridgeLift.bassAnchored = true;
    mandopopBridgeLift.complexity = 3;
    mandopopBridgeLift.energy = 4;
    patterns.push_back(mandopopBridgeLift);

    // 华语副歌驱动 (CPopChorusDriveFamily)
    auto cpopChorusDrive = makePattern("cpop_chorus_drive", cn(u8"华语副歌驱动"), "C-Pop Chorus Drive",
                                       ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                       { step(0.0, 0.45, 0, -1, 8, true),
                                         step(0.5, 0.4, 2, 0, -1),
                                         step(1.0, 0.45, 3, 1, 5),
                                         step(1.5, 0.4, 1, 0, -2) });
    cpopChorusDrive.styleTags.addArray({ "pop", "c-pop", "ballad" });
    cpopChorusDrive.moodTags.addArray({ "energetic", "bright", "hopeful" });
    cpopChorusDrive.regionTags.addArray({ "chinese" });
    cpopChorusDrive.functionFit.addArray({ "T", "S", "D" });
    cpopChorusDrive.formFit.addArray({ "chorus" });
    cpopChorusDrive.familyId = "CPopChorusDriveFamily";
    cpopChorusDrive.grooveAffinity = "Straight";
    cpopChorusDrive.bassAnchored = true;
    cpopChorusDrive.complexity = 2;
    cpopChorusDrive.energy = 4;
    patterns.push_back(cpopChorusDrive);

    // 流行前奏疑问句 (BrokenPianoFamily)
    auto popIntroQuestion = makePattern("pop_intro_question", cn(u8"流行前奏疑问"), "Pop Intro Question",
                                        ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                        { step(0.0, 1.2, 0, -1, 4, true),
                                          step(1.2, 0.9, 2, 0, 0),
                                          step(2.2, 1.5, 1, 0, -2),
                                          step(3.7, 0.3, 3, 1, 3) });
    popIntroQuestion.styleTags.addArray({ "pop", "ballad", "c-pop" });
    popIntroQuestion.moodTags.addArray({ "questioning", "mysterious", "calm" });
    popIntroQuestion.regionTags.addArray({ "chinese" });
    popIntroQuestion.functionFit.addArray({ "T", "S" });
    popIntroQuestion.formFit.addArray({ "intro" });
    popIntroQuestion.familyId = "BrokenPianoFamily";
    popIntroQuestion.grooveAffinity = "Straight";
    popIntroQuestion.bassAnchored = true;
    popIntroQuestion.complexity = 2;
    popIntroQuestion.energy = 1;
    patterns.push_back(popIntroQuestion);

    // 流行间奏过渡 (BrokenPianoFamily)
    auto cpopInterlude = makePattern("cpop_interlude", cn(u8"流行间奏"), "C-Pop Interlude",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                     { step(0.0, 0.45, 0, -1, 5, true),
                                       step(0.5, 0.35, 4, 0, 2),
                                       step(1.0, 0.5, 2, 0, -1),
                                       step(1.5, 0.3, 1, 0, 3) });
    cpopInterlude.styleTags.addArray({ "pop", "c-pop" });
    cpopInterlude.moodTags.addArray({ "transition", "calm", "flowing" });
    cpopInterlude.regionTags.addArray({ "chinese" });
    cpopInterlude.functionFit.addArray({ "S", "T" });
    cpopInterlude.formFit.addArray({ "interlude" });
    cpopInterlude.familyId = "BrokenPianoFamily";
    cpopInterlude.grooveAffinity = "Straight";
    cpopInterlude.bassAnchored = true;
    cpopInterlude.complexity = 2;
    cpopInterlude.energy = 2;
    patterns.push_back(cpopInterlude);

    // 尾奏下行 (CadentialBreathFamily)
    auto mandopopOutroDown = makePattern("mandopop_outro_down", cn(u8"华语尾奏下行"), "Mandopop Outro Down",
                                         ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                         { step(0.0, 1.2, 0, -1, 4, true),
                                           step(1.2, 1.0, 2, 0, -1),
                                           step(2.2, 0.8, 1, 0, 1),
                                           step(3.0, 0.9, 0, -1, 2, true) });
    mandopopOutroDown.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopOutroDown.moodTags.addArray({ "resolved", "calm", "sad" });
    mandopopOutroDown.regionTags.addArray({ "chinese" });
    mandopopOutroDown.functionFit.addArray({ "T", "D" });
    mandopopOutroDown.formFit.addArray({ "outro", "cadence" });
    mandopopOutroDown.familyId = "CadentialBreathFamily";
    mandopopOutroDown.grooveAffinity = "Straight";
    mandopopOutroDown.bassAnchored = true;
    mandopopOutroDown.complexity = 2;
    mandopopOutroDown.energy = 1;
    patterns.push_back(mandopopOutroDown);

    // ========================================================================
    // 古风情绪细分 (Guofeng Mood)
    // ========================================================================

    // 古风伤感留白 (GuqinSparseFamily)
    auto guofengSadSparse = makePattern("guofeng_sad_sparse", cn(u8"古风伤感留白"), "Guofeng Sad Sparse",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                        { step(0.0, 3.5, 0, -1, 3, true),
                                          step(2.0, 1.2, 2, 0, -1) });
    guofengSadSparse.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengSadSparse.moodTags.addArray({ "sad", "calm", "solemn", "wide" });
    guofengSadSparse.regionTags.addArray({ "chinese" });
    guofengSadSparse.functionFit.addArray({ "T", "t" });
    guofengSadSparse.formFit.addArray({ "intro", "verse", "outro" });
    guofengSadSparse.familyId = "GuqinSparseFamily";
    guofengSadSparse.grooveAffinity = "Straight";
    guofengSadSparse.bassAnchored = true;
    guofengSadSparse.pedalFriendly = true;
    guofengSadSparse.complexity = 1;
    guofengSadSparse.energy = 1;
    patterns.push_back(guofengSadSparse);

    // 古风史诗驱动 (GuofengFlowFamily)
    auto guofengEpicDrive = makePattern("guofeng_epic_drive", cn(u8"古风史诗驱动"), "Guofeng Epic Drive",
                                        ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                        { step(0.0, 0.6, 0, -1, 8, true),
                                          step(0.6, 0.5, 4, 0, 4),
                                          step(1.2, 0.6, 7, 1, 7),
                                          step(1.8, 0.5, 4, 0, 2) });
    guofengEpicDrive.styleTags.addArray({ "chinese", "ancient_style", "cinematic" });
    guofengEpicDrive.moodTags.addArray({ "epic", "powerful", "heroic" });
    guofengEpicDrive.regionTags.addArray({ "chinese" });
    guofengEpicDrive.functionFit.addArray({ "D", "S" });
    guofengEpicDrive.formFit.addArray({ "chorus", "bridge" });
    guofengEpicDrive.familyId = "GuofengFlowFamily";
    guofengEpicDrive.grooveAffinity = "Straight";
    guofengEpicDrive.bassAnchored = true;
    guofengEpicDrive.complexity = 3;
    guofengEpicDrive.energy = 5;
    patterns.push_back(guofengEpicDrive);

    // 古风浪漫流动 (GuofengFlowFamily)
    auto guofengRomanceFlow = makePattern("guofeng_romance_flow", cn(u8"古风浪漫流动"), "Guofeng Romance Flow",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                          { step(0.0, 1.2, 0, -1, 5, true),
                                            step(1.0, 0.9, 2, 0, -1),
                                            step(2.0, 1.0, 4, 1, 3),
                                            step(3.0, 0.8, 1, 0, -2) });
    guofengRomanceFlow.styleTags.addArray({ "chinese", "ancient_style", "romance" });
    guofengRomanceFlow.moodTags.addArray({ "romantic", "warm", "flowing" });
    guofengRomanceFlow.regionTags.addArray({ "chinese" });
    guofengRomanceFlow.functionFit.addArray({ "T", "S" });
    guofengRomanceFlow.formFit.addArray({ "verse", "chorus" });
    guofengRomanceFlow.familyId = "GuofengFlowFamily";
    guofengRomanceFlow.grooveAffinity = "Straight";
    guofengRomanceFlow.bassAnchored = true;
    guofengRomanceFlow.complexity = 3;
    guofengRomanceFlow.energy = 2;
    patterns.push_back(guofengRomanceFlow);

    // 古风战斗脉冲 (MarchPulseFamily)
    auto guofengBattlePulse = makePattern("guofeng_battle_pulse", cn(u8"古风战斗脉冲"), "Guofeng Battle Pulse",
                                          ArpeggioDirection::BlockPulse, ArpeggioDensity::Quarter, 2.0,
                                          { step(0.0, 0.35, 0, -1, 9, true),
                                            step(0.5, 0.3, 4, -1, 7, true),
                                            step(1.0, 0.35, 0, -1, 9, true),
                                            step(1.5, 0.3, 4, -1, 8, true) });
    guofengBattlePulse.styleTags.addArray({ "chinese", "ancient_style", "cinematic", "war" });
    guofengBattlePulse.moodTags.addArray({ "intense", "epic", "powerful" });
    guofengBattlePulse.regionTags.addArray({ "chinese" });
    guofengBattlePulse.functionFit.addArray({ "D", "S" });
    guofengBattlePulse.formFit.addArray({ "chorus", "bridge", "build-up" });
    guofengBattlePulse.familyId = "MarchPulseFamily";
    guofengBattlePulse.grooveAffinity = "Tight";
    guofengBattlePulse.bassAnchored = true;
    guofengBattlePulse.complexity = 2;
    guofengBattlePulse.energy = 5;
    patterns.push_back(guofengBattlePulse);

    // ========================================================================
    // 中国民乐器动作库 (Chinese Instrument Gestures)
    // ========================================================================

    // 二胡乐句 (GuofengFlowFamily)
    auto erhuPhrase = makePattern("erhu_phrase", cn(u8"二胡乐句"), "Erhu Phrase",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                  { step(0.0, 1.2, 0, -1, 6, true),
                                    step(1.2, 0.8, 2, 0, -2),
                                    step(2.0, 1.0, 4, 1, 4),
                                    step(3.0, 0.6, 1, 0, -1) });
    erhuPhrase.styleTags.addArray({ "chinese", "erhu", "folk", "cinematic" });
    erhuPhrase.moodTags.addArray({ "sad", "emotional", "wide" });
    erhuPhrase.regionTags.addArray({ "chinese" });
    erhuPhrase.functionFit.addArray({ "T", "S", "t" });
    erhuPhrase.formFit.addArray({ "verse", "bridge", "outro" });
    erhuPhrase.familyId = "GuofengFlowFamily";
    erhuPhrase.grooveAffinity = "Straight";
    erhuPhrase.bassAnchored = true;
    erhuPhrase.complexity = 3;
    erhuPhrase.energy = 2;
    patterns.push_back(erhuPhrase);

    // 笛子颤音 (GuzhengRollFamily)
    auto diziFlutter = makePattern("dizi_flutter", cn(u8"笛子颤音"), "Dizi Flutter",
                                   ArpeggioDirection::Alternating, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.08, 0, 0, 7, true),
                                     step(0.1, 0.08, 0, 0, 6, true),
                                     step(0.2, 0.08, 0, 0, 8, true),
                                     step(0.3, 0.08, 2, 0, 3),
                                     step(0.4, 0.08, 0, 0, 7, true),
                                     step(0.5, 0.08, 0, 0, 5, true),
                                     step(0.6, 0.08, 0, 0, 6, true),
                                     step(0.7, 0.08, 3, 1, 5) });
    diziFlutter.styleTags.addArray({ "chinese", "dizi", "folk" });
    diziFlutter.moodTags.addArray({ "bright", "playful", "energetic" });
    diziFlutter.regionTags.addArray({ "chinese" });
    diziFlutter.functionFit.addArray({ "S", "D" });
    diziFlutter.formFit.addArray({ "chorus", "bridge" });
    diziFlutter.familyId = "GuzhengRollFamily";
    diziFlutter.grooveAffinity = "Tight";
    diziFlutter.bassAnchored = false;
    diziFlutter.complexity = 4;
    diziFlutter.energy = 4;
    patterns.push_back(diziFlutter);

    // 扬琴滚奏 (GuzhengRollFamily)
    auto yangqinRoll = makePattern("yangqin_roll", cn(u8"扬琴滚奏"), "Yangqin Roll",
                                   ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 1.0,
                                   { step(0.0, 0.12, 0, -1, 6, true),
                                     step(0.125, 0.1, 2, 0, -1),
                                     step(0.25, 0.12, 1, 0, 2),
                                     step(0.375, 0.1, 3, 1, 5),
                                     step(0.5, 0.12, 2, 0, 1),
                                     step(0.625, 0.1, 0, -1, 4, true),
                                     step(0.75, 0.12, 3, 1, 3),
                                     step(0.875, 0.1, 1, 0, -2) });
    yangqinRoll.styleTags.addArray({ "chinese", "yangqin", "folk" });
    yangqinRoll.moodTags.addArray({ "bright", "energetic", "playful" });
    yangqinRoll.regionTags.addArray({ "chinese" });
    yangqinRoll.functionFit.addArray({ "S", "D" });
    yangqinRoll.formFit.addArray({ "chorus", "bridge" });
    yangqinRoll.familyId = "GuzhengRollFamily";
    yangqinRoll.grooveAffinity = "Tight";
    yangqinRoll.bassAnchored = false;
    yangqinRoll.complexity = 4;
    yangqinRoll.energy = 4;
    patterns.push_back(yangqinRoll);

    // 唢呐呼唤 (MarchPulseFamily)
    auto suonaCall = makePattern("suona_call", cn(u8"唢呐呼唤"), "Suona Call",
                                 ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                 { step(0.0, 0.6, 0, 1, 9, true),
                                   step(0.6, 0.5, 4, 2, 7),
                                   step(1.2, 0.6, 7, 3, 9),
                                   step(2.0, 1.8, 5, 2, 5) });
    suonaCall.styleTags.addArray({ "chinese", "suona", "folk", "cinematic" });
    suonaCall.moodTags.addArray({ "heroic", "epic", "bright" });
    suonaCall.regionTags.addArray({ "chinese" });
    suonaCall.functionFit.addArray({ "D", "T" });
    suonaCall.formFit.addArray({ "intro", "chorus" });
    suonaCall.familyId = "MarchPulseFamily";
    suonaCall.grooveAffinity = "Straight";
    suonaCall.bassAnchored = false;
    suonaCall.complexity = 3;
    suonaCall.energy = 5;
    patterns.push_back(suonaCall);

    // ========================================================================
    // 华语 R&B / 国风 R&B (Chinese RnB)
    // ========================================================================

    // 华语 R&B 晚切分 (JayStyleSyncopationFamily)
    auto chineseRnbLate = makePattern("chinese_rnb_late", cn(u8"华语 R&B 晚切分"), "Chinese RnB Late",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                      { step(0.0, 1.0, 0, -1, 5, true),
                                        step(0.9, 0.5, 2, 0, -2),
                                        step(1.8, 0.7, 3, 1, 4),
                                        step(2.6, 0.6, 1, 0, -1),
                                        step(3.4, 0.4, 2, 0, 2) });
    chineseRnbLate.styleTags.addArray({ "pop", "c-pop", "rnb", "neo-soul" });
    chineseRnbLate.moodTags.addArray({ "smooth", "warm", "sensual" });
    chineseRnbLate.regionTags.addArray({ "chinese" });
    chineseRnbLate.functionFit.addArray({ "S", "D" });
    chineseRnbLate.formFit.addArray({ "verse", "bridge" });
    chineseRnbLate.familyId = "JayStyleSyncopationFamily";
    chineseRnbLate.grooveAffinity = "Late";
    chineseRnbLate.bassAnchored = true;
    chineseRnbLate.supportsExtensions = true;
    chineseRnbLate.complexity = 4;
    chineseRnbLate.energy = 3;
    patterns.push_back(chineseRnbLate);

    // 周杰伦五声 R&B (JayStyleSyncopationFamily)
    auto jayPentatonicRnb = makePattern("jay_pentatonic_rnb", cn(u8"周杰伦五声 R&B"), "Jay Pentatonic RnB",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                        { step(0.0, 1.1, 0, -1, 6, true),
                                          step(1.1, 0.55, 4, 0, 2),
                                          step(2.0, 0.8, 2, 0, -3),
                                          step(2.8, 0.65, 5, 1, 5),
                                          step(3.5, 0.45, 1, 0, -1) });
    jayPentatonicRnb.styleTags.addArray({ "pop", "c-pop", "jay_chou", "rnb", "chinese" });
    jayPentatonicRnb.moodTags.addArray({ "nostalgic", "smooth", "warm" });
    jayPentatonicRnb.regionTags.addArray({ "chinese" });
    jayPentatonicRnb.functionFit.addArray({ "T", "S", "D" });
    jayPentatonicRnb.formFit.addArray({ "verse", "chorus" });
    jayPentatonicRnb.familyId = "JayStyleSyncopationFamily";
    jayPentatonicRnb.grooveAffinity = "Late";
    jayPentatonicRnb.bassAnchored = true;
    jayPentatonicRnb.supportsExtensions = true;
    jayPentatonicRnb.complexity = 4;
    jayPentatonicRnb.energy = 3;
    patterns.push_back(jayPentatonicRnb);

    // Lo-fi 华语流行 (NeoSoulFamily)
    auto lofiMandopop = makePattern("lofi_mandopop", cn(u8"Lo-fi 华语流行"), "Lo-fi Mandopop",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                    { step(0.0, 0.85, 0, -1, 4, true),
                                      step(0.85, 0.4, 2, 0, -3),
                                      step(1.45, 0.6, 1, 0, -1),
                                      step(2.2, 0.55, 3, 1, 2),
                                      step(2.95, 0.5, 2, 0, -2) });
    lofiMandopop.styleTags.addArray({ "lofi", "hip-hop", "c-pop", "chinese" });
    lofiMandopop.moodTags.addArray({ "warm", "calm", "nostalgic" });
    lofiMandopop.regionTags.addArray({ "chinese" });
    lofiMandopop.functionFit.addArray({ "T", "S", "t" });
    lofiMandopop.formFit.addArray({ "verse", "bridge" });
    lofiMandopop.familyId = "NeoSoulFamily";
    lofiMandopop.grooveAffinity = "Shuffle";
    lofiMandopop.bassAnchored = true;
    lofiMandopop.complexity = 3;
    lofiMandopop.energy = 2;
    patterns.push_back(lofiMandopop);

    // 都市华语灵魂 (JayStyleSyncopationFamily)
    auto urbanChineseSoul = makePattern("urban_chinese_soul", cn(u8"都市华语灵魂"), "Urban Chinese Soul",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                        { step(0.0, 1.2, 0, -1, 5, true),
                                          step(1.2, 0.6, 2, 0, -2),
                                          step(2.0, 0.9, 3, 1, 4),
                                          step(2.9, 0.5, 1, 0, -1),
                                          step(3.5, 0.4, 2, 0, 2) });
    urbanChineseSoul.styleTags.addArray({ "pop", "c-pop", "rnb", "soul", "urban" });
    urbanChineseSoul.moodTags.addArray({ "smooth", "warm", "emotional" });
    urbanChineseSoul.regionTags.addArray({ "chinese" });
    urbanChineseSoul.functionFit.addArray({ "S", "D" });
    urbanChineseSoul.formFit.addArray({ "verse", "bridge" });
    urbanChineseSoul.familyId = "JayStyleSyncopationFamily";
    urbanChineseSoul.grooveAffinity = "Late";
    urbanChineseSoul.bassAnchored = true;
    urbanChineseSoul.supportsExtensions = true;
    urbanChineseSoul.complexity = 4;
    urbanChineseSoul.energy = 3;
    patterns.push_back(urbanChineseSoul);


        // ========================================================================
    // 补充: MandopopPrechorusFamily 多速度版本 (slow/mid/energetic)
    // ========================================================================

    auto mandopopPrechorusSlow = makePattern("mandopop_prechorus_slow", cn(u8"华语主歌过渡慢速"), "Mandopop Prechorus Slow",
                                             ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                             { step(0.0, 1.2, 0, -1, 5, true),
                                               step(1.2, 0.9, 2, 0, -2),
                                               step(2.2, 1.0, 4, 1, 3),
                                               step(3.2, 0.8, 1, 0, -1) });
    mandopopPrechorusSlow.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopPrechorusSlow.moodTags.addArray({ "building", "calm", "tension" });
    mandopopPrechorusSlow.regionTags.addArray({ "chinese" });
    mandopopPrechorusSlow.functionFit.addArray({ "S", "D" });
    mandopopPrechorusSlow.formFit.addArray({ "prechorus", "bridge" });
    mandopopPrechorusSlow.familyId = "MandopopPrechorusFamily";
    mandopopPrechorusSlow.grooveAffinity = "Straight";
    mandopopPrechorusSlow.bassAnchored = true;
    mandopopPrechorusSlow.complexity = 2;
    mandopopPrechorusSlow.energy = 2;
    patterns.push_back(mandopopPrechorusSlow);

    auto mandopopPrechorusEnergetic = makePattern("mandopop_prechorus_energetic", cn(u8"华语主歌过渡激进"), "Mandopop Prechorus Energetic",
                                                  ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 2.0,
                                                  { step(0.0, 0.25, 0, -1, 8, true),
                                                    step(0.25, 0.2, 2, 0, -1),
                                                    step(0.5, 0.25, 3, 1, 5),
                                                    step(0.75, 0.2, 1, 0, -2),
                                                    step(1.0, 0.25, 4, 1, 7),
                                                    step(1.25, 0.2, 2, 0, 0),
                                                    step(1.5, 0.25, 1, 0, 3),
                                                    step(1.75, 0.2, 0, -1, 6, true) });
    mandopopPrechorusEnergetic.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopPrechorusEnergetic.moodTags.addArray({ "building", "intense", "hopeful" });
    mandopopPrechorusEnergetic.regionTags.addArray({ "chinese" });
    mandopopPrechorusEnergetic.functionFit.addArray({ "S", "D" });
    mandopopPrechorusEnergetic.formFit.addArray({ "prechorus", "bridge" });
    mandopopPrechorusEnergetic.familyId = "MandopopPrechorusFamily";
    mandopopPrechorusEnergetic.grooveAffinity = "Tight";
    mandopopPrechorusEnergetic.bassAnchored = true;
    mandopopPrechorusEnergetic.complexity = 3;
    mandopopPrechorusEnergetic.energy = 4;
    patterns.push_back(mandopopPrechorusEnergetic);

    // ========================================================================
    // 补充: MandopopBridgeFamily 多速度版本
    // ========================================================================

    auto mandopopBridgeSlow = makePattern("mandopop_bridge_slow", cn(u8"华语桥段慢速"), "Mandopop Bridge Slow",
                                          ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                          { step(0.0, 1.5, 0, -1, 4, true),
                                            step(1.5, 1.0, 2, 0, -1),
                                            step(2.5, 1.2, 3, 1, 3),
                                            step(3.5, 0.5, 1, 0, -2) });
    mandopopBridgeSlow.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopBridgeSlow.moodTags.addArray({ "climax", "emotional", "calm" });
    mandopopBridgeSlow.regionTags.addArray({ "chinese" });
    mandopopBridgeSlow.functionFit.addArray({ "D", "S" });
    mandopopBridgeSlow.formFit.addArray({ "bridge" });
    mandopopBridgeSlow.familyId = "MandopopBridgeFamily";
    mandopopBridgeSlow.grooveAffinity = "Straight";
    mandopopBridgeSlow.bassAnchored = true;
    mandopopBridgeSlow.complexity = 2;
    mandopopBridgeSlow.energy = 2;
    patterns.push_back(mandopopBridgeSlow);

    auto mandopopBridgeEnergetic = makePattern("mandopop_bridge_energetic", cn(u8"华语桥段激进"), "Mandopop Bridge Energetic",
                                               ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 2.0,
                                               { step(0.0, 0.2, 0, -1, 9, true),
                                                 step(0.25, 0.18, 2, 0, -2),
                                                 step(0.5, 0.2, 3, 1, 6),
                                                 step(0.75, 0.18, 4, 1, 8),
                                                 step(1.0, 0.2, 1, 0, -1),
                                                 step(1.25, 0.18, 2, 0, 2),
                                                 step(1.5, 0.2, 5, 2, 7),
                                                 step(1.75, 0.18, 0, -1, 9, true) });
    mandopopBridgeEnergetic.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopBridgeEnergetic.moodTags.addArray({ "climax", "epic", "intense" });
    mandopopBridgeEnergetic.regionTags.addArray({ "chinese" });
    mandopopBridgeEnergetic.functionFit.addArray({ "D", "S" });
    mandopopBridgeEnergetic.formFit.addArray({ "bridge" });
    mandopopBridgeEnergetic.familyId = "MandopopBridgeFamily";
    mandopopBridgeEnergetic.grooveAffinity = "Tight";
    mandopopBridgeEnergetic.bassAnchored = true;
    mandopopBridgeEnergetic.complexity = 4;
    mandopopBridgeEnergetic.energy = 5;
    patterns.push_back(mandopopBridgeEnergetic);

    // ========================================================================
    // 补充: CPopChorusDriveFamily 多速度版本
    // ========================================================================

    auto cpopChorusDriveSlow = makePattern("cpop_chorus_drive_slow", cn(u8"华语副歌驱动慢速"), "C-Pop Chorus Drive Slow",
                                           ArpeggioDirection::Up, ArpeggioDensity::Half, 4.0,
                                           { step(0.0, 1.2, 0, -1, 5, true),
                                             step(1.2, 0.8, 2, 0, -1),
                                             step(2.2, 1.0, 3, 1, 4),
                                             step(3.2, 0.6, 1, 0, -2) });
    cpopChorusDriveSlow.styleTags.addArray({ "pop", "c-pop", "ballad" });
    cpopChorusDriveSlow.moodTags.addArray({ "hopeful", "bright", "calm" });
    cpopChorusDriveSlow.regionTags.addArray({ "chinese" });
    cpopChorusDriveSlow.functionFit.addArray({ "T", "S", "D" });
    cpopChorusDriveSlow.formFit.addArray({ "chorus" });
    cpopChorusDriveSlow.familyId = "CPopChorusDriveFamily";
    cpopChorusDriveSlow.grooveAffinity = "Straight";
    cpopChorusDriveSlow.bassAnchored = true;
    cpopChorusDriveSlow.complexity = 2;
    cpopChorusDriveSlow.energy = 3;
    patterns.push_back(cpopChorusDriveSlow);

    auto cpopChorusDriveEnergetic = makePattern("cpop_chorus_drive_energetic", cn(u8"华语副歌驱动激进"), "C-Pop Chorus Drive Energetic",
                                                ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 2.0,
                                                { step(0.0, 0.2, 0, -1, 9, true),
                                                  step(0.25, 0.18, 2, 0, -1),
                                                  step(0.5, 0.2, 4, 1, 6),
                                                  step(0.75, 0.18, 1, 0, -2),
                                                  step(1.0, 0.2, 3, 1, 7),
                                                  step(1.25, 0.18, 2, 0, 0),
                                                  step(1.5, 0.2, 5, 2, 8),
                                                  step(1.75, 0.18, 0, -1, 9, true) });
    cpopChorusDriveEnergetic.styleTags.addArray({ "pop", "c-pop", "ballad" });
    cpopChorusDriveEnergetic.moodTags.addArray({ "energetic", "bright", "hopeful" });
    cpopChorusDriveEnergetic.regionTags.addArray({ "chinese" });
    cpopChorusDriveEnergetic.functionFit.addArray({ "T", "S", "D" });
    cpopChorusDriveEnergetic.formFit.addArray({ "chorus" });
    cpopChorusDriveEnergetic.familyId = "CPopChorusDriveFamily";
    cpopChorusDriveEnergetic.grooveAffinity = "Tight";
    cpopChorusDriveEnergetic.bassAnchored = true;
    cpopChorusDriveEnergetic.complexity = 4;
    cpopChorusDriveEnergetic.energy = 5;
    patterns.push_back(cpopChorusDriveEnergetic);

    // ========================================================================
    // 中国流行编曲角色 (Intro Hook, Interlude, Outro Fade, Piano Hook, Guitar/Piano Layer)
    // ========================================================================

    auto mandopopIntroHook = makePattern("mandopop_intro_hook", cn(u8"华语前奏动机"), "Mandopop Intro Hook",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                         { step(0.0, 0.8, 0, -1, 4, true),
                                           step(0.8, 0.5, 2, 0, -1),
                                           step(1.6, 1.0, 3, 1, 3),
                                           step(2.6, 0.4, 1, 0, -2),
                                           step(3.0, 0.6, 0, -1, 5, true) });
    mandopopIntroHook.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopIntroHook.moodTags.addArray({ "catchy", "hopeful", "bright" });
    mandopopIntroHook.regionTags.addArray({ "chinese" });
    mandopopIntroHook.functionFit.addArray({ "T", "S" });
    mandopopIntroHook.formFit.addArray({ "intro" });
    mandopopIntroHook.familyId = "BrokenPianoFamily";
    mandopopIntroHook.grooveAffinity = "Straight";
    mandopopIntroHook.bassAnchored = true;
    mandopopIntroHook.complexity = 3;
    mandopopIntroHook.energy = 2;
    patterns.push_back(mandopopIntroHook);

    auto mandopopInterlude = makePattern("mandopop_interlude", cn(u8"华语间奏"), "Mandopop Interlude",
                                         ArpeggioDirection::Down, ArpeggioDensity::Eighth, 2.0,
                                         { step(0.0, 0.5, 0, -1, 5, true),
                                           step(0.75, 0.45, 2, 0, -1),
                                           step(1.25, 0.5, 1, 0, 2),
                                           step(1.75, 0.45, 2, 0, -2) });
    mandopopInterlude.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopInterlude.moodTags.addArray({ "transition", "calm", "flowing" });
    mandopopInterlude.regionTags.addArray({ "chinese" });
    mandopopInterlude.functionFit.addArray({ "S", "T" });
    mandopopInterlude.formFit.addArray({ "interlude" });
    mandopopInterlude.familyId = "BrokenPianoFamily";
    mandopopInterlude.grooveAffinity = "Straight";
    mandopopInterlude.bassAnchored = true;
    mandopopInterlude.complexity = 2;
    mandopopInterlude.energy = 2;
    patterns.push_back(mandopopInterlude);

    auto mandopopOutroFade = makePattern("mandopop_outro_fade", cn(u8"华语尾奏渐弱"), "Mandopop Outro Fade",
                                         ArpeggioDirection::Down, ArpeggioDensity::Whole, 4.0,
                                         { step(0.0, 1.5, 0, -1, 3, true),
                                           step(1.5, 1.2, 2, 0, -2),
                                           step(2.7, 1.0, 1, 0, -1),
                                           step(3.7, 0.3, 0, -1, 2, true) });
    mandopopOutroFade.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopOutroFade.moodTags.addArray({ "resolved", "calm", "sad" });
    mandopopOutroFade.regionTags.addArray({ "chinese" });
    mandopopOutroFade.functionFit.addArray({ "T", "D" });
    mandopopOutroFade.formFit.addArray({ "outro", "cadence" });
    mandopopOutroFade.familyId = "CadentialBreathFamily";
    mandopopOutroFade.grooveAffinity = "Straight";
    mandopopOutroFade.bassAnchored = true;
    mandopopOutroFade.complexity = 1;
    mandopopOutroFade.energy = 1;
    patterns.push_back(mandopopOutroFade);

    auto cpopPianoHook = makePattern("cpop_piano_hook", cn(u8"华语钢琴动机"), "C-Pop Piano Hook",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0,
                                     { step(0.0, 0.5, 0, -1, 6, true),
                                       step(0.5, 0.4, 2, 0, -2),
                                       step(1.0, 0.6, 3, 1, 4),
                                       step(1.6, 0.3, 1, 0, -1) });
    cpopPianoHook.styleTags.addArray({ "pop", "c-pop", "piano", "hook" });
    cpopPianoHook.moodTags.addArray({ "catchy", "bright", "hopeful" });
    cpopPianoHook.regionTags.addArray({ "chinese" });
    cpopPianoHook.functionFit.addArray({ "T", "S" });
    cpopPianoHook.formFit.addArray({ "intro", "chorus" });
    cpopPianoHook.familyId = "CPopChorusDriveFamily";
    cpopPianoHook.grooveAffinity = "Straight";
    cpopPianoHook.bassAnchored = true;
    cpopPianoHook.complexity = 2;
    cpopPianoHook.energy = 3;
    patterns.push_back(cpopPianoHook);

    auto cpopGuitarPianoLayer = makePattern("cpop_guitar_piano_layer", cn(u8"华语吉他钢琴层"), "C-Pop Guitar/Piano Layer",
                                            ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                            { step(0.0, 0.45, 0, -1, 5, true),
                                              step(0.5, 0.4, 2, 0, -1),
                                              step(1.0, 0.45, 4, 0, 2),
                                              step(1.5, 0.4, 1, 0, -2) });
    cpopGuitarPianoLayer.styleTags.addArray({ "pop", "c-pop", "guitar", "piano" });
    cpopGuitarPianoLayer.moodTags.addArray({ "warm", "bright", "hopeful" });
    cpopGuitarPianoLayer.regionTags.addArray({ "chinese" });
    cpopGuitarPianoLayer.functionFit.addArray({ "T", "S" });
    cpopGuitarPianoLayer.formFit.addArray({ "verse", "chorus" });
    cpopGuitarPianoLayer.familyId = "GuitarPickingFamily";
    cpopGuitarPianoLayer.grooveAffinity = "Straight";
    cpopGuitarPianoLayer.bassAnchored = true;
    cpopGuitarPianoLayer.complexity = 2;
    cpopGuitarPianoLayer.energy = 3;
    patterns.push_back(cpopGuitarPianoLayer);

    // ========================================================================
    // 国风场景 (Palace / Jianghu / Fairy / Lonely Mountain / Festival)
    // ========================================================================

    auto guofengPalace = makePattern("guofeng_palace", cn(u8"宫廷"), "Guofeng Palace",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                     { step(0.0, 1.2, 0, -1, 5, true),
                                       step(1.2, 0.8, 2, 0, -1),
                                       step(2.0, 1.0, 4, 1, 4),
                                       step(3.0, 0.6, 1, 0, -2) });
    guofengPalace.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengPalace.moodTags.addArray({ "solemn", "elegant", "wide" });
    guofengPalace.regionTags.addArray({ "chinese" });
    guofengPalace.functionFit.addArray({ "T", "S" });
    guofengPalace.formFit.addArray({ "verse", "chorus" });
    guofengPalace.familyId = "GuofengFlowFamily";
    guofengPalace.grooveAffinity = "Straight";
    guofengPalace.bassAnchored = true;
    guofengPalace.complexity = 2;
    guofengPalace.energy = 2;
    patterns.push_back(guofengPalace);

    auto guofengJianghu = makePattern("guofeng_jianghu", cn(u8"江湖"), "Guofeng Jianghu",
                                      ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                      { step(0.0, 0.6, 0, -1, 7, true),
                                        step(0.6, 0.5, 4, 0, 3),
                                        step(1.2, 0.6, 2, 0, -1),
                                        step(1.8, 0.5, 5, 1, 6) });
    guofengJianghu.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengJianghu.moodTags.addArray({ "heroic", "brave", "energetic" });
    guofengJianghu.regionTags.addArray({ "chinese" });
    guofengJianghu.functionFit.addArray({ "S", "D" });
    guofengJianghu.formFit.addArray({ "chorus", "bridge" });
    guofengJianghu.familyId = "MarchPulseFamily";
    guofengJianghu.grooveAffinity = "Tight";
    guofengJianghu.bassAnchored = true;
    guofengJianghu.complexity = 3;
    guofengJianghu.energy = 4;
    patterns.push_back(guofengJianghu);

    auto guofengFairy = makePattern("guofeng_fairy", cn(u8"仙侠"), "Guofeng Fairy",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                    { step(0.0, 1.5, 0, -1, 4, true),
                                      step(1.2, 0.9, 2, 0, -1),
                                      step(2.2, 1.2, 4, 1, 3),
                                      step(3.4, 0.6, 1, 0, -2) });
    guofengFairy.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengFairy.moodTags.addArray({ "ethereal", "dreamy", "wide" });
    guofengFairy.regionTags.addArray({ "chinese" });
    guofengFairy.functionFit.addArray({ "T", "S" });
    guofengFairy.formFit.addArray({ "intro", "verse" });
    guofengFairy.familyId = "GuofengFlowFamily";
    guofengFairy.grooveAffinity = "Straight";
    guofengFairy.bassAnchored = true;
    guofengFairy.pedalFriendly = true;
    guofengFairy.complexity = 2;
    guofengFairy.energy = 2;
    patterns.push_back(guofengFairy);

    auto guofengLonelyMountain = makePattern("guofeng_lonely_mountain", cn(u8"孤山"), "Guofeng Lonely Mountain",
                                             ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                             { step(0.0, 2.0, 0, -1, 3, true),
                                               step(1.5, 1.2, 2, 0, -1),
                                               step(2.8, 1.0, 1, 0, -2),
                                               step(3.8, 0.2, 0, -1, 2, true) });
    guofengLonelyMountain.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengLonelyMountain.moodTags.addArray({ "sad", "wide", "solemn" });
    guofengLonelyMountain.regionTags.addArray({ "chinese" });
    guofengLonelyMountain.functionFit.addArray({ "T", "t" });
    guofengLonelyMountain.formFit.addArray({ "intro", "outro" });
    guofengLonelyMountain.familyId = "GuqinSparseFamily";
    guofengLonelyMountain.grooveAffinity = "Straight";
    guofengLonelyMountain.bassAnchored = true;
    guofengLonelyMountain.pedalFriendly = true;
    guofengLonelyMountain.complexity = 1;
    guofengLonelyMountain.energy = 1;
    patterns.push_back(guofengLonelyMountain);

    auto guofengFestival = makePattern("guofeng_festival", cn(u8"节庆"), "Guofeng Festival",
                                       ArpeggioDirection::UpDown, ArpeggioDensity::Sixteenth, 2.0,
                                       { step(0.0, 0.2, 0, -1, 8, true),
                                         step(0.25, 0.18, 2, 0, 0),
                                         step(0.5, 0.2, 3, 1, 6),
                                         step(0.75, 0.18, 1, 0, -2),
                                         step(1.0, 0.2, 4, 1, 7),
                                         step(1.25, 0.18, 2, 0, 1),
                                         step(1.5, 0.2, 5, 2, 8),
                                         step(1.75, 0.18, 0, -1, 9, true) });
    guofengFestival.styleTags.addArray({ "chinese", "ancient_style", "guofeng", "festival" });
    guofengFestival.moodTags.addArray({ "bright", "playful", "energetic" });
    guofengFestival.regionTags.addArray({ "chinese" });
    guofengFestival.functionFit.addArray({ "S", "D" });
    guofengFestival.formFit.addArray({ "chorus", "bridge" });
    guofengFestival.familyId = "GuzhengRollFamily";
    guofengFestival.grooveAffinity = "Tight";
    guofengFestival.bassAnchored = false;
    guofengFestival.complexity = 4;
    guofengFestival.energy = 5;
    patterns.push_back(guofengFestival);

    // ========================================================================
    // 民乐器动作补充 (Yangqin Bounce, Sheng Chord Pulse, Xiao Breath, Zhongruan Pick)
    // ========================================================================

    auto yangqinBounce = makePattern("yangqin_bounce", cn(u8"扬琴弹跳"), "Yangqin Bounce",
                                     ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.35, 0, -1, 6, true),
                                       step(0.5, 0.3, 2, 0, -2),
                                       step(1.0, 0.35, 4, 0, 3),
                                       step(1.5, 0.3, 2, 0, -1) });
    yangqinBounce.styleTags.addArray({ "chinese", "yangqin", "folk" });
    yangqinBounce.moodTags.addArray({ "bright", "playful", "hopeful" });
    yangqinBounce.regionTags.addArray({ "chinese" });
    yangqinBounce.functionFit.addArray({ "S", "D" });
    yangqinBounce.formFit.addArray({ "verse", "chorus" });
    yangqinBounce.familyId = "GuzhengRollFamily";
    yangqinBounce.grooveAffinity = "Straight";
    yangqinBounce.bassAnchored = false;
    yangqinBounce.complexity = 3;
    yangqinBounce.energy = 3;
    patterns.push_back(yangqinBounce);

    auto shengChordPulse = makePattern("sheng_chord_pulse", cn(u8"笙和弦脉冲"), "Sheng Chord Pulse",
                                       ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 4.0,
                                       { step(0.0, 1.8, 0, -1, 5, true),
                                         step(0.0, 1.8, 2, -1, 4, true),
                                         step(0.0, 1.8, 4, 0, 3),
                                         step(2.0, 1.6, 0, -1, 4, true),
                                         step(2.0, 1.6, 2, -1, 3, true),
                                         step(2.0, 1.6, 4, 0, 2) });
    shengChordPulse.styleTags.addArray({ "chinese", "sheng", "folk", "ancient_style" });
    shengChordPulse.moodTags.addArray({ "wide", "calm", "bright" });
    shengChordPulse.regionTags.addArray({ "chinese" });
    shengChordPulse.functionFit.addArray({ "T", "S" });
    shengChordPulse.formFit.addArray({ "verse", "chorus" });
    shengChordPulse.familyId = "PadFamily";
    shengChordPulse.grooveAffinity = "Straight";
    shengChordPulse.bassAnchored = true;
    shengChordPulse.pedalFriendly = true;
    shengChordPulse.complexity = 2;
    shengChordPulse.energy = 2;
    patterns.push_back(shengChordPulse);

    auto xiaoBreath = makePattern("xiao_breath", cn(u8"箫气息"), "Xiao Breath",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                  { step(0.0, 3.5, 0, -1, 4, true),
                                    step(1.5, 1.5, 2, 0, -1),
                                    step(3.0, 0.9, 1, 0, 1) });
    xiaoBreath.styleTags.addArray({ "chinese", "xiao", "folk", "ancient_style" });
    xiaoBreath.moodTags.addArray({ "sad", "calm", "wide" });
    xiaoBreath.regionTags.addArray({ "chinese" });
    xiaoBreath.functionFit.addArray({ "T", "t" });
    xiaoBreath.formFit.addArray({ "intro", "verse", "outro" });
    xiaoBreath.familyId = "GuqinSparseFamily";
    xiaoBreath.grooveAffinity = "Straight";
    xiaoBreath.bassAnchored = true;
    xiaoBreath.pedalFriendly = true;
    xiaoBreath.complexity = 1;
    xiaoBreath.energy = 1;
    patterns.push_back(xiaoBreath);

    auto zhongruanPick = makePattern("zhongruan_pick", cn(u8"中阮弹拨"), "Zhongruan Pick",
                                     ArpeggioDirection::Alternating, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.45, 0, -1, 6, true),
                                       step(0.5, 0.4, 2, 0, -2),
                                       step(1.0, 0.45, 4, 0, 3),
                                       step(1.5, 0.4, 1, 0, -1) });
    zhongruanPick.styleTags.addArray({ "chinese", "zhongruan", "folk", "ancient_style" });
    zhongruanPick.moodTags.addArray({ "bright", "calm", "elegant" });
    zhongruanPick.regionTags.addArray({ "chinese" });
    zhongruanPick.functionFit.addArray({ "T", "S" });
    zhongruanPick.formFit.addArray({ "verse", "chorus" });
    zhongruanPick.familyId = "GuitarPickingFamily";
    zhongruanPick.grooveAffinity = "Straight";
    zhongruanPick.bassAnchored = true;
    zhongruanPick.complexity = 2;
    zhongruanPick.energy = 3;
    patterns.push_back(zhongruanPick);


        // ========================================================================
    // 华语流行功能场景 (Mandopop Functional Scenes)
    // ========================================================================

    auto mandopopPianoIntroSad = makePattern("mandopop_piano_intro_sad", cn(u8"华语钢琴前奏伤感"), "Mandopop Piano Intro Sad",
                                             ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                             { step(0.0, 1.5, 0, -1, 3, true),
                                               step(1.5, 1.0, 2, 0, -1),
                                               step(2.5, 1.2, 1, 0, -2),
                                               step(3.7, 0.3, 0, -1, 2, true) });
    mandopopPianoIntroSad.styleTags.addArray({ "pop", "mandopop", "c-pop", "piano" });
    mandopopPianoIntroSad.moodTags.addArray({ "sad", "calm", "nostalgic" });
    mandopopPianoIntroSad.regionTags.addArray({ "chinese" });
    mandopopPianoIntroSad.functionFit.addArray({ "T", "t" });
    mandopopPianoIntroSad.formFit.addArray({ "intro" });
    mandopopPianoIntroSad.familyId = "BrokenPianoFamily";
    mandopopPianoIntroSad.grooveAffinity = "Straight";
    mandopopPianoIntroSad.bassAnchored = true;
    mandopopPianoIntroSad.complexity = 1;
    mandopopPianoIntroSad.energy = 1;
    patterns.push_back(mandopopPianoIntroSad);

    auto mandopopPrechorusTension = makePattern("mandopop_prechorus_tension", cn(u8"华语预副歌张力"), "Mandopop Prechorus Tension",
                                                ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0,
                                                { step(0.0, 0.5, 0, -1, 6, true),
                                                  step(0.5, 0.4, 2, 0, -1),
                                                  step(1.0, 0.5, 3, 1, 5),
                                                  step(1.5, 0.4, 1, 0, -2) });
    mandopopPrechorusTension.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopPrechorusTension.moodTags.addArray({ "tension", "building", "urgent" });
    mandopopPrechorusTension.regionTags.addArray({ "chinese" });
    mandopopPrechorusTension.functionFit.addArray({ "D", "S" });
    mandopopPrechorusTension.formFit.addArray({ "prechorus" });
    mandopopPrechorusTension.familyId = "MandopopPrechorusFamily";
    mandopopPrechorusTension.grooveAffinity = "Straight";
    mandopopPrechorusTension.bassAnchored = true;
    mandopopPrechorusTension.complexity = 2;
    mandopopPrechorusTension.energy = 3;
    patterns.push_back(mandopopPrechorusTension);

    auto cpopChorusHalfTime = makePattern("cpop_chorus_half_time", cn(u8"华语副歌半速"), "C-Pop Chorus Half-Time",
                                          ArpeggioDirection::UpDown, ArpeggioDensity::Half, 4.0,
                                          { step(0.0, 1.2, 0, -1, 6, true),
                                            step(1.2, 0.8, 2, 0, -1),
                                            step(2.2, 1.0, 3, 1, 4),
                                            step(3.2, 0.8, 1, 0, -2) });
    cpopChorusHalfTime.styleTags.addArray({ "pop", "c-pop", "ballad" });
    cpopChorusHalfTime.moodTags.addArray({ "groovy", "relaxed", "hopeful" });
    cpopChorusHalfTime.regionTags.addArray({ "chinese" });
    cpopChorusHalfTime.functionFit.addArray({ "T", "S" });
    cpopChorusHalfTime.formFit.addArray({ "chorus" });
    cpopChorusHalfTime.familyId = "CPopChorusDriveFamily";
    cpopChorusHalfTime.grooveAffinity = "Straight";
    cpopChorusHalfTime.bassAnchored = true;
    cpopChorusHalfTime.complexity = 2;
    cpopChorusHalfTime.energy = 3;
    patterns.push_back(cpopChorusHalfTime);

    auto mandopopFinalChorusBig = makePattern("mandopop_final_chorus_big", cn(u8"华语副歌终极大"), "Mandopop Final Chorus Big",
                                              ArpeggioDirection::UpDown, ArpeggioDensity::Mixed, 4.0,
                                              { step(0.0, 0.8, 0, -1, 9, true),
                                                step(0.8, 0.5, 2, 0, -1),
                                                step(1.6, 0.7, 3, 1, 6),
                                                step(2.4, 0.6, 4, 1, 8),
                                                step(3.2, 0.8, 1, 0, -2) });
    mandopopFinalChorusBig.styleTags.addArray({ "pop", "mandopop", "c-pop" });
    mandopopFinalChorusBig.moodTags.addArray({ "epic", "climax", "powerful" });
    mandopopFinalChorusBig.regionTags.addArray({ "chinese" });
    mandopopFinalChorusBig.functionFit.addArray({ "D", "T" });
    mandopopFinalChorusBig.formFit.addArray({ "final_chorus", "chorus" });
    mandopopFinalChorusBig.familyId = "CPopChorusDriveFamily";
    mandopopFinalChorusBig.grooveAffinity = "Tight";
    mandopopFinalChorusBig.bassAnchored = true;
    mandopopFinalChorusBig.complexity = 4;
    mandopopFinalChorusBig.energy = 5;
    patterns.push_back(mandopopFinalChorusBig);

    // ========================================================================
    // 国风现代混合 (Guofeng Modern Fusion)
    // ========================================================================

    auto guofengPopVerse = makePattern("guofeng_pop_verse", cn(u8"国风流行主歌"), "Guofeng Pop Verse",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                       { step(0.0, 0.55, 0, -1, 5, true),
                                         step(0.75, 0.45, 2, 0, -1),
                                         step(1.25, 0.55, 4, 0, 2),
                                         step(1.75, 0.45, 1, 0, -2) });
    guofengPopVerse.styleTags.addArray({ "pop", "c-pop", "chinese", "ancient_style", "guofeng" });
    guofengPopVerse.moodTags.addArray({ "calm", "warm", "hopeful" });
    guofengPopVerse.regionTags.addArray({ "chinese" });
    guofengPopVerse.functionFit.addArray({ "T", "S" });
    guofengPopVerse.formFit.addArray({ "verse" });
    guofengPopVerse.familyId = "GuofengFlowFamily";
    guofengPopVerse.grooveAffinity = "Straight";
    guofengPopVerse.bassAnchored = true;
    guofengPopVerse.complexity = 2;
    guofengPopVerse.energy = 2;
    patterns.push_back(guofengPopVerse);

    auto guofengRnbLate = makePattern("guofeng_rnb_late", cn(u8"国风 R&B 晚切分"), "Guofeng RnB Late",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                      { step(0.0, 1.2, 0, -1, 5, true),
                                        step(1.2, 0.6, 2, 0, -2),
                                        step(2.0, 0.9, 4, 1, 4),
                                        step(2.9, 0.5, 1, 0, -1),
                                        step(3.5, 0.4, 2, 0, 2) });
    guofengRnbLate.styleTags.addArray({ "pop", "c-pop", "chinese", "ancient_style", "rnb", "neo-soul" });
    guofengRnbLate.moodTags.addArray({ "smooth", "warm", "nostalgic" });
    guofengRnbLate.regionTags.addArray({ "chinese" });
    guofengRnbLate.functionFit.addArray({ "S", "D" });
    guofengRnbLate.formFit.addArray({ "verse", "bridge" });
    guofengRnbLate.familyId = "JayStyleSyncopationFamily";
    guofengRnbLate.grooveAffinity = "Late";
    guofengRnbLate.bassAnchored = true;
    guofengRnbLate.supportsExtensions = true;
    guofengRnbLate.complexity = 4;
    guofengRnbLate.energy = 3;
    patterns.push_back(guofengRnbLate);

    auto guofengRockChorus = makePattern("guofeng_rock_chorus", cn(u8"国风摇滚副歌"), "Guofeng Rock Chorus",
                                         ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 2.0,
                                         { step(0.0, 0.45, 0, -1, 8, true),
                                           step(0.5, 0.4, 2, 0, -1),
                                           step(1.0, 0.45, 3, 1, 6),
                                           step(1.5, 0.4, 1, 0, -2) });
    guofengRockChorus.styleTags.addArray({ "pop", "c-pop", "chinese", "ancient_style", "rock", "chinese_rock" });
    guofengRockChorus.moodTags.addArray({ "energetic", "bright", "powerful" });
    guofengRockChorus.regionTags.addArray({ "chinese" });
    guofengRockChorus.functionFit.addArray({ "S", "D" });
    guofengRockChorus.formFit.addArray({ "chorus" });
    guofengRockChorus.familyId = "MarchPulseFamily";
    guofengRockChorus.grooveAffinity = "Tight";
    guofengRockChorus.bassAnchored = true;
    guofengRockChorus.complexity = 3;
    guofengRockChorus.energy = 5;
    patterns.push_back(guofengRockChorus);

    auto guofengElectroPulse = makePattern("guofeng_electro_pulse", cn(u8"国风电音脉冲"), "Guofeng Electro Pulse",
                                           ArpeggioDirection::BlockPulse, ArpeggioDensity::Sixteenth, 1.0,
                                           { step(0.0, 0.12, 0, -1, 8, true),
                                             step(0.25, 0.1, 4, 0, 4),
                                             step(0.5, 0.12, 0, -1, 7, true),
                                             step(0.75, 0.1, 4, 0, 5) });
    guofengElectroPulse.styleTags.addArray({ "pop", "c-pop", "chinese", "ancient_style", "edm", "electro" });
    guofengElectroPulse.moodTags.addArray({ "energetic", "bright", "dance" });
    guofengElectroPulse.regionTags.addArray({ "chinese" });
    guofengElectroPulse.functionFit.addArray({ "S", "D" });
    guofengElectroPulse.formFit.addArray({ "chorus", "bridge" });
    guofengElectroPulse.familyId = "SyncopatedElectronicFamily";
    guofengElectroPulse.grooveAffinity = "Tight";
    guofengElectroPulse.bassAnchored = true;
    guofengElectroPulse.complexity = 3;
    guofengElectroPulse.energy = 4;
    patterns.push_back(guofengElectroPulse);

    // ========================================================================
    // 低能量候选 (Intro / Verse / Outro, Soft)
    // ========================================================================

    auto softCpopIntro = makePattern("soft_cpop_intro", cn(u8"轻柔华语前奏"), "Soft C-Pop Intro",
                                     ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0,
                                     { step(0.0, 2.0, 0, -1, 3, true),
                                       step(2.0, 1.5, 2, 0, -2),
                                       step(3.5, 0.5, 1, 0, -1) });
    softCpopIntro.styleTags.addArray({ "pop", "c-pop", "ballad" });
    softCpopIntro.moodTags.addArray({ "calm", "warm", "dreamy" });
    softCpopIntro.regionTags.addArray({ "chinese" });
    softCpopIntro.functionFit.addArray({ "T" });
    softCpopIntro.formFit.addArray({ "intro" });
    softCpopIntro.familyId = "PadFamily";
    softCpopIntro.grooveAffinity = "Straight";
    softCpopIntro.bassAnchored = true;
    softCpopIntro.pedalFriendly = true;
    softCpopIntro.complexity = 1;
    softCpopIntro.energy = 1;
    patterns.push_back(softCpopIntro);

    auto guofengEmptyIntro = makePattern("guofeng_empty_intro", cn(u8"古风留白前奏"), "Guofeng Empty Intro",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Whole, 4.0,
                                         { step(0.0, 3.5, 0, -1, 2, true),
                                           step(2.5, 1.0, 4, -1, 1, true) });
    guofengEmptyIntro.styleTags.addArray({ "chinese", "ancient_style", "guofeng" });
    guofengEmptyIntro.moodTags.addArray({ "calm", "wide", "solemn" });
    guofengEmptyIntro.regionTags.addArray({ "chinese" });
    guofengEmptyIntro.functionFit.addArray({ "T" });
    guofengEmptyIntro.formFit.addArray({ "intro" });
    guofengEmptyIntro.familyId = "GuqinSparseFamily";
    guofengEmptyIntro.grooveAffinity = "Straight";
    guofengEmptyIntro.bassAnchored = true;
    guofengEmptyIntro.pedalFriendly = true;
    guofengEmptyIntro.complexity = 1;
    guofengEmptyIntro.energy = 1;
    patterns.push_back(guofengEmptyIntro);

    auto xiaoOutroBreath = makePattern("xiao_outro_breath", cn(u8"箫尾声气息"), "Xiao Outro Breath",
                                       ArpeggioDirection::Down, ArpeggioDensity::Whole, 4.0,
                                       { step(0.0, 3.0, 0, -1, 3, true),
                                         step(2.0, 1.5, 2, 0, -2),
                                         step(3.5, 0.5, 1, 0, -1) });
    xiaoOutroBreath.styleTags.addArray({ "chinese", "xiao", "folk", "ancient_style" });
    xiaoOutroBreath.moodTags.addArray({ "sad", "calm", "wide" });
    xiaoOutroBreath.regionTags.addArray({ "chinese" });
    xiaoOutroBreath.functionFit.addArray({ "T" });
    xiaoOutroBreath.formFit.addArray({ "outro", "cadence" });
    xiaoOutroBreath.familyId = "GuqinSparseFamily";
    xiaoOutroBreath.grooveAffinity = "Straight";
    xiaoOutroBreath.bassAnchored = true;
    xiaoOutroBreath.pedalFriendly = true;
    xiaoOutroBreath.complexity = 1;
    xiaoOutroBreath.energy = 1;
    patterns.push_back(xiaoOutroBreath);

    auto pianoSingleNotePulse = makePattern("piano_single_note_pulse", cn(u8"钢琴单音脉冲"), "Piano Single Note Pulse",
                                            ArpeggioDirection::BlockPulse, ArpeggioDensity::Half, 2.0,
                                            { step(0.0, 0.8, 0, -1, 4, true),
                                              step(1.0, 0.8, 0, -1, 3, true) });
    pianoSingleNotePulse.styleTags.addArray({ "pop", "c-pop", "piano" });
    pianoSingleNotePulse.moodTags.addArray({ "calm", "minimal", "wide" });
    pianoSingleNotePulse.regionTags.addArray({ "chinese" });
    pianoSingleNotePulse.functionFit.addArray({ "T", "t" });
    pianoSingleNotePulse.formFit.addArray({ "intro", "verse", "outro" });
    pianoSingleNotePulse.familyId = "PadFamily";
    pianoSingleNotePulse.grooveAffinity = "Straight";
    pianoSingleNotePulse.bassAnchored = true;
    pianoSingleNotePulse.pedalFriendly = true;
    pianoSingleNotePulse.complexity = 1;
    pianoSingleNotePulse.energy = 1;
    patterns.push_back(pianoSingleNotePulse);

    // ========================================================================
    // 节拍/拍号特殊型 (3/4, 6/8, 12/8)
    // ========================================================================

    auto cpop6_8Ballad = makePattern("cpop_6_8_ballad", cn(u8"6/8 华语抒情"), "C-Pop 6/8 Ballad",
                                     ArpeggioDirection::UpDown, ArpeggioDensity::Eighth, 3.0,   // 6/8 每小节 3 拍子（每拍八分）
                                     { step(0.0, 0.55, 0, -1, 5, true),
                                       step(1.0, 0.45, 2, 0, -1),
                                       step(2.0, 0.55, 3, 1, 4) });
    cpop6_8Ballad.styleTags.addArray({ "pop", "c-pop", "ballad", "6/8", "waltz" });
    cpop6_8Ballad.moodTags.addArray({ "flowing", "warm", "romantic" });
    cpop6_8Ballad.regionTags.addArray({ "chinese" });
    cpop6_8Ballad.functionFit.addArray({ "T", "S" });
    cpop6_8Ballad.formFit.addArray({ "verse", "chorus" });
    cpop6_8Ballad.familyId = "CPopBalladPianoFamily";
    cpop6_8Ballad.grooveAffinity = "Straight";
    cpop6_8Ballad.bassAnchored = true;
    cpop6_8Ballad.complexity = 2;
    cpop6_8Ballad.energy = 2;
    patterns.push_back(cpop6_8Ballad);

    auto guofeng3_4Dance = makePattern("guofeng_3_4_dance", cn(u8"3/4 古风舞曲"), "Guofeng 3/4 Dance",
                                       ArpeggioDirection::Alternating, ArpeggioDensity::Quarter, 3.0,
                                       { step(0.0, 0.45, 0, -1, 6, true),
                                         step(1.0, 0.4, 2, 0, -2),
                                         step(2.0, 0.45, 4, 0, 3) });
    guofeng3_4Dance.styleTags.addArray({ "chinese", "ancient_style", "guofeng", "waltz", "3/4" });
    guofeng3_4Dance.moodTags.addArray({ "bright", "playful", "hopeful" });
    guofeng3_4Dance.regionTags.addArray({ "chinese" });
    guofeng3_4Dance.functionFit.addArray({ "S", "D" });
    guofeng3_4Dance.formFit.addArray({ "chorus", "bridge" });
    guofeng3_4Dance.familyId = "GuofengFlowFamily";
    guofeng3_4Dance.grooveAffinity = "Straight";
    guofeng3_4Dance.bassAnchored = true;
    guofeng3_4Dance.complexity = 2;
    guofeng3_4Dance.energy = 3;
    patterns.push_back(guofeng3_4Dance);

    auto folk12_8Sway = makePattern("folk_12_8_sway", cn(u8"12/8 民谣摇摆"), "Folk 12/8 Sway",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,   // 12/8 每小节 4 个三连音组
                                    { step(0.0, 0.45, 0, -1, 5, true),
                                      step(0.5, 0.4, 2, 0, -1),
                                      step(1.0, 0.45, 1, 0, 2),
                                      step(1.5, 0.4, 2, 0, -2),
                                      step(2.0, 0.45, 0, -1, 4, true),
                                      step(2.5, 0.4, 3, 1, 3),
                                      step(3.0, 0.45, 1, 0, 1),
                                      step(3.5, 0.4, 2, 0, -1) });
    folk12_8Sway.styleTags.addArray({ "folk", "c-pop", "world", "12/8" });
    folk12_8Sway.moodTags.addArray({ "swaying", "warm", "hopeful" });
    folk12_8Sway.regionTags.addArray({ "chinese" });
    folk12_8Sway.functionFit.addArray({ "T", "S" });
    folk12_8Sway.formFit.addArray({ "verse", "chorus" });
    folk12_8Sway.familyId = "GuitarPickingFamily";
    folk12_8Sway.grooveAffinity = "Shuffle";
    folk12_8Sway.bassAnchored = true;
    folk12_8Sway.complexity = 3;
    folk12_8Sway.energy = 3;
    patterns.push_back(folk12_8Sway);


    // ========================================================================
    // 中国传统与民族风格 (Chinese Traditional & Ethnic)
    // ========================================================================

    // 西北民歌风情 (甘肃青海线，宽广高亢，空灵五声)
    auto northwestFolk = makePattern("northwest_folk_gansu", cn(u8"西北陇上放歌"), "Northwest Folk Flow",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0,
                                     { step(0.0, 1.8, 0, -1, 4, true),   // 宽广的低音根音锚点
                                       step(2.0, 0.9, 2, 0, -1),         // 三音（五声音阶中的间音或对应音）
                                       step(3.0, 1.0, 4, 1, 3) });       // 八度跳跃高音
    northwestFolk.styleTags.addArray({ "folk", "world", "traditional", cn(u8"角调") }); // 命中“角调”
    northwestFolk.moodTags.addArray({ "epic", "calm", "hopeful", cn(u8"空灵"), cn(u8"治愈") }); // 命中“空灵”、“治愈”
    northwestFolk.regionTags.addArray({ "chinese", cn(u8"甘肃"), cn(u8"青海") }); // 精准命中“甘肃”、“青海”
    northwestFolk.functionFit.addArray({ "T", "D", "t" });
    northwestFolk.formFit.addArray({ "intro", "verse" });
    northwestFolk.familyId = "ChineseOpenFamily";
    northwestFolk.grooveAffinity = "Straight";
    northwestFolk.bassAnchored = true;
    northwestFolk.complexity = 2;
    northwestFolk.energy = 2;
    patterns.push_back(northwestFolk);

    // 草原马头琴律动 (蒙古族风情，五声宫调，连绵起伏)
    auto mongolianPrairie = makePattern("mongolian_prairie", cn(u8"草原马头琴"), "Mongolian Prairie Wave",
                                       ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0,
                                       { step(0.0, 0.95, 0, -1, 5, true),
                                         step(1.0, 0.45, 1, 0, -2),
                                         step(1.5, 0.45, 2, 0, 1),
                                         step(2.0, 0.95, 0, 0, 4, true),
                                         step(3.0, 0.45, 3, 1, 2),
                                         step(3.5, 0.45, 2, 0, -1) });
    mongolianPrairie.styleTags.addArray({ "world", "folk", cn(u8"蒙古"), cn(u8"蒙古族"), cn(u8"马头琴"), cn(u8"五声宫调") }); // 完美补全“五声宫调”标签
    mongolianPrairie.moodTags.addArray({ "warm", "hopeful", cn(u8"庄严") });
    mongolianPrairie.regionTags.addArray({ "chinese", cn(u8"马奶") }); // 顺带激活极为有趣的垂直地缘标签“马奶”
    mongolianPrairie.functionFit.addArray({ "T", "S" });
    mongolianPrairie.formFit.addArray({ "verse", "chorus" });
    mongolianPrairie.familyId = "GuofengFlowFamily";
    mongolianPrairie.grooveAffinity = "Straight";
    mongolianPrairie.bassAnchored = true;
    mongolianPrairie.complexity = 3;
    mongolianPrairie.energy = 3;
    patterns.push_back(mongolianPrairie);

    // 西南火把节律动 (少数民族欢快切分，七和弦/挂留驱动)
    auto torchFestival = makePattern("torch_festival_groove", cn(u8"火把节之夜"), "Torch Festival Groove",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.45, 0, -1, 8, true),
                                       step(0.5, 0.45, 2, 0, 2),         // 快速切分
                                       step(1.0, 0.95, 1, 0, 6),
                                       step(1.5, 0.45, 3, 1, 4) });
    torchFestival.styleTags.addArray({ "folk", "world", "dance", cn(u8"七和弦"), cn(u8"挂留") }); // 命中“七和弦”、“挂留”
    torchFestival.moodTags.addArray({ "happy", "energetic", cn(u8"快乐"), cn(u8"明亮") }); // 命中“快乐”、“明亮”
    torchFestival.regionTags.addArray({ "chinese", cn(u8"火把节") }); // 精准命中节日标签“火把节”
    torchFestival.functionFit.addArray({ "T", "S", "D" });
    torchFestival.formFit.addArray({ "chorus", "bridge" });
    torchFestival.familyId = "ChineseOperaOrnamentFamily";
    torchFestival.grooveAffinity = "Early"; // 带来向前冲的切分律动感
    torchFestival.bassAnchored = true;
    torchFestival.complexity = 3;
    torchFestival.energy = 4;
    patterns.push_back(torchFestival);

    // 戏曲风韵古风伴奏 (戏曲/黄梅戏/藏戏元素，华彩分解)
    auto operaSpringFlow = makePattern("chinese_opera_flow", cn(u8"梨园春华"), "Chinese Opera Flow",
                                     ArpeggioDirection::Down, ArpeggioDensity::Mixed, 4.0,
                                     { step(0.0, 1.4, 0, -1, 6, true),
                                       step(1.5, 0.25, 4, 1, -3),        // 快速装饰音
                                       step(1.75, 0.25, 3, 1, -1),
                                       step(2.0, 0.9, 2, 0, 4),
                                       step(3.0, 0.9, 1, 0, 2) });
    operaSpringFlow.styleTags.addArray({ "traditional", "classical", cn(u8"古风"), cn(u8"黄梅戏"), cn(u8"藏戏") }); // 命中“古风”、“黄梅戏”、“藏戏”
    operaSpringFlow.moodTags.addArray({ "nostalgic", cn(u8"伤感"), cn(u8"温暖") }); // 命中“伤感”、“温暖”
    operaSpringFlow.regionTags.addArray({ "chinese" });
    operaSpringFlow.functionFit.addArray({ "t", "s", "T" });
    operaSpringFlow.formFit.addArray({ "intro", "bridge" });
    operaSpringFlow.familyId = "ChineseOperaOrnamentFamily";
    operaSpringFlow.grooveAffinity = "Straight";
    operaSpringFlow.bassAnchored = true;
    operaSpringFlow.complexity = 4;
    operaSpringFlow.energy = 2;
    patterns.push_back(operaSpringFlow);

    // ========================================================================
    // 现代流行与融合进化 (Modern Pop & Fusion)
    // ========================================================================

    // 闽南语经典流行抒情 (悲情和弦色彩，自然小调/和声小调交替)
    auto hokkienBallad = makePattern("hokkien_pop_sad", cn(u8"闽南雨夜伤情"), "Hokkien Pop Sad Ballad",
                                     ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0,
                                     { step(0.0, 0.9, 0, -1, 7, true),
                                       step(1.0, 0.45, 1, 0, -2),
                                       step(1.5, 0.45, 2, 0, 1),
                                       step(2.0, 0.9, 3, 0, 5),
                                       step(3.0, 0.45, 2, 0, -1),
                                       step(3.5, 0.45, 1, 0, 2) });
    hokkienBallad.styleTags.addArray({ "pop", "ballad", cn(u8"闽南语"), cn(u8"自然小调"), cn(u8"和声小调") }); // 命中“闽南语”、“自然小调”、“和声小调”
    hokkienBallad.moodTags.addArray({ "sad", "nostalgic", cn(u8"伤感"), cn(u8"独立") }); // 命中“伤感”、“独立”
    hokkienBallad.regionTags.addArray({ "chinese", cn(u8"台湾") }); // 命中“台湾”
    hokkienBallad.functionFit.addArray({ "t", "s", "D" });
    hokkienBallad.formFit.addArray({ "verse", "chorus" });
    hokkienBallad.familyId = "CPopBalladPianoFamily";
    hokkienBallad.grooveAffinity = "Straight";
    hokkienBallad.bassAnchored = true;
    hokkienBallad.complexity = 3;
    hokkienBallad.energy = 2;
    patterns.push_back(hokkienBallad);


    // ========================================================================
    // 新增补充：东方传统调式与非遗节庆特色库 (Ethnic & Traditional Specialty)
    // ========================================================================

    // 1. 鄂伦春林海萨满律动 (针对冷门民族标签：鄂伦春、阿尔泰、长鼓、空灵)
    auto oroqenForest = makePattern("oroqen_shaman_pulse_010", cn(u8"鄂伦春林海脉冲"), "Oroqen Shaman Pulse",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                    { step(0.0, 0.45, 0, -1, 4, true),   // 沉稳的根音作为基底
                                      step(0.5, 0.4, 2, 0, -2), 
                                      step(1.0, 0.45, 1, 0, 1),
                                      step(1.5, 0.4, 4, 1, 5),          // 高音长鼓点模拟
                                      step(2.0, 0.45, 0, -1, 2, true),
                                      step(2.5, 0.4, 2, 0, -1),
                                      step(3.0, 0.45, 3, 0, 3),
                                      step(3.5, 0.4, 1, 0, -3) });
    oroqenForest.styleTags.addArray({ "world", "folk", cn(u8"鄂伦春"), cn(u8"阿尔泰"), cn(u8"长鼓") }); // 命中垂直民族标签
    oroqenForest.moodTags.addArray({ "mysterious", cn(u8"空灵"), cn(u8"庄严") }); 
    oroqenForest.regionTags.addArray({ "chinese" });
    oroqenForest.functionFit.addArray({ "t", "s", "T" });
    oroqenForest.formFit.addArray({ "intro", "verse" });
    oroqenForest.familyId = "ChineseOpenFamily"; // 契合智能匹配器中的家族倾向得分
    oroqenForest.grooveAffinity = "Straight";
    oroqenForest.bassAnchored = true;
    oroqenForest.complexity = 3;
    oroqenForest.energy = 3;
    patterns.push_back(oroqenForest);

    // 2. 壮族蚂拐节庆典 (针对特定民俗标签：蚂拐节、踩歌堂、快乐、明亮)
    auto maguaFestival = makePattern("magua_festival_dance_011", cn(u8"蚂拐鸣春踩歌堂"), "Magua Festival Dance",
                                     ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 2.0, // 16分高密度展现欢快感
                                     { step(0.0, 0.2, 0, -1, 6, true),
                                       step(0.25, 0.2, 1, 0, 2),
                                       step(0.5, 0.2, 2, 0, 0),
                                       step(0.75, 0.2, 3, 0, 4),
                                       step(1.0, 0.2, 4, 1, 6),
                                       step(1.25, 0.2, 2, 0, 1),
                                       step(1.5, 0.2, 3, 0, 3),
                                       step(1.75, 0.2, 1, 0, -2) });
    maguaFestival.styleTags.addArray({ "folk", "dance", cn(u8"蚂拐节"), cn(u8"踩歌堂") }); // 精准命中节庆
    maguaFestival.moodTags.addArray({ "happy", cn(u8"快乐"), cn(u8"明亮") });
    maguaFestival.regionTags.addArray({ "chinese" });
    maguaFestival.functionFit.addArray({ "T", "S", "D" });
    maguaFestival.formFit.addArray({ "chorus", "bridge" });
    maguaFestival.familyId = "PipaSweepFamily"; // 用琵琶扫弦家族逻辑去拟合高频切分
    maguaFestival.grooveAffinity = "Early";
    maguaFestival.bassAnchored = true;
    maguaFestival.complexity = 4;
    maguaFestival.energy = 5; // 高能量
    patterns.push_back(maguaFestival);

    // 3. 古琴松风寒潭 (薄弱项：传统器乐质感；标签：古琴、清乐、伤感、独立)
    auto guqinSolitude = makePattern("guqin_solitude_flow_012", cn(u8"古琴松风寒潭"), "Guqin Solitude Flow",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 极简稀疏的织体
                                   { step(0.0, 1.9, 0, -2, 8, true),   // 极深沉的低音散音
                                     step(2.0, 1.5, 2, -1, -2),        // 按音
                                     step(3.5, 0.45, 4, 0, 4) });      // 泛音清亮收尾
    guqinSolitude.styleTags.addArray({ "traditional", "classical", cn(u8"古琴"), cn(u8"清乐") }); // 命中乐器与调式类标签
    guqinSolitude.moodTags.addArray({ "calm", cn(u8"伤感"), cn(u8"独立") });
    guqinSolitude.regionTags.addArray({ "chinese" });
    guqinSolitude.functionFit.addArray({ "t", "s" });
    guqinSolitude.formFit.addArray({ "intro", "outro" });
    guqinSolitude.familyId = "GuqinSparseFamily"; // 完美命中系统的 Guqin 特色得分项
    guqinSolitude.grooveAffinity = "Late"; // 带有中国传统音乐中自由散板、渐慢滞后的呼吸感
    guqinSolitude.bassAnchored = true;
    guqinSolitude.complexity = 1; // 极简复杂度
    guqinSolitude.energy = 1;
    patterns.push_back(guqinSolitude);

    // 4. 西南阿昌族织锦律动 (针对特定民俗标签：阿昌族、蜡染、马尾绣、温柔)
    auto achangWeave = makePattern("achang_weaving_texture_013", cn(u8"阿昌女儿织锦"), "Achang Weaving Rhythm",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                   { step(0.0, 0.45, 0, -1, 4, true),
                                     step(0.5, 0.45, 2, 0, -2),
                                     step(1.0, 0.45, 1, 0, 2),
                                     step(1.5, 0.45, 3, 0, -1) }); // 循环往复，模拟织布梭子
    achangWeave.styleTags.addArray({ "world", "folk", cn(u8"阿昌族"), cn(u8"蜡染"), cn(u8"马尾绣") }); // 命中特定手工艺、民族标签
    achangWeave.moodTags.addArray({ "warm", cn(u8"温暖"), cn(u8"治愈") });
    achangWeave.regionTags.addArray({ "chinese" });
    achangWeave.functionFit.addArray({ "T", "S" });
    achangWeave.formFit.addArray({ "verse" });
    achangWeave.familyId = "GuofengFlowFamily";
    achangWeave.grooveAffinity = "Straight";
    achangWeave.bassAnchored = true;
    achangWeave.complexity = 2;
    achangWeave.energy = 2;
    patterns.push_back(achangWeave);

    // 5. 羌族释比祭祀之舞 (针对神秘学/地域标签：释比、阿涅节、紧张、蓄力)
    auto shibiRitual = makePattern("shibi_ritual_dance_014", cn(u8"释比断续神舞"), "Shibi Ritual Dance",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 带有不平稳的切分突强
                                   { step(0.0, 0.7, 0, -1, 10, true),   // 重音砸下
                                     step(1.0, 0.2, 1, 0, -4),
                                     step(1.25, 0.2, 2, 0, -2),
                                     step(2.0, 0.7, 3, 0, 8),           // 突强
                                     step(3.0, 0.45, 4, 1, 6),
                                     step(3.5, 0.45, 2, 0, 0) });
    shibiRitual.styleTags.addArray({ "world", "folk", cn(u8"释比"), cn(u8"阿涅节") }); // 命中高度稀缺的羌族特定标签
    shibiRitual.moodTags.addArray({ "epic", cn(u8"紧张"), cn(u8"蓄力") }); // 用于副歌前的极好铺垫
    shibiRitual.regionTags.addArray({ "chinese" });
    shibiRitual.functionFit.addArray({ "t", "D" });
    shibiRitual.formFit.addArray({ "intro", "bridge" });
    shibiRitual.familyId = "ChineseOperaOrnamentFamily";
    shibiRitual.grooveAffinity = "Early";
    shibiRitual.bassAnchored = true;
    shibiRitual.complexity = 4;
    shibiRitual.energy = 4;
    patterns.push_back(shibiRitual);

    // 6. 现代国潮电音闪客 (薄弱项：现代融合；标签：闪客/快闪、EDM/电子舞曲、动作、七和弦)
    auto cyberFlashmob = makePattern("cyber_flashmob_edm_015", cn(u8"国潮街头快闪"), "Cyber Flashmob Pop",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0,
                                     { step(0.0, 0.2, 0, -1, 8, true),
                                       step(0.25, 0.2, 3, 0, 4),        // 命中七音/扩展音
                                       step(0.5, 0.2, 1, 0, 2),
                                       step(0.75, 0.2, 4, 1, 6),
                                       step(1.0, 0.2, 2, 0, 3),
                                       step(1.5, 0.4, 3, 0, 8) });      // 切分长音
    cyberFlashmob.styleTags.addArray({ "pop", "edm", cn(u8"EDM/电子舞曲"), cn(u8"闪客/快闪"), cn(u8"七和弦") }); // 融合国潮与EDM快闪
    cyberFlashmob.moodTags.addArray({ "energetic", cn(u8"动作"), cn(u8"明亮") });
    cyberFlashmob.regionTags.addArray({ "chinese" });
    cyberFlashmob.functionFit.addArray({ "T", "S", "D" });
    cyberFlashmob.formFit.addArray({ "chorus" });
    cyberFlashmob.familyId = "CPopChorusDriveFamily"; // 匹配高能量驱动推进家族
    cyberFlashmob.grooveAffinity = "Straight";
    cyberFlashmob.bassAnchored = true;
    cyberFlashmob.complexity = 3;
    cyberFlashmob.energy = 5;
    patterns.push_back(cyberFlashmob);

    // ========================================================================
    // 新增补充：教会调式、器乐特效与未覆盖少数民族库 (Advanced Modes & Specialties)
    // ========================================================================

    // 1. 古筝流光摇指 (针对薄弱项：古筝刮奏特效；标签：古筝、空灵、温婉、柔美)
    // 匹配匹配器家族：GuzhengRollFamily (获得极高特定家族加分)
    auto guzhengCascadeRoll = makePattern("guzheng_roll_cascade_020", cn(u8"古筝流光摇指"), "Guzheng Cascade Roll",
                                   ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0,
                                   { step(0.0, 0.2, 0, -1, 4, true),
                                     step(1.0, 0.2, 1, 0, -2),
                                     step(2.0, 0.15, 2, 0, 1),   // 2.0拍开始进入绵密的流水刮奏模拟
                                     step(2.25, 0.15, 3, 0, 3),
                                     step(2.5, 0.15, 4, 1, 5),
                                     step(2.75, 0.15, 2, 0, 2),
                                     step(3.0, 0.2, 0, 0, 4),
                                     step(3.5, 0.45, 1, 1, 6) });
    guzhengCascadeRoll.styleTags.addArray({ "traditional", "classical", cn(u8"古筝") });
    guzhengCascadeRoll.moodTags.addArray({ cn(u8"空灵"), cn(u8"温婉"), cn(u8"柔美") }); // 完美补全温婉、柔美空白
    guzhengCascadeRoll.regionTags.addArray({ "chinese" });
    guzhengCascadeRoll.functionFit.addArray({ "T", "S" });
    guzhengCascadeRoll.formFit.addArray({ "intro", "bridge", "outro" });
    guzhengCascadeRoll.familyId = "GuzhengRollFamily"; // 命中特定加分家族
    guzhengCascadeRoll.grooveAffinity = "Straight";
    guzhengCascadeRoll.bassAnchored = true;
    guzhengCascadeRoll.complexity = 4;
    guzhengCascadeRoll.energy = 3;
    patterns.push_back(guzhengCascadeRoll);

    // 2. 华语流行导歌悬念 (针对薄弱项：预副歌过渡织体；标签：C-Pop/华语流行、挂留、蓄力)
    // 匹配匹配器家族：MandopopPrechorusFamily
    auto mpopPrechorus = makePattern("mandopop_prechorus_sus_021", cn(u8"华语流行导歌悬念"), "Mandopop Pre-Chorus Sus",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0,
                                     { step(0.0, 0.95, 0, -1, 6, true), // 低音根音拉长持住
                                       step(1.0, 0.45, 3, 0, 2),        // 强调挂留四音(音程3)，制造向上拉扯感
                                       step(1.5, 0.45, 2, 0, 4) });     // 解决到三音，暗示进入副歌
    mpopPrechorus.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行"), cn(u8"挂留") });
    mpopPrechorus.moodTags.addArray({ "warm", cn(u8"蓄力") });
    mpopPrechorus.regionTags.addArray({ "chinese" });
    mpopPrechorus.functionFit.addArray({ "S", "D" });                   // 极其适合下属和属和弦导向
    mpopPrechorus.formFit.addArray({ "pre-chorus", "bridge" });         // 严丝合缝对齐段落
    mpopPrechorus.familyId = "MandopopPrechorusFamily";                 // 完美匹配特定加分项
    mpopPrechorus.grooveAffinity = "Straight";
    mpopPrechorus.bassAnchored = true;
    mpopPrechorus.complexity = 2;
    mpopPrechorus.energy = 3;
    patterns.push_back(mpopPrechorus);

    // 3. 藏族康巴锅庄弦子 (针对盲区标签：锅庄、大调、明亮、快乐)
    auto guozhuangDance = makePattern("tibetan_guozhuang_dance_022", cn(u8"康巴锅庄弦子"), "Tibetan Guozhuang Dance",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 典型双拍子附点切分律动
                                      { step(0.0, 0.6, 0, -1, 8, true), 
                                        step(0.75, 0.35, 2, 0, 2),      // 后半拍附点起跳
                                        step(1.0, 0.9, 1, 0, 4),
                                        step(2.0, 0.6, 0, 0, 6, true),
                                        step(2.75, 0.35, 4, 1, 5),
                                        step(3.0, 0.9, 2, 0, 3) });
    guozhuangDance.styleTags.addArray({ "world", "folk", "dance", cn(u8"锅庄"), cn(u8"大调") }); // 命中锅庄
    guozhuangDance.moodTags.addArray({ "happy", cn(u8"快乐"), cn(u8"明亮") });
    guozhuangDance.regionTags.addArray({ "chinese" });
    guozhuangDance.functionFit.addArray({ "T", "S" });
    guozhuangDance.formFit.addArray({ "verse", "chorus" });
    guozhuangDance.familyId = "ChineseOpenFamily";
    guozhuangDance.grooveAffinity = "Early"; // 略带附点前冲感
    guozhuangDance.bassAnchored = true;
    guozhuangDance.complexity = 3;
    guozhuangDance.energy = 4;
    patterns.push_back(guozhuangDance);

    // 4. 利底亚赛博极光 (针对盲区标签：Lydian/利底亚、梦幻、空灵)
    auto lydianAurora = makePattern("lydian_cyber_aurora_023", cn(u8"利底亚赛博极光"), "Lydian Cyber Aurora",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0,
                                    { step(0.0, 1.9, 0, -1, 5, true),
                                      step(1.5, 0.45, 3, 0, 4),         // 特意高亮和弦外的第4级音(#4 Lydian特征音)
                                      step(2.0, 0.95, 1, 0, 2),
                                      step(3.0, 0.95, 4, 1, 6) });
    lydianAurora.styleTags.addArray({ "edm", "ambient", cn(u8"Lydian/利底亚"), cn(u8"Lydian") }); // 命中利底亚标签
    lydianAurora.moodTags.addArray({ "mysterious", cn(u8"梦幻"), cn(u8"空灵") });
    lydianAurora.regionTags.addArray({ "global" });
    lydianAurora.functionFit.addArray({ "T", "S" });
    lydianAurora.formFit.addArray({ "intro", "outro", "verse" });
    lydianAurora.familyId = "ElectronicArpFamily";
    lydianAurora.grooveAffinity = "Straight";
    lydianAurora.bassAnchored = true;
    lydianAurora.complexity = 3;
    lydianAurora.energy = 2;
    patterns.push_back(lydianAurora);

    // 5. 高山族逐浪飞歌 (针对盲区标签：高山族、鲜花节、快乐、明亮)
    auto gaoshanOcean = makePattern("gaoshan_ocean_song_024", cn(u8"高山逐浪飞歌"), "Gaoshan Ocean Song",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0, // 欢快密集的复合波浪
                                    { step(0.0, 0.45, 0, -1, 7, true),
                                      step(0.5, 0.45, 2, 0, 1),
                                      step(1.0, 0.45, 4, 0, 5),
                                      step(1.5, 0.45, 1, 1, 3) });
    gaoshanOcean.styleTags.addArray({ "world", "folk", cn(u8"高山族"), cn(u8"鲜花节") }); // 精准补全高山族、鲜花节标签
    gaoshanOcean.moodTags.addArray({ "happy", cn(u8"快乐"), cn(u8"明亮"), cn(u8"节日") }); // 命中节日标签
    gaoshanOcean.regionTags.addArray({ "chinese", cn(u8"台湾") });
    gaoshanOcean.functionFit.addArray({ "T", "S", "D" });
    gaoshanOcean.formFit.addArray({ "chorus" });
    gaoshanOcean.familyId = "CampusFolkFamily";
    gaoshanOcean.grooveAffinity = "Straight";
    gaoshanOcean.bassAnchored = true;
    gaoshanOcean.complexity = 2;
    gaoshanOcean.energy = 4;
    patterns.push_back(gaoshanOcean);

    // 6. 裕固族祁连牧歌 (针对盲区标签：裕固族、旋律小调、伤感)
    auto yugurPastoral = makePattern("yugur_pastoral_ode_025", cn(u8"裕固祁连牧歌"), "Yugur Pastoral Ode",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 悠长辽阔、起伏大
                                     { step(0.0, 1.4, 0, -2, 6, true),  // 极低音宽广锚点
                                       step(1.5, 0.45, 1, -1, -2),
                                       step(2.0, 0.95, 3, 0, 3),        // 旋律小调上行音阶骨架音
                                       step(3.0, 0.95, 2, 0, 1) });
    yugurPastoral.styleTags.addArray({ "world", "folk", cn(u8"裕固族"), cn(u8"旋律小调") }); // 命中裕固族与特殊小调
    yugurPastoral.moodTags.addArray({ "epic", "calm", cn(u8"伤感") });
    yugurPastoral.regionTags.addArray({ "chinese" });
    yugurPastoral.functionFit.addArray({ "t", "s" });
    yugurPastoral.formFit.addArray({ "intro", "verse" });
    yugurPastoral.familyId = "ChineseOpenFamily";
    yugurPastoral.grooveAffinity = "Late"; // 带有高原牧歌自由舒缓的滞后呼吸感
    yugurPastoral.bassAnchored = true;
    yugurPastoral.complexity = 3;
    yugurPastoral.energy = 2;
    patterns.push_back(yugurPastoral);


    // ========================================================================
    // 新增深度补充：攻克剩余盲区标签与未覆盖家族 (Deep Tag Coverage Batch)
    // ========================================================================

    // 1. 侗族大歌鼓楼回声 (针对未覆盖盲区：鼓楼、侗族大歌风格；标签：鼓楼、清乐、温暖)
    auto dongDrumTower = makePattern("dong_drum_tower_031", cn(u8"鼓楼多声部大歌"), "Dong Drum Tower Chorus",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, 
                                     { step(0.0, 1.9, 0, -1, 4, true),   // 雄浑的低音底层
                                       step(1.0, 0.9, 2, 0, 0),          // 模拟侗族大歌的人声持续支声
                                       step(2.0, 1.9, 4, 0, 2),          // 形成纯五度/纯四度的平稳交织
                                       step(3.0, 0.9, 7, 1, 4) });       // 极高声部飘逸的支声线条
    dongDrumTower.styleTags.addArray({ "world", "folk", cn(u8"鼓楼"), cn(u8"清乐") }); // 100%命中极其罕见的“鼓楼”标签
    dongDrumTower.moodTags.addArray({ "calm", cn(u8"温暖") });
    dongDrumTower.regionTags.addArray({ "chinese" });
    dongDrumTower.functionFit.addArray({ "T", "S", "t" });
    dongDrumTower.formFit.addArray({ "intro", "verse" });
    dongDrumTower.familyId = "ChineseOpenFamily"; // 契合传统开放式五声音程结构
    dongDrumTower.grooveAffinity = "Straight";
    dongDrumTower.bassAnchored = true;
    dongDrumTower.complexity = 2;
    dongDrumTower.energy = 2;
    patterns.push_back(dongDrumTower);

    // 2. 华语流行宏大主歌 (针对未覆盖家族：MandopopVerseFamily 极其关键的加分项；标签：C-Pop/华语流行、伤感)
    auto mpopEpicVerse = makePattern("mandopop_epic_verse_032", cn(u8"经典华语弦乐主歌"), "Mandopop Grand Verse",
                                     ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 宽广而略带叙事感
                                     { step(0.0, 0.9, 0, -1, 6, true), 
                                       step(1.0, 0.45, 2, 0, -1), 
                                       step(1.5, 0.45, 4, 0, 1),
                                       step(2.0, 0.9, 1, -1, 3, true),  // 转位根音变化
                                       step(3.0, 0.45, 3, 0, 0),
                                       step(3.5, 0.45, 5, 1, 5) });
    mpopEpicVerse.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行") });
    mpopEpicVerse.moodTags.addArray({ "sad", cn(u8"伤感") });
    mpopEpicVerse.regionTags.addArray({ "chinese" });
    mpopEpicVerse.functionFit.addArray({ "T", "t", "S", "s" });
    mpopEpicVerse.formFit.addArray({ "verse" }); // 精准匹配主歌
    mpopEpicVerse.familyId = "MandopopVerseFamily"; // 激活智能系统中的主歌定制家族加分
    mpopEpicVerse.grooveAffinity = "Straight";
    mpopEpicVerse.bassAnchored = true;
    mpopEpicVerse.complexity = 3;
    mpopEpicVerse.energy = 2;
    patterns.push_back(mpopEpicVerse);

    // 3. 多利亚游戏史诗战斗 (针对未覆盖盲区：Dorian教会调式；标签：Dorian/多利亚、紧张、动作、史诗)
    auto dorianBattle = makePattern("dorian_epic_battle_033", cn(u8"多利亚狂战律动"), "Dorian Battle Pulse",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 极高频的16分切分
                                    { step(0.0, 0.2, 0, -1, 10, true), 
                                      step(0.25, 0.2, 1, 0, -2), 
                                      step(0.5, 0.2, 5, 0, 6),         // 核心高亮 Dorian 标志性的 #6 音（音程5）
                                      step(0.75, 0.2, 4, 0, 2),
                                      step(1.0, 0.2, 2, 0, 4),
                                      step(1.25, 0.2, 5, 1, 8),        // 再次高亮特征音
                                      step(1.5, 0.4, 0, 0, 5) });
    dorianBattle.styleTags.addArray({ "epic", "rock", cn(u8"Dorian/多利亚"), cn(u8"Dorian") }); // 完美覆盖多利亚标签
    dorianBattle.moodTags.addArray({ "epic", cn(u8"紧张"), cn(u8"动作") });
    dorianBattle.regionTags.addArray({ "global" });
    dorianBattle.functionFit.addArray({ "t", "D" }); // Dorian 调式经典的 i - IV (大四级) 或 i - v 倾向
    dorianBattle.formFit.addArray({ "chorus", "bridge" });
    dorianBattle.familyId = "ElectronicArpFamily";
    dorianBattle.grooveAffinity = "Early"; // 具有攻击性的前冲感
    dorianBattle.bassAnchored = true;
    dorianBattle.complexity = 4;
    dorianBattle.energy = 5; // 爆发级能量
    patterns.push_back(dorianBattle);

    // 4. 哈尼族阿涅节庆典 (针对极其生僻的民俗盲区：阿涅节；标签：阿涅节、快乐、明亮)
    auto haniFestival = makePattern("hani_ane_festival_034", cn(u8"哈尼阿涅山寨舞"), "Hani Ane Festival Dance",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0, // 三连音或摇摆律动驱动
                                    { step(0.0, 0.45, 0, -1, 7, true),
                                      step(0.5, 0.45, 2, 0, 2),
                                      step(1.0, 0.45, 4, 1, 6),
                                      step(1.5, 0.45, 1, 0, 0) });
    haniFestival.styleTags.addArray({ "world", "folk", "dance", cn(u8"阿涅节") }); // 精准歼灭空白标签
    haniFestival.moodTags.addArray({ "happy", cn(u8"快乐"), cn(u8"明亮") });
    haniFestival.regionTags.addArray({ "chinese" });
    haniFestival.functionFit.addArray({ "T", "D" });
    haniFestival.formFit.addArray({ "chorus" });
    haniFestival.familyId = "PipaSweepFamily";
    haniFestival.grooveAffinity = "Straight";
    haniFestival.bassAnchored = true;
    haniFestival.complexity = 2;
    haniFestival.energy = 4;
    patterns.push_back(haniFestival);

    // 5. 华语流行经典副歌推力 (针对未覆盖家族：CPopChorusDriveFamily 评级项；标签：C-Pop/华语流行、蓄力、温暖)
    auto cpopChorusDriveFast = makePattern("cpop_chorus_drive_035", cn(u8"华语副歌极速推进"), "C-Pop Chorus Drive Pulse",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 密集分解，为副歌情绪提供强推力
                                       { step(0.0, 0.2, 0, -1, 8, true),
                                         step(0.5, 0.2, 2, 0, 1),
                                         step(1.0, 0.2, 1, 0, 3),
                                         step(1.5, 0.2, 4, 1, 5),
                                         step(2.0, 0.2, 0, 0, 4, true),
                                         step(2.5, 0.2, 3, 0, 2),
                                         step(3.0, 0.2, 2, 0, 4),
                                         step(3.5, 0.2, 6, 1, 7) }); // 扫至极高音冲向高潮
    cpopChorusDriveFast.styleTags.addArray({ "pop", "c-pop", cn(u8"C-Pop/华语流行") });
    cpopChorusDriveFast.moodTags.addArray({ "energetic", cn(u8"蓄力"), cn(u8"温暖") });
    cpopChorusDriveFast.regionTags.addArray({ "chinese" });
    cpopChorusDriveFast.functionFit.addArray({ "T", "S", "D" });
    cpopChorusDriveFast.formFit.addArray({ "chorus" }); // 专为副歌而生
    cpopChorusDriveFast.familyId = "CPopChorusDriveFamily"; // 100%匹配智能系统的副歌高能驱动规则
    cpopChorusDriveFast.grooveAffinity = "Straight";
    cpopChorusDriveFast.bassAnchored = true;
    cpopChorusDriveFast.complexity = 4;
    cpopChorusDriveFast.energy = 5;
    patterns.push_back(cpopChorusDriveFast);

    // ========================================================================
    // 新增补充：攻克最后盲区标签与匹配器核心家族 (Final Corner & Core Families)
    // ========================================================================

    // 1. 周杰伦式标志性切分 (针对未覆盖家族：JayStyleSyncopationFamily；标签：C-Pop/华语流行、挂留)
    auto jaySyncopation = makePattern("jay_style_syncopation_040", cn(u8"十一月的切分音"), "Jay Style Syncopation",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 经典 3-3-2 或双重切分钢琴纹理
                                      { step(0.0, 0.35, 0, -1, 6, true), 
                                        step(0.375, 0.35, 2, 0, 1),       // 16分音符级别的跨小节切分起跳
                                        step(0.75, 0.7, 1, 0, 3), 
                                        step(1.5, 0.35, 4, 1, 5),
                                        step(2.0, 0.35, 0, 0, 4, true),
                                        step(2.375, 0.35, 3, 0, 2),       // 挂留音/扩展音切分
                                        step(2.75, 0.7, 2, 0, 4),
                                        step(3.5, 0.45, 1, 0, -1) });
    jaySyncopation.styleTags.addArray({ "pop", "c-pop", "piano", cn(u8"C-Pop/华语流行"), cn(u8"挂留") });
    jaySyncopation.moodTags.addArray({ "nostalgic", "hopeful", cn(u8"明亮") });
    jaySyncopation.regionTags.addArray({ "chinese", cn(u8"台湾") });
    jaySyncopation.functionFit.addArray({ "T", "S", "D" });
    jaySyncopation.formFit.addArray({ "verse", "chorus" });
    jaySyncopation.familyId = "JayStyleSyncopationFamily"; // 完美激活周氏切分专属算法加分
    jaySyncopation.grooveAffinity = "Late"; // 带有R&B微小的慵懒滞后感
    jaySyncopation.bassAnchored = true;
    jaySyncopation.complexity = 4;
    jaySyncopation.energy = 4;
    patterns.push_back(jaySyncopation);

    // 2. 华语流行经典副歌大开大合 (针对未覆盖家族：MandopopChorusFamily；标签：C-Pop/华语流行、温暖、七和弦)
    auto mpopChorusBallad = makePattern("mandopop_chorus_ballad_041", cn(u8"华语巅峰情歌副歌"), "Mandopop Master Chorus",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 铺垫感极强的满幅八分分解
                                        { step(0.0, 0.95, 0, -1, 8, true), 
                                          step(1.0, 0.45, 2, 0, 2), 
                                          step(1.5, 0.45, 1, 0, 4),
                                          step(2.0, 0.95, 3, 0, 6),       // 突显七弦音色
                                          step(3.0, 0.45, 4, 1, 5),
                                          step(3.5, 0.45, 2, 0, 1) });
    mpopChorusBallad.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行"), cn(u8"七和弦") });
    mpopChorusBallad.moodTags.addArray({ "romantic", cn(u8"温暖"), cn(u8"伤感") });
    mpopChorusBallad.regionTags.addArray({ "chinese" });
    mpopChorusBallad.functionFit.addArray({ "T", "S", "D", "t" });
    mpopChorusBallad.formFit.addArray({ "chorus" }); // 专供副歌
    mpopChorusBallad.familyId = "MandopopChorusFamily"; // 激活核心的副歌评估规则
    mpopChorusBallad.grooveAffinity = "Straight";
    mpopChorusBallad.bassAnchored = true;
    mpopChorusBallad.complexity = 3;
    mpopChorusBallad.energy = 4;
    patterns.push_back(mpopChorusBallad);

    // 3. 华语黄金离调桥段 (针对未覆盖家族：MandopopBridgeFamily；标签：C-Pop/华语流行、蓄力、九和弦)
    auto mpopBridgeSus = makePattern("mandopop_bridge_sus_042", cn(u8"黄金离调过渡桥段"), "Mandopop Bridge Transition",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 带有强烈的色彩转换感
                                     { step(0.0, 1.4, 0, -1, 6, true), 
                                       step(1.5, 0.45, 4, 0, 3),         // 九音（音程4）
                                       step(2.0, 1.4, 1, 0, 5), 
                                       step(3.5, 0.45, 3, 1, 7) });      // 向上攀升，引导回副歌
    mpopBridgeSus.styleTags.addArray({ "pop", "c-pop", cn(u8"C-Pop/华语流行"), cn(u8"九和弦") });
    mpopBridgeSus.moodTags.addArray({ cn(u8"蓄力"), cn(u8"柔美") });
    mpopBridgeSus.regionTags.addArray({ "chinese" });
    mpopBridgeSus.functionFit.addArray({ "SecD", "D", "s" });            // 专为副歌前的 Bridge 离调/副属和弦设计
    mpopBridgeSus.formFit.addArray({ "bridge" }); // 专供桥段
    mpopBridgeSus.familyId = "MandopopBridgeFamily"; // 完美闭环闲置规则
    mpopBridgeSus.grooveAffinity = "Straight";
    mpopBridgeSus.bassAnchored = true;
    mpopBridgeSus.complexity = 3;
    mpopBridgeSus.energy = 3;
    patterns.push_back(mpopBridgeSus);

    // 4. 黄梅情缘华彩分解 (针对未覆盖盲区标签：黄梅戏、温婉、柔美、清乐)
    auto huangmeiOpera = makePattern("huangmei_opera_flow_043", cn(u8"黄梅烟雨情缘"), "Huangmei Opera Romance",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 模拟江淮戏曲板眼的断续华彩
                                     { step(0.0, 0.9, 0, -1, 4, true), 
                                       step(1.0, 0.45, 2, 0, -2), 
                                       step(1.5, 0.45, 4, 0, 2), 
                                       step(2.0, 1.4, 1, 0, 5),          // 戏曲大跳
                                       step(3.5, 0.45, 3, 1, 6) });
    huangmeiOpera.styleTags.addArray({ "traditional", "classical", cn(u8"黄梅戏"), cn(u8"清乐") }); // 灭活生僻戏曲标签
    huangmeiOpera.moodTags.addArray({ cn(u8"温婉"), cn(u8"柔美"), cn(u8"治愈") });
    huangmeiOpera.regionTags.addArray({ "chinese" });
    huangmeiOpera.functionFit.addArray({ "T", "S" });
    huangmeiOpera.formFit.addArray({ "intro", "verse", "bridge" });
    huangmeiOpera.familyId = "ChineseOperaOrnamentFamily";
    huangmeiOpera.grooveAffinity = "Straight";
    huangmeiOpera.bassAnchored = true;
    huangmeiOpera.complexity = 3;
    huangmeiOpera.energy = 2;
    patterns.push_back(huangmeiOpera);

    // 5. 弗里吉亚暗黑哥特 (针对绝对盲区标签：Phrygian/弗里吉亚、紧张、动作)
    auto phrygianGothic = makePattern("phrygian_gothic_dark_044", cn(u8"弗里吉亚暗黑编织"), "Phrygian Gothic Darkness",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 极其压抑、诡异的半音程向下压迫感
                                      { step(0.0, 0.2, 0, -1, 10, true), 
                                        step(0.25, 0.2, 1, 0, 8),        // 核心高亮 Phrygian 的标志性小二级降音（音程1）
                                        step(0.5, 0.2, 2, 0, 4), 
                                        step(0.75, 0.2, 1, 0, 6), 
                                        step(1.0, 0.2, 3, 0, 9), 
                                        step(1.25, 0.2, 1, 0, 7), 
                                        step(1.5, 0.4, 0, 0, 12) });     // 强音砸下解决
    phrygianGothic.styleTags.addArray({ "epic", "ambient", cn(u8"Phrygian/弗里吉亚"), cn(u8"Phrygian") }); // 攻克弗里吉亚标签
    phrygianGothic.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    phrygianGothic.regionTags.addArray({ "global" });
    phrygianGothic.functionFit.addArray({ "t", "D" });                   // 极度贴合小二级到主和弦的神秘阻碍进行
    phrygianGothic.formFit.addArray({ "intro", "bridge" });
    phrygianGothic.familyId = "ElectronicArpFamily";
    phrygianGothic.grooveAffinity = "Straight";
    phrygianGothic.bassAnchored = true;
    phrygianGothic.complexity = 4;
    phrygianGothic.energy = 4;
    patterns.push_back(phrygianGothic);

    // 6. 混合利底亚迷幻摇滚 (针对绝对盲区标签：Mixolydian/混合利底亚、独立、梦幻)
    auto mixolydianPsychedelic = makePattern("mixolydian_psychedelic_045", cn(u8"混合利底亚太空漫游"), "Mixolydian Space Odyssey",
                                             ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 带有复古摇滚与现代太空融合的律动
                                             { step(0.0, 0.9, 0, -1, 6, true), 
                                               step(1.0, 0.45, 2, 0, 2), 
                                               step(1.5, 0.45, 6, 0, 5), // 核心高亮 Mixolydian 标志性的降b7级降音（音程6）
                                               step(2.0, 0.9, 4, 1, 4), 
                                               step(3.0, 0.9, 6, 0, 3) });
    mixolydianPsychedelic.styleTags.addArray({ "rock", "pop", cn(u8"Mixolydian/混合利底亚"), cn(u8"Mixolydian") }); // 攻克混合利底亚标签
    mixolydianPsychedelic.moodTags.addArray({ "mysterious", cn(u8"独立"), cn(u8"梦幻") });
    mixolydianPsychedelic.regionTags.addArray({ "global" });
    mixolydianPsychedelic.functionFit.addArray({ "T", "bVII" });          // 极其经典的 Mixolydian 降VII级大和弦特色驱动
    mixolydianPsychedelic.formFit.addArray({ "verse", "chorus" });
    mixolydianPsychedelic.familyId = "ElectronicArpFamily";
    mixolydianPsychedelic.grooveAffinity = "Straight";
    mixolydianPsychedelic.bassAnchored = true;
    mixolydianPsychedelic.complexity = 3;
    mixolydianPsychedelic.energy = 3;
    patterns.push_back(mixolydianPsychedelic);


    // ========================================================================
    // 新增补充：世界军乐、进行曲与仪式国歌专用库 (Military Marches & Hymns)
    // ========================================================================

    // 1. 皇家铜管大进行曲 (针对盲区：古典/欧洲军乐、强有力进行曲节奏)
    auto royalMilitaryMarch = makePattern("royal_military_march_050", cn(u8"皇家仪仗队进行曲"), "Royal Military March",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 4/4拍 典型的正拍与附点重音切换
                                         { step(0.0, 0.45, 0, -1, 12, true),   // 极其坚固的左手正拍根音（军鼓重击）
                                           step(0.5, 0.45, 2, 0, 4), 
                                           step(1.0, 0.35, 1, 0, 6),           // 模拟铜管附点：1.0拍和1.375拍的跳跃
                                           step(1.375, 0.125, 3, 0, 8), 
                                           step(2.0, 0.45, 0, 0, 10, true),    // 第二声重拍
                                           step(2.5, 0.45, 4, 1, 6), 
                                           step(3.0, 0.95, 2, 0, 8) });        // 庄严拉长收尾
    royalMilitaryMarch.styleTags.addArray({ "classical", "traditional", "march", cn(u8"大调") }); // 命中进行曲
    royalMilitaryMarch.moodTags.addArray({ "epic", "energetic", cn(u8"庄严"), cn(u8"明亮"), cn(u8"动作") }); // 绝配军队威严感
    royalMilitaryMarch.regionTags.addArray({ "global" });
    royalMilitaryMarch.functionFit.addArray({ "T", "D", "S" });           // 极其适合正统的 I-IV-V 进行
    royalMilitaryMarch.formFit.addArray({ "intro", "chorus" });
    royalMilitaryMarch.familyId = "ChineseOpenFamily"; // 借助开放式五音/纯五度框架保证声音的铿锵有力
    royalMilitaryMarch.grooveAffinity = "Straight";    // 进行曲必须是绝对严谨的直拍，毫无滞后
    royalMilitaryMarch.bassAnchored = true;
    royalMilitaryMarch.complexity = 3;
    royalMilitaryMarch.energy = 5;                     // 最高级推进能量
    patterns.push_back(royalMilitaryMarch);

    // 2. 凯旋国歌圣咏织体 (针对盲区：国歌类宽广颂歌、复调柱式分解)
    auto triumphalAnthem = makePattern("triumphal_anthem_hymn_051", cn(u8"凯旋赞歌与国歌"), "Triumphal Anthem Hymn",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 宽广的四分音符，模拟合唱团与管风琴
                                       { step(0.0, 0.95, 0, -1, 10, true),  // 八度深沉低音
                                         step(1.0, 0.95, 2, 0, 6),          // 三音铺垫
                                         step(2.0, 0.95, 4, 0, 7),          // 五音交织
                                         step(3.0, 0.95, 7, 1, 9) });       // 极高声部荣耀顶峰
    triumphalAnthem.styleTags.addArray({ "classical", "anthem", "hymn", cn(u8"大调") }); // 攻克国歌/圣咏标签
    triumphalAnthem.moodTags.addArray({ "epic", "calm", cn(u8"庄严"), cn(u8"温暖"), cn(u8"hopeful") });
    triumphalAnthem.regionTags.addArray({ "global" });
    triumphalAnthem.functionFit.addArray({ "T", "S", "D" });
    triumphalAnthem.formFit.addArray({ "chorus", "outro" });
    triumphalAnthem.familyId = "ChineseOpenFamily";
    triumphalAnthem.grooveAffinity = "Straight";
    triumphalAnthem.bassAnchored = true;
    triumphalAnthem.complexity = 2; // 极简但极其庄重
    triumphalAnthem.energy = 4;
    patterns.push_back(triumphalAnthem);

    // 3. 哥萨克骑兵进行曲 (针对盲区：东欧/冷战地缘军队风情、小调悲壮进行曲)
    auto cossackMarch = makePattern("cossack_cavalry_march_052", cn(u8"哥萨克铁骑进行曲"), "Cossack Cavalry March",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 2/4 拍或快速 4/4 拍的急行军律动
                                    { step(0.0, 0.45, 0, -1, 12, true), 
                                      step(0.5, 0.2, 1, 0, 4),           // 极速马蹄声切分
                                      step(0.75, 0.2, 2, 0, 4), 
                                      step(1.0, 0.45, 3, 0, 10),         // 和声小调导音砸下
                                      step(1.5, 0.45, 2, 0, 6), 
                                      step(2.0, 0.45, 0, 0, 10, true), 
                                      step(2.5, 0.45, 4, 1, 8), 
                                      step(3.0, 0.95, 1, 0, 5) });
    cossackMarch.styleTags.addArray({ "world", "folk", "march", cn(u8"和声小调") }); // 完美对应悲壮小调军乐
    cossackMarch.moodTags.addArray({ "epic", cn(u8"紧张"), cn(u8"蓄力"), cn(u8"伤感") }); // 悲壮的战争史诗感
    cossackMarch.regionTags.addArray({ "global" });
    cossackMarch.functionFit.addArray({ "t", "s", "D" });              // 经典的 i - iv - V 铁血进行
    cossackMarch.formFit.addArray({ "verse", "chorus" });
    cossackMarch.familyId = "ChineseOperaOrnamentFamily"; // 借助断续切分和高爆发力适配
    cossackMarch.grooveAffinity = "Early";               // 略微前冲，模拟骑兵冲锋
    cossackMarch.bassAnchored = true;
    cossackMarch.complexity = 4;
    cossackMarch.energy = 5;
    patterns.push_back(cossackMarch);

    // 4. 三连音军鼓前哨律动 (针对盲区：军队出征、电影配乐中的军鼓滚奏织体)
    auto militaryVanguard = makePattern("military_vanguard_triple_053", cn(u8"军前哨三连音突击"), "Military Vanguard Pulse",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 高密度的连续三连音/16分音符滚奏模拟
                                        { step(0.0, 0.2, 0, -1, 10, true), 
                                          step(0.25, 0.1, 1, 0, 4),        // 密集的低力度滚奏
                                          step(0.375, 0.1, 2, 0, 4), 
                                          step(0.5, 0.4, 4, 0, 9),         // 正拍突强
                                          step(1.0, 0.2, 0, 0, 8, true), 
                                          step(1.25, 0.1, 3, 1, 5), 
                                          step(1.375, 0.1, 2, 0, 5), 
                                          step(1.5, 0.4, 1, 0, 9) });
    militaryVanguard.styleTags.addArray({ "epic", "march", cn(u8"挂留") });
    militaryVanguard.moodTags.addArray({ cn(u8"紧张"), cn(u8"动作"), cn(u8"蓄力") }); // 绝配战斗、行军过渡场景
    militaryVanguard.regionTags.addArray({ "global" });
    militaryVanguard.functionFit.addArray({ "t", "D", "S" });
    militaryVanguard.formFit.addArray({ "intro", "pre-chorus", "bridge" }); // 极佳的导歌与前奏铺垫
    militaryVanguard.familyId = "ElectronicArpFamily";   // 借用高频电子琶音链路实现物理滚奏
    militaryVanguard.grooveAffinity = "Straight";
    militaryVanguard.bassAnchored = true;
    militaryVanguard.complexity = 4;
    militaryVanguard.energy = 4;
    patterns.push_back(militaryVanguard);

    // 5. 东方古风沙场点兵 (针对盲区：中国传统军队、秦王破阵、沙场出征)
    auto dynamicGuofengMarch = makePattern("guofeng_military_march_054", cn(u8"大风起兮沙场点兵"), "Guofeng Warrior March",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 铿锵有力的东方金石律动
                                           { step(0.0, 0.9, 0, -1, 12, true),   // 战鼓隆隆（大重音根音）
                                             step(1.0, 0.45, 4, 0, 6),          // 五音跃动
                                             step(1.5, 0.45, 7, 1, 9),          // 八度大跳突强
                                             step(2.0, 0.9, 0, 0, 10, true), 
                                             step(3.0, 0.45, 2, 0, 5), 
                                             step(3.5, 0.45, 4, 0, 7) });
    dynamicGuofengMarch.styleTags.addArray({ "traditional", "world", "folk", cn(u8"角调") }); 
    dynamicGuofengMarch.moodTags.addArray({ "epic", cn(u8"庄严"), cn(u8"空灵"), cn(u8"动作") }); // 古风悲壮与威严融合
    dynamicGuofengMarch.regionTags.addArray({ "chinese", cn(u8"甘肃") }); // 模拟大漠边关地缘
    dynamicGuofengMarch.functionFit.addArray({ "T", "t", "S" });
    dynamicGuofengMarch.formFit.addArray({ "intro", "chorus" });
    dynamicGuofengMarch.familyId = "ChineseOpenFamily"; // 完美命中中国传统开放和弦框架
    dynamicGuofengMarch.grooveAffinity = "Straight";
    dynamicGuofengMarch.bassAnchored = true;
    dynamicGuofengMarch.complexity = 3;
    dynamicGuofengMarch.energy = 5;
    patterns.push_back(dynamicGuofengMarch);

    // 6. 苏格兰高地风笛兵团 (针对盲区：凯尔特/高地军队律动、风笛持续低音特征)
    auto highlandBagpipeMarch = makePattern("highland_bagpipe_march_055", cn(u8"高地风笛兵团律动"), "Highland Bagpipe March",
                                            ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 独特的持续空五度“无人机”低音背景
                                            { step(0.0, 3.9, 0, -1, 9, true),   // 根音极度拉长，模仿风笛底层持续不灭的 Drone 音
                                              step(0.0, 3.9, 4, 0, 7, false),  // 纯五度音同步拉长，形成风笛标志性空五度基底
                                              step(1.0, 0.45, 1, 0, 6),         // 右手声部在风笛持续音之上进行分解跳跃
                                              step(1.5, 0.45, 2, 0, 5), 
                                              step(2.0, 0.45, 3, 1, 8), 
                                              step(2.5, 0.45, 2, 0, 4), 
                                              step(3.0, 0.95, 1, 0, 6) });
    highlandBagpipeMarch.styleTags.addArray({ "world", "folk", "march", cn(u8"Dorian/多利亚") }); // 凯尔特最爱多利亚调式
    highlandBagpipeMarch.moodTags.addArray({ "epic", "hopeful", cn(u8"明亮"), cn(u8"快乐") });
    highlandBagpipeMarch.regionTags.addArray({ "global" });
    highlandBagpipeMarch.functionFit.addArray({ "T", "bVII" });          // 完美契合风笛调式特有的降7级大和弦交替进行
    highlandBagpipeMarch.formFit.addArray({ "verse", "chorus" });
    highlandBagpipeMarch.familyId = "ChineseOpenFamily"; // 开放式架构同样对齐风笛织体
    highlandBagpipeMarch.grooveAffinity = "Straight";
    highlandBagpipeMarch.bassAnchored = true;
    highlandBagpipeMarch.complexity = 3;
    highlandBagpipeMarch.energy = 4;
    patterns.push_back(highlandBagpipeMarch);


    // ========================================================================
    // 新增补充：交响弦乐、长音铺底与影视配乐专设库 (Strings & Cinematic Scoring)
    // ========================================================================

    // 1. 节奏型短断奏弦乐 (针对薄弱项：短促律动弦乐；标签：动作、紧张、March/进行曲)
    auto staccatoStringPulse = makePattern("staccato_string_pulse_060", cn(u8"交响断奏律动弦乐"), "Orchestral Staccato Pulse",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 典型的 3-3-2 影视预告片动作律动
                                           { step(0.0, 0.2, 0, -1, 12, true),   // 极强音起跑
                                             step(0.25, 0.15, 2, 0, 4), 
                                             step(0.5, 0.15, 1, 0, 2), 
                                             step(0.75, 0.2, 4, 1, 10),         // 切分重音
                                             step(1.0, 0.15, 2, 0, 4), 
                                             step(1.25, 0.15, 1, 0, 2), 
                                             step(1.5, 0.4, 3, 0, 8) });
    staccatoStringPulse.styleTags.addArray({ "classical", "epic", "march", cn(u8"和声小调") }); 
    staccatoStringPulse.moodTags.addArray({ "energetic", cn(u8"紧张"), cn(u8"动作"), cn(u8"蓄力") }); // 完美命中大片动作标签
    staccatoStringPulse.regionTags.addArray({ "global" });
    staccatoStringPulse.functionFit.addArray({ "t", "D", "s" });
    staccatoStringPulse.formFit.addArray({ "intro", "pre-chorus", "chorus" });
    staccatoStringPulse.familyId = "OrchestralStaccatoFamily"; // 专设交响断奏家族
    staccatoStringPulse.grooveAffinity = "Straight";           // 断奏必须严格卡死时钟
    staccatoStringPulse.bassAnchored = true;
    staccatoStringPulse.complexity = 4;
    staccatoStringPulse.energy = 5;                            // 高能量
    patterns.push_back(staccatoStringPulse);

    // 2. 史诗三连音弦乐推进 (针对薄弱项：影视配乐动作类；标签：EDM/电子舞曲、紧张、动作)
    auto cinematicTripletString = makePattern("cinematic_triplet_str_061", cn(u8"史诗战役三连音弦乐"), "Cinematic Triplet Strings",
                                              ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 经典 12/8 三连音交响推进，大片标配
                                              { step(0.0, 0.4, 0, -1, 10, true), 
                                                step(0.5, 0.4, 1, 0, 4), 
                                                step(1.0, 0.4, 2, 0, 6), 
                                                step(1.5, 0.4, 0, 0, 8, true), 
                                                step(2.0, 0.4, 3, 1, 10),        // 高音爆发
                                                step(2.5, 0.4, 2, 0, 5), 
                                                step(3.0, 0.9, 4, 0, 8) });
    cinematicTripletString.styleTags.addArray({ "epic", "rock", cn(u8"Dorian/多利亚"), "12/8" });
    cinematicTripletString.moodTags.addArray({ "epic", cn(u8"紧张"), cn(u8"动作") });
    cinematicTripletString.regionTags.addArray({ "global" });
    cinematicTripletString.functionFit.addArray({ "t", "S", "D" });
    cinematicTripletString.formFit.addArray({ "chorus", "bridge" });
    cinematicTripletString.familyId = "OrchestralStaccatoFamily";
    cinematicTripletString.grooveAffinity = "Straight";
    cinematicTripletString.bassAnchored = true;
    cinematicTripletString.complexity = 3;
    cinematicTripletString.energy = 5;
    patterns.push_back(cinematicTripletString);

    // 3. 深邃极光慢铺底弦乐 (针对薄弱项：环境氛围慢弦乐；标签：空灵、温婉、柔美、梦幻)
    auto ambientStringPad = makePattern("ambient_string_pad_062", cn(u8"深邃极光慢弦乐铺底"), "Ambient String Bed Pad",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 极度舒缓、平稳、大时值的长音叠加
                                        { step(0.0, 3.95, 0, -1, 6, true),   // 根音横跨整小节，平滑淡入感
                                          step(0.0, 3.95, 4, 0, 4, false),   // 纯五度音平铺，制造宏大的空间感
                                          step(1.5, 2.45, 2, 0, 5),          // 三音在小节中途温暖切入，避免正拍冲击
                                          step(2.5, 1.45, 7, 1, 5) });       // 高声部泛音轻轻柔化
    ambientStringPad.styleTags.addArray({ "ambient", "traditional", cn(u8"清乐") });
    ambientStringPad.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"温婉"), cn(u8"柔美"), cn(u8"梦幻") }); // 全面覆盖慢速氛围标签
    ambientStringPad.regionTags.addArray({ "global" });
    ambientStringPad.functionFit.addArray({ "T", "S" });
    ambientStringPad.formFit.addArray({ "intro", "outro", "verse" });   // 主打前奏和尾奏垫底
    ambientStringPad.familyId = "CinematicStringPadFamily";             // 专属慢铺底家族
    ambientStringPad.grooveAffinity = "Late";                           // 带有非常舒缓的呼吸滞后感
    ambientStringPad.bassAnchored = true;
    ambientStringPad.complexity = 1;                                    // 物理结构极简
    ambientStringPad.energy = 1;                                        // 能量最低，用于营造极致安静或宏大的背景
    patterns.push_back(ambientStringPad);

    // 4. 华语流行宏大交响铺底 (针对薄弱项：和声类弦乐/副歌托底；标签：C-Pop/华语流行、温暖、蓄力)
    auto mpopSustainedString = makePattern("mpop_sustained_string_063", cn(u8"华语巅峰副歌交响弦乐"), "Mandopop Symphony String Bed",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, 
                                           { step(0.0, 1.95, 0, -1, 8, true),  // 前半段根音
                                             step(0.0, 3.95, 2, 0, 6, false),  // 三音全小节贯穿，保证和声色彩浓郁
                                             step(2.0, 1.95, 1, -1, 7, true),  // 后半段低音做转位下行走音
                                             step(2.0, 1.95, 4, 1, 8) });      // 高声部在弱拍攀升
    mpopSustainedString.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行"), cn(u8"七和弦") });
    mpopSustainedString.moodTags.addArray({ "romantic", cn(u8"温暖"), cn(u8"蓄力") });
    mpopSustainedString.regionTags.addArray({ "chinese" });
    mpopSustainedString.functionFit.addArray({ "T", "S", "D", "t" });
    mpopSustainedString.formFit.addArray({ "chorus", "bridge" });       // 副歌高级托底
    mpopSustainedString.familyId = "CinematicStringPadFamily";
    mpopSustainedString.grooveAffinity = "Straight";
    mpopSustainedString.bassAnchored = true;
    mpopSustainedString.complexity = 2;
    mpopSustainedString.energy = 3;
    patterns.push_back(mpopSustainedString);

    // 5. 影视九和弦异域编织 (针对薄弱项：影视配乐高级和声；标签：Lydian/利底亚、九和弦、梦幻、空灵)
    auto cinematicNineWeave = makePattern("cinematic_nine_weave_064", cn(u8"影视利底亚九和弦织体"), "Cinematic Lydian 9th Weave",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 极其高级的九和弦开阔排列（Open Voicing）
                                          { step(0.0, 1.9, 0, -1, 8, true), 
                                            step(0.5, 0.9, 4, 0, 5), 
                                            step(1.0, 0.9, 8, 1, 7),           // 高亮九音（扩展音 index 8）
                                            step(2.0, 1.9, 1, 0, 6, true),     // 稳住和声中枢
                                            step(2.5, 0.9, 3, 0, 5), 
                                            step(3.0, 0.9, 6, 1, 8) });        // 高亮 Lydian 标志性的 #4 音
    cinematicNineWeave.styleTags.addArray({ "ambient", cn(u8"Lydian/利底亚"), cn(u8"九和弦"), cn(u8"挂留") });
    cinematicNineWeave.moodTags.addArray({ "mysterious", cn(u8"梦幻"), cn(u8"空灵") });
    cinematicNineWeave.regionTags.addArray({ "global" });
    cinematicNineWeave.functionFit.addArray({ "T", "S" });
    cinematicNineWeave.formFit.addArray({ "intro", "verse", "bridge" });
    cinematicNineWeave.familyId = "HybridEpicStringFamily";            // 混合史诗/异域弦乐家族
    cinematicNineWeave.grooveAffinity = "Straight";
    cinematicNineWeave.bassAnchored = true;
    cinematicNineWeave.complexity = 4;
    cinematicNineWeave.energy = 2;
    patterns.push_back(cinematicNineWeave);

    // 6. 赛博朋克融合弦乐 (针对薄弱项：现代跨界影视配乐；标签：EDM/电子舞曲、紧张、动作)
    auto cyberCinematicString = makePattern("cyber_cinematic_str_065", cn(u8"赛博朋克融合重低音弦乐"), "Cyberpunk Hybrid Strings",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 带有强烈工业和锯齿感的低音剧烈切分
                                            { step(0.0, 0.45, 0, -2, 12, true),  // 极具毁灭感的超低音砸下
                                              step(0.5, 0.2, 1, -1, 8), 
                                              step(0.75, 0.2, 2, -1, 10),        // 快速半音程向下压迫
                                              step(1.0, 0.45, 0, -2, 11, true), 
                                              step(1.5, 0.2, 5, 0, 9),           // 特征调式音异军突起
                                              step(1.75, 0.2, 4, 0, 7) });
    cyberCinematicString.styleTags.addArray({ "edm", "rock", cn(u8"Phrygian/弗里吉亚"), cn(u8"七和弦") });
    cyberCinematicString.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    cyberCinematicString.regionTags.addArray({ "global" });
    cyberCinematicString.functionFit.addArray({ "t", "D" });
    cyberCinematicString.formFit.addArray({ "chorus", "bridge" });
    cyberCinematicString.familyId = "HybridEpicStringFamily";
    cyberCinematicString.grooveAffinity = "Early";                      // 激进的前冲切分
    cyberCinematicString.bassAnchored = true;
    cyberCinematicString.complexity = 4;
    cyberCinematicString.energy = 5;
    patterns.push_back(cyberCinematicString);


    // ========================================================================
    // 新增补充：现代与传统民谣吉他指弹/扫弦专用库 (Folk & Guitar Picking Master)
    // ========================================================================

    // 1. 经典吉他特拉维斯指弹 (针对核心盲区：经典民谣指弹；标签：Folk/民谣、温暖、治愈)
    auto travisPicking = makePattern("travis_picking_folk_070", cn(u8"特拉维斯经典民谣指弹"), "Classic Travis Picking",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4/4 拍经典的交替低音指弹
                                     { step(0.0, 0.45, 0, -1, 10, true),   // 大拇指强交替低音 1
                                       step(0.5, 0.45, 2, 0, -2), 
                                       step(1.0, 0.45, 1, 0, -4),          // 大拇指交替低音 2
                                       step(1.5, 0.45, 3, 1, 4),           // 高音弦勾指
                                       step(2.0, 0.45, 0, 0, 8, true),     // 循环交替低音 3
                                       step(2.5, 0.45, 2, 0, -2), 
                                       step(3.0, 0.45, 1, -1, -4), 
                                       step(3.5, 0.45, 4, 1, 6) });
    travisPicking.styleTags.addArray({ "folk", "acoustic", cn(u8"Folk/民谣") });
    travisPicking.moodTags.addArray({ "warm", "hopeful", cn(u8"温暖"), cn(u8"治愈") });
    travisPicking.regionTags.addArray({ "global" });
    travisPicking.functionFit.addArray({ "T", "S", "D" });
    travisPicking.formFit.addArray({ "verse", "chorus" });
    travisPicking.familyId = "GuitarPickingFamily"; // 完美激活吉他指弹专属家族
    travisPicking.grooveAffinity = "Straight";
    travisPicking.bassAnchored = true;
    travisPicking.complexity = 3;                    // 规避匹配器的复杂度扣分惩罚
    travisPicking.energy = 2;                        // 走轻柔中低能路线
    patterns.push_back(travisPicking);

    // 2. 台湾校园民谣复合三连音 (针对盲区家族：CampusFolkFamily；标签：校园民谣、青春、温暖)
    auto campusFolkArp = makePattern("campus_folk_acoustic_071", cn(u8"白衣飘飘的年代"), "Taiwan Campus Folk Wave",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 带有极其经典的12/8或复拍子民谣晃动感
                                     { step(0.0, 0.95, 0, -1, 8, true), 
                                       step(1.0, 0.45, 2, 0, 2), 
                                       step(1.5, 0.45, 4, 0, 0), 
                                       step(2.0, 0.95, 1, 0, 6, true),    // 弱拍根音移动
                                       step(3.0, 0.45, 3, 1, 4), 
                                       step(3.5, 0.45, 2, 0, 2) });
    campusFolkArp.styleTags.addArray({ "folk", "acoustic", "ballad", cn(u8"CampusFolkFamily") }); // 绑定校园民谣
    campusFolkArp.moodTags.addArray({ "nostalgic", cn(u8"温暖"), cn(u8"治愈") });
    campusFolkArp.regionTags.addArray({ "chinese", cn(u8"台湾") });       // 精准命中地缘
    campusFolkArp.functionFit.addArray({ "T", "S", "t" });
    campusFolkArp.formFit.addArray({ "verse" });
    campusFolkArp.familyId = "CampusFolkFamily";    // 100% 激活闲置加分规则
    campusFolkArp.grooveAffinity = "Straight";
    campusFolkArp.bassAnchored = true;
    campusFolkArp.complexity = 2;
    campusFolkArp.energy = 2;
    patterns.push_back(campusFolkArp);

    // 3. 西北民谣大漠花儿 (针对生僻地缘标签：甘肃、青海、民谣大跳律动)
    auto northwesternFolk = makePattern("northwest_desert_folk_072", cn(u8"陇上黄土花儿"), "Northwest Desert Folk Arp",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 粗犷、高亢、五声音程跳跃大
                                        { step(0.0, 1.4, 0, -1, 12, true),  // 沉稳的黄土地根音
                                          step(1.5, 0.45, 4, 0, 4),         // 纯五度向上大跳
                                          step(2.0, 0.95, 7, 1, 8),         // 高音呼喊模拟
                                          step(3.0, 0.95, 2, 0, 6) });
    northwesternFolk.styleTags.addArray({ "world", "folk", cn(u8"Folk/民谣"), cn(u8"清乐") });
    northwesternFolk.moodTags.addArray({ "epic", "calm", cn(u8"伤感") });
    northwesternFolk.regionTags.addArray({ "chinese", cn(u8"甘肃"), cn(u8"青海") }); // 歼灭盲区省份标签
    northwesternFolk.functionFit.addArray({ "t", "s", "T" });
    northwesternFolk.formFit.addArray({ "intro", "verse" });
    northwesternFolk.familyId = "ChineseOpenFamily";
    northwesternFolk.grooveAffinity = "Late";        // 带有西北民歌自由拉腔的微弱滞后
    northwesternFolk.bassAnchored = true;
    northwesternFolk.complexity = 3;
    northwesternFolk.energy = 3;
    patterns.push_back(northwesternFolk);

    // 4. 英伦民谣切分扫弦模拟 (针对薄弱项：民谣吉他扫弦织体；标签：独立、动作、EDM/电子舞曲)
    auto indieBritishFolk = makePattern("indie_british_folk_073", cn(u8"英伦独立民谣扫弦"), "Indie British Folk Strum",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 16分切分吉他下上扫弦物理还原
                                        { step(0.0, 0.2, 0, -1, 10, true),   // 下击根音
                                          step(0.25, 0.15, 1, 0, -4),        // 轻轻上挑和弦音
                                          step(0.5, 0.2, 2, 0, 6),           // 切分下击
                                          step(0.75, 0.15, 4, 1, -2),        // 上挑
                                          step(1.0, 0.15, 0, 0, 4), 
                                          step(1.25, 0.15, 3, 0, -4), 
                                          step(1.5, 0.35, 2, 0, 8) });       // 强力下扫解决
    indieBritishFolk.styleTags.addArray({ "folk", "rock", "pop", "c-pop" });
    indieBritishFolk.moodTags.addArray({ "energetic", cn(u8"独立"), cn(u8"动作") }); // 对应现代独立民谣摇滚
    indieBritishFolk.regionTags.addArray({ "global" });
    indieBritishFolk.functionFit.addArray({ "T", "S", "D" });
    indieBritishFolk.formFit.addArray({ "chorus", "bridge" });          // 极其适合副歌扫弦推进
    indieBritishFolk.familyId = "GuitarPickingFamily";
    indieBritishFolk.grooveAffinity = "Early";       // 微弱前冲，制造现代Indie乐队的蓬勃朝气
    indieBritishFolk.bassAnchored = true;
    indieBritishFolk.complexity = 4;
    indieBritishFolk.energy = 4;                    // 高能量民谣
    patterns.push_back(indieBritishFolk);

    // 5. 凯尔特爱尔兰叙事（针对盲区标签：自然小调、多利亚、梦幻）
    auto celticStoryteller = makePattern("celtic_storyteller_074", cn(u8"翡翠海湾民谣叙事"), "Celtic Storyteller Harp",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 模仿竖琴与吉他的涟漪交织
                                         { step(0.0, 1.4, 0, -1, 8, true), 
                                           step(1.0, 0.45, 2, 0, 2), 
                                           step(1.5, 0.45, 4, 0, 4), 
                                           step(2.0, 1.4, 1, 0, 6, true), 
                                           step(3.0, 0.45, 5, 1, 8),         // 高音色彩交织
                                           step(3.5, 0.45, 3, 0, 2) });
 celticStoryteller.styleTags.addArray({ "world", "folk", cn(u8"自然小调"), cn(u8"Dorian/多利亚") });
    celticStoryteller.moodTags.addArray({ "mysterious", cn(u8"梦幻"), cn(u8"空灵") });
    celticStoryteller.regionTags.addArray({ "global" });
    celticStoryteller.functionFit.addArray({ "t", "s", "bVII" });
    celticStoryteller.formFit.addArray({ "intro", "verse" });
    celticStoryteller.familyId = "GuitarPickingFamily";
    celticStoryteller.grooveAffinity = "Straight";
    celticStoryteller.bassAnchored = true;
    celticStoryteller.complexity = 3;
    celticStoryteller.energy = 2;
    patterns.push_back(celticStoryteller);

    // 6. 现代民谣舒缓六音指弹 (针对薄弱项：慢速六拍子民谣预设；标签：Folk/民谣、温暖)
    auto slowFolkSixBeat = makePattern("slow_folk_six_beat_075", cn(u8"篝火余烬六拍指弹"), "Slow Folk Six-Beat Picking",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 3.0, // 3/4拍 或 6/8拍 经典民谣三拍子律动
                                       { step(0.0, 0.45, 0, -1, 10, true),  // 正拍大拇指重音
                                         step(0.5, 0.45, 1, 0, 0), 
                                         step(1.0, 0.45, 2, 0, 2), 
                                         step(1.5, 0.45, 4, 1, 4),          // 高音交织
                                         step(2.0, 0.45, 2, 0, 0), 
                                         step(2.5, 0.45, 1, 0, -2) });
    slowFolkSixBeat.styleTags.addArray({ "folk", "acoustic", "3/4", "12/8" });
    slowFolkSixBeat.moodTags.addArray({ "calm", cn(u8"温暖"), cn(u8"治愈") });
    slowFolkSixBeat.regionTags.addArray({ "global" });
    slowFolkSixBeat.functionFit.addArray({ "T", "S" });
    slowFolkSixBeat.formFit.addArray({ "verse", "outro" });
    slowFolkSixBeat.familyId = "GuitarPickingFamily";
    slowFolkSixBeat.grooveAffinity = "Straight";
    slowFolkSixBeat.bassAnchored = true;
    slowFolkSixBeat.complexity = 2;
    slowFolkSixBeat.energy = 1;                    // 适合极致安静的吟唱背景
    patterns.push_back(slowFolkSixBeat);

    // ========================================================================
    // 新增补充：基于681条预设特性定制库 (Preset-Driven Advanced Patterns)
    // ========================================================================

    // 1. J-Pop二次元王道切分 (针对预设：黑田进行、王道进行；标签：J-Pop/日系、明亮、蓄力)
    auto jpopAnimeAnimeDrive = makePattern("jpop_anime_drive_080", cn(u8"二次元王道激燃切分"), "Anime J-Pop High-Energy Drive",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // ACG热血钢琴/吉他核心：高频八度大跳与切分
                                           { step(0.0, 0.2, 0, -1, 12, true),   // 左手极强低音砸下
                                             step(0.25, 0.15, 2, 0, 4),         // 右手迅速补入和弦内部音
                                             step(0.5, 0.2, 0, 0, 8, true),     // 八度高位根音跃动
                                             step(0.75, 0.15, 4, 1, 6),         // 高音弦切分
                                             step(1.0, 0.15, 1, 0, 3), 
                                             step(1.25, 0.15, 3, 0, 5), 
                                             step(1.5, 0.35, 2, 1, 10),         // 1.5拍强力前半拍切分
                                             step(2.0, 0.2, 0, -1, 11, true),   // 第二周期循环
                                             step(2.5, 0.2, 4, 0, 6), 
                                             step(2.75, 0.15, 7, 1, 9), 
                                             step(3.5, 0.45, 1, 0, 2) });
    jpopAnimeAnimeDrive.styleTags.addArray({ "pop", "rock", cn(u8"J-Pop/日系"), cn(u8"大调") }); // 完美对应黑田/王道预设
    jpopAnimeAnimeDrive.moodTags.addArray({ "energetic", "happy", cn(u8"明亮"), cn(u8"蓄力") });
    jpopAnimeAnimeDrive.regionTags.addArray({ "global" });
    jpopAnimeAnimeDrive.functionFit.addArray({ "S", "D", "T" });        // 极度适合 IV-V-III-VI 这种下属起步的进行
    jpopAnimeAnimeDrive.formFit.addArray({ "chorus", "pre-chorus" });   // 副歌高能强推
    jpopAnimeAnimeDrive.familyId = "JayStyleSyncopationFamily";         // 借用高效切分计算链路
    jpopAnimeAnimeDrive.grooveAffinity = "Straight";
    jpopAnimeAnimeDrive.bassAnchored = true;
    jpopAnimeAnimeDrive.complexity = 4;
    jpopAnimeAnimeDrive.energy = 5;                                     // 爆发级能量
    patterns.push_back(jpopAnimeAnimeDrive);

    // 2. 12小节标准蓝调摇摆 (针对预设：12小节蓝调、Blues；标签：Blues/布鲁斯、Jazz/爵士、七和弦)
    auto bluesShuffle7th = makePattern("blues_shuffle_7th_081", cn(u8"标准蓝调12小节摇摆"), "Standard Blues Shuffle 7th",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 2.0, // 核心：极度强调一音与属七音（Index 3）的交替
                                       { step(0.0, 0.6, 0, -1, 10, true),   // 根音拉长
                                         step(0.66, 0.3, 2, 0, 2),          // 严格卡在三连音最后一个音符的 Shuffle 滞后感
                                         step(1.0, 0.6, 3, 0, 8),           // 灵魂：高亮属七音（Index 3），制造浓郁蓝调味
                                         step(1.66, 0.3, 4, 0, 4) });       // 走向五音
    bluesShuffle7th.styleTags.addArray({ "jazz", "blues", cn(u8"Blues/布鲁斯"), cn(u8"Jazz/爵士"), cn(u8"七和弦") }); // 闭环蓝调预设
    bluesShuffle7th.moodTags.addArray({ "warm", cn(u8"独立") });
    bluesShuffle7th.regionTags.addArray({ "global" });
    bluesShuffle7th.functionFit.addArray({ "T", "S", "D" });
    bluesShuffle7th.formFit.addArray({ "verse", "chorus" });
    bluesShuffle7th.familyId = "GuitarPickingFamily";
    bluesShuffle7th.grooveAffinity = "Late";                               // 摇摆乐天然的滞后呼吸感
    bluesShuffle7th.bassAnchored = true;
    bluesShuffle7th.complexity = 3;
    bluesShuffle7th.energy = 3;
    patterns.push_back(bluesShuffle7th);

    // 3. 爵士2-5-1优雅九和弦 (针对预设：2-5-1爵士、Jazz；标签：Jazz/爵士、九和弦、柔美)
    auto jazzWalkNine = makePattern("jazz_walk_nine_082", cn(u8"爵士五巡九和弦行走"), "Jazz Walking Nine Voicing",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 经典爵士钢琴的散步式分解（A/B Voicing）
                                    { step(0.0, 0.95, 0, -1, 8, true), 
                                      step(1.0, 0.45, 2, 0, 0), 
                                      step(1.5, 0.45, 3, 0, 4),            // 高亮七音
                                      step(2.0, 0.95, 4, 1, 6),            // 核心：高亮九音（Index 4/或扩展音），带来高级感
                                      step(3.0, 0.95, 1, 0, 2) });
    jazzWalkNine.styleTags.addArray({ "jazz", cn(u8"Jazz/爵士"), cn(u8"九和弦"), cn(u8"七和弦") }); // 精准匹配2-5-1预设
    jazzWalkNine.moodTags.addArray({ "calm", "romantic", cn(u8"柔美"), cn(u8"温婉") });
    jazzWalkNine.regionTags.addArray({ "global" });
    jazzWalkNine.functionFit.addArray({ "SecD", "D", "T" });            // 完美对齐 ii - V - I 逻辑
    jazzWalkNine.formFit.addArray({ "verse", "bridge" });
    jazzWalkNine.familyId = "CPopBalladPianoFamily";
    jazzWalkNine.grooveAffinity = "Late";
    jazzWalkNine.bassAnchored = true;
    jazzWalkNine.complexity = 3;
    jazzWalkNine.energy = 2;
    patterns.push_back(jazzWalkNine);

    // 4. 蝙蝠侠半音阶对峙 (针对预设：黑暗骑士半音爬升、007间谍紧张；标签：紧张、动作、半音阶、电影配乐)
    auto cinematicDarkTritone = makePattern("cinematic_dark_tritone_083", cn(u8"黑暗骑士半音磨损"), "Cinematic Dark Tritone Pulse",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 低音极度重击 + 右手半音程急速摩擦
                                            { step(0.0, 0.45, 0, -2, 12, true),  // 超低音铺底
                                              step(0.5, 0.15, 1, 0, 10),         // 核心：强行摩擦小二级音/降五度音（针对 bII / bV 预设）
                                              step(0.75, 0.15, 0, 0, 6), 
                                              step(1.0, 0.45, 1, -1, 11, true),  // 低音转位压迫
                                              step(1.5, 0.15, 2, 0, 9), 
                                              step(1.75, 0.15, 1, 0, 8) });
    cinematicDarkTritone.styleTags.addArray({ "epic", "ambient", cn(u8"和声小调"), cn(u8"旋律小调") }); // 对齐暗黑大片进行
    cinematicDarkTritone.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    cinematicDarkTritone.regionTags.addArray({ "global" });
    cinematicDarkTritone.functionFit.addArray({ "t", "D" });             // 极适合 i - bII 或 i - bV 进行
    cinematicDarkTritone.formFit.addArray({ "intro", "bridge" });
    cinematicDarkTritone.familyId = "HybridEpicStringFamily";
    cinematicDarkTritone.grooveAffinity = "Straight";
    cinematicDarkTritone.bassAnchored = true;
    cinematicDarkTritone.complexity = 4;
    cinematicDarkTritone.energy = 5;
    patterns.push_back(cinematicDarkTritone);

    // 5. 戏曲欢快板眼急奏 (针对预设：黄梅烟雨、女驸马进行；标签：黄梅戏、戏曲、快乐、三和弦)
    auto operaHuangmeiPulse = makePattern("opera_huangmei_pulse_084", cn(u8"黄梅女驸马板眼"), "Huangmei Opera Fast Pulse",
                                          ArpeggioDirection::Up, ArpeggioDensity::Eighth, 2.0, // 模拟戏曲“紧打慢唱”的板眼顿音
                                          { step(0.0, 0.2, 0, -1, 8, true),    // 板鼓点（正拍极短顿音）
                                            step(0.5, 0.45, 2, 0, 4),          // 眼位切入
                                            step(1.0, 0.2, 1, 0, 6), 
                                            step(1.5, 0.45, 4, 1, 9) });       // 戏曲花腔大跳
    operaHuangmeiPulse.styleTags.addArray({ "traditional", "classical", cn(u8"黄梅戏") }); // 100% 灭活黄梅戏专属预设
    operaHuangmeiPulse.moodTags.addArray({ "happy", cn(u8"明亮"), cn(u8"温婉") });
    operaHuangmeiPulse.regionTags.addArray({ "chinese" });
    operaHuangmeiPulse.functionFit.addArray({ "T", "S" });
    operaHuangmeiPulse.formFit.addArray({ "verse", "chorus" });
    operaHuangmeiPulse.familyId = "ChineseOperaOrnamentFamily"; // 激活特定高分家族
    operaHuangmeiPulse.grooveAffinity = "Straight";
    operaHuangmeiPulse.bassAnchored = true;
    operaHuangmeiPulse.complexity = 3;
    operaHuangmeiPulse.energy = 4;
    patterns.push_back(operaHuangmeiPulse);

    // 6. R&B慵懒切分九和弦 (针对预设：周杰伦式离调进行、R&B；标签：C-Pop/华语流行、挂留、九和弦)
    auto rnbLazySyncopation = makePattern("rnb_lazy_syncopation_085", cn(u8"迷雾霓虹慵懒九和弦"), "R&B Lazy Syncopation 9th",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 经典 R&B / Neo-Soul 极其慵懒的后半拍大落差切分
                                          { step(0.0, 0.7, 0, -1, 9, true), 
                                            step(0.75, 0.2, 4, 0, 4),          // 九音（Index 4）弱拍极其温柔的勾指
                                            step(1.0, 1.4, 2, 0, 6),           // 和弦中枢音拉长
                                            step(2.5, 0.3, 3, 1, 8),           // 2.5拍切分七音
                                            step(3.0, 0.9, 1, 0, 2) });
    rnbLazySyncopation.styleTags.addArray({ "pop", "c-pop", cn(u8"C-Pop/华语流行"), cn(u8"九 shadow"), cn(u8"九和弦") });
    rnbLazySyncopation.moodTags.addArray({ "romantic", cn(u8"柔美"), cn(u8"温暖") });
    rnbLazySyncopation.regionTags.addArray({ "chinese", cn(u8"台湾") });
    rnbLazySyncopation.functionFit.addArray({ "T", "t", "S" });
    rnbLazySyncopation.formFit.addArray({ "verse", "bridge" });
    rnbLazySyncopation.familyId = "JayStyleSyncopationFamily"; // 完美命中周董R&B切分规则
    rnbLazySyncopation.grooveAffinity = "Late";                 // 极致的慵懒后靠（Layback）
    rnbLazySyncopation.bassAnchored = true;
    rnbLazySyncopation.complexity = 4;
    rnbLazySyncopation.energy = 2;
    patterns.push_back(rnbLazySyncopation);

    // ========================================================================
    // 新增补充：基于681个预设名称关键字深度定制库 (Name-Driven Specialized Catalog)
    // ========================================================================

    // 1. 加勒比海盗式 12/8 奔腾交响 (针对预设：加勒比海盗律动、史诗战役；标签：动作、紧张、12/8)
    auto piratesEpicMarch = makePattern("pirates_epic_march_090", cn(u8"加勒比海盗奔腾律动"), "Pirates Epic 12/8 Drive",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 3.0, // 3.0拍循环 极其纯正的 12/8 拍马蹄铁滚动感
                                        { step(0.0, 0.3, 0, -1, 12, true),   // 正拍战鼓（强根音）
                                          step(0.33, 0.15, 2, 0, 4),         // 快速滚奏弱拍音 1
                                          step(0.66, 0.15, 1, 0, 2),         // 快速滚奏弱拍音 2
                                          step(1.0, 0.3, 3, 0, 10),          // 第二声重拍大跳
                                          step(1.33, 0.15, 4, 1, 6), 
                                          step(1.66, 0.15, 2, 0, 4), 
                                          step(2.0, 0.3, 0, 0, 11, true),    // 攀爬回中枢
                                          step(2.33, 0.15, 5, 1, 8), 
                                          step(2.66, 0.15, 3, 0, 5) });
    piratesEpicMarch.styleTags.addArray({ "epic", "march", "12/8", cn(u8"Dorian/多利亚") }); // 完美对应加勒比海盗等传奇小调12/8预设
    piratesEpicMarch.moodTags.addArray({ "energetic", cn(u8"动作"), cn(u8"紧张") });
    piratesEpicMarch.regionTags.addArray({ "global" });
    piratesEpicMarch.functionFit.addArray({ "t", "S", "bVII" });         // 极度适合 i - bVII - v 这种海盗式离调大开大合进行
    piratesEpicMarch.formFit.addArray({ "chorus", "bridge" });
    piratesEpicMarch.familyId = "HybridEpicStringFamily";
    piratesEpicMarch.grooveAffinity = "Straight";
    piratesEpicMarch.bassAnchored = true;
    piratesEpicMarch.complexity = 4;
    piratesEpicMarch.energy = 5;                                         // 史诗级最高能级
    patterns.push_back(piratesEpicMarch);

    // 2. 卡农黄金进行柔美叠进 (针对预设：卡农进行、黄金卡农、万能进行；标签：大调、温暖、治愈)
    auto canonGoldenFlow = makePattern("canon_golden_flow_091", cn(u8"卡农黄金大调叠进"), "Classic Canon Golden Flow",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4/4 拍最经典、四平八稳的流线性对位分解
                                       { step(0.0, 0.95, 0, -1, 8, true),   // 沉稳根音
                                         step(0.5, 0.45, 2, 0, 1),          // 向上流动的和弦内音
                                         step(1.0, 0.95, 4, 0, 4), 
                                         step(1.5, 0.45, 1, 0, 2), 
                                         step(2.0, 0.95, 7, 1, 6),          // 高声部泛音泛起（天籁感）
                                         step(2.5, 0.45, 3, 0, 3), 
                                         step(3.0, 0.95, 2, 0, 4), 
                                         step(3.5, 0.45, 4, 0, 1) });
    canonGoldenFlow.styleTags.addArray({ "pop", "classical", "ballad", cn(u8"大调") }); // 锁定任何叫“卡农”的预设
    canonGoldenFlow.moodTags.addArray({ "romantic", "hopeful", cn(u8"温暖"), cn(u8"治愈") });
    canonGoldenFlow.regionTags.addArray({ "global" });
    canonGoldenFlow.functionFit.addArray({ "T", "D", "t", "S" });        // 精准对齐 I-V-vi-iii-IV-I-IV-V
    canonGoldenFlow.formFit.addArray({ "verse", "chorus" });
    canonGoldenFlow.familyId = "CPopBalladPianoFamily";
    canonGoldenFlow.grooveAffinity = "Straight";
    canonGoldenFlow.bassAnchored = true;
    canonGoldenFlow.complexity = 3;
    canonGoldenFlow.energy = 3;
    patterns.push_back(canonGoldenFlow);

    // 3. 重金属/极端金属低音锯齿 (针对预设：黑森林金属、重金属、极端金属、tritone；标签：Rock/摇滚、减五度)
    auto heavyMetalTritone = makePattern("heavy_metal_tritone_092", cn(u8"重金属魔鬼音程锯齿"), "Heavy Metal Tritone Chug",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 极其激进的重金属吉他掌心闷音（Palm Mute）低音重击
                                         { step(0.0, 0.15, 0, -2, 12, true),  // 极强超低根音
                                           step(0.25, 0.1, 0, -2, 8, true),   // 闷音碎击 1
                                           step(0.5, 0.15, 4, -1, 10),        // 核心：若预设带降五度音（bV），在此处剧烈摩擦
                                           step(0.75, 0.1, 0, -2, 8, true),   // 闷音碎击 2
                                           step(1.0, 0.15, 1, -1, 11),        // 小二级音切分压迫
                                           step(1.25, 0.1, 0, -2, 7, true), 
                                           step(1.5, 0.35, 2, -1, 9) });
    heavyMetalTritone.styleTags.addArray({ "rock", "epic", cn(u8"和声小调"), cn(u8"减五度") });
    heavyMetalTritone.moodTags.addArray({ "energetic", cn(u8"紧张"), cn(u8"动作") }); // 彻底闭环摇滚金属区
    heavyMetalTritone.regionTags.addArray({ "global" });
    heavyMetalTritone.functionFit.addArray({ "t", "D" });
    heavyMetalTritone.formFit.addArray({ "chorus", "bridge" });
    heavyMetalTritone.familyId = "ElectronicArpFamily";                  // 借助高爆律动
    heavyMetalTritone.grooveAffinity = "Early";                           // 情绪极度前冲
    heavyMetalTritone.bassAnchored = true;
    heavyMetalTritone.complexity = 4;
    heavyMetalTritone.energy = 5;
    patterns.push_back(heavyMetalTritone);

    // 4. 西北边关大漠苍凉（针对预设：大漠花儿、西北边关、广袤、风情；标签：甘肃、青海、空灵、角调）
    auto desertGansuVibe = makePattern("desert_gansu_vibe_093", cn(u8"大漠黄沙长河落日"), "Gansu Desert Wilderness",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 极度宽广、极少音符、大时值的声部留白
                                       { step(0.0, 1.9, 0, -1, 10, true),   // 厚重的长低音
                                         step(0.0, 3.9, 4, 0, 4, false),   // 空纯五度全小节长音托底，没有任何三音的模糊感
                                         step(2.0, 1.9, 7, 1, 8) });       // 极高声部如大漠羌笛孤零零地一声长叹
    desertGansuVibe.styleTags.addArray({ "world", "folk", cn(u8"角调"), cn(u8"清乐") });
    desertGansuVibe.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"伤感") });
    desertGansuVibe.regionTags.addArray({ "chinese", cn(u8"甘肃"), cn(u8"青海") }); // 精准歼灭大漠/西北边关类预设
    desertGansuVibe.functionFit.addArray({ "t", "S" });
    desertGansuVibe.formFit.addArray({ "intro", "outro" });
    desertGansuVibe.familyId = "GuqinSparseFamily";                     // 古琴般高雅留白
    desertGansuVibe.grooveAffinity = "Late";
    desertGansuVibe.bassAnchored = true;
    desertGansuVibe.complexity = 1;                                    // 规避复杂度评分
    desertGansuVibe.energy = 1;                                        // 苍凉、极低能量
    patterns.push_back(desertGansuVibe);

    // 5. 华语巅峰流行黄金进行 (针对预设：黄金进行、华语流行、周氏、情歌；标签：C-Pop/华语流行、温暖、七和弦)
    auto mpopGoldenBallad = makePattern("mpop_golden_ballad_094", cn(u8"华语情歌巅峰黄金律动"), "Mandopop Ultimate Golden Ballad",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 完美的“前4后8”大拉伸流行钢琴伴奏
                                        { step(0.0, 1.95, 0, -1, 9, true),  // 前半拍超长和弦低音根基
                                          step(0.0, 1.95, 2, 0, 4, false), 
                                          step(2.0, 0.45, 1, 0, 6),          // 后半拍情绪拉满，做流畅精细的八分音符切分流动
                                          step(2.5, 0.45, 3, 0, 5),          // 高亮七音
                                          step(3.0, 0.45, 4, 1, 7),          // 向上攀爬
                                          step(3.5, 0.45, 2, 0, 3) });
    mpopGoldenBallad.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行"), cn(u8"七和弦") }); // 绝配681预设中的所有流行黄金大件
    mpopGoldenBallad.moodTags.addArray({ "romantic", cn(u8"温暖"), cn(u8"蓄力") });
    mpopGoldenBallad.regionTags.addArray({ "chinese" });
    mpopGoldenBallad.functionFit.addArray({ "T", "S", "t", "D" });       // 通杀 I-V-vi-IV 极其变体进行
    mpopGoldenBallad.formFit.addArray({ "verse", "chorus" });
    mpopGoldenBallad.familyId = "CPopBalladPianoFamily";
    mpopGoldenBallad.grooveAffinity = "Straight";
    mpopGoldenBallad.bassAnchored = true;
    mpopGoldenBallad.complexity = 3;
    mpopGoldenBallad.energy = 3;
    patterns.push_back(mpopGoldenBallad);

    // 6. 欧式巴洛克赋格十一和弦 (针对预设：巴洛克、古典赞歌、十一和弦；标签：Classical/古典、大调、九和弦)
    auto baroqueFugue11th = makePattern("baroque_fugue_11th_095", cn(u8"巴洛克复调十一和弦"), "Baroque Polyphonic 11th Fugue",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 左右手交织，高度高亮高阶和弦的复调线条（Polyphony）
                                        { step(0.0, 0.45, 0, -1, 8, true), 
                                          step(0.5, 0.45, 2, 0, 3), 
                                          step(1.0, 0.45, 4, 0, 5),          // 九音
                                          step(1.5, 0.45, 6, 1, 8),          // 核心：高亮十一音（Index 6/延伸音），极具欧洲宫廷复古色彩
                                          step(2.0, 0.45, 1, 0, 4), 
                                          step(2.5, 0.45, 3, 0, 6), 
                                          step(3.0, 0.95, 5, 1, 7) });
    baroqueFugue11th.styleTags.addArray({ "classical", cn(u8"大调"), cn(u8"九和弦"), cn(u8"十一和弦") });
    baroqueFugue11th.moodTags.addArray({ "calm", "hopeful", cn(u8"明亮") });
    baroqueFugue11th.regionTags.addArray({ "global" });
    baroqueFugue11th.functionFit.addArray({ "T", "D", "S" });
    baroqueFugue11th.formFit.addArray({ "intro", "verse", "bridge" });
    baroqueFugue11th.familyId = "ChineseOpenFamily"; // 借用纯净开放骨架
    baroqueFugue11th.grooveAffinity = "Straight";
    baroqueFugue11th.bassAnchored = true;
    baroqueFugue11th.complexity = 4;
    baroqueFugue11th.energy = 3;
    patterns.push_back(baroqueFugue11th);

    // ========================================================================
    // 新增补充：基于曲式段落与作曲技法深度定制库 (Form & Sectional Specialized Patterns)
    // ========================================================================

    // 1. 前奏专设：空间感氛围淡入 (针对曲式：前奏 Intro；标签：空灵、梦幻、柔美)
    auto introSpacialFade = makePattern("form_intro_fade_100", cn(u8"前奏：晨曦空间氛围长音"), "Intro Spacial Ambient Fade",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 4/4拍 极简高位分解，为歌手进歌留足空间
                                        { step(0.0, 3.9, 0, 0, 6, true),    // 根音轻柔长音托底
                                          step(1.0, 2.9, 4, 1, 4),          // 高五度泛音在后半拍轻柔漂浮
                                          step(2.5, 1.4, 7, 1, 5) });       // 冠音微弱闪烁
    introSpacialFade.styleTags.addArray({ "ambient", "pop", "ballad" });
    introSpacialFade.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"梦幻"), cn(u8"柔美") });
    introSpacialFade.regionTags.addArray({ "global" });
    introSpacialFade.functionFit.addArray({ "T", "S" });
    introSpacialFade.formFit.addArray({ "intro" });                         // 锁定前奏
    introSpacialFade.familyId = "CinematicStringPadFamily";
    introSpacialFade.grooveAffinity = "Late";
    introSpacialFade.bassAnchored = true;
    introSpacialFade.complexity = 1;
    introSpacialFade.energy = 1;                                            // 极低能级
    patterns.push_back(introSpacialFade);

    // 2. 主歌专设：叙事性轻柔对话 (针对曲式：主歌 Verse；标签：温暖、治愈、轻柔)
    auto verseStorytelling = makePattern("form_verse_talk_101", cn(u8"主歌：娓娓道来叙事分解"), "Verse Gentle Storytelling Piano",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 经典中低音区交替对位，像在讲故事
                                         { step(0.0, 1.9, 0, -1, 8, true),   // 沉稳左手低音
                                           step(0.5, 0.45, 2, 0, 2),         // 右手平稳中音区回应
                                           step(1.0, 0.9, 1, 0, 4), 
                                           step(1.5, 0.45, 3, 0, 3), 
                                           step(2.0, 1.9, 0, 0, 6, true),    // 中枢音拉长
                                           step(2.5, 0.45, 4, 0, 2), 
                                           step(3.0, 0.9, 2, 0, 4), 
                                           step(3.5, 0.45, 1, 0, 3) });
    verseStorytelling.styleTags.addArray({ "pop", "c-pop", "ballad", cn(u8"C-Pop/华语流行") });
    verseStorytelling.moodTags.addArray({ "warm", "calm", cn(u8"温暖"), cn(u8"治愈") });
    verseStorytelling.regionTags.addArray({ "chinese" });
    verseStorytelling.functionFit.addArray({ "T", "t", "S" });
    verseStorytelling.formFit.addArray({ "verse" });                        // 锁主歌
    verseStorytelling.familyId = "MandopopVerseFamily";
    verseStorytelling.grooveAffinity = "Straight";
    verseStorytelling.bassAnchored = true;
    verseStorytelling.complexity = 2;
    verseStorytelling.energy = 2;
    patterns.push_back(verseStorytelling);

    // 3. 过渡段专设：小节末尾16分音符阶梯爬升 (针对曲式：过渡 Pre-Chorus；标签：蓄力、紧张)
    auto preChorusClimb = makePattern("form_prechorus_climb_102", cn(u8"过渡：阶梯波浪情绪爬升"), "Pre-Chorus Tension Build-Up",
                                      ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0, // 3.0拍保持平稳，最后1拍做密集16分音符大跨度切分爬升
                                      { step(0.0, 0.9, 0, -1, 9, true), 
                                        step(1.0, 0.9, 2, 0, 6), 
                                        step(2.0, 0.45, 4, 0, 7), 
                                        step(2.5, 0.45, 1, 0, 6), 
                                        step(3.0, 0.2, 0, 0, 8, true),      // 第4拍开始极速阶梯式轰炸
                                        step(3.25, 0.2, 2, 0, 9), 
                                        step(3.5, 0.2, 4, 1, 10), 
                                        step(3.75, 0.2, 7, 1, 12) });       // 顶峰突强，完美交接给副歌
    preChorusClimb.styleTags.addArray({ "pop", "rock", cn(u8"七和弦") });
    preChorusClimb.moodTags.addArray({ "energetic", cn(u8"蓄力"), cn(u8"紧张") }); // 爆发前夜
    preChorusClimb.regionTags.addArray({ "global" });
    preChorusClimb.functionFit.addArray({ "S", "D", "SecD" });           // 极其适合向副歌I级过渡的IV级、V级和弦
    preChorusClimb.formFit.addArray({ "pre-chorus" });                     // 锁导歌/过渡
    preChorusClimb.familyId = "MandopopPrechorusFamily";
    preChorusClimb.grooveAffinity = "Straight";
    preChorusClimb.bassAnchored = true;
    preChorusClimb.complexity = 4;
    preChorusClimb.energy = 4;
    patterns.push_back(preChorusClimb);

    // 4. 副歌专设：全声部黄金八度推动 (针对曲式：副歌 Chorus；标签：明亮、快乐、爆发)
    auto chorusUltimateDrive = makePattern("form_chorus_drive_103", cn(u8"副歌：黄金八度全景海浪"), "Chorus Ultimate Octave Drive",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 16分音符极高密度，左手八度，右手切分音雨，能量拉满
                                           { step(0.0, 0.2, 0, -1, 12, true),   // 八度超强根音
                                             step(0.25, 0.15, 4, 0, 8), 
                                             step(0.5, 0.2, 0, 0, 10, true),    // 八度高位跃动
                                             step(0.75, 0.15, 7, 1, 11),        // 右手冠音大跳
                                             step(1.0, 0.15, 2, 0, 7), 
                                             step(1.25, 0.15, 3, 0, 9),         // 高亮色彩音
                                             step(1.5, 0.35, 4, 1, 12),         // 核心正半拍切分
                                             step(2.0, 0.2, 1, -1, 11, true),   // 下半周期
                                             step(2.5, 0.2, 4, 0, 8), 
                                             step(2.75, 0.15, 9, 1, 12),        // 高扩展音大跳
                                             step(3.5, 0.45, 2, 0, 6) });
    chorusUltimateDrive.styleTags.addArray({ "pop", "c-pop", "rock", cn(u8"C-Pop/华语流行") });
    chorusUltimateDrive.moodTags.addArray({ "energetic", "happy", cn(u8"明亮"), cn(u8"蓄力") });
    chorusUltimateDrive.regionTags.addArray({ "chinese" });
    chorusUltimateDrive.functionFit.addArray({ "T", "S", "D" });
    chorusUltimateDrive.formFit.addArray({ "chorus" });                     // 锁副歌
    chorusUltimateDrive.familyId = "CPopChorusDriveFamily";
    chorusUltimateDrive.grooveAffinity = "Straight";
    chorusUltimateDrive.bassAnchored = true;
    chorusUltimateDrive.complexity = 5;                                    // 物理结构极其宏大
    chorusUltimateDrive.energy = 5;                                        // 最高能级
    patterns.push_back(chorusUltimateDrive);

    // 5. 间奏专设：器乐化交织连奏 (针对曲式：间奏 Bridge / Solo；标签：独立、梦幻)
    auto bridgeInstrumental = makePattern("form_bridge_solo_104", cn(u8"间奏：器乐化华彩交织线条"), "Bridge Instrumental Polyphony",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 破除常规规律分解，左右手音符形成互补交织的Solo线条
                                          { step(0.0, 0.9, 0, -1, 10, true), 
                                            step(0.5, 0.45, 4, 1, 8),          // 跨越两个八度的大跳
                                            step(1.0, 0.45, 2, 0, 6), 
                                            step(1.5, 0.45, 7, 1, 9), 
                                            step(2.0, 0.9, 1, 0, 8, true), 
                                            step(2.5, 0.45, 3, 0, 5), 
                                            step(3.0, 0.45, 8, 1, 10),         // 极高泛音线
                                            step(3.5, 0.45, 4, 0, 4) });
    bridgeInstrumental.styleTags.addArray({ "classical", "jazz", "pop" });
    bridgeInstrumental.moodTags.addArray({ "mysterious", "romantic", cn(u8"独立"), cn(u8"梦幻") });
    bridgeInstrumental.regionTags.addArray({ "global" });
    bridgeInstrumental.functionFit.addArray({ "T", "SecD", "t" });
    bridgeInstrumental.formFit.addArray({ "bridge" });                      // 锁间奏/桥段
    bridgeInstrumental.familyId = "MandopopBridgeFamily";
    bridgeInstrumental.grooveAffinity = "Straight";
    bridgeInstrumental.bassAnchored = true;
    bridgeInstrumental.complexity = 4;
    bridgeInstrumental.energy = 3;
    patterns.push_back(bridgeInstrumental);

    // 6. 尾奏专设：渐慢自由散板 (针对曲式：尾奏 Outro；标签：伤感、治愈、安静)
    auto outroRubatoSlowing = makePattern("form_outro_rubato_105", cn(u8"尾奏：落幕余音渐慢散板"), "Outro Rubato Dispersal",
                                          ArpeggioDirection::Down, ArpeggioDensity::Quarter, 4.0, // 时值越往后越长，音量越后越弱，模拟渐慢（Ritardando）
                                          { step(0.0, 0.95, 7, 1, 6),          // 右手最高音极其轻柔地向下滚动下滑
                                            step(1.0, 1.45, 4, 0, 4),          // 时值变长，拉开物理呼吸
                                            step(2.5, 1.45, 2, 0, 3),          // 进一步拉长变慢
                                            step(4.0, 3.95, 0, -1, 2, true) }); // 最终静止在极轻的低音主音根音上
    outroRubatoSlowing.styleTags.addArray({ "ambient", "ballad", cn(u8"自然小调") });
    outroRubatoSlowing.moodTags.addArray({ "calm", cn(u8"伤感"), cn(u8"治愈") });
    outroRubatoSlowing.regionTags.addArray({ "global" });
    outroRubatoSlowing.functionFit.addArray({ "T", "t" });
    outroRubatoSlowing.formFit.addArray({ "outro" });                       // 锁尾奏/落幕
    outroRubatoSlowing.familyId = "GuqinSparseFamily";
    outroRubatoSlowing.grooveAffinity = "Late";                             // 极度滞后，形成散板感
    outroRubatoSlowing.bassAnchored = true;
    outroRubatoSlowing.complexity = 1;
    outroRubatoSlowing.energy = 1;
    patterns.push_back(outroRubatoSlowing);

    // 7. 终结式专设：正拍强力全音符正格阻截 (针对作曲法：终止式 Cadence；标签：庄严、蓄力)
    auto cadenceAuthenticStab = makePattern("tech_cadence_stab_106", cn(u8"终止式：铁血正格柱式大合流"), "Authentic Cadence Solid Stab",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 纯物理全音符无琵音分解，全声部在 0.0 拍瞬间重击炸裂
                                            { step(0.0, 3.95, 0, -1, 12, true),  // 全声部铁板一块砸下（柱式和弦最高境界）
                                              step(0.0, 3.95, 2, 0, 10, false), 
                                              step(0.0, 3.95, 4, 0, 10, false), 
                                              step(0.0, 3.95, 7, 1, 12, false) });
    cadenceAuthenticStab.styleTags.addArray({ "classical", "epic", "march" });
    cadenceAuthenticStab.moodTags.addArray({ "epic", cn(u8"庄严"), cn(u8"蓄力") });
    cadenceAuthenticStab.regionTags.addArray({ "global" });
    cadenceAuthenticStab.functionFit.addArray({ "D", "T", "t" });        // 通杀 V-I 正格终止与离调终止式
    cadenceAuthenticStab.formFit.addArray({ "outro", "chorus" });
    cadenceAuthenticStab.familyId = "ChineseOpenFamily";
    cadenceAuthenticStab.grooveAffinity = "Straight";
    cadenceAuthenticStab.bassAnchored = true;
    cadenceAuthenticStab.complexity = 1;                                 // 物理形态极简
    cadenceAuthenticStab.energy = 5;                                     // 但瞬态冲击能量最高
    patterns.push_back(cadenceAuthenticStab);

    // 8. 循环进行专设：无穷动莫比乌斯环 (针对作曲法：循环进行 Loop；标签：快乐、明亮、律动)
    auto infiniteLoopMoebius = makePattern("tech_loop_moebius_107", cn(u8"循环进行：莫比乌斯流线环"), "Infinite Loop Moebius Flow",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 首尾声部音高像素级平滑对齐，无论怎么循环都毫无顿挫感
                                           { step(0.0, 0.45, 0, -1, 8, true),   // 周期性无限波浪
                                             step(0.5, 0.45, 2, 0, 5), 
                                             step(1.0, 0.45, 4, 0, 6), 
                                             step(1.5, 0.45, 7, 1, 8), 
                                             step(2.0, 0.45, 3, 0, 6),          // 高位平滑平铺
                                             step(2.5, 0.45, 4, 0, 5), 
                                             step(3.0, 0.45, 2, 0, 7), 
                                             step(3.5, 0.45, 1, 0, 4) });       // 3.5拍完美回到低位，无缝咬合下一小节0.0拍
    infiniteLoopMoebius.styleTags.addArray({ "pop", "edm", "ballad", cn(u8"大调") });
    infiniteLoopMoebius.moodTags.addArray({ "happy", "hopeful", cn(u8"明亮") });
    infiniteLoopMoebius.regionTags.addArray({ "global" });
    infiniteLoopMoebius.functionFit.addArray({ "T", "S", "D", "t" });
    infiniteLoopMoebius.formFit.addArray({ "verse", "chorus" });
    infiniteLoopMoebius.familyId = "ElectronicArpFamily";
    infiniteLoopMoebius.grooveAffinity = "Straight";
    infiniteLoopMoebius.bassAnchored = true;
    infiniteLoopMoebius.complexity = 3;
    infiniteLoopMoebius.energy = 3;
    patterns.push_back(infiniteLoopMoebius);

    // 9. 模进技法专设：严格阶梯级音程爬行 (针对作曲法：模进 Sequence；标签：紧张、动作)
    auto sequenceStepClimber = makePattern("tech_sequence_climb_108", cn(u8"模进：严格级进音程爬虫"), "Strict Sequence Step Climber",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 音符呈现严格的 [0->1->2->4] 定向阶梯上行，完美激发出模进和弦的级进感
                                           { step(0.0, 0.2, 0, -1, 10, true),   // 音程级进级攀爬
                                             step(0.25, 0.2, 1, 0, 6), 
                                             step(0.5, 0.2, 2, 0, 8), 
                                             step(0.75, 0.2, 4, 1, 10),         // 第一波高点
                                             step(1.0, 0.2, 1, 0, 9, true),     // 整体框架上移一个阶梯
                                             step(1.25, 0.2, 2, 0, 7), 
                                             step(1.5, 0.2, 4, 1, 9), 
                                             step(1.75, 0.2, 7, 1, 12) });      // 第二波更高点
    sequenceStepClimber.styleTags.addArray({ "classical", "epic", cn(u8"和声小调") });
    sequenceStepClimber.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    sequenceStepClimber.regionTags.addArray({ "global" });
    sequenceStepClimber.functionFit.addArray({ "t", "D", "SecD" });      // 完美支撑 681 库里的任何“二级下行”或“四度循环模进”
    sequenceStepClimber.formFit.addArray({ "bridge", "pre-chorus" });
    sequenceStepClimber.familyId = "ElectronicArpFamily";
    sequenceStepClimber.grooveAffinity = "Straight";
    sequenceStepClimber.bassAnchored = true;
    sequenceStepClimber.complexity = 4;
    sequenceStepClimber.energy = 4;
    patterns.push_back(sequenceStepClimber);


    // ========================================================================
    // 新增补充：基于情感色彩深度定制库 (Emotion & Mood Driven Patterns)
    // ========================================================================

    // 1. 极致伤感：雨滴滑落低连线 (针对情感：伤感、悲伤、忧郁；标签：小调、柔美)
    auto moodMelancholyRain = makePattern("mood_melancholy_rain_110", cn(u8"伤感：窗边雨滴滑落低连线"), "Melancholy Weeping Raindrops",
                                          ArpeggioDirection::Down, ArpeggioDensity::Eighth, 4.0, // 4/4 拍全部采用向下滚动的弱奏，模拟眼泪或雨滴滑落
                                          { step(0.0, 1.9, 0, -1, 10, true),   // 沉重的低音铺底
                                            step(0.5, 0.45, 4, 1, -6),         // 极轻的高位声部开始下行（力度大幅衰减）
                                            step(1.0, 0.45, 2, 0, -8), 
                                            step(1.5, 0.45, 1, 0, -10), 
                                            step(2.0, 1.9, 1, -1, 8, true),    // 弱化转位低音
                                            step(2.5, 0.45, 3, 0, -6), 
                                            step(3.0, 0.45, 2, 0, -8), 
                                            step(3.5, 0.45, 0, 0, -12) });     // 结尾消失在空气中
    moodMelancholyRain.styleTags.addArray({ "pop", "ballad", "ambient", cn(u8"自然小调") });
    moodMelancholyRain.moodTags.addArray({ "calm", cn(u8"伤感"), cn(u8"忧郁"), cn(u8"空灵") });
    moodMelancholyRain.regionTags.addArray({ "global" });
    moodMelancholyRain.functionFit.addArray({ "t", "s", "v" });          // 极其契合小调进行
    moodMelancholyRain.formFit.addArray({ "verse", "outro" });
    moodMelancholyRain.familyId = "GuqinSparseFamily";                  // 借用高孤寂留白框架
    moodMelancholyRain.grooveAffinity = "Late";                          // 微弱滞后感
    moodMelancholyRain.bassAnchored = true;
    moodMelancholyRain.complexity = 2;
    moodMelancholyRain.energy = 1;                                      // 极低能级
    patterns.push_back(moodMelancholyRain);

    // 2. 纯净治愈：星光摇篮波浪线 (针对情感：治愈、温暖、梦幻；标签：大调、轻柔)
    auto moodHealingStarlight = makePattern("mood_healing_star_111", cn(u8"治愈：夜空星光闪烁摇篮"), "Pure Healing Starlight Waves",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 正反波浪平滑连接，力度均匀，极具安全感与治愈力
                                            { step(0.0, 0.95, 0, -1, 6, true),   // 极度轻柔的根音支撑
                                              step(0.5, 0.45, 2, 0, 0),          // 环绕型上行
                                              step(1.0, 0.45, 4, 0, 2), 
                                              step(1.5, 0.45, 7, 1, 4),          // 高声部清澈泛音
                                              step(2.0, 0.95, 4, 0, 2),          // 平滑对称下行
                                              step(2.5, 0.45, 2, 0, 0), 
                                              step(3.0, 0.45, 1, 0, -2), 
                                              step(3.5, 0.45, 2, 0, 0) });
    moodHealingStarlight.styleTags.addArray({ "pop", "ballad", cn(u8"大调") });
    moodHealingStarlight.moodTags.addArray({ "warm", "hopeful", "romantic", cn(u8"治愈"), cn(u8"温暖"), cn(u8"梦幻") });
    moodHealingStarlight.regionTags.addArray({ "global" });
    moodHealingStarlight.functionFit.addArray({ "T", "S" });
    moodHealingStarlight.formFit.addArray({ "intro", "verse" });
    moodHealingStarlight.familyId = "CPopBalladPianoFamily";
    moodHealingStarlight.grooveAffinity = "Straight";
    moodHealingStarlight.bassAnchored = true;
    moodHealingStarlight.complexity = 2;
    moodHealingStarlight.energy = 2;
    patterns.push_back(moodHealingStarlight);

    // 3. 欢快元气：跃动阳光切分碎音 (针对情感：欢快、快乐、明亮；标签：大调、16分)
    auto moodHappySunshine = makePattern("mood_happy_sunshine_112", cn(u8"欢快：金色阳光碎步跃动"), "Happy Energetic Sunshine Pulse",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 2拍短循环，高频后半拍大跳切分，洋溢青春元气
                                         { step(0.0, 0.2, 0, -1, 10, true),   // 活泼的正拍点
                                           step(0.25, 0.15, 4, 0, 6),         // 弱拍高声部快速跳跃
                                           step(0.5, 0.15, 2, 0, 4), 
                                           step(0.75, 0.15, 7, 1, 8),         // 高八度阳光音
                                           step(1.0, 0.2, 0, 0, 6, true), 
                                           step(1.25, 0.15, 3, 0, 4), 
                                           step(1.5, 0.35, 4, 0, 8) });       // 强切分结束
    moodHappySunshine.styleTags.addArray({ "pop", "rock", cn(u8"大调") });
    moodHappySunshine.moodTags.addArray({ "energetic", "happy", cn(u8"欢快"), cn(u8"快乐"), cn(u8"明亮") });
    moodHappySunshine.regionTags.addArray({ "global" });
    moodHappySunshine.functionFit.addArray({ "T", "S", "D" });
    moodHappySunshine.formFit.addArray({ "verse", "chorus" });
    moodHappySunshine.familyId = "JayStyleSyncopationFamily";          // 完美驱动快乐切分
    moodHappySunshine.grooveAffinity = "Early";                          // 微弱前冲制造热情
    moodHappySunshine.bassAnchored = true;
    moodHappySunshine.complexity = 3;
    moodHappySunshine.energy = 4;
    patterns.push_back(moodHappySunshine);

    // 4. 极致孤独：旷野冷色空心双音 (针对情感：孤独、寂寞、空灵；标签：小调、挂留)
    auto moodLonelyWilderness = makePattern("mood_lonely_wild_113", cn(u8"孤独：寒冷旷野北极星光"), "Lonely Cold Wilderness Hollow",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 抽离掉和弦中最有情绪倾向的三音（Index 1），只跑纯五度和纯八度空心音
                                            { step(0.0, 3.9, 0, -1, 12, true),   // 超低孤绝地基
                                              step(1.0, 1.9, 4, 0, 2),           // 只有纯五度，不带感情色彩
                                              step(2.5, 1.4, 7, 1, 6) });        // 极高位寂寞呼应
    moodLonelyWilderness.styleTags.addArray({ "ambient", "world", cn(u8"自然小调"), cn(u8"挂留") });
    moodLonelyWilderness.moodTags.addArray({ "calm", cn(u8"孤独"), cn(u8"空灵"), cn(u8"伤感") });
    moodLonelyWilderness.regionTags.addArray({ "global" });
    moodLonelyWilderness.functionFit.addArray({ "t", "v" });
    moodLonelyWilderness.formFit.addArray({ "intro", "outro" });
    moodLonelyWilderness.familyId = "GuqinSparseFamily";
    moodLonelyWilderness.grooveAffinity = "Late";
    moodLonelyWilderness.bassAnchored = true;
    moodLonelyWilderness.complexity = 1;
    moodLonelyWilderness.energy = 1;
    patterns.push_back(moodLonelyWilderness);

    // 5. 愤怒狂躁：重轰鸣锯齿风暴 (针对情感：愤怒、狂躁、紧张；标签：摇滚、极端金属)
    auto moodAngryStorm = makePattern("mood_angry_storm_114", cn(u8"愤怒：黑色焦灼风暴巨浪"), "Angry Furious Metal Chug",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 低音区双音同击（Power Chord）极高流速，力度死咬最大值
                                      { step(0.0, 0.15, 0, -1, 14, true),  // 极限愤怒重击
                                        step(0.0, 0.15, 4, -1, 12, false), // 强力五度同砸
                                        step(0.25, 0.1, 0, -1, 15, true),  // 连续闷音碎击
                                        step(0.5, 0.15, 1, 0, 14),         // 离调摩擦
                                        step(0.75, 0.1, 0, -1, 15, true), 
                                        step(1.0, 0.15, 2, 0, 14), 
                                        step(1.25, 0.1, 0, -1, 13, true), 
                                        step(1.5, 0.35, 0, -1, 15, true) });
    moodAngryStorm.styleTags.addArray({ "rock", "epic", cn(u8"减五度"), cn(u8"和声小调") });
    moodAngryStorm.moodTags.addArray({ "energetic", cn(u8"愤怒"), cn(u8"狂躁"), cn(u8"紧张") }); // 精准闭环极端重型
    moodAngryStorm.regionTags.addArray({ "global" });
    moodAngryStorm.functionFit.addArray({ "t", "D" });
    moodAngryStorm.formFit.addArray({ "chorus", "bridge" });
    moodAngryStorm.familyId = "ElectronicArpFamily";
    moodAngryStorm.grooveAffinity = "Early";                            // 狂躁前冲律动
    moodAngryStorm.bassAnchored = true;
    moodAngryStorm.complexity = 4;
    moodAngryStorm.energy = 5;                                          // 破坏级能级
    patterns.push_back(moodAngryStorm);

    // 6. 迷幻复古：流光霓虹延音 (针对情感：迷幻、微醺、梦幻、浪漫；标签：R&B、九和弦)
    auto moodPsychedelicNeon = makePattern("mood_psychedelic_115", cn(u8"迷幻：霓虹流光微醺延音"), "Psychedelic Neon Dreamy Waves",
                                           ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 经典 Neo-Soul/R&B 极度摇摆且带有微弱拖拍的延伸音流
                                           { step(0.0, 1.4, 0, -1, 8, true), 
                                             step(0.75, 0.45, 4, 0, 4),         // 错位九音
                                             step(1.5, 0.95, 3, 0, 6),          // 高亮浪漫七音
                                             step(2.5, 0.45, 5, 1, 5),          // 挂留色彩
                                             step(3.0, 0.95, 2, 0, 3) });
    moodPsychedelicNeon.styleTags.addArray({ "jazz", "pop", cn(u8"Jazz/爵士"), cn(u8"九和弦"), cn(u8"七和弦") });
    moodPsychedelicNeon.moodTags.addArray({ "romantic", "calm", cn(u8"迷幻"), cn(u8"梦幻") }); // 闭环迷幻/微醺
    moodPsychedelicNeon.regionTags.addArray({ "global" });
    moodPsychedelicNeon.functionFit.addArray({ "T", "t", "SecD" });
    moodPsychedelicNeon.formFit.addArray({ "verse", "bridge" });
    moodPsychedelicNeon.familyId = "JayStyleSyncopationFamily";
    moodPsychedelicNeon.grooveAffinity = "Late";                        // 慵懒拖拍（Layback）
    moodPsychedelicNeon.bassAnchored = true;
    moodPsychedelicNeon.complexity = 4;
    moodPsychedelicNeon.energy = 2;
    patterns.push_back(moodPsychedelicNeon);

    // 7. 史诗壮阔：交响齐奏潮汐巨浪 (针对情感：史诗、震撼、大气、庄严；标签：大调、小调)
    auto moodEpicGrandeur = makePattern("mood_epic_grand_116", cn(u8"史诗：交响齐奏山海巨浪"), "Epic Cinematic Grand Horizon",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 宽广低音滚动与高音区八度齐奏，营造气吞山河的电影长焦画卷
                                        { step(0.0, 0.45, 0, -2, 14, true),  // 超重影院级超低音
                                          step(0.5, 0.2, 0, -1, 12, true), 
                                          step(1.0, 0.2, 4, 0, 10), 
                                          step(1.25, 0.2, 7, 0, 10), 
                                          step(1.5, 0.45, 0, 1, 13),         // 1.5拍强爆发高潮
                                          step(2.0, 0.45, 2, 0, 11), 
                                          step(2.5, 0.2, 4, 1, 12), 
                                          step(3.0, 0.95, 7, 1, 14) });      // 最终冠音撕开天际
    moodEpicGrandeur.styleTags.addArray({ "epic", "march", cn(u8"和声小调") });
    moodEpicGrandeur.moodTags.addArray({ "energetic", cn(u8"史诗"), cn(u8"紧张"), cn(u8"动作"), cn(u8"庄严") }); // 灭活一切大片标签
    moodEpicGrandeur.regionTags.addArray({ "global" });
    moodEpicGrandeur.functionFit.addArray({ "t", "S", "D" });
    moodEpicGrandeur.formFit.addArray({ "chorus", "bridge" });
    moodEpicGrandeur.familyId = "HybridEpicStringFamily";
    moodEpicGrandeur.grooveAffinity = "Straight";
    moodEpicGrandeur.bassAnchored = true;
    moodEpicGrandeur.complexity = 4;
    moodEpicGrandeur.energy = 5;
    patterns.push_back(moodEpicGrandeur);

    // ========================================================================
    // 新增补充：基于调式音程特征深度定制库 (Modal Character Driven Patterns)
    // ========================================================================

    // 1. Dorian (多利亚) 调式：明亮小调线条 (针对预设/调式：Dorian、加勒比海盗；特征音：大六度)
    auto modeDorianBrightMinor = makePattern("mode_dorian_bright_120", cn(u8"Dorian调式：大六度深夜流光"), "Dorian Modal Bright Minor Flow",
                                             ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 核心：在 Index 5 或转位中极力高亮 Dorian 标志性的大六度音
                                             { step(0.0, 0.95, 0, -1, 10, true),  // 沉稳小调地基
                                               step(0.5, 0.45, 2, 0, 4), 
                                               step(1.0, 0.45, 4, 0, 6), 
                                               step(1.5, 0.45, 5, 1, 9),          // 灵魂：高亮大六度音（Index 5），流露出高贵、略带魔幻的明亮小调感
                                               step(2.0, 0.95, 2, 0, 5), 
                                               step(2.5, 0.45, 4, 0, 3), 
                                               step(3.0, 0.95, 0, 0, 7), 
                                               step(3.5, 0.45, 5, 1, 8) });       // 再次强调大六度
    modeDorianBrightMinor.styleTags.addArray({ "pop", "epic", "12/8", "Dorian", cn(u8"Dorian/多利亚") }); // 精准闭环
    modeDorianBrightMinor.moodTags.addArray({ "mysterious", "romantic", cn(u8"梦幻"), cn(u8"独立") });
    modeDorianBrightMinor.regionTags.addArray({ "global" });
    modeDorianBrightMinor.functionFit.addArray({ "t", "S" });             // 完美契合 i - IV（大四级）的 Dorian 经典进行
    modeDorianBrightMinor.formFit.addArray({ "verse", "bridge" });
    modeDorianBrightMinor.familyId = "GuitarPickingFamily";
    modeDorianBrightMinor.grooveAffinity = "Straight";
    modeDorianBrightMinor.bassAnchored = true;
    modeDorianBrightMinor.complexity = 3;
    modeDorianBrightMinor.energy = 3;
    patterns.push_back(modeDorianBrightMinor);

    // 2. Phrygian (弗里吉亚) 调式：弗拉门戈异域重击 (针对预设/调式：Phrygian、间谍；特征音：小二级)
    auto modePhrygianExotic = makePattern("mode_phrygian_heavy_121", cn(u8"Phrygian调式：异域重金属小二级"), "Phrygian Flamenco Heavy Pulse",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 核心：极速下行摩擦小二级音，带来西班牙弗拉门戈或重金属暗黑压迫感
                                          { step(0.0, 0.15, 0, -1, 14, true),  // 强力重击
                                            step(0.25, 0.15, 1, 0, 12),        // 灵魂：瞬间切入 Phrygian 特征半音阶——小二级音（Index 1）
                                            step(0.5, 0.1, 0, -1, 10, true), 
                                            step(0.75, 0.15, 2, 0, 9), 
                                            step(1.0, 0.15, 1, 0, 13),         // 再次剧烈摩擦小二级
                                            step(1.25, 0.1, 0, -1, 8, true), 
                                            step(1.5, 0.35, 4, 0, 11) });
    modePhrygianExotic.styleTags.addArray({ "rock", "epic", "Phrygian", cn(u8"Phrygian/弗里吉亚") });
    modePhrygianExotic.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"愤怒"), cn(u8"动作") });
    modePhrygianExotic.regionTags.addArray({ "global" });
    modePhrygianExotic.functionFit.addArray({ "t", "bII" });            // 绝配 i - bII 的弗里吉亚火焰半音阶进行
    modePhrygianExotic.formFit.addArray({ "chorus", "bridge" });
    modePhrygianExotic.familyId = "ElectronicArpFamily";
    modePhrygianExotic.grooveAffinity = "Early";
    modePhrygianExotic.bassAnchored = true;
    modePhrygianExotic.complexity = 4;
    modePhrygianExotic.energy = 5;
    patterns.push_back(modePhrygianExotic);

    // 3. Lydian (利底亚) 调式：太空悬浮天籁 (针对预设/调式：Lydian、广袤、天空；特征音：增四度)
    auto modeLydianSpace = makePattern("mode_lydian_space_122", cn(u8"Lydian调式：太空悬浮增四度"), "Lydian Cosmic Floating Air",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 核心：高位大时值留白，特意拉长增四度，营造空灵不着地、飞向宇宙的悬浮感
                                       { step(0.0, 3.9, 0, -1, 8, true),    // 地面低音长音
                                         step(1.0, 1.9, 3, 1, 6),           // 灵魂：天空中亮起 Lydian 标志性增四度音（Index 3），产生极强科技/科幻美感
                                         step(2.0, 1.9, 4, 1, 5),           // 平滑推移到五音
                                         step(3.0, 0.9, 7, 2, 7) });        // 极高位泛音闪烁
    modeLydianSpace.styleTags.addArray({ "ambient", "Lydian", cn(u8"Lydian/利底亚") });
    modeLydianSpace.moodTags.addArray({ "calm", "hopeful", cn(u8"空灵"), cn(u8"梦幻") });
    modeLydianSpace.regionTags.addArray({ "global" });
    modeLydianSpace.functionFit.addArray({ "T", "II" });                 // 极度匹配大调 Lydian 经典 I - II 离调大三级进行
    modeLydianSpace.formFit.addArray({ "intro", "outro" });
    modeLydianSpace.familyId = "CinematicStringPadFamily";
    modeLydianSpace.grooveAffinity = "Late";
    modeLydianSpace.bassAnchored = true;
    modeLydianSpace.complexity = 2;
    modeLydianSpace.energy = 1;
    patterns.push_back(modeLydianSpace);

    // 4. Mixolydian (混合利底亚) 调式：复古摇滚赞歌 (针对预设/调式：Mixolydian、蓝调；特征音：小七度)
    auto modeMixolydianRock = makePattern("mode_mixo_rock_123", cn(u8"Mixolydian调式：复古摇滚小七度"), "Mixolydian Retro Rock Hymn",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 核心：前4后8的弹跳律动，高亮小七度音，展现纯正的苏格兰风笛风情或老摇滚律动
                                          { step(0.0, 1.9, 0, -1, 11, true),  // 奔跑的低音
                                            step(0.5, 0.45, 2, 0, 5), 
                                            step(1.0, 0.45, 4, 0, 7), 
                                            step(1.5, 0.45, 6, 1, 10),         // 灵魂：重砸出 Mixolydian 特征音小七度（Index 6），极具叛逆洒脱感
                                            step(2.0, 0.45, 4, 0, 6), 
                                            step(2.5, 0.45, 2, 0, 4), 
                                            step(3.0, 0.45, 1, 0, 5), 
                                            step(3.5, 0.45, 6, 0, 8) });       // 小七度中位收尾
    modeMixolydianRock.styleTags.addArray({ "rock", "pop", "blues", "Mixolydian", cn(u8"Mixolydian/混合利底亚") });
    modeMixolydianRock.moodTags.addArray({ "energetic", "happy", cn(u8"明亮") });
    modeMixolydianRock.regionTags.addArray({ "global" });
    modeMixolydianRock.functionFit.addArray({ "T", "bVII" });            // 通杀大调 I - bVII（降七级大和弦）进行
    modeMixolydianRock.formFit.addArray({ "verse", "chorus" });
    modeMixolydianRock.familyId = "JayStyleSyncopationFamily";
    modeMixolydianRock.grooveAffinity = "Straight";
    modeMixolydianRock.bassAnchored = true;
    modeMixolydianRock.complexity = 3;
    modeMixolydianRock.energy = 4;
    patterns.push_back(modeMixolydianRock);

    // 5. Locrian (洛克里亚) 调式：深海极度深渊 (针对预设/调式：Locrian、极端金属；特征音：减五度)
    auto modeLocrianAbyss = makePattern("mode_locrian_abyss_124", cn(u8"Locrian调式：减五度深海极渊"), "Locrian Dark Desolate Abyss",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 核心：极度不安稳的调式，死咬减五度音不放，营造近乎窒息的灾难绝望感
                                        { step(0.0, 0.45, 0, -2, 12, true),  // 超沉地狱低音
                                          step(0.5, 0.15, 4, -1, 11),        // 灵魂：强行凸显 Locrian 的核心痛点——减五度音（Index 4）
                                          step(0.75, 0.15, 1, -1, 10),       // 摩擦小二级音
                                          step(1.0, 0.45, 0, -1, 9, true), 
                                          step(1.5, 0.15, 4, 0, 11),         // 再次凄厉撕扯减五度
                                          step(1.75, 0.15, 2, -1, 8) });
    modeLocrianAbyss.styleTags.addArray({ "rock", "epic", "Locrian", cn(u8"Locrian/洛克里亚"), cn(u8"减五度") });
    modeLocrianAbyss.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"狂躁"), cn(u8"伤感") });
    modeLocrianAbyss.regionTags.addArray({ "global" });
    modeLocrianAbyss.functionFit.addArray({ "t", "D" });
    modeLocrianAbyss.formFit.addArray({ "intro", "bridge" });
    modeLocrianAbyss.familyId = "ElectronicArpFamily";
    modeLocrianAbyss.grooveAffinity = "Straight";
    modeLocrianAbyss.bassAnchored = true;
    modeLocrianAbyss.complexity = 4;
    modeLocrianAbyss.energy = 4;
    patterns.push_back(modeLocrianAbyss);

    // 6. 和声小调 (Harmonic Minor)：新古典埃及艳后 (针对预设/调式：和声小调、黑暗骑士；特征音：大七度)
    auto modeHarmonicMinorEpic = makePattern("mode_harmonic_minor_125", cn(u8"和声小调：新古典大七度华彩"), "Harmonic Minor Neo-Classical Run",
                                             ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0, // 核心：在小调框架里突然向上剧烈级进攀爬到大七度，绽放浓郁的中东、神秘埃及或古典交响风情
                                             { step(0.0, 0.45, 0, -1, 12, true),  // 根音重砸
                                               step(0.5, 0.15, 1, 0, 7), 
                                               step(0.75, 0.15, 2, 0, 8), 
                                               step(1.0, 0.45, 4, 0, 9), 
                                               step(1.5, 0.15, 6, 1, 13),         // 灵魂核心：突然飙向极强、极具尖锐摩擦感的导音/大七度音（Index 6）
                                               step(1.75, 0.15, 2, 0, 8), 
                                               step(2.0, 0.45, 0, 0, 10, true),   // 周期中点转位
                                               step(2.5, 0.15, 1, 0, 8), 
                                               step(2.75, 0.15, 4, 1, 9), 
                                               step(3.0, 0.15, 6, 1, 14),         // 再次用大七度锋利割裂
                                               step(3.25, 0.15, 7, 2, 11), 
                                               step(3.5, 0.45, 4, 0, 7) });
    modeHarmonicMinorEpic.styleTags.addArray({ "epic", "rock", "classical", cn(u8"和声小调") });
    modeHarmonicMinorEpic.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    modeHarmonicMinorEpic.regionTags.addArray({ "global" });
    modeHarmonicMinorEpic.functionFit.addArray({ "t", "D" });             // 完美契合小调 i - V7（大三和弦带导音）的终极交响撕裂进行
    modeHarmonicMinorEpic.formFit.addArray({ "chorus", "bridge" });
    modeHarmonicMinorEpic.familyId = "HybridEpicStringFamily";
    modeHarmonicMinorEpic.grooveAffinity = "Straight";
    modeHarmonicMinorEpic.bassAnchored = true;
    modeHarmonicMinorEpic.complexity = 5;                                 // 音程流极其陡峭
    modeHarmonicMinorEpic.energy = 5;
    patterns.push_back(modeHarmonicMinorEpic);

    // 7. 中国传统角调式 (Jue Mode)：江南春水烟雨 (针对预设/调式：角调、大漠花儿；特征音：纯五度/清乐)
    auto modeChineseJueFlow = makePattern("mode_chinese_jue_126", cn(u8"角调式：江南烟雨春水流线"), "Chinese Jue Mode Elegant Stream",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 核心：严格遵循五声角调式骨架，剔除半音摩擦，线条连绵如国画流水
                                          { step(0.0, 1.9, 0, -1, 8, true),    // 抱笙长低音托底
                                            step(0.5, 0.45, 4, 0, 3),          // 纯五度清澈流动
                                            step(1.0, 0.9, 2, 0, 5), 
                                            step(1.5, 0.45, 7, 1, 6),          // 高位泛音跃动
                                            step(2.0, 1.9, 0, 0, 7, true), 
                                            step(2.5, 0.45, 4, 0, 4), 
                                            step(3.0, 0.9, 1, 0, 5),           // 空灵游走
                                            step(3.5, 0.45, 2, 0, 3) });
    modeChineseJueFlow.styleTags.addArray({ "traditional", "ballad", cn(u8"角调"), cn(u8"清乐") });
    modeChineseJueFlow.moodTags.addArray({ "calm", cn(u8"柔美"), cn(u8"温婉"), cn(u8"空灵") });
    modeChineseJueFlow.regionTags.addArray({ "chinese" });                // 完美匹配国风大类
    modeChineseJueFlow.functionFit.addArray({ "t", "s" });
    modeChineseJueFlow.formFit.addArray({ "intro", "verse" });
    modeChineseJueFlow.familyId = "GuofengFlowFamily";
    modeChineseJueFlow.grooveAffinity = "Late";                          // 带有微弱的东方呼吸感
    modeChineseJueFlow.bassAnchored = true;
    modeChineseJueFlow.complexity = 2;
    modeChineseJueFlow.energy = 2;
    patterns.push_back(modeChineseJueFlow);

    // 左右手双音交替，极其肥厚温暖，适合富有空气感的电钢琴/吉他
    auto textureDyadSway = makePattern("text_dyad_sway_130", cn(u8"织体：Neo-Soul双音错位摇摆"), "Neo-Soul Dyad Layback",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                       { step(0.0, 1.9, 0, -1, 6, true),    // 根音
                                         step(0.5, 0.45, 2, 0, 2),          // 单音过渡
                                         step(1.0, 0.95, 4, 0, 4, false),   // 双音同击核心（三音+五音）
                                         step(1.0, 0.95, 5, 1, 4, false), 
                                         step(2.0, 0.45, 1, 0, 1), 
                                         step(2.5, 0.95, 3, 0, 5, false),   // 双音高位切分
                                         step(2.5, 0.95, 6, 1, 5, false), 
                                         step(3.5, 0.45, 2, 0, 2) });
    textureDyadSway.styleTags.addArray({ "pop", "jazz", "r&b", cn(u8"九和弦"), cn(u8"七和弦") });
    textureDyadSway.moodTags.addArray({ "romantic", cn(u8"迷幻"), cn(u8"温暖") });
    textureDyadSway.familyId = "JayStyleSyncopationFamily";
    textureDyadSway.grooveAffinity = "Late";
    patterns.push_back(textureDyadSway);


    // 模拟乐器真机滚奏（Sweep/Roll），在 0.1 拍内极速将和弦内音依次滑过，产生晶莹剔透的水花感
    auto textureSweepCascade = makePattern("text_sweep_cascade_131", cn(u8"织体：古筝流水涟漪滚奏"), "Guzheng Cascade Sweeping Roll",
                                           ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 2.0,
                                           { step(0.0, 1.9, 0, -1, 10, true),   // 正拍根音
                                             step(0.5, 0.1, 1, 0, 4),           // 极速滚奏点 1
                                             step(0.6, 0.1, 2, 0, 5),           // 极速滚奏点 2
                                             step(0.7, 0.1, 4, 1, 6),           // 极速滚奏点 3
                                             step(0.8, 1.1, 7, 1, 8),           // 滚奏落点冠音延音
                                             step(1.5, 0.45, 4, 0, 3) });
    textureSweepCascade.styleTags.addArray({ "traditional", "folk", "ballad", cn(u8"清乐") });
    textureSweepCascade.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"柔美") });
    textureSweepCascade.regionTags.addArray({ "chinese" });
    textureSweepCascade.familyId = "GuzhengRollFamily";
    patterns.push_back(textureSweepCascade);


    // 严格遵循 2:1 的前长后短 Swing 物理时值，完美咬合 City-Pop 或标准 12 小节蓝调预设
    auto grooveSwingCity = makePattern("groove_swing_city_132", cn(u8"律动：CityPop摩登微醺摇摆"), "City-Pop Modern Swing Groovin",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                       { step(0.0, 0.66, 0, -1, 9, true),   // Swing 前半拍（占 2/3 时值）
                                         step(0.66, 0.33, 2, 0, 3),          // Swing 后半拍（占 1/3 时值）
                                         step(1.0, 0.66, 4, 0, 6), 
                                         step(1.66, 0.33, 1, 0, 4), 
                                         step(2.0, 0.66, 7, 1, 8), 
                                         step(2.66, 0.33, 3, 0, 5), 
                                         step(3.0, 0.66, 4, 0, 7), 
                                         step(3.66, 0.33, 2, 0, 4) });
    grooveSwingCity.styleTags.addArray({ "pop", "blues", "jazz", "Mixolydian", cn(u8"Mixolydian/混合利底亚") });
    grooveSwingCity.moodTags.addArray({ "happy", cn(u8"迷幻"), cn(u8"明亮") });
    grooveSwingCity.familyId = "JayStyleSyncopationFamily";
    grooveSwingCity.grooveAffinity = "Late"; // 产生慵懒的微醺推背感
    patterns.push_back(grooveSwingCity);


    // 模拟现代电子音乐或高能八音盒中突发的 16 分音符三连音/碎音，打破线性无聊感
    auto grooveTrapHiHat = makePattern("groove_trap_burst_133", cn(u8"律动：赛博国风极限电子碎音"), "Cyber Guofeng Trap Grid Burst",
                                       ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 2.0,
                                       { step(0.0, 0.45, 0, -1, 12, true),   // 正拍大重击
                                         step(0.5, 0.22, 2, 0, 6), 
                                         step(0.75, 0.22, 4, 0, 7), 
                                         step(1.0, 0.11, 1, 0, 8),          // 后半周期突发极速机枪轰炸
                                         step(1.11, 0.11, 2, 0, 9), 
                                         step(1.22, 0.11, 4, 1, 10), 
                                         step(1.33, 0.11, 6, 1, 11), 
                                         step(1.44, 0.11, 7, 1, 12),
                                         step(1.5, 0.45, 0, 0, 8) });
    grooveTrapHiHat.styleTags.addArray({ "edm", "pop", cn(u8"九和弦"), cn(u8"十一和弦") });
    grooveTrapHiHat.moodTags.addArray({ "energetic", cn(u8"紧张"), cn(u8"动作") });
    grooveTrapHiHat.familyId = "ElectronicArpFamily";
    grooveTrapHiHat.grooveAffinity = "Early"; // 情绪前冲
    patterns.push_back(grooveTrapHiHat);


    // 模拟马头琴草原长调的长音与马蹄奔腾的切分音相结合（前十六后八 / 连续切分）
    auto regionMongolianGallop = makePattern("reg_mongolian_gallop_134", cn(u8"大草原：马头琴狂飙马蹄疾"), "Mongolian Grassland Galloping Horse",
                                             ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0,
                                             { step(0.0, 0.45, 0, -1, 11, true),  // 辽阔长音底盘
                                               step(0.5, 0.2, 4, 0, 6),           // 马蹄碎步切分启动（哒哒 哒）
                                               step(0.66, 0.15, 2, 0, 4), 
                                               step(0.8, 0.15, 1, 0, 5), 
                                               step(1.0, 0.3, 0, 0, 9, true),     // 正拍马蹄重音
                                               step(1.33, 0.15, 5, 1, 7), 
                                               step(1.5, 0.45, 4, 0, 6) });
    regionMongolianGallop.styleTags.addArray({ "world", "folk", cn(u8"自然小调") });
    regionMongolianGallop.moodTags.addArray({ "energetic", cn(u8"庄严"), cn(u8"广袤") });
    regionMongolianGallop.regionTags.addArray({ "chinese", cn(u8"蒙古"), cn(u8"蒙古族") }); // 闭环锁定
    regionMongolianGallop.familyId = "GuitarPickingFamily"; // 借用拨弦
    patterns.push_back(regionMongolianGallop);


    // 极致纯正、充满时代眼泪的台湾民谣吉他经典“T1213121”指弹分解骨架
    auto regionTaiwanFolk = makePattern("reg_taiwan_folk_135", cn(u8"经典：台湾校园民谣吉他指弹"), "Taiwan Campus Folk Classic Picking",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0,
                                        { step(0.0, 0.45, 0, -1, 8, true),  // T (Bass)
                                          step(0.5, 0.45, 2, 0, 2),          // 1
                                          step(1.0, 0.45, 4, 0, 4),          // 2
                                          step(1.5, 0.45, 2, 0, 1),          // 1
                                          step(2.0, 0.45, 7, 1, 6),          // 3 (高音冠音)
                                          step(2.5, 0.45, 2, 0, 2),          // 1
                                          step(3.0, 0.45, 4, 0, 3),          // 2
                                          step(3.5, 0.45, 2, 0, 1) });       // 1
    regionTaiwanFolk.styleTags.addArray({ "folk", "pop", "ballad" });
    regionTaiwanFolk.moodTags.addArray({ "warm", "calm", cn(u8"温暖"), cn(u8"治愈"), cn(u8"伤感") });
    regionTaiwanFolk.regionTags.addArray({ "chinese", cn(u8"台湾") });
    regionTaiwanFolk.familyId = "CampusFolkFamily";
    patterns.push_back(regionTaiwanFolk);


    // 当遇到 III7 或 VI7 等强烈具有撕裂感的副属和弦瞬间，该范式会强行在高位悬停、颤音，将内含的离调半音（变化音）推向顶峰
    auto tensionSecondaryDom = makePattern("tension_secdom_run_136", cn(u8"和声：离调副属和弦颤音华彩"), "Secondary Dominant Tension Flare",
                                           ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0,
                                           { step(0.0, 0.95, 0, -1, 10, true),  // 重根音
                                             step(1.0, 0.2, 2, 0, 4), 
                                             step(1.25, 0.2, 4, 0, 6), 
                                             step(1.5, 0.45, 6, 1, 9),          // 核心变化音位置：极度拉高、强奏
                                             step(2.0, 0.2, 7, 1, 11),          // 七音剧烈摩擦
                                             step(2.25, 0.2, 6, 1, 8), 
                                             step(2.5, 0.2, 4, 0, 7), 
                                             step(2.75, 1.2, 2, 0, 6) });       // 悬浮等待解决
    tensionSecondaryDom.styleTags.addArray({ "classical", "jazz", "epic", cn(u8"七和弦") });
    tensionSecondaryDom.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"蓄力") });
    tensionSecondaryDom.functionFit.addArray({ "SecD", "D" }); // 强力咬合副属和弦
    tensionSecondaryDom.familyId = "ChineseOpenFamily";
    patterns.push_back(tensionSecondaryDom);


    //新增
    // ========================================================================
    // 新增补充：基础通用类核心矩阵库 (Essential & Universal Foundation Patterns)
    // ========================================================================

    // 1. 经典大线条：标准开放式华彩 (百搭基础：通杀绝大多数流行、民谣、缓拍音乐)
    auto genUniversalOpenFlow = makePattern("gen_univ_open_100", cn(u8"通用：标准流行开放式八度流线"), "Universal Standard Open Voicing Flow",
                                            ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4/4 拍最稳固的1-5-8-9级数向上跨越，再优雅下行
                                            { step(0.0, 0.95, 0, -1, 10, true),  // 稳固的根音地基
                                              step(0.5, 0.45, 4, 0, 2),          // 五音过渡
                                              step(1.0, 0.45, 0, 0, 4),          // 八度纯净音
                                              step(1.5, 0.45, 1, 0, 6),          // 九音点缀（高级流行感来源）
                                              step(2.0, 0.95, 2, 0, 5, true),    // 转位中继支撑
                                              step(2.5, 0.45, 4, 0, 3), 
                                              step(3.0, 0.95, 0, 0, 4), 
                                              step(3.5, 0.45, 1, 0, 2) });
    genUniversalOpenFlow.styleTags.addArray({ "pop", "ballad", "folk", "c-pop" });
    genUniversalOpenFlow.moodTags.addArray({ "warm", "calm", "hopeful", cn(u8"温暖"), cn(u8"治愈") });
    genUniversalOpenFlow.regionTags.addArray({ "global" });
    genUniversalOpenFlow.functionFit.addArray({ "T", "t", "S", "s" });   // 真正意义上的全功能通杀
    genUniversalOpenFlow.formFit.addArray({ "verse", "chorus" });
    genUniversalOpenFlow.familyId = "CPopBalladPianoFamily";
    genUniversalOpenFlow.grooveAffinity = "Straight";
    genUniversalOpenFlow.bassAnchored = true;
    genUniversalOpenFlow.complexity = 2;
    genUniversalOpenFlow.energy = 2;
    patterns.push_back(genUniversalOpenFlow);

    // 2. 现代律动：16分音符交错动力 (百搭基础：为中快歌、R&B或轻快流行注入绵密律动)
    auto genUniversalSixteenthDrive = makePattern("gen_univ_drive_101", cn(u8"通用：16分音符交错推进线条"), "Universal 16th Cross-Rhythm Drive",
                                                 ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 2拍高频短循环，通过音高交错产生前推力
                                                 { step(0.0, 0.2, 0, -1, 11, true),   // 正拍重音
                                                   step(0.25, 0.15, 2, 0, 4), 
                                                   step(0.5, 0.15, 4, 0, 6), 
                                                   step(0.75, 0.15, 2, 0, 3), 
                                                   step(1.0, 0.2, 1, 0, 8, false),    // 错位中位音
                                                   step(1.25, 0.15, 4, 0, 5), 
                                                   step(1.5, 0.35, 7, 1, 9) });       // 冠音收尾
    genUniversalSixteenthDrive.styleTags.addArray({ "pop", "r&b", "rock", "c-pop" });
    genUniversalSixteenthDrive.moodTags.addArray({ "energetic", cn(u8"明亮"), cn(u8"欢快") });
    genUniversalSixteenthDrive.regionTags.addArray({ "global" });
    genUniversalSixteenthDrive.functionFit.addArray({ "T", "S", "D" });
    genUniversalSixteenthDrive.formFit.addArray({ "chorus", "pre-chorus" });
    genUniversalSixteenthDrive.familyId = "CPopChorusDriveFamily";
    genUniversalSixteenthDrive.grooveAffinity = "Straight";
    genUniversalSixteenthDrive.bassAnchored = true;
    genUniversalSixteenthDrive.complexity = 3;
    genUniversalSixteenthDrive.energy = 4;
    patterns.push_back(genUniversalSixteenthDrive);

    // 3. 空灵留白：四分音符长延音垫子 (百搭基础：通杀所有前奏 Intro、尾奏 Outro、或影视氛围 Ambient)
    auto genUniversalQuarterPad = makePattern("gen_univ_pad_102", cn(u8"通用：四分音符极简空灵留白"), "Universal Quarter Note Minimal Pad",
                                              ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 每拍一个音，大时值，给乐曲提供呼吸和环境感
                                              { step(0.0, 3.9, 0, -1, 8, true),    // 根音超长延音
                                                step(1.0, 0.9, 4, 0, 4),           // 纯净五音
                                                step(2.0, 1.9, 2, 0, 5),           // 三音在中轴稳固色彩
                                                step(3.0, 0.9, 7, 1, 6) });        // 八度微光
    genUniversalQuarterPad.styleTags.addArray({ "ambient", "ballad", "folk" });
    genUniversalQuarterPad.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"孤独"), cn(u8"治愈") });
    genUniversalQuarterPad.regionTags.addArray({ "global" });
    genUniversalQuarterPad.functionFit.addArray({ "T", "t", "v" });
    genUniversalQuarterPad.formFit.addArray({ "intro", "outro" });
    genUniversalQuarterPad.familyId = "GuqinSparseFamily"; // 借用高留白底盘
    genUniversalQuarterPad.pedalFriendly = true;
    genUniversalQuarterPad.complexity = 1;
    genUniversalQuarterPad.energy = 1;
    patterns.push_back(genUniversalQuarterPad);

    // 4. 叙事呼吸：标准三连音摇摆 (百搭基础：通杀所有 6/8 拍或 12/8 拍的长线抒情预设)
    auto genUniversalTripletSway = makePattern("gen_univ_triplet_103", cn(u8"通用：6/8拍叙事复合三连音"), "Universal 6/8 Triplet Narrative Sway",
                                               ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 3.0, // 3拍（半小节）一循环的标准三连音，具有极强的荡漾感
                                               { step(0.0, 0.95, 0, -1, 10, true),  // 重心
                                                 step(0.5, 0.45, 2, 0, 4), 
                                                 step(1.0, 0.45, 4, 0, 5), 
                                                 step(1.5, 0.95, 7, 1, 7),          // 高位冠音荡回
                                                 step(2.0, 0.45, 4, 0, 4), 
                                                 step(2.5, 0.45, 2, 0, 3) });
    genUniversalTripletSway.styleTags.addArray({ "folk", "ballad", "12/8", "c-pop" });
    genUniversalTripletSway.moodTags.addArray({ "warm", "romantic", cn(u8"柔美"), cn(u8"温婉") });
    genUniversalTripletSway.regionTags.addArray({ "global" });
    genUniversalTripletSway.functionFit.addArray({ "T", "t", "S" });
    genUniversalTripletSway.formFit.addArray({ "verse", "bridge" });
    genUniversalTripletSway.familyId = "GuitarPickingFamily";
    genUniversalTripletSway.complexity = 2;
    genUniversalTripletSway.energy = 2;
    patterns.push_back(genUniversalTripletSway);

    // 5. 律动切分：后半拍错位跃动 (百搭基础：为轻快、快乐、元气类流行歌提供核心律动)
    auto genUniversalSyncopation = makePattern("gen_univ_sync_104", cn(u8"通用：经典后半拍切分跃动"), "Universal Classic Syncopated Pulse",
                                               ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0, // 2拍循环，大量音符咬在后半拍（0.5, 1.5），制造轻盈踩空感
                                               { step(0.0, 0.45, 0, -1, 10, true),  // 正拍根音
                                                 step(0.5, 0.45, 4, 0, 7),          // 强切分后半拍
                                                 step(1.0, 0.45, 2, 0, 5),          // 错位内音
                                                 step(1.5, 0.45, 7, 1, 8) });       // 强切分高音点缀
    genUniversalSyncopation.styleTags.addArray({ "pop", "rock", "c-pop" });
    genUniversalSyncopation.moodTags.addArray({ "happy", "energetic", cn(u8"欢快"), cn(u8"明亮") });
    genUniversalSyncopation.regionTags.addArray({ "global" });
    genUniversalSyncopation.functionFit.addArray({ "T", "S", "D" });
    genUniversalSyncopation.formFit.addArray({ "verse", "chorus" });
    genUniversalSyncopation.familyId = "JayStyleSyncopationFamily";
    genUniversalSyncopation.grooveAffinity = "Early"; // 微弱前冲
    genUniversalSyncopation.complexity = 2;
    genUniversalSyncopation.energy = 3;
    patterns.push_back(genUniversalSyncopation);

    // 6. 纵向骨架：柱式与分解交织 (百搭基础：针对钢琴、吉他等需要乐器感极强的经典声部编织)
    auto genUniversalPolyBlock = makePattern("gen_univ_block_105", cn(u8"通用：交响柱式与单音交织"), "Universal Polyphonic Block & Line",
                                             ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 正拍单音，弱拍突发“双音同击”柱式，声部极丰满
                                             { step(0.0, 0.95, 0, -1, 11, true),  // 重砸地基
                                               step(1.0, 0.45, 2, 0, 5, false),   // 双音同击柱式和小和弦
                                               step(1.0, 0.45, 4, 0, 5, false), 
                                               step(1.5, 0.45, 7, 1, 7),          // 飘出一个单音
                                               step(2.0, 0.95, 1, 0, 8, true),    // 中轴转位
                                               step(3.0, 0.45, 4, 0, 5, false),   // 后半段柱式交替
                                               step(3.0, 0.45, 5, 1, 5, false), 
                                               step(3.5, 0.45, 2, 0, 6) });
    genUniversalPolyBlock.styleTags.addArray({ "pop", "ballad", "epic" });
    genUniversalPolyBlock.moodTags.addArray({ "warm", "romantic", cn(u8"庄严"), cn(u8"大气") });
    genUniversalPolyBlock.regionTags.addArray({ "global" });
    genUniversalPolyBlock.functionFit.addArray({ "T", "t", "S", "D" });
    genUniversalPolyBlock.formFit.addArray({ "chorus", "pre-chorus" });
    genUniversalPolyBlock.familyId = "CPopBalladPianoFamily";
    genUniversalPolyBlock.complexity = 3;
    genUniversalPolyBlock.energy = 3;
    patterns.push_back(genUniversalPolyBlock);

    // 7. 古典色彩：巴洛克16分连绵级进 (百搭基础：针对任何带有学院派、新古典、复古或流畅跑音需求的预设)
    auto genUniversalBaroqueRun = makePattern("gen_univ_baroque_106", cn(u8"通用：巴洛克新古典连绵级进"), "Universal Baroque Neo-Classical Run",
                                              ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0, // 4拍连绵不绝的全16分跑调，犹如无穷动，极具华丽编曲感
                                              { step(0.0, 0.2, 0, -1, 10, true), 
                                                step(0.25, 0.15, 2, 0, 5), 
                                                step(0.5, 0.15, 4, 0, 5), 
                                                step(0.75, 0.15, 7, 1, 6), 
                                                step(1.0, 0.2, 2, 0, 7, true), 
                                                step(1.25, 0.15, 4, 0, 6), 
                                                step(1.5, 0.15, 7, 1, 6), 
                                                step(1.75, 0.15, 1, 1, 8), 
                                                step(2.0, 0.2, 4, 0, 7, true), 
                                                step(2.25, 0.15, 7, 1, 6), 
                                                step(2.5, 0.15, 1, 1, 7), 
                                                step(2.75, 0.15, 2, 1, 8),
                                                step(3.0, 0.2, 7, 1, 9), 
                                                step(3.25, 0.15, 4, 0, 6), 
                                                step(3.5, 0.15, 2, 0, 6), 
                                                step(3.75, 0.15, 1, 0, 5) });
    genUniversalBaroqueRun.styleTags.addArray({ "classical", "pop", "epic" });
    genUniversalBaroqueRun.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"动作") });
    genUniversalBaroqueRun.regionTags.addArray({ "global" });
    genUniversalBaroqueRun.functionFit.addArray({ "T", "t", "D" });
    genUniversalBaroqueRun.formFit.addArray({ "chorus", "bridge" });
    genUniversalBaroqueRun.familyId = "ElectronicArpFamily"; // 极度适合高流速电音或古典弦乐
    genUniversalBaroqueRun.complexity = 4;
    genUniversalBaroqueRun.energy = 4;
    patterns.push_back(genUniversalBaroqueRun);


    // ========================================================================
    // 新增补充：高级氛围与长音铺底类核心库 (Cinematic Ambient & Pad Patterns)
    // ========================================================================

    // 1. 潮汐涌动：影院级渐进铺底 (针对标签：大气、史诗、电影配乐、前奏；特征：力度平滑渐强)
    auto ambTidalSwell = makePattern("amb_tidal_swell_140", cn(u8"氛围：影院交响潮汐渐进铺底"), "Cinematic Tidal Swell Pad",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 4拍长线，后半拍声部交错切入，模拟潮汐涌动
                                     { step(0.0, 3.9, 0, -2, -5, true),   // 极深邃的超低音开场（弱奏起）
                                       step(0.5, 3.4, 4, 0, 0),           // 0.5拍悄然浮现的纯五度
                                       step(1.5, 2.4, 2, 0, 5),           // 1.5拍柔和加入的三音，完成和声色彩确立
                                       step(2.0, 1.9, 7, 1, 10) });        // 2.0拍推向高位冠音，力度最强，形成微型情绪浪潮
    ambTidalSwell.styleTags.addArray({ "ambient", "epic", "pop", "ballad" });
    ambTidalSwell.moodTags.addArray({ "calm", cn(u8"大气"), cn(u8"史诗"), cn(u8"震撼"), cn(u8"梦幻") });
    ambTidalSwell.regionTags.addArray({ "global" });
    ambTidalSwell.functionFit.addArray({ "T", "t", "S", "s" });
    ambTidalSwell.formFit.addArray({ "intro", "outro", "verse" });
    ambTidalSwell.familyId = "CinematicStringPadFamily";
    ambTidalSwell.pedalFriendly = true;
    ambTidalSwell.complexity = 1;
    ambTidalSwell.energy = 2;
    patterns.push_back(ambTidalSwell);

    // 2. 极寒旷野：高空冰晶错位延音 (针对标签：暗黑、冷色调、孤独、空灵、无人区)
    auto ambIceCrystal = makePattern("amb_ice_crystal_141", cn(u8"氛围：极寒旷野冰晶错位延音"), "Glacial Ice Crystal Long Suspension",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0, // 2拍大时值步进，音符时值故意相互重叠错开，极具冷冽的空间感
                                     { step(0.0, 3.9, 0, -1, 4, true),    // 冰冷低音铺底
                                       step(0.0, 1.9, 7, 1, 8),           // 正拍高位纯八度冰晶
                                       step(1.0, 2.9, 1, 1, 6),           // 1拍突入的“挂留九音”长拉弦，产生清冷摩擦
                                       step(2.5, 1.4, 4, 1, 4) });        // 2.5拍下行至五音解决
    ambIceCrystal.styleTags.addArray({ "ambient", cn(u8"挂留"), cn(u8"自然小调") });
    ambIceCrystal.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"孤独"), cn(u8"伤感") });
    ambIceCrystal.regionTags.addArray({ "global" });
    ambIceCrystal.functionFit.addArray({ "t", "v" });
    ambIceCrystal.formFit.addArray({ "intro", "outro" });
    ambIceCrystal.familyId = "GuqinSparseFamily"; // 极高留白
    ambIceCrystal.complexity = 2;
    ambIceCrystal.energy = 1;
    patterns.push_back(ambIceCrystal);

    // 3. 圣洁微光：极光折射泛音流 (针对标签：治愈、温暖、梦幻、天籁、新世纪)
    auto ambAuroraGlow = makePattern("amb_aurora_glow_142", cn(u8"氛围：北极光之璀璨折射微光"), "Holy Aurora Shimmering Glow",
                                     ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 8分音符极度稀疏，力度降至极限，专为营造神圣不着地的缥缈感
                                     { step(0.0, 3.9, 0, -2, -10, true),  // 超低空长音托底，若隐若现
                                       step(1.0, 0.9, 2, 1, -12),         // 高八度晶莹内音闪烁
                                       step(1.5, 0.9, 4, 1, -15), 
                                       step(2.5, 0.9, 7, 2, -10),         // 飙向极高位天空泛音（Octave +2）
                                       step(3.0, 0.9, 1, 2, -14) });       // 悬浮色彩解决
    ambAuroraGlow.styleTags.addArray({ "ambient", "ballad", cn(u8"大调") });
    ambAuroraGlow.moodTags.addArray({ "warm", "hopeful", cn(u8"治愈"), cn(u8"温暖"), cn(u8"梦幻") });
    ambAuroraGlow.regionTags.addArray({ "global" });
    ambAuroraGlow.functionFit.addArray({ "T", "S" });
    ambAuroraGlow.formFit.addArray({ "intro", "verse" });
    ambAuroraGlow.familyId = "CinematicStringPadFamily";
    ambAuroraGlow.pedalFriendly = true;
    ambAuroraGlow.complexity = 2;
    ambAuroraGlow.energy = 1;
    patterns.push_back(ambAuroraGlow);

    // 4. 赛博霓虹：脉冲冥想延迟线 (针对标签：电子、暗黑、紧张、迷幻、现代合成器)
    auto ambCyberPulse = makePattern("amb_cyber_pulse_143", cn(u8"氛围：赛博朋克冥想脉冲长音"), "Cyberpunk Neon Meditative Pulse",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0, // 2拍短循环，模拟合成器 LFO 缓慢调制低通滤波器的律动铺底
                                     { step(0.0, 1.9, 0, -1, 12, true),   // 沉重脉冲基音
                                       step(0.25, 0.25, 4, -1, 6),        // 快速低位五度咬合
                                       step(0.75, 1.2, 3, 0, 10),         // 色彩音平滑切入并延长
                                       step(1.5, 0.4, 0, 0, 8) });
    ambCyberPulse.styleTags.addArray({ "edm", "rock", "ambient" });
    ambCyberPulse.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"迷幻"), cn(u8"狂躁") });
    ambCyberPulse.regionTags.addArray({ "global" });
    ambCyberPulse.functionFit.addArray({ "t", "D", "SecD" });
    ambCyberPulse.formFit.addArray({ "verse", "bridge" });
    ambCyberPulse.familyId = "ElectronicArpFamily"; // 极度适合电子脉冲
    ambCyberPulse.complexity = 3;
    ambCyberPulse.energy = 3;
    patterns.push_back(ambCyberPulse);

    // 5. 国风禅意：空谷残音高孤寂 (针对标签：古筝、琵琶、古琴、甘肃、青海、青绿山水)
    auto ambZenVoid = makePattern("amb_zen_void_144", cn(u8"氛围：空谷禅意古琴残音留白"), "Zen Void Oriental Solitary Soundscape",
                                  ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 极度极简，音符绝不连着弹，每一声都留有巨大的虚无衰减期
                                  { step(0.0, 3.9, 0, -1, 6, true),    // 散音深沉地基
                                    step(1.5, 2.4, 4, 0, 2),           // 1.5拍才极其缓慢地飘出一声五音（泛音效果）
                                    step(3.0, 0.9, 7, 1, 5) });        // 3.0拍极高位空灵回应
    ambZenVoid.styleTags.addArray({ "traditional", "world", cn(u8"角调"), cn(u8"清乐") });
    ambZenVoid.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"柔美"), cn(u8"温婉") });
    ambZenVoid.regionTags.addArray({ "chinese", cn(u8"甘肃"), cn(u8"青海") }); // 完美命中大漠西北标签
    ambZenVoid.functionFit.addArray({ "t", "s" });
    ambZenVoid.formFit.addArray({ "intro", "outro" });
    ambZenVoid.familyId = "GuqinSparseFamily";
    ambZenVoid.grooveAffinity = "Late"; // 强烈空灵呼吸感
    ambZenVoid.complexity = 1;
    ambZenVoid.energy = 1;
    patterns.push_back(ambZenVoid);

    // 6. 深海幽闭：潜流涌动减音程 (针对标签：深海、减五度、悬疑、科幻、克苏鲁)
    auto ambDeepAbyss = makePattern("amb_deep_abyss_145", cn(u8"氛围：深海幽闭潜流减五度"), "Submerged Oceanic Abyss Tritone Pad",
                                    ArpeggioDirection::Down, ArpeggioDensity::Half, 4.0, // 完全向下滚动，低音区极长延音，死死扣住不和谐的张力
                                    { step(0.0, 3.9, 0, -2, 10, true),   // 深海最底层水压基音
                                      step(1.0, 2.9, 4, -1, 8),          // 灵魂：不和谐减五度/三度音，死锁压迫感
                                      step(2.0, 1.9, 1, 0, 6),           // 小二级暗黑色彩注入
                                      step(3.0, 0.9, 2, -1, 5) });
    ambDeepAbyss.styleTags.addArray({ "ambient", "epic", cn(u8"减五度"), "Locrian" });
    ambDeepAbyss.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"伤感") });
    ambDeepAbyss.regionTags.addArray({ "global" });
    ambDeepAbyss.functionFit.addArray({ "t", "D" });
    ambDeepAbyss.formFit.addArray({ "intro", "bridge" });
    ambDeepAbyss.familyId = "ElectronicArpFamily";
    ambDeepAbyss.complexity = 3;
    ambDeepAbyss.energy = 2;
    patterns.push_back(ambDeepAbyss);

    // 7. 永恒赞歌：全景交响圣咏墙 (针对标签：史诗、庄严、宏大、大片、高潮)
    auto ambEpicWall = makePattern("amb_epic_wall_146", cn(u8"氛围：全景交响弦乐圣咏长音墙"), "Epic Symphony Eternal String Wall",
                                   ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 密集的多声部全音符齐奏，完全连成一面气势磅礴的和声巨墙
                                   { step(0.0, 3.9, 0, -2, 14, true),  // 超重低音提琴齐奏
                                     step(0.0, 3.9, 4, -1, 11, false), // 雄厚低音五度支撑
                                     step(0.0, 3.9, 2, 0, 12, false),  // 中音区中提琴色彩
                                     step(0.0, 3.9, 7, 1, 13, false),  // 高音区第一小提琴冠音
                                     step(2.0, 1.9, 1, 1, 11, false) }); // 2拍中途内声部平滑级进换音，墙体内部在呼吸
    ambEpicWall.styleTags.addArray({ "epic", "march", cn(u8"和声小调") });
    ambEpicWall.moodTags.addArray({ "energetic", cn(u8"史诗"), cn(u8"庄严"), cn(u8"大气"), cn(u8"震撼") });
    ambEpicWall.regionTags.addArray({ "global" });
    ambEpicWall.functionFit.addArray({ "t", "T", "S", "D" });
    ambEpicWall.formFit.addArray({ "chorus", "bridge" });
    ambEpicWall.familyId = "HybridEpicStringFamily";
    ambEpicWall.pedalFriendly = true;
    ambEpicWall.complexity = 4; // 声部密度极高
    ambEpicWall.energy = 5;      // 极限史诗能级
    patterns.push_back(ambEpicWall);


    // ========================================================================
    // 新增补充：影视科幻与网络信息时代核心库 (Cinematic Sci-Fi & Cyber Tech)
    // ========================================================================

    // 1. 神经网络：量子数据流闪烁 (针对标签：人工智能、AI、神经网络、数据流、量子)
    auto sciFiNeuralNet = makePattern("scifi_neural_net_150", cn(u8"科幻：人工智能神经网络数字流"), "AI Neural Network Data Stream",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍全16分音符，通过非对称的音阶跳跃模拟0和1的代码流动
                                      { step(0.0, 0.2, 0, -1, 10, true),   // 地基数据核心
                                        step(0.25, 0.15, 2, 1, 8),         // 瞬间跃升至高位（放电闪烁）
                                        step(0.5, 0.15, 4, 0, 5), 
                                        step(0.75, 0.15, 1, 1, 7),         // 九音错位微光
                                        step(1.0, 0.2, 7, 1, 9), 
                                        step(1.25, 0.15, 4, 0, 4), 
                                        step(1.5, 0.15, 2, 0, 6), 
                                        step(1.75, 0.15, 0, 1, 8),
                                        step(2.0, 0.2, 1, -1, 9, true),    // 周期中点重调
                                        step(2.25, 0.15, 4, 1, 7), 
                                        step(2.5, 0.15, 7, 1, 9), 
                                        step(2.75, 0.15, 2, 2, 11),        // 偶尔冲向超高空极轴（Octave +2）
                                        step(3.0, 0.2, 4, 1, 6), 
                                        step(3.25, 0.15, 1, 0, 5), 
                                        step(3.5, 0.15, 0, 1, 8), 
                                        step(3.75, 0.15, 2, 0, 4) });
    sciFiNeuralNet.styleTags.addArray({ "ambient", "edm", "pop", cn(u8"九和弦"), cn(u8"十一和弦") });
    sciFiNeuralNet.moodTags.addArray({ "mysterious", cn(u8"迷幻"), cn(u8"动作"), cn(u8"空灵") });
    sciFiNeuralNet.regionTags.addArray({ "global" });
    sciFiNeuralNet.formFit.addArray({ "verse", "chorus" });
    sciFiNeuralNet.familyId = "ElectronicArpFamily";
    sciFiNeuralNet.grooveAffinity = "Straight"; // 绝对冷酷、高精度无情感的电子时钟律动
    sciFiNeuralNet.complexity = 4;
    sciFiNeuralNet.energy = 4;
    patterns.push_back(sciFiNeuralNet);

    // 2. 星际漂流：引力边缘悬浮长音 (针对标签：星际、宇宙、未知、科幻、冷色调)
    auto sciFiGravityFloat = makePattern("scifi_gravity_151", cn(u8"科幻：星际引力边缘悬浮长音"), "Interstellar Gravity Edge Float",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 大时值留白，音符长长延音，拒绝踩实地面，带有极其强烈的失重感
                                         { step(0.0, 3.9, 0, -2, 6, true),    // 庞大星舰低音母核（Octave -2）
                                           step(1.0, 2.9, 3, 1, 8),           // 灵魂：增四度/四度悬浮音（Index 3/Lydian），产生星空离调美感
                                           step(2.0, 1.9, 1, 0, 4),           // 九音柔和切入
                                           step(3.0, 0.9, 7, 2, 7) });        // 超高空冷冽星光闪烁
    sciFiGravityFloat.styleTags.addArray({ "ambient", "Lydian", cn(u8"利底亚"), cn(u8"挂连") });
    sciFiGravityFloat.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"孤独"), cn(u8"梦幻"), cn(u8"广袤") });
    sciFiGravityFloat.regionTags.addArray({ "global" });
    sciFiGravityFloat.functionFit.addArray({ "T", "II", "v" });          // 绝配 I - II 宇宙飞翔离调大二级进行
    sciFiGravityFloat.formFit.addArray({ "intro", "outro", "verse" });
    sciFiGravityFloat.familyId = "GuqinSparseFamily"; // 借用极限高留白框架
    sciFiGravityFloat.pedalFriendly = true;
    sciFiGravityFloat.complexity = 2;
    sciFiGravityFloat.energy = 1;
    patterns.push_back(sciFiGravityFloat);

    // 3. 数字废墟：赛博朋克霓虹碎音 (针对标签：赛博朋克、黑客、网络、反乌托邦、暗黑科技)
    auto sciFiCyberGlitch = makePattern("scifi_cyber_glitch_152", cn(u8"科幻：赛博霓虹错位脉冲碎音"), "Cyberpunk Neon Digital Glitch",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 2拍短循环，运用“前长后碎”和不规则重音，模拟损坏的数据卡带和霓虹灯闪烁
                                        { step(0.0, 0.45, 0, -1, 12, true),   // 正拍坚固的机械脉冲
                                          step(0.5, 0.1, 4, 0, 7), 
                                          step(0.75, 0.1, 1, 1, 10),         // 突发高位强力度重砸
                                          step(1.0, 0.1, 0, 0, 4, true),     // 快速断音错位
                                          step(1.25, 0.05, 2, 0, 8),         // 极限 1/32 拍既视感的碎音连击效果
                                          step(1.35, 0.05, 4, 0, 9), 
                                          step(1.5, 0.45, 7, 1, 11) });       // 尖锐的霓虹光芒收尾
    sciFiCyberGlitch.styleTags.addArray({ "rock", "edm", "ambient" });
    sciFiCyberGlitch.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"愤怒"), cn(u8"动作") });
    sciFiCyberGlitch.regionTags.addArray({ "global" });
    sciFiCyberGlitch.functionFit.addArray({ "t", "bII", "D" });          // 极度契合暗黑半音、弗里吉亚对峙进行
    sciFiCyberGlitch.formFit.addArray({ "verse", "bridge" });
    sciFiCyberGlitch.familyId = "ElectronicArpFamily";
    sciFiCyberGlitch.grooveAffinity = "Early"; // 产生强烈的神经质催促感
    sciFiCyberGlitch.complexity = 5; // 骨骼极其嶙峋
    sciFiCyberGlitch.energy = 4;
    patterns.push_back(sciFiCyberGlitch);

    // 4. 信息风暴：光纤矩阵点阵云 (针对标签：网络、信息时代、大数据、云端、科技发布会)
    auto sciFiFiberMatrix = makePattern("scifi_fiber_matrix_153", cn(u8"科技：光纤矩阵点阵网络云"), "Fiber-Optic Matrix Dot Cloud",
                                        ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 连续的8分音符，但左右手采用“两声部交叉重叠”，织体犹如精密的硅片矩阵
                                        { step(0.0, 0.95, 0, -1, 9, true),   // 左手主干
                                          step(0.5, 0.45, 4, 0, 5, false),   // 右手柔和嵌入
                                          step(1.0, 0.95, 2, 0, 7, true),    // 左手九音走线
                                          step(1.5, 0.45, 7, 1, 6, false),   // 右手高位对位
                                          step(2.0, 0.95, 4, 0, 8, true), 
                                          step(2.5, 0.45, 1, 1, 6, false), 
                                          step(3.0, 0.95, 0, 0, 7), 
                                          step(3.5, 0.45, 2, 1, 5, false) });
    sciFiFiberMatrix.styleTags.addArray({ "pop", "ballad", "c-pop" });
    sciFiFiberMatrix.moodTags.addArray({ "warm", "hopeful", cn(u8"明亮"), cn(u8"大气"), cn(u8"温暖") });
    sciFiFiberMatrix.regionTags.addArray({ "global" });
    sciFiFiberMatrix.functionFit.addArray({ "T", "S" });
    sciFiFiberMatrix.formFit.addArray({ "verse", "chorus" });
    sciFiFiberMatrix.familyId = "CPopBalladPianoFamily"; // 大众高接受度高级感
    sciFiFiberMatrix.complexity = 3;
    sciFiFiberMatrix.energy = 2;
    patterns.push_back(sciFiFiberMatrix);

    // 5. 机械之心：自动化精密齿轮 (针对标签：机械、自动化、硬核科技、工业、未来工厂)
    auto sciFiClockwork = makePattern("scifi_clockwork_154", cn(u8"科技：未来机械精密自动化齿轮"), "Futuristic Clockwork Automation",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 严格以“大跳->级进->大跳”的几何图形对称运动，模仿完美的机械咬合
                                      { step(0.0, 0.45, 0, -1, 11, true),  // 重合点 1
                                        step(0.5, 0.45, 7, 1, 8),          // 极限向上飞跃（模拟机械臂突进）
                                        step(1.0, 0.45, 4, 0, 6),          // 平滑回落
                                        step(1.5, 0.45, 2, 0, 7), 
                                        step(2.0, 0.45, 0, 0, 10, true),   // 重合点 2
                                        step(2.5, 0.45, 7, 0, 8), 
                                        step(3.0, 0.45, 4, 1, 7), 
                                        step(3.5, 0.45, 1, 0, 6) });
    sciFiClockwork.styleTags.addArray({ "march", "epic", "pop" });
    sciFiClockwork.moodTags.addArray({ "energetic", cn(u8"庄严"), cn(u8"动作"), cn(u8"紧张") });
    sciFiClockwork.regionTags.addArray({ "global" });
    sciFiClockwork.functionFit.addArray({ "T", "t", "D" });
    sciFiClockwork.formFit.addArray({ "pre-chorus", "chorus" });
    sciFiClockwork.familyId = "JayStyleSyncopationFamily";
    sciFiClockwork.complexity = 3;
    sciFiClockwork.energy = 3;
    patterns.push_back(sciFiClockwork);

    // 6. 量子叠加：平行时空幽灵双音 (针对标签：科幻、量子、迷幻、悬疑、多维空间)
    auto sciFiQuantumGhost = makePattern("scifi_quantum_155", cn(u8"科幻：量子叠加平行时空双音"), "Quantum Superposition Ghost Dyad",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0, // 运用高音区的“双音同击二度摩擦”和突发留白，模拟粒子不确定的幽灵残影
                                         { step(0.0, 3.9, 0, -1, 7, true),    // 模糊的基音托底
                                           step(0.0, 1.9, 1, 1, 6, false),    // 灵魂：高位二级九音与三音（Index 1 & 2）同时砸下！
                                           step(0.0, 1.9, 2, 1, 6, false),    // 产生极具现代电影高级感的“小二度半音紧密摩擦”
                                           step(2.0, 1.9, 4, 1, 5, false),    // 2拍后平滑位移至五音与七音对峙
                                           step(2.0, 1.9, 6, 1, 5, false) });
    sciFiQuantumGhost.styleTags.addArray({ "ambient", cn(u8"七和弦"), cn(u8"九和弦"), "Jazz" });
    sciFiQuantumGhost.moodTags.addArray({ "mysterious", cn(u8"空灵"), cn(u8"柔美"), cn(u8"孤独") });
    sciFiQuantumGhost.regionTags.addArray({ "global" });
    sciFiQuantumGhost.functionFit.addArray({ "t", "T", "v" });
    sciFiQuantumGhost.formFit.addArray({ "intro", "outro", "bridge" });
    sciFiQuantumGhost.familyId = "CinematicStringPadFamily"; // 借用长音交织网
    sciFiQuantumGhost.pedalFriendly = true;
    sciFiQuantumGhost.complexity = 4;
    sciFiQuantumGhost.energy = 2;
    patterns.push_back(sciFiQuantumGhost);

    // 7. 奇点降临：史诗科技引力坍缩 (针对标签：史诗、宏大、高潮、震撼、终局、科幻大片)
    auto sciFiSingularity = makePattern("scifi_singularity_156", cn(u8"科幻：黑洞奇点降临引力坍缩"), "Singularity Event Horizon Epic Cascade",
                                        ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 4.0, // 极速下行的全16分瀑布跑音，声部从极高空一路砸向无底深渊，展现吞噬一切的宏大科技感
                                        { step(0.0, 0.45, 0, -2, 14, true),  // 视界边缘第一声巨响
                                          step(0.5, 0.15, 7, 2, 11),         // 从极高天际线（Octave +2）开启疯狂跌落
                                          step(0.75, 0.15, 4, 1, 9), 
                                          step(1.0, 0.45, 2, 1, 10, true),   // 重力加速度
                                          step(1.5, 0.15, 1, 1, 8), 
                                          step(1.75, 0.15, 0, 0, 9), 
                                          step(2.0, 0.45, 4, -1, 12, true),  // 跌入低音核心
                                          step(2.5, 0.15, 7, 0, 10), 
                                          step(2.75, 0.15, 2, 0, 8), 
                                          step(3.0, 0.15, 1, -1, 11),        // 极限摩擦
                                          step(3.25, 0.15, 0, -1, 12), 
                                          step(3.5, 0.45, 4, -2, 13, true) }); // 最终坠入奇点死寂
    sciFiSingularity.styleTags.addArray({ "epic", "rock", "classical", cn(u8"和声小调"), "Locrian" });
    sciFiSingularity.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"震撼"), cn(u8"大气"), cn(u8"动作") });
    sciFiSingularity.regionTags.addArray({ "global" });
    sciFiSingularity.functionFit.addArray({ "t", "D" });
    sciFiSingularity.formFit.addArray({ "chorus", "bridge" });
    sciFiSingularity.familyId = "HybridEpicStringFamily";
    sciFiSingularity.complexity = 5; // 物理跨度与手速要求极大
    sciFiSingularity.energy = 5;      // 极限史诗能级
    patterns.push_back(sciFiSingularity);


    // ========================================================================
    // 新增补充：影视科幻与网络信息时代进阶库 (Advanced Sci-Fi & Quantum Cyber)
    // ========================================================================

    // 1. 无限镜像：极简主义相位相位交错 (针对标签：数字时代、平行宇宙、多维空间、极简主义)
    auto sciFiMirrorPhase = makePattern("scifi_mirror_160", cn(u8"科幻：极简主义无限镜像相位差"), "Minimalist Infinite Mirror Phase",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍循环，通过3+3+2的非对称相位差，让高音区产生机械且空灵的咬合
                                        { step(0.0, 0.2, 0, -1, 8, true),    // 根音时钟底盘
                                          step(0.0, 0.2, 7, 1, 6, false),    // 高空右手机械点 1
                                          step(0.375, 0.2, 2, 0, 5),          // 错开半个16分音符的交错点 2
                                          step(0.75, 0.2, 4, 1, 6),           // 点 3
                                          step(1.125, 0.2, 1, 0, 7),          // 挂留九音错位相位
                                          step(1.5, 0.2, 7, 1, 5), 
                                          step(1.875, 0.2, 2, 0, 6), 
                                          step(2.25, 0.2, 0, -1, 8, true),   // 第二周期地基
                                          step(2.625, 0.2, 4, 1, 7), 
                                          step(3.0, 0.2, 1, 0, 6), 
                                          step(3.375, 0.2, 7, 1, 8), 
                                          step(3.75, 0.2, 2, 0, 5) });
    sciFiMirrorPhase.styleTags.addArray({ "ambient", "classical", cn(u8"大调"), cn(u8"挂留") });
    sciFiMirrorPhase.moodTags.addArray({ "calm", cn(u8"空灵"), cn(u8"孤独"), cn(u8"梦幻") });
    sciFiMirrorPhase.regionTags.addArray({ "global" });
    sciFiMirrorPhase.functionFit.addArray({ "T", "S" });
    sciFiMirrorPhase.familyId = "CinematicStringPadFamily"; // 借用长线网
    sciFiMirrorPhase.complexity = 4;
    sciFiMirrorPhase.energy = 2;
    patterns.push_back(sciFiMirrorPhase);

    // 2. 二进制危机：黑客帝国代码轰炸 (针对标签：黑客、反乌托邦、紧张、密码、网络战)
    auto sciFiCyberAttack = makePattern("scifi_cyber_atk_161", cn(u8"科幻：二进制危机黑客矩阵轰炸"), "Binary Matrix Hacker Cyber Attack",
                                        ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 2.0, // 2拍极限高速循环，全下行大跳摩擦，极具赛博暴躁感与迫近感
                                        { step(0.0, 0.15, 0, -1, 14, true),   // 机械重砸（Velocity大加分）
                                          step(0.25, 0.1, 7, 1, 11),         // 从高空开启密码下行爆破
                                          step(0.375, 0.1, 3, 0, 10),         // 小三度/变化音摩擦
                                          step(0.5, 0.1, 1, 0, 9), 
                                          step(0.75, 0.15, 4, -1, 12, true),  // 中途低音拦截
                                          step(1.0, 0.1, 7, 1, 10), 
                                          step(1.125, 0.1, 3, 0, 11), 
                                          step(1.25, 0.1, 2, 0, 9), 
                                          step(1.5, 0.1, 1, 0, 10), 
                                          step(1.75, 0.1, 0, -1, 13) });
    sciFiCyberAttack.styleTags.addArray({ "rock", "edm", cn(u8"减五度"), "Phrygian" });
    sciFiCyberAttack.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"愤怒"), cn(u8"动作") });
    sciFiCyberAttack.regionTags.addArray({ "global" });
    sciFiCyberAttack.functionFit.addArray({ "t", "D", "bII" });
    sciFiCyberAttack.formFit.addArray({ "chorus", "bridge" });
    sciFiCyberAttack.familyId = "ElectronicArpFamily";
    sciFiCyberAttack.grooveAffinity = "Early"; // 极度前冲
    sciFiCyberAttack.complexity = 5;
    sciFiCyberAttack.energy = 5;
    patterns.push_back(sciFiCyberAttack);

    // 3. 量子计算机：非线性叠加态碎音 (针对标签：量子、科学家、高科技、未知、未来)
    auto sciFiQuantumState = makePattern("scifi_quantum_162", cn(u8"科幻：量子计算机叠加态碎音"), "Quantum Computing Superposition Glitch",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 4.0, // 运用不可预测的三连音与十六分音符混合时值，模拟量子比特坍缩前的幽灵态
                                         { step(0.0, 0.95, 0, -1, 9, true),   // 确定态地基
                                           step(0.66, 0.2, 4, 1, 6),          // 三连音错位点 1
                                           step(1.0, 0.45, 1, 1, 8),          // 突发挂留
                                           step(1.5, 0.2, 7, 2, 11),          // 飙向极高空泛音
                                           step(1.75, 0.2, 2, 1, 6), 
                                           step(2.0, 0.95, 0, -1, 10, true),  // 第二核心
                                           step(2.5, 0.15, 3, 0, 7), 
                                           step(2.66, 0.15, 4, 1, 8), 
                                           step(3.0, 0.45, 7, 1, 7), 
                                           step(3.5, 0.45, 1, 2, 10) });
    sciFiQuantumState.styleTags.addArray({ "ambient", cn(u8"挂留"), "Jazz" });
    sciFiQuantumState.moodTags.addArray({ "mysterious", cn(u8"空灵"), cn(u8"迷幻") });
    sciFiQuantumState.regionTags.addArray({ "global" });
    sciFiQuantumState.functionFit.addArray({ "t", "T" });
    sciFiQuantumState.familyId = "ElectronicArpFamily";
    sciFiQuantumState.complexity = 4;
    sciFiQuantumState.energy = 3;
    patterns.push_back(sciFiQuantumState);

    // 4. 数字雨：高空像素点阵下坠 (针对标签：代码、数字雨、虚拟现实、元宇宙、科技感)
    auto sciFiDigitalRain = makePattern("scifi_rain_163", cn(u8"科幻：母体数字雨像素点阵下坠"), "The Matrix Digital Rain Cascade",
                                        ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 4.0, // 全小节密集下行跑音，力度高低交错，完美重现屏幕上绿色代码雨倾泻而下的视觉声学
                                        { step(0.0, 0.2, 7, 2, 10, true),    // 起始点位于极高空（Octave +2）
                                          step(0.25, 0.15, 4, 1, 6), 
                                          step(0.5, 0.15, 2, 1, 5), 
                                          step(0.75, 0.15, 1, 0, 7), 
                                          step(1.0, 0.2, 0, 0, 9, false),    // 坠落至中音区
                                          step(1.25, 0.15, 4, 0, 6), 
                                          step(1.5, 0.15, 2, -1, 8), 
                                          step(1.75, 0.15, 0, -1, 11),       // 砸入低音深渊
                                          step(2.0, 0.2, 7, 1, 9, true),     // 第二波数字雨启动
                                          step(2.25, 0.15, 4, 1, 6), 
                                          step(2.5, 0.15, 1, 0, 7), 
                                          step(2.75, 0.15, 0, 0, 5), 
                                          step(3.0, 0.2, 4, -1, 8), 
                                          step(3.25, 0.15, 2, -1, 7), 
                                          step(3.5, 0.15, 1, -1, 9), 
                                          step(3.75, 0.15, 0, -2, 12) });
    sciFiDigitalRain.styleTags.addArray({ "ambient", "classical", cn(u8"自然小调") });
    sciFiDigitalRain.moodTags.addArray({ "mysterious", cn(u8"孤独"), cn(u8"紧张") });
    sciFiDigitalRain.regionTags.addArray({ "global" });
    sciFiDigitalRain.functionFit.addArray({ "t", "s", "D" });
    sciFiDigitalRain.formFit.addArray({ "verse", "bridge" });
    sciFiDigitalRain.familyId = "ElectronicArpFamily";
    sciFiDigitalRain.complexity = 5;
    sciFiDigitalRain.energy = 4;
    patterns.push_back(sciFiDigitalRain);

    // 5. 机械觉醒：自动化指令集递归 (针对标签：机器人、自动化、控制论、AI觉醒、赛博)
    auto sciFiRobotAwaken = makePattern("scifi_robot_164", cn(u8"科幻：机械觉醒自动化递归指令"), "Cybernetic Robot Awakening Recursion",
                                        ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 严格对称的八分音符双向跳跃，中间突发一次八度同击，模拟AI代码自我迭代觉醒的瞬间
                                        { step(0.0, 0.45, 0, -1, 12, true),  // 指令启动
                                          step(0.5, 0.45, 2, 0, 7), 
                                          step(1.0, 0.45, 4, 0, 8), 
                                          step(1.5, 0.45, 7, 1, 10), 
                                          step(2.0, 0.45, 0, 1, 13, false),  // 核心高光：2拍正拍高位八度同击（模拟逻辑觉醒点火）
                                          step(2.0, 0.45, 0, 0, 11, false), 
                                          step(2.5, 0.45, 4, 0, 8), 
                                          step(3.0, 0.45, 2, 0, 7), 
                                          step(3.5, 0.45, 1, 0, 9) });
    sciFiRobotAwaken.styleTags.addArray({ "march", "pop", "epic" });
    sciFiRobotAwaken.moodTags.addArray({ "energetic", cn(u8"庄严"), cn(u8"大气"), cn(u8"震撼") });
    sciFiRobotAwaken.regionTags.addArray({ "global" });
    sciFiRobotAwaken.functionFit.addArray({ "T", "t", "S" });
    sciFiRobotAwaken.formFit.addArray({ "pre-chorus", "chorus" });
    sciFiRobotAwaken.familyId = "JayStyleSyncopationFamily";
    sciFiRobotAwaken.complexity = 3;
    sciFiRobotAwaken.energy = 4;
    patterns.push_back(sciFiRobotAwaken);

    // 6. 暗物质：引力坍缩次声波 (针对标签：暗黑科技、宇宙、重力、克苏鲁、终局、未知)
    auto sciFiDarkMatter = makePattern("scifi_dark_165", cn(u8"科幻：暗物质引力坍缩次声波"), "Dark Matter Gravitational Infrasound",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Half, 4.0, // 极端的大长音，完全集中在左手钢琴的极限低音区（Octave -2 和 -1），模拟黑洞视界的沉闷低频压迫
                                       { step(0.0, 3.9, 0, -2, 13, true),   // 引力中心极其沉重的锤击
                                         step(0.0, 3.9, 4, -1, 10, false),  // 纯五度锁死架构
                                         step(1.5, 2.4, 3, -1, 9),          // 1.5拍幽暗切入的 Lydian 特征变化音
                                         step(3.0, 0.9, 1, 0, 8) });        // 九音在低空漂浮
    sciFiDarkMatter.styleTags.addArray({ "ambient", "Lydian", cn(u8"减五度"), "Locrian" });
    sciFiDarkMatter.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"孤独"), cn(u8"大气") });
    sciFiDarkMatter.regionTags.addArray({ "global" });
    sciFiDarkMatter.functionFit.addArray({ "t", "II", "D" });
    sciFiDarkMatter.formFit.addArray({ "intro", "bridge" });
    sciFiDarkMatter.familyId = "CinematicStringPadFamily";
    sciFiDarkMatter.pedalFriendly = true;
    sciFiDarkMatter.complexity = 2;
    sciFiDarkMatter.energy = 2;
    patterns.push_back(sciFiDarkMatter);

    // 7. 量子跃迁：超光速粒子引擎 (针对标签：科幻大片、星际穿越、宏大、高潮、光速)
    auto sciFiWarpDrive = makePattern("scifi_warp_166", cn(u8"科幻：超光速引力奇点量子跃迁"), "Tachyon Warp Drive Singularity",
                                      ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0, // 4拍连绵不绝的复合跨度跑音。每一拍都在做螺旋上升（Upward Spiral），模拟飞船冲入虫洞的无尽张力
                                      { step(0.0, 0.2, 0, -1, 11, true), 
                                        step(0.25, 0.15, 4, 0, 6), 
                                        step(0.5, 0.15, 1, 0, 7),          // 九音蓄力
                                        step(0.75, 0.15, 7, 1, 8), 
                                        step(1.0, 0.2, 2, 0, 9, true),     // 螺旋阶梯第二级
                                        step(1.25, 0.15, 4, 1, 7), 
                                        step(1.5, 0.15, 7, 1, 8), 
                                        step(1.75, 0.15, 1, 2, 10),        // 冲入极高空（Octave +2）
                                        step(2.0, 0.2, 4, 0, 11, true),    // 中轴引力重组
                                        step(2.25, 0.15, 7, 1, 8), 
                                        step(2.5, 0.15, 1, 1, 9), 
                                        step(2.75, 0.15, 2, 2, 11),
                                        step(3.0, 0.2, 7, 1, 12),          // 光速引擎全开
                                        step(3.25, 0.15, 4, 1, 9), 
                                        step(3.5, 0.15, 2, 0, 8), 
                                        step(3.75, 0.15, 1, 0, 10) });
    sciFiWarpDrive.styleTags.addArray({ "epic", "classical", "pop", cn(u8"和声小调") });
    sciFiWarpDrive.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"震撼"), cn(u8"大气"), cn(u8"希望") });
    sciFiWarpDrive.regionTags.addArray({ "global" });
    sciFiWarpDrive.functionFit.addArray({ "t", "T", "D", "S" });
    sciFiWarpDrive.formFit.addArray({ "chorus", "bridge" });
    sciFiWarpDrive.familyId = "HybridEpicStringFamily";
    sciFiWarpDrive.complexity = 5;
    sciFiWarpDrive.energy = 5;
    patterns.push_back(sciFiWarpDrive);


    // ========================================================================
    // 新增补充：媒体、新闻报道与信息传播类核心库 (Broadcast Journalism & Info Flow)
    // ========================================================================

    // 1. 突发事件：整点电报脉冲时钟 (针对标签：突发新闻、Breaking News、紧急、电报码)
    auto mediaBreakingNews = makePattern("media_breaking_170", cn(u8"媒体：突发新闻整点脉冲时钟"), "Breaking News Broadcast Clock Pulse",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 2拍高频强循环，通过单音与双音交替，模拟新闻片头倒计时时钟与电报滴答声
                                         { step(0.0, 0.15, 0, -1, 13, true),   // 正拍坚固地基（权威发布）
                                           step(0.25, 0.1, 7, 1, 10),         // 极高频电报脉冲 1
                                           step(0.375, 0.1, 7, 1, 9),         // 电报脉冲 2
                                           step(0.5, 0.15, 4, 0, 8),          // 中轴信息流
                                           step(0.75, 0.1, 7, 1, 10), 
                                           step(1.0, 0.15, 0, 0, 11, true),   // 1拍正拍“大时钟机械咬合”
                                           step(1.25, 0.1, 7, 1, 11, false),  // 高位同击双音（紧迫感）
                                           step(1.25, 0.1, 2, 1, 10, false), 
                                           step(1.5, 0.15, 4, 0, 8), 
                                           step(1.75, 0.1, 1, 0, 9) });
    mediaBreakingNews.styleTags.addArray({ "march", "pop", cn(u8"挂留"), "Phrygian" }); // 略带客观紧张感
    mediaBreakingNews.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"庄严"), cn(u8"大气") });
    mediaBreakingNews.regionTags.addArray({ "global" });
    mediaBreakingNews.functionFit.addArray({ "t", "T", "D" });
    mediaBreakingNews.formFit.addArray({ "intro", "pre-chorus" });
    mediaBreakingNews.familyId = "ElectronicArpFamily"; // 极契合电子时钟
    mediaBreakingNews.grooveAffinity = "Straight"; // 绝对冷酷的时钟机芯律动
    mediaBreakingNews.complexity = 4;
    mediaBreakingNews.energy = 4;
    patterns.push_back(mediaBreakingNews);

    // 2. 深度调查：纪实文学冷静级进 (针对标签：纪录片、深度报道、社会纪实、黑白、客观)
    auto mediaDocumentary = makePattern("media_doc_invest_171", cn(u8"媒体：社会纪实深度调查冷静级进"), "Documentary Investigative Calm Line",
                                        ArpeggioDirection::Up, ArpeggioDensity::Eighth, 4.0, // 4拍极度克制的8分音符全上行慢跑，拒绝大跳，专注于还原冰冷事实背后的长线叙事
                                        { step(0.0, 1.9, 0, -1, 8, true),    // 克制的根音
                                          step(0.5, 0.45, 1, 0, 5),          // 标志性的九音级进（极度理性客观）
                                          step(1.0, 0.45, 4, 0, 6),          // 五音支撑
                                          step(1.5, 0.45, 0, 0, 7), 
                                          step(2.0, 1.9, 2, 0, 7, true),     // 中段色彩微调
                                          step(2.5, 0.45, 4, 0, 6), 
                                          step(3.0, 0.45, 1, 0, 5), 
                                          step(3.5, 0.45, 0, 0, 6) });
    mediaDocumentary.styleTags.addArray({ "ambient", "ballad", cn(u8"自然小调"), cn(u8"挂留") });
    mediaDocumentary.moodTags.addArray({ "calm", cn(u8"孤独"), cn(u8"伤感"), cn(u8"冷酷") });
    mediaDocumentary.regionTags.addArray({ "global" });
    mediaDocumentary.functionFit.addArray({ "t", "v", "s" });
    mediaDocumentary.formFit.addArray({ "verse", "bridge" });
    mediaDocumentary.familyId = "CPopBalladPianoFamily";
    mediaDocumentary.complexity = 2;
    mediaDocumentary.energy = 2;
    patterns.push_back(mediaDocumentary);

    // 3. 财经周刊：全球信息枢纽迭代 (针对标签：财经、股市、科技发布会、国际论坛、高端)
    auto mediaFinancial = makePattern("media_finance_172", cn(u8"媒体：全球财经高端信息流矩阵"), "Global Financial Matrix Iteration",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍连绵不绝的16分开放式流线，高音区音符交错跃动，模拟数据图表、股市K线闪烁上升的高端律动
                                      { step(0.0, 0.2, 0, -1, 10, true),   // 资本宏大托底
                                        step(0.25, 0.15, 4, 0, 6), 
                                        step(0.5, 0.15, 1, 0, 8),          // 摩登的 Add9 色彩
                                        step(0.75, 0.15, 7, 1, 9), 
                                        step(1.0, 0.2, 0, 0, 7, true), 
                                        step(1.25, 0.15, 2, 0, 6), 
                                        step(1.5, 0.15, 4, 1, 8), 
                                        step(1.75, 0.15, 7, 1, 10),        // 攀升向高位
                                        step(2.0, 0.2, 2, -1, 9, true), 
                                        step(2.25, 0.15, 4, 0, 7), 
                                        step(2.5, 0.15, 7, 1, 9), 
                                        step(2.75, 0.15, 1, 1, 8), 
                                        step(3.0, 0.2, 0, 0, 7), 
                                        step(3.25, 0.15, 4, 0, 6), 
                                        step(3.5, 0.15, 2, 0, 5), 
                                        step(3.75, 0.15, 1, 0, 4) });
    mediaFinancial.styleTags.addArray({ "pop", "classical", cn(u8"大调"), cn(u8"九和弦") });
    mediaFinancial.moodTags.addArray({ "warm", "hopeful", cn(u8"明亮"), cn(u8"大气") });
    mediaFinancial.regionTags.addArray({ "global" });
    mediaFinancial.functionFit.addArray({ "T", "S", "D" });
    mediaFinancial.formFit.addArray({ "verse", "chorus" });
    mediaFinancial.familyId = "CPopBalladPianoFamily";
    mediaFinancial.complexity = 3;
    mediaFinancial.energy = 3;
    patterns.push_back(mediaFinancial);

    // 4. 观点访谈：知性留白对话体 (针对标签：访谈节目、人物专题、高端对话、讲述)
    auto mediaInterview = makePattern("media_interview_173", cn(u8"媒体：知性高端访谈对话留白"), "Intellectual Talk-Show Dialogue Void",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 四分音符大留白，音符力度温和，不抢话筒，留出巨大的语言解说空间
                                      { step(0.0, 3.9, 0, -1, 7, true),    // 温暖沉稳的倾听者基音
                                        step(1.0, 0.9, 1, 0, 4),           // 九音柔和递进
                                        step(2.0, 1.9, 4, 0, 6),           // 五音回应
                                        step(3.0, 0.9, 2, 0, 5) });        // 色彩中性落脚
    mediaInterview.styleTags.addArray({ "ambient", "ballad", cn(u8"大调") });
    mediaInterview.moodTags.addArray({ "warm", "calm", cn(u8"治愈"), cn(u8"柔美") });
    mediaInterview.regionTags.addArray({ "global" });
    mediaInterview.functionFit.addArray({ "T", "S" });
    mediaInterview.formFit.addArray({ "intro", "outro", "verse" });
    mediaInterview.familyId = "GuqinSparseFamily"; // 借用高留白架构
    mediaInterview.pedalFriendly = true;
    mediaInterview.complexity = 1;
    mediaInterview.energy = 1;
    patterns.push_back(mediaInterview);

    // 5. 时代号角：国家电台庄严史诗 (针对标签：新闻联播、国家重大发布、进行曲、宏大)
    auto mediaStateAnthem = makePattern("media_anthem_174", cn(u8"媒体：国家电台庄严宏大开场"), "State Broadcast Majestic Anthem",
                                        ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4拍正拍。高频突发“八度同步砸奏（Stabs）”与军鼓律动级进，具有极高主权权威感与时代号角质感
                                        { step(0.0, 0.95, 0, -1, 14, true),  // 重砸权威基石
                                          step(1.0, 0.45, 0, 0, 12, false),  // 核心标志：1.0拍爆发的高音区八度同击！
                                          step(1.0, 0.45, 0, 1, 12, false), 
                                          step(1.5, 0.45, 4, 0, 9),          // 级进向后推进
                                          step(2.0, 0.95, 7, 0, 11, true),   // 五音转位支撑
                                          step(3.0, 0.45, 2, 0, 10, false),  // 3.0拍再次爆发双音柱式切分
                                          step(3.0, 0.45, 4, 0, 10, false), 
                                          step(3.5, 0.45, 7, 0, 9) });
    mediaStateAnthem.styleTags.addArray({ "march", "epic", cn(u8"大调") });
    mediaStateAnthem.moodTags.addArray({ "energetic", cn(u8"庄严"), cn(u8"大气"), cn(u8"震撼") });
    mediaStateAnthem.regionTags.addArray({ "global" });
    mediaStateAnthem.functionFit.addArray({ "T", "D", "S" });
    mediaStateAnthem.formFit.addArray({ "intro", "chorus" });
    mediaStateAnthem.familyId = "HybridEpicStringFamily"; // 巨墙底盘
    mediaStateAnthem.complexity = 3;
    mediaStateAnthem.energy = 5; // 极限能级
    patterns.push_back(mediaStateAnthem);

    // 6. 信息碎片：打字机流动像素云 (针对标签：快讯、新媒体、自媒体、科技短播)
    auto mediaTypewriter = makePattern("media_typewriter_175", cn(u8"媒体：新媒体打字机流动快讯"), "New Media Typewriter Data Fragment",
                                       ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 2.0, // 2拍短循环，运用极度跳跃、清脆的十六分断音，完美拟真打字机敲击与屏幕弹窗快讯
                                       { step(0.0, 0.1, 0, -1, 11, true),   // 机械起始
                                         step(0.25, 0.1, 4, 0, 6), 
                                         step(0.5, 0.1, 1, 1, 9),           // 九音跳跃敲击
                                         step(0.75, 0.1, 7, 1, 8), 
                                         step(1.0, 0.1, 2, 0, 7, true),     // 中继卡点
                                         step(1.25, 0.05, 4, 0, 8),         // 突发双碎音（模仿空格换行）
                                         step(1.35, 0.05, 7, 1, 9), 
                                         step(1.5, 0.45, 0, 1, 10) });      // 脆爽收尾
    mediaTypewriter.styleTags.addArray({ "edm", "pop", "c-pop" });
    mediaTypewriter.moodTags.addArray({ "happy", cn(u8"明亮"), cn(u8"欢快") });
    mediaTypewriter.regionTags.addArray({ "global" });
    mediaTypewriter.functionFit.addArray({ "T", "S" });
    mediaTypewriter.familyId = "ElectronicArpFamily";
    mediaTypewriter.grooveAffinity = "Early"; // 带着一点点新媒体的雷厉风行
    mediaTypewriter.complexity = 4;
    mediaTypewriter.energy = 3;
    patterns.push_back(mediaTypewriter);

    // 7. 地缘冲突：暗流涌动博弈线 (针对标签：国际新闻、深度观察、地缘政治、暗流、冷战)
    auto mediaGeopolitics = makePattern("media_geopol_176", cn(u8"媒体：国际地缘冲突暗流博弈"), "Geopolitical Undercurrent Strategy Line",
                                        ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 4.0, // 4拍极低音区的全下行滚动，死死咬合半音摩擦，极具宏大历史叙事的沉重感
                                        { step(0.0, 0.2, 0, -1, 12, true),   // 棋盘低音起
                                          step(0.25, 0.15, 4, -1, 8), 
                                          step(0.5, 0.15, 3, -1, 9),         // 灵魂半音摩擦（Index 3小三度暗色调）
                                          step(0.75, 0.15, 1, -1, 7), 
                                          step(1.0, 0.2, 0, -2, 11, true),   // 沉入深渊底盘
                                          step(1.25, 0.15, 7, -1, 8), 
                                          step(1.5, 0.15, 4, -1, 6), 
                                          step(1.75, 0.15, 1, -1, 7), 
                                          step(2.0, 0.2, 0, -1, 10, true),   // 循环中线重组
                                          step(2.25, 0.15, 3, -1, 8), 
                                          step(2.5, 0.15, 4, -1, 9), 
                                          step(2.75, 0.15, 7, 0, 7), 
                                          step(3.0, 0.2, 1, 0, 8), 
                                          step(3.25, 0.15, 4, -1, 6), 
                                          step(3.5, 0.15, 3, -1, 7), 
                                          step(3.75, 0.15, 0, -2, 10) });
    mediaGeopolitics.styleTags.addArray({ "ambient", "epic", cn(u8"和声小调"), "Phrygian" });
    mediaGeopolitics.moodTags.addArray({ "mysterious", cn(u8"紧张"), cn(u8"庄严"), cn(u8"冷酷") });
    mediaGeopolitics.regionTags.addArray({ "global" });
    mediaGeopolitics.functionFit.addArray({ "t", "D", "bII" });
    mediaGeopolitics.formFit.addArray({ "verse", "bridge" });
    mediaGeopolitics.familyId = "HybridEpicStringFamily";
    mediaGeopolitics.complexity = 5;
    mediaGeopolitics.energy = 3;
    patterns.push_back(mediaGeopolitics);

    // ========================================================================
    // 新增补充：全维度查漏补缺全矩阵库 (Vlog, ACG Game, Magic, Corporate, History)
    // ========================================================================

    // 1. Vlog轻快：短视频自媒体松弛律动 (ID: 180)
    auto mediaVlogFlow = makePattern("media_vlog_180", cn(u8"自媒体：短视频Vlog阳光松弛律动"), "Vlog Sunshine Chill Flow",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 2.0, // 2拍短循环，前轻后重，极其具有日常生活的松弛美感
                                     { step(0.0, 0.45, 0, -1, 10, true),   // 柔和地基
                                       step(0.5, 0.45, 1, 0, 6),           // 九音跳跃
                                       step(1.0, 0.45, 4, 0, 5), 
                                       step(1.5, 0.45, 2, 0, 7) });        // 错位落脚
    mediaVlogFlow.styleTags.addArray({ "pop", "folk", "c-pop" });
    mediaVlogFlow.moodTags.addArray({ "happy", "warm", cn(u8"明亮"), cn(u8"治愈"), cn(u8"温馨") });
    mediaVlogFlow.regionTags.addArray({ "global" });
    mediaVlogFlow.formFit.addArray({ "verse", "chorus" });
    mediaVlogFlow.familyId = "GuitarPickingFamily";
    mediaVlogFlow.complexity = 2;
    mediaVlogFlow.energy = 2;
    patterns.push_back(mediaVlogFlow);

    // 2. 网易云EMO：深夜微观抑郁挂留 (ID: 181)
    auto mediaEmoSigh = makePattern("media_emo_181", cn(u8"自媒体：深夜网易云EMO微观挂留"), "Midnight Emo Sus-2 Solitary Sigh",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Quarter, 4.0, // 大时值低流速，死扣九音与内声部摩擦，模拟孤独与叹息
                                    { step(0.0, 3.9, 0, -1, 6, true),     // 深夜底盘
                                      step(1.0, 1.9, 1, 0, 4, false),     // 1拍突入的二度九音摩擦（不连着弹，长延音）
                                      step(1.0, 1.9, 2, 0, 4, false), 
                                      step(3.0, 0.9, 4, 0, 5) });         // 虚无中模糊解决
    mediaEmoSigh.styleTags.addArray({ "ambient", "ballad", cn(u8"挂留") });
    mediaEmoSigh.moodTags.addArray({ "calm", cn(u8"孤独"), cn(u8"伤感"), cn(u8"冷酷") });
    mediaEmoSigh.regionTags.addArray({ "global" });
    mediaEmoSigh.functionFit.addArray({ "t", "v" });
    mediaEmoSigh.formFit.addArray({ "intro", "outro" });
    mediaEmoSigh.familyId = "GuqinSparseFamily"; // 极限留白
    mediaEmoSigh.complexity = 2;
    mediaEmoSigh.energy = 1;
    patterns.push_back(mediaEmoSigh);

    // 3. ACG热血：二次元爆发高燃16分切分 (ID: 190)
    auto gameAcgBurn = makePattern("game_acg_burn_190", cn(u8"游戏：二次元高燃ACG爆发切分线"), "Anime J-Rock High-Energy Fire Line",
                                   ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 4.0, // 4拍极限速度，右手连续弱拍切分大跳，能量瞬间炸裂
                                   { step(0.0, 0.2, 0, -2, 14, true),    // 左手超重低音砸
                                     step(0.25, 0.15, 4, -1, 11), 
                                     step(0.5, 0.15, 0, 0, 10, false),   // 右手突发后半拍咬合
                                     step(0.75, 0.15, 7, 1, 12),         // 飙向极高空
                                     step(1.0, 0.2, 2, -1, 11, true), 
                                     step(1.25, 0.15, 4, 0, 9), 
                                     step(1.5, 0.15, 7, 1, 13),          // 极限尖锐冠音
                                     step(1.75, 0.15, 1, 1, 10), 
                                     step(2.0, 0.2, 0, -2, 13, true), 
                                     step(2.5, 0.15, 4, 0, 9), 
                                     step(2.75, 0.15, 7, 1, 12), 
                                     step(3.0, 0.2, 2, 0, 10), 
                                     step(3.5, 0.15, 4, 1, 11), 
                                     step(3.75, 0.15, 1, 0, 8) });
    gameAcgBurn.styleTags.addArray({ "rock", "edm", "epic", cn(u8"和声小调") });
    gameAcgBurn.moodTags.addArray({ "energetic", cn(u8"紧张"), cn(u8"动作"), cn(u8"震撼") });
    gameAcgBurn.regionTags.addArray({ "global" });
    gameAcgBurn.functionFit.addArray({ "t", "T", "S", "D" });
    gameAcgBurn.formFit.addArray({ "chorus", "bridge" });
    gameAcgBurn.familyId = "HybridEpicStringFamily";
    gameAcgBurn.complexity = 5;
    gameAcgBurn.energy = 5; // 极限热血能级
    patterns.push_back(gameAcgBurn);

    // 4. 像素冒险：8-Bit红白机复古怀旧脉冲 (ID: 191)
    auto gamePixelRetro = makePattern("game_pixel_191", cn(u8"游戏：8Bit红白机复古像素脉冲"), "Retro 8-Bit Pixel Clockwork Pulse",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 2.0, // 2拍硬核对称循环，纯五四度无三音架构，极其强烈的极简红白机感
                                      { step(0.0, 0.15, 0, -1, 12, true),  // 经典方波地基
                                        step(0.25, 0.1, 7, 0, 9),          // 机械纯五度
                                        step(0.5, 0.1, 4, 0, 8),           // 纯四/五度交叠
                                        step(0.75, 0.1, 7, 0, 9), 
                                        step(1.0, 0.15, 0, 1, 11, true),   // 八度高空跳跃
                                        step(1.25, 0.1, 4, 0, 8), 
                                        step(1.5, 0.15, 7, 1, 10) });
    gamePixelRetro.styleTags.addArray({ "edm", cn(u8"挂留") });
    gamePixelRetro.moodTags.addArray({ "happy", "energetic", cn(u8"明亮"), cn(u8"欢快") });
    gamePixelRetro.regionTags.addArray({ "global" });
    gamePixelRetro.functionFit.addArray({ "T", "t" });
    gamePixelRetro.familyId = "ElectronicArpFamily";
    gamePixelRetro.complexity = 3;
    gamePixelRetro.energy = 4;
    patterns.push_back(gamePixelRetro);

    // 5. 魔法童话：迪士尼晶莹八音盒 (ID: 200)
    auto childMagicBox = makePattern("child_magic_200", cn(u8"童话：迪士尼魔法璀璨八音盒"), "Disney Magic Shimmering Musicbox",
                                     ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 4.0, // 4拍极高音区高速滚奏，模拟仙女棒金粉、星光倾泻
                                     { step(0.0, 0.15, 7, 2, 8, true),     // 起始点在太空极高音区（Octave +2）
                                       step(0.25, 0.1, 4, 2, 6), 
                                       step(0.5, 0.1, 2, 2, 5), 
                                       step(0.75, 0.1, 1, 1, 7),           // 九音折射
                                       step(1.0, 0.15, 0, 1, 9), 
                                       step(1.25, 0.1, 4, 1, 6), 
                                       step(1.5, 0.1, 2, 1, 5), 
                                       step(1.75, 0.1, 7, 2, 7), 
                                       step(2.0, 0.15, 4, 2, 9, true),     // 第二波反弹
                                       step(2.25, 0.1, 1, 1, 6), 
                                       step(2.5, 0.1, 0, 1, 7), 
                                       step(2.75, 0.1, 4, 1, 5), 
                                       step(3.0, 0.15, 2, 1, 8), 
                                       step(3.25, 0.1, 7, 2, 6), 
                                       step(3.5, 0.1, 4, 1, 5), 
                                       step(3.75, 0.1, 1, 1, 7) });
    childMagicBox.styleTags.addArray({ "ambient", "classical", cn(u8"大调") });
    childMagicBox.moodTags.addArray({ "happy", "hopeful", cn(u8"明亮"), cn(u8"梦幻"), cn(u8"治愈") });
    childMagicBox.regionTags.addArray({ "global" });
    childMagicBox.functionFit.addArray({ "T", "S" });
    childMagicBox.formFit.addArray({ "intro", "verse" });
    childMagicBox.familyId = "ElectronicArpFamily"; // 极高频闪烁
    childMagicBox.complexity = 4;
    childMagicBox.energy = 2;
    patterns.push_back(childMagicBox);

    // 6. 温馨童谣：旋转木马浪漫华尔兹 (ID: 201)
    auto childCarouselWaltz = makePattern("child_waltz_201", cn(u8"童话：旋转木马浪漫三拍圆舞曲"), "Carousel Carousel Romantic Waltz",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Mixed, 3.0, // 严格的标准3/4拍，1打底，2和3中轴同击，童谣标配
                                          { step(0.0, 0.95, 0, -1, 10, true),   // 1拍：重底座
                                            step(1.0, 0.45, 4, 0, 6, false),   // 2拍：中位同击双音（恰）
                                            step(1.0, 0.45, 7, 0, 6, false), 
                                            step(2.0, 0.45, 2, 0, 7, false),   // 3拍：中位同击双音（恰）
                                            step(2.0, 0.45, 4, 1, 7, false) });
    childCarouselWaltz.styleTags.addArray({ "classical", "folk", "3/4" });
    childCarouselWaltz.moodTags.addArray({ "warm", "romantic", cn(u8"温馨"), cn(u8"柔美") });
    childCarouselWaltz.regionTags.addArray({ "global" });
    childCarouselWaltz.functionFit.addArray({ "T", "t", "S", "D" });
    childCarouselWaltz.formFit.addArray({ "verse", "chorus" });
    childCarouselWaltz.familyId = "CPopBalladPianoFamily";
    childCarouselWaltz.complexity = 2;
    childCarouselWaltz.energy = 2;
    patterns.push_back(childCarouselWaltz);

    // 7. 高端商业：企业发布会高格局流线 (ID: 210)
    auto corpLuxuryFlow = makePattern("corp_luxury_210", cn(u8"商业：奢华高端企业形象发布流线"), "Premium Elite Corporate Luxury Flow",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 极宽声部开放排列，带有强烈的“科技先锋、精英格局、未来可期”质感
                                      { step(0.0, 0.2, 0, -1, 12, true),   // 大气资本托底
                                        step(0.5, 0.15, 4, 0, 5), 
                                        step(1.0, 0.2, 1, 0, 7),           // 精英高级九音点缀
                                        step(1.5, 0.15, 7, 1, 6), 
                                        step(2.0, 0.2, 2, 0, 8, true),     // 中轴上扬
                                        step(2.5, 0.15, 4, 1, 6), 
                                        step(3.0, 0.2, 7, 1, 7), 
                                        step(3.5, 0.15, 1, 2, 9) });       // 冲向高级的未来地平线
    corpLuxuryFlow.styleTags.addArray({ "pop", "ballad", cn(u8"大调"), cn(u8"九和弦") });
    corpLuxuryFlow.moodTags.addArray({ "hopeful", cn(u8"大气"), cn(u8"庄严"), cn(u8"温暖") });
    corpLuxuryFlow.regionTags.addArray({ "global" });
    corpLuxuryFlow.functionFit.addArray({ "T", "S" });
    corpLuxuryFlow.formFit.addArray({ "chorus", "pre-chorus" });
    corpLuxuryFlow.familyId = "CPopBalladPianoFamily";
    corpLuxuryFlow.complexity = 3;
    corpLuxuryFlow.energy = 3;
    patterns.push_back(corpLuxuryFlow);

    // 8. 宏大纪实：丝绸之路悲壮大漠文旅 (ID: 220)
    auto histDesertLine = makePattern("hist_desert_220", cn(u8"历史：河西走廊大漠悲壮纪实线"), "Silk Road Desolate Historical Epic",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 极其沉重的历史厚重感，悲壮大片，一眼万年
                                      { step(0.0, 0.45, 0, -2, 14, true),  // 黄土大漠重砸地基
                                        step(0.5, 0.15, 7, -1, 10),        // 苍凉空洞的纯五度
                                        step(1.0, 0.45, 4, -1, 11), 
                                        step(1.5, 0.15, 3, 0, 9),          // 绝望凄美的小三度色彩切入
                                        step(2.0, 0.45, 0, -1, 12, true),  // 历史车轮滚滚向前
                                        step(2.5, 0.15, 7, 0, 11), 
                                        step(3.0, 0.45, 1, 0, 10),         // 离调悲壮九音
                                        step(3.5, 0.15, 2, 1, 8) });
    histDesertLine.styleTags.addArray({ "epic", "ambient", cn(u8"和声小调"), "Phrygian" });
    histDesertLine.moodTags.addArray({ "mysterious", cn(u8"庄严"), cn(u8"大气"), cn(u8"震撼"), cn(u8"伤感") });
    histDesertLine.regionTags.addArray({ "chinese", cn(u8"甘肃"), cn(u8"青海") }); // 完美覆盖大西北文旅
    histDesertLine.functionFit.addArray({ "t", "D", "bII" });
    histDesertLine.formFit.addArray({ "verse", "bridge" });
    histDesertLine.familyId = "HybridEpicStringFamily";
    histDesertLine.complexity = 4;
    histDesertLine.energy = 4;
    patterns.push_back(histDesertLine);

    // ========================================================================
    // 新增补充：都市流行、时尚奢华与摩登律动核心库 (Urban Pop, Fashion & Neo-Soul)
    // ========================================================================

    // 1. 慵懒午后：Neo-Soul丝绒切分摩擦 (针对标签：都市流行、时尚、Neo-Soul、慵懒、小资)
    auto urbanNeoSoul = makePattern("urban_soul_230", cn(u8"都市：Neo-Soul丝绒后半拍切分"), "Urban Neo-Soul Velvet Laid-Back Sync",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍循环，死咬16分音符后半拍，产生极其慵懒时尚的摇摆感
                                    { step(0.0, 0.45, 0, -1, 9, true),    // 左手松弛根音
                                      step(0.25, 0.15, 6, 0, 6, false),   // 右手核心：七音与九音（Index 6 & 1）并排砸下！
                                      step(0.25, 0.15, 1, 1, 6, false),   // 形成高级的都市内声部二度摩擦
                                      step(0.75, 0.15, 4, 0, 7),          // 慵懒弱拍接住
                                      step(1.0, 0.45, 2, 0, 8), 
                                      step(1.5, 0.2, 0, 0, 6, false),     // 1.5拍突发华彩切分
                                      step(1.5, 0.2, 2, 1, 6, false), 
                                      step(2.0, 0.45, 0, -1, 10, true),   // 第二周期地基
                                      step(2.25, 0.15, 6, 0, 6), 
                                      step(2.5, 0.15, 1, 1, 7), 
                                      step(3.0, 0.45, 4, 0, 5), 
                                      step(3.25, 0.15, 2, 1, 8), 
                                      step(3.75, 0.15, 0, 0, 4) });
    urbanNeoSoul.styleTags.addArray({ "pop", "Jazz", "R&B", cn(u8"九和弦"), cn(u8"十一和弦") });
    urbanNeoSoul.moodTags.addArray({ "warm", "romantic", cn(u8"柔美"), cn(u8"治愈"), cn(u8"温馨") });
    urbanNeoSoul.regionTags.addArray({ "global" });
    urbanNeoSoul.formFit.addArray({ "verse", "chorus" });
    urbanNeoSoul.familyId = "JayStyleSyncopationFamily"; // 极度契合高端切分
    urbanNeoSoul.grooveAffinity = "Late"; // 故意稍微滞后的 Laid-back 律动偏好！
    urbanNeoSoul.complexity = 4;
    urbanNeoSoul.energy = 3;
    patterns.push_back(urbanNeoSoul);

    // 2. 摩登流线：时尚秀场奢华流线 (针对标签：秀场、奢华、商业广告、高端、大都会)
    auto urbanFashionRun = makePattern("urban_fashion_231", cn(u8"都市：时尚秀场奢华开放式流线"), "Modern Runway Luxury Open Flow",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 极其宽泛的 Open Voicing 左右手交织，像秀场丝绸般高级平滑
                                       { step(0.0, 0.45, 0, -1, 11, true),   // 大气左手开放五度打底
                                         step(0.0, 0.45, 4, -1, 10, false), 
                                         step(0.5, 0.15, 1, 0, 8),          // 右手精妙的九音（Index 1）切入
                                         step(0.75, 0.15, 6, 0, 7),          // 大七音
                                         step(1.0, 0.45, 2, 0, 9),          // 攀升
                                         step(1.5, 0.15, 4, 1, 6), 
                                         step(1.75, 0.15, 1, 1, 8),         // 冲向高位高光
                                         step(2.0, 0.45, 0, -1, 10, true), 
                                         step(2.5, 0.15, 6, 0, 8), 
                                         step(2.75, 0.15, 2, 1, 7), 
                                         step(3.0, 0.45, 4, 1, 6), 
                                         step(3.5, 0.15, 1, 0, 5), 
                                         step(3.75, 0.15, 0, 0, 4) });
    urbanFashionRun.styleTags.addArray({ "pop", "ballad", cn(u8"大调"), cn(u8"九和弦") });
    urbanFashionRun.moodTags.addArray({ "hopeful", cn(u8"明亮"), cn(u8"大气"), cn(u8"温馨") });
    urbanFashionRun.regionTags.addArray({ "global" });
    urbanFashionRun.functionFit.addArray({ "T", "S" });
    urbanFashionRun.formFit.addArray({ "chorus", "pre-chorus" });
    urbanFashionRun.familyId = "CPopBalladPianoFamily";
    urbanFashionRun.complexity = 3;
    urbanFashionRun.energy = 3;
    patterns.push_back(urbanFashionRun);

    // 3. 霓虹律动：CityPop复古摩登打字机 (针对标签：CityPop、霓虹灯、东京、复古、欢快)
    auto urbanCityPop = makePattern("urban_citypop_232", cn(u8"都市：CityPop霓虹复古八分切分"), "Retro Tokyo City-Pop Neon Pulse",
                                    ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 严格的8分音符，但左右手呈“波浪式交错踢踏”，天生自带日本竹内玛莉亚式的都市欢快复古范
                                    { step(0.0, 0.45, 0, -1, 12, true),   // 左手下落
                                      step(0.5, 0.45, 4, 0, 8, false),    // 右手反拍柱式弹跳 1
                                      step(0.5, 0.45, 6, 0, 8, false), 
                                      step(1.0, 0.45, 1, 0, 10, true),    // 左手跳跃九音
                                      step(1.5, 0.45, 2, 0, 7, false),    // 右手反拍柱式弹跳 2
                                      step(1.5, 0.45, 4, 1, 7, false), 
                                      step(2.0, 0.45, 0, -1, 11, true), 
                                      step(2.5, 0.45, 4, 0, 8, false), 
                                      step(3.0, 0.45, 6, 0, 9), 
                                      step(3.5, 0.45, 1, 1, 7) });
    urbanCityPop.styleTags.addArray({ "pop", "edm", "Jazz", cn(u8"大调") });
    urbanCityPop.moodTags.addArray({ "happy", "energetic", cn(u8"明亮"), cn(u8"欢快") });
    urbanCityPop.regionTags.addArray({ "global" });
    urbanCityPop.functionFit.addArray({ "T", "S", "D" });
    urbanCityPop.formFit.addArray({ "verse", "chorus" });
    urbanCityPop.familyId = "JayStyleSyncopationFamily";
    urbanCityPop.complexity = 3;
    urbanCityPop.energy = 4;
    patterns.push_back(urbanCityPop);

    // 4. 深夜小酌：微醺Lounge小调琶音 (针对标签：酒吧、微醺、深夜、爵士、性感、高级)
    auto urbanLoungeBar = makePattern("urban_lounge_233", cn(u8"都市：深夜酒廊微醺爵士小调"), "Midnight Lounge Bar Jazzy Minor",
                                      ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍连绵小调流线，带有极强的北欧都市冷冽感与慵懒性感
                                      { step(0.0, 0.2, 0, -1, 8, true),    // 忧郁底盘
                                        step(0.25, 0.15, 3, 0, 5),         // 小三度色彩
                                        step(0.5, 0.15, 1, 0, 7),          // 九音冷冽摩擦
                                        step(0.75, 0.15, 6, 0, 6),         // 小七音锁死
                                        step(1.0, 0.2, 4, 0, 7), 
                                        step(1.25, 0.15, 1, 1, 8),         // 高空悬浮
                                        step(1.5, 0.15, 3, 1, 9), 
                                        step(1.75, 0.15, 6, 1, 8), 
                                        step(2.0, 0.2, 0, -1, 9, true),    // 循环中点
                                        step(2.5, 0.15, 1, 0, 6), 
                                        step(2.75, 0.15, 3, 0, 5), 
                                        step(3.0, 0.2, 4, 0, 7), 
                                        step(3.5, 0.15, 6, 0, 6), 
                                        step(3.75, 0.15, 1, 0, 4) });
    urbanLoungeBar.styleTags.addArray({ "ambient", "Jazz", cn(u8"自然小调"), cn(u8"九和弦") });
    urbanLoungeBar.moodTags.addArray({ "calm", "mysterious", cn(u8"孤独"), cn(u8"柔美") });
    urbanLoungeBar.regionTags.addArray({ "global" });
    urbanLoungeBar.functionFit.addArray({ "t", "v", "s" });
    urbanLoungeBar.formFit.addArray({ "intro", "verse" });
    urbanLoungeBar.familyId = "CPopBalladPianoFamily";
    urbanLoungeBar.pedalFriendly = true;
    urbanLoungeBar.complexity = 4;
    urbanLoungeBar.energy = 2;
    patterns.push_back(urbanLoungeBar);

    // 5. 律动香水：当代R&B流线推搡 (针对标签：R&B、欧美流行、时尚、动感、香水)
    auto urbanRnbPerfume = makePattern("urban_rnb_234", cn(u8"都市：当代R&B香水流线推搡"), "Contemporary R&B Perfume Glide",
                                       ArpeggioDirection::Up, ArpeggioDensity::Sixteenth, 2.0, // 2拍高频短循环。右手声部先做极速下坠，随即突发大跳跃解决，模拟欧美R&B顶级天后编曲的动态
                                       { step(0.0, 0.1, 0, -1, 13, true),   // 重击
                                         step(0.125, 0.05, 6, 1, 10),       // 突发16分音符中段的1/32拍错位碎滑音
                                         step(0.175, 0.05, 4, 1, 9), 
                                         step(0.25, 0.1, 1, 0, 8),          // 下沉九音
                                         step(0.5, 0.15, 2, 0, 7, true),    // 弱拍正拍卡点
                                         step(0.75, 0.1, 6, 1, 11),         // 瞬间飙向超高空极亮冠音！
                                         step(1.0, 0.1, 4, 0, 9), 
                                         step(1.25, 0.1, 1, 1, 10), 
                                         step(1.5, 0.15, 0, 0, 8) });
    urbanRnbPerfume.styleTags.addArray({ "rock", "edm", "Jazz", "R&B" });
    urbanRnbPerfume.moodTags.addArray({ "energetic", cn(u8"紧张"), cn(u8"动作"), cn(u8"柔美") });
    urbanRnbPerfume.regionTags.addArray({ "global" });
    urbanRnbPerfume.familyId = "ElectronicArpFamily";
    urbanRnbPerfume.grooveAffinity = "Early"; // 极具进攻性
    urbanRnbPerfume.complexity = 5;
    urbanRnbPerfume.energy = 4;
    patterns.push_back(urbanRnbPerfume);

    // 6. 独立摩登：知性半音内声部爬行 (针对标签：独立音乐、知性、轻奢、唯美、叙事)
    auto urbanLineCliche = makePattern("urban_cliche_235", cn(u8"都市：知性轻奢半音内声部爬行"), "Intellectual Urban Line-Cliche Walk",
                                       ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4拍标准8分，亮点在于左手死守地基，右手内声部做半音阶平滑下滑（Line Cliché），极度知性优雅
                                       { step(0.0, 3.9, 0, -1, 6, true),    // 稳固左手
                                         step(0.0, 0.9, 4, 0, 5, false),    // 右手内声部核心 1
                                         step(1.0, 0.9, 3, 0, 5, false),    // 灵魂核心：半音阶向下挪动一格（Index 4 -> 3 摩擦）
                                         step(2.0, 0.9, 2, 0, 5, false),    // 继续平滑向下走一格（Index 2）
                                         step(3.0, 0.9, 1, 0, 4, false),    // 最终落脚在高级九音上
                                         step(0.5, 0.45, 6, 0, 7),          // 外层高音大七度微光点缀
                                         step(1.5, 0.45, 6, 0, 7), 
                                         step(2.5, 0.45, 6, 0, 7), 
                                         step(3.5, 0.45, 6, 0, 6) });
    urbanLineCliche.styleTags.addArray({ "classical", "ambient", cn(u8"大调"), cn(u8"七和弦") });
    urbanLineCliche.moodTags.addArray({ "calm", "warm", cn(u8"治愈"), cn(u8"柔美"), cn(u8"温馨") });
    urbanLineCliche.regionTags.addArray({ "global" });
    urbanLineCliche.functionFit.addArray({ "t", "T" });
    urbanLineCliche.familyId = "GuqinSparseFamily";
    urbanLineCliche.complexity = 4;
    urbanLineCliche.energy = 2;
    patterns.push_back(urbanLineCliche);

    // 7. 资本风暴：未来轻奢华丽全音域矩阵 (针对标签：豪车大片、大都会金融中心、极度高端、震撼)
    auto urbanPremiumEpic = makePattern("urban_premium_236", cn(u8"都市：大都会轻奢华丽全音域瀑布"), "Metropolitan Premium Epic Cascade",
                                        ArpeggioDirection::Down, ArpeggioDensity::Sixteenth, 4.0, // 全16分全音域下行开放抛线，将大调九和弦切碎从天际线砸向地面，展现极度自信、掌控一切的顶级高级感
                                        { step(0.0, 0.45, 0, -2, 13, true),  // 摩天大楼第一声轰鸣地基
                                          step(0.5, 0.15, 6, 2, 11),         // 从极高空（Octave +2）开启大七九和弦碎裂下行
                                          step(0.75, 0.15, 4, 1, 9), 
                                          step(1.0, 0.45, 1, 1, 10, true),   // 精英九音重击中继
                                          step(1.5, 0.15, 0, 0, 8), 
                                          step(1.75, 0.15, 6, 0, 7), 
                                          step(2.0, 0.45, 4, -1, 12, true),  // 跌入中低音核心
                                          step(2.5, 0.15, 1, -1, 9), 
                                          step(2.75, 0.15, 0, -1, 8), 
                                          step(3.0, 0.15, 6, -1, 11), 
                                          step(3.25, 0.15, 4, -1, 10), 
                                          step(3.5, 0.45, 1, -2, 12, true) }); // 最终沉入极深资本核心
    urbanPremiumEpic.styleTags.addArray({ "epic", "pop", cn(u8"大调"), cn(u8"九和弦") });
    urbanPremiumEpic.moodTags.addArray({ "hopeful", cn(u8"庄严"), cn(u8"大气"), cn(u8"震撼") });
    urbanPremiumEpic.regionTags.addArray({ "global" });
    urbanPremiumEpic.formFit.addArray({ "chorus", "bridge" });
    urbanPremiumEpic.familyId = "HybridEpicStringFamily";
    urbanPremiumEpic.complexity = 5;
    urbanPremiumEpic.energy = 5; // 顶级能量
    patterns.push_back(urbanPremiumEpic);

    // 8. 摩登摩擦：Neo-Soul内声部二度音程揉捏 (针对：变音、属变音、复杂进行)
    auto urbanVibeFriction = makePattern("urban_friction_237", cn(u8"都市：高级变音内声部二度摩擦"), "Modern Altered Chord Voice Friction",
                                         ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 4拍循环，专门通过Index 1(九音)和Index 2(三音)的极近距离排布，咬碎属变音的苦涩感
                                         { step(0.0, 0.45, 0, -1, 11, true),   // 坚实根音
                                           step(0.25, 0.15, 1, 0, 7, false),   // 核心二度摩擦点启动（让九音和三音同时在弱拍发声！）
                                           step(0.25, 0.15, 2, 0, 7, false), 
                                           step(0.5, 0.15, 6, 0, 8),           // 大七音折射
                                           step(0.75, 0.15, 4, 0, 6), 
                                           step(1.0, 0.45, 0, 0, 9, true),     // 中轴上行
                                           step(1.5, 0.2, 1, 1, 8, false),     // 1.5拍高空突发变音咬合
                                           step(1.5, 0.2, 3, 1, 8, false), 
                                           step(2.0, 0.45, 2, -1, 10, true), 
                                           step(2.5, 0.15, 6, 0, 7), 
                                           step(2.75, 0.15, 4, 0, 6), 
                                           step(3.0, 0.45, 1, 0, 8), 
                                           step(3.5, 0.15, 0, 1, 9) });
    urbanVibeFriction.styleTags.addArray({ "Jazz", "R&B", cn(u8"九和弦"), cn(u8"减五度") });
    urbanVibeFriction.moodTags.addArray({ "mysterious", cn(u8"孤独"), cn(u8"柔美"), cn(u8"冷酷") });
    urbanVibeFriction.familyId = "JayStyleSyncopationFamily";
    urbanVibeFriction.grooveAffinity = "Late"; // 放松的后轴摇摆
    urbanVibeFriction.complexity = 5; // 高复杂度
    urbanVibeFriction.energy = 3;
    patterns.push_back(urbanVibeFriction);

    // 9. 霓虹浪潮：CityPop反拍双音切分跳跃 (针对：日系经典、借调、离调进行)
    auto urbanNeonWave = makePattern("urban_neon_238", cn(u8"都市：CityPop霓虹反拍双音浪潮"), "Retro Neon Wave Off-Beat Stabs",
                                     ArpeggioDirection::Smart, ArpeggioDensity::Eighth, 4.0, // 4拍标准8分，左手大跨度单音，右手疯狂咬合“后半拍（Off-beat）”双音，复古高级感炸裂
                                     { step(0.0, 0.45, 0, -1, 13, true),   // 左手拍头
                                       step(0.5, 0.45, 4, 0, 9, false),    // 右手反拍：纯正的都市推搡 Stabs！
                                       step(0.5, 0.45, 6, 1, 9, false), 
                                       step(1.0, 0.45, 7, -1, 11),         // 左手转位过渡
                                       step(1.5, 0.45, 1, 0, 10, false),   // 右手再次反拍推搡（咬住高级九音）
                                       step(1.5, 0.45, 4, 0, 10, false), 
                                       step(2.0, 0.45, 0, -1, 12, true), 
                                       step(2.5, 0.45, 4, 1, 9, false), 
                                       step(2.5, 0.45, 6, 1, 9, false), 
                                       step(3.0, 0.45, 2, 0, 8), 
                                       step(3.5, 0.45, 1, 0, 10) });
    urbanNeonWave.styleTags.addArray({ "pop", "edm", "Jazz", cn(u8"大调") });
    urbanNeonWave.moodTags.addArray({ "happy", "energetic", cn(u8"明亮"), cn(u8"欢快") });
    urbanNeonWave.familyId = "JayStyleSyncopationFamily";
    urbanNeonWave.complexity = 4;
    urbanNeonWave.energy = 4;
    patterns.push_back(urbanNeonWave);

    // 10. 奢华流体：大都会金融中心全音域华彩 (针对：中音突变、大宣传片、高端豪车)
    auto urbanLuxuryCascade = makePattern("urban_cascade_239", cn(u8"都市：大都会轻奢全音域悬浮华彩"), "Metropolitan Premium Shimmering Cascade",
                                          ArpeggioDirection::Smart, ArpeggioDensity::Sixteenth, 4.0, // 右手在高空做不着地的级进回旋（Octave +1 到 +2），左手大开放五度托底，极其大气华贵
                                          { step(0.0, 0.45, 0, -1, 12, true),  // 左手宏大五度
                                            step(0.0, 0.45, 4, -1, 10, false), 
                                            step(0.5, 0.15, 6, 1, 9),          // 右手高空璀璨切入
                                            step(0.75, 0.15, 1, 2, 8),         // 飙向极高区九音
                                            step(1.0, 0.2, 4, 1, 7), 
                                            step(1.25, 0.15, 2, 1, 6), 
                                            step(1.5, 0.15, 1, 1, 8), 
                                            step(1.75, 0.15, 6, 2, 9), 
                                            step(2.0, 0.45, 4, 0, 11, true),   // 中轴承接
                                            step(2.5, 0.15, 1, 1, 8), 
                                            step(2.75, 0.15, 0, 1, 7), 
                                            step(3.0, 0.2, 6, 0, 6), 
                                            step(3.5, 0.15, 4, 1, 8), 
                                            step(3.75, 0.15, 1, 2, 10) });     // 高位收尾
    urbanLuxuryCascade.styleTags.addArray({ "epic", "pop", "ballad", cn(u8"九和弦") });
    urbanLuxuryCascade.moodTags.addArray({ "hopeful", cn(u8"大气"), cn(u8"庄严"), cn(u8"震撼"), cn(u8"温馨") });
    urbanLuxuryCascade.formFit.addArray({ "chorus", "pre-chorus" });
    urbanLuxuryCascade.familyId = "CPopBalladPianoFamily";
    urbanLuxuryCascade.complexity = 5;
    urbanLuxuryCascade.energy = 4;
    patterns.push_back(urbanLuxuryCascade);





    return patterns;
}

bool ArpeggioPatternLibrary::findPatternById(const juce::String& id, ArpeggioPattern& outPattern)
{
    for (const auto& pattern : getBuiltInPatterns())
    {
        if (pattern.id == id)
        {
            outPattern = pattern;
            return true;
        }
    }

    return false;
}
