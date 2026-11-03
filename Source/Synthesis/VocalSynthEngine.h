// VocalSynthEngine.h
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include "FormantTables.h"
#include "PhonemeEngine.h"

namespace TagForge {

class VocalSynthEngine {
public:
    VocalSynthEngine() = default;
    void prepare(double sampleRate) { mSampleRate = sampleRate; }
    juce::AudioBuffer<float> synthesize(const std::string& text, VoicePreset preset);

private:
    double mSampleRate { 44100.0 };

    struct Resonator {
        float x1 { 0.0f }, x2 { 0.0f }, y1 { 0.0f }, y2 { 0.0f };
        float a0 { 0.0f }, a1 { 0.0f }, a2 { 0.0f }, b1 { 0.0f }, b2 { 0.0f };

        void setCoefficients(float freqHz, float bandwidthHz, double sr) {
            float w = 2.0f * 3.14159265f * (freqHz / static_cast<float>(sr));
            float bw = 2.0f * 3.14159265f * (bandwidthHz / static_cast<float>(sr));
            float r = 1.0f - (bw / 2.0f);
            b1 = 2.0f * r * std::cos(w);
            b2 = -r * r;
            a0 = 1.0f - r;
            a1 = 0.0f;
            a2 = 0.0f;
        }

        float process(float in) {
            float out = a0 * in + b1 * y1 + b2 * y2;
            y2 = y1;
            y1 = out;
            return out;
        }

        void reset() { x1 = x2 = y1 = y2 = 0.0f; }
    };

    Resonator mF1, mF2, mF3;
};

} // namespace TagForge