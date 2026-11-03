#pragma once
#include <vector>
#include <string>
#include <cctype>
#include "FormantTables.h"

namespace TagForge {

struct PhonemeToken {
    PhonemeType type;
    float durationSec;
    bool isVoiced;
};

class PhonemeEngine {
public:
    static std::vector<PhonemeToken> textToPhonemes(const std::string& input) {
        std::vector<PhonemeToken> tokens;
        for (char c : input) {
            char lower = static_cast<char>(std::tolower(c));
            switch (lower) {
                case 'a': tokens.push_back({ PhonemeType::Vowel_AA, 0.12f, true }); break;
                case 'e': tokens.push_back({ PhonemeType::Vowel_EH, 0.11f, true }); break;
                case 'i': tokens.push_back({ PhonemeType::Vowel_IY, 0.11f, true }); break;
                case 'o': tokens.push_back({ PhonemeType::Vowel_OW, 0.13f, true }); break;
                case 'u': tokens.push_back({ PhonemeType::Vowel_UW, 0.12f, true }); break;
                case 's': case 'c': case 'z':
                    tokens.push_back({ PhonemeType::Consonant_S, 0.07f, false }); break;
                case 't': case 'd': case 'p': case 'b': case 'k':
                    tokens.push_back({ PhonemeType::Consonant_T, 0.05f, false }); break;
                case ' ':
                    tokens.push_back({ PhonemeType::Silence, 0.06f, false }); break;
                default:
                    if (std::isalpha(lower))
                        tokens.push_back({ PhonemeType::Vowel_EH, 0.08f, true });
                    break;
            }
        }
        if (tokens.empty())
            tokens.push_back({ PhonemeType::Vowel_AA, 0.25f, true });
        return tokens;
    }
};

} // namespace TagForge