#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace TagForge {

class WaveformDragComponent : public juce::Component {
public:
    WaveformDragComponent();
    ~WaveformDragComponent() override = default;

    void setAudioBuffer(const juce::AudioBuffer<float>& buffer, double sampleRate, double bpm);
    void setPlaybackPosition(int samplePos);

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;

    std::function<void()> onPlayRequested;

private:
    juce::AudioBuffer<float> mBuffer;
    double mSampleRate { 44100.0 };
    double mBpm { 140.0 };
    int mPlayheadPos { -1 };
    juce::File mTempFile;

    juce::TextButton mPlayButton { "Play" };
    juce::TextButton mExportButton { "Export" };

    void exportBufferToTempWav();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDragComponent)
};

} // namespace TagForge