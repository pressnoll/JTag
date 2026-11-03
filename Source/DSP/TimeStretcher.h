// TimeStretcher.h
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

namespace TagForge {

class TimeStretcher {
public:
    static juce::AudioBuffer<float> stretchBuffer(
        const juce::AudioBuffer<float>& input,
        int targetLengthSamples,
        double sampleRate
    );
};

} // namespace TagForge