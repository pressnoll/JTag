// VocalSynthEngine.cpp
#include "VocalSynthEngine.h"
#include <random>

namespace TagForge {

juce::AudioBuffer<float> VocalSynthEngine::synthesize(const std::string& text, VoicePreset preset) {
    const auto& profile = VOICE_PROFILES[static_cast<int>(preset)];
    auto tokens = PhonemeEngine::textToPhonemes(text);

    float totalSec = 0.0f;
    for (const auto& t : tokens) totalSec += t.durationSec;
    int totalSamples = std::max(1, static_cast<int>(totalSec * mSampleRate));

    juce::AudioBuffer<float> buffer(2, totalSamples);
    buffer.clear();

    mF1.reset(); mF2.reset(); mF3.reset();

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> noiseDist(-1.0f, 1.0f);

    float phase = 0.0f;
    float phaseInc = (profile.basePitchHz / static_cast<float>(mSampleRate));
    int writeIndex = 0;

    for (const auto& token : tokens) {
        int tokenSamples = static_cast<int>(token.durationSec * mSampleRate);
        auto formants = getFormants(token.type);

        mF1.setCoefficients(formants.f1 * profile.formantShift, 90.0f, mSampleRate);
        mF2.setCoefficients(formants.f2 * profile.formantShift, 110.0f, mSampleRate);
        mF3.setCoefficients(formants.f3 * profile.formantShift, 130.0f, mSampleRate);

        for (int i = 0; i < tokenSamples && writeIndex < totalSamples; ++i, ++writeIndex) {
            float excitation = 0.0f;
            if (token.isVoiced) {
                phase += phaseInc;
                if (phase >= 1.0f) phase -= 1.0f;
                excitation = (phase < 0.2f) ? (phase * 5.0f) : (1.0f - (phase - 0.2f) / 0.8f);
                if (profile.isRobot) excitation = (phase < 0.5f) ? 0.7f : -0.7f;
            } else if (token.type != PhonemeType::Silence) {
                excitation = noiseDist(rng) * 0.4f;
            }

            float sample = mF1.process(excitation) * 0.5f
                         + mF2.process(excitation) * 0.35f
                         + mF3.process(excitation) * 0.15f;

            sample = std::tanh(sample * profile.drive);
            buffer.setSample(0, writeIndex, sample);
            buffer.setSample(1, writeIndex, sample);
        }
    }
    return buffer;
}

} // namespace TagForge