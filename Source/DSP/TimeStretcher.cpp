// TimeStretcher.cpp
#include "TimeStretcher.h"

namespace TagForge {

juce::AudioBuffer<float> TimeStretcher::stretchBuffer(
    const juce::AudioBuffer<float>& input,
    int targetLengthSamples,
    double /*sampleRate*/
) {
    int srcLen = input.getNumSamples();
    int channels = input.getNumChannels();

    if (srcLen == 0 || targetLengthSamples <= 0)
        return juce::AudioBuffer<float>(channels, std::max(1, targetLengthSamples));

    juce::AudioBuffer<float> output(channels, targetLengthSamples);
    output.clear();

    double ratio = static_cast<double>(srcLen - 1) / static_cast<double>(targetLengthSamples - 1);

    for (int ch = 0; ch < channels; ++ch) {
        const float* src = input.getReadPointer(ch);
        float* dst = output.getWritePointer(ch);

        for (int i = 0; i < targetLengthSamples; ++i) {
            double srcIdx = i * ratio;
            int idxFloor = static_cast<int>(srcIdx);
            int idxCeil = std::min(idxFloor + 1, srcLen - 1);
            float frac = static_cast<float>(srcIdx - idxFloor);
            dst[i] = src[idxFloor] * (1.0f - frac) + src[idxCeil] * frac;
        }
    }
    return output;
}

} // namespace TagForge