#pragma once
#include <string>

namespace TagForge {

enum class VoicePreset {
    DeepTrap = 0,
    FemaleHype,
    CyberRobot,
    RadioMegaphone,
    DemonBass,
    Arcade90s
};

struct VoiceProfile {
    const char* name;
    float basePitchHz;
    float formantShift;
    float resonanceQ;
    float drive;
    bool isRobot;
    bool isMegaphone;
};

static constexpr VoiceProfile VOICE_PROFILES[] = {
    { "Deep Trap Drop",    65.0f,  0.80f, 6.0f, 1.3f, false, false },
    { "Female Hype Tag",   220.0f, 1.25f, 4.5f, 1.0f, false, false },
    { "Cyber Robot",       110.0f, 1.00f, 8.0f, 1.1f, true,  false },
    { "Radio Megaphone",   140.0f, 1.10f, 5.0f, 2.2f, false, true  },
    { "Demon / Dark Bass", 48.0f,  0.65f, 7.5f, 2.0f, false, false },
    { "90s Arcade Bit",    160.0f, 1.15f, 9.0f, 1.5f, true,  false }
};

enum class PhonemeType { Vowel_AA, Vowel_IY, Vowel_UW, Vowel_EH, Vowel_OW, Consonant_S, Consonant_T, Silence };

struct FormantFreqs { float f1, f2, f3; };

inline FormantFreqs getFormants(PhonemeType p) {
    switch (p) {
        case PhonemeType::Vowel_AA: return { 730.0f, 1090.0f, 2440.0f };
        case PhonemeType::Vowel_IY: return { 270.0f, 2290.0f, 3010.0f };
        case PhonemeType::Vowel_UW: return { 300.0f, 870.0f,  2240.0f };
        case PhonemeType::Vowel_EH: return { 530.0f, 1840.0f, 2480.0f };
        case PhonemeType::Vowel_OW: return { 570.0f, 840.0f,  2410.0f };
        default:                    return { 500.0f, 1500.0f, 2500.0f };
    }
}

} // namespace TagForge