#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Synthesis/VocalSynthEngine.h"
#include "DSP/TimeStretcher.h"

namespace TagForge {

class TagForgeAudioProcessor : public juce::AudioProcessor {
public:
    TagForgeAudioProcessor();
    ~TagForgeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "JTag"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setText(const juce::String& text);
    juce::String getText() const { return mCurrentText; }
    void setVoicePreset(VoicePreset preset);
    VoicePreset getVoicePreset() const { return mCurrentPreset; }

    void setAutoSync(bool enabled);
    bool isAutoSync() const { return mAutoSyncHost; }
    void setManualBpm(double bpm);
    double getActiveBpm() const { return mActiveBpm; }

    void renderVocalTag();
    void triggerPlayback();
    void stopPlayback();

    const juce::AudioBuffer<float>& getProcessedBuffer() const { return mProcessedBuffer; }
    int getPlaybackSamplePosition() const { return mPlaybackSamplePos.load(); }

    std::function<void()> onBufferUpdated;

private:
    double mSampleRate { 44100.0 };
    double mHostBpm { 120.0 };
    double mManualBpm { 140.0 };
    double mActiveBpm { 140.0 };
    bool mAutoSyncHost { true };

    juce::String mCurrentText { "Murda on the beat" };
    VoicePreset mCurrentPreset { VoicePreset::DeepTrap };

    VocalSynthEngine mSynthEngine;
    juce::AudioBuffer<float> mRawBuffer;
    juce::AudioBuffer<float> mProcessedBuffer;

    std::atomic<bool> mIsPlaying { false };
    std::atomic<int> mPlaybackSamplePos { -1 };
    juce::CriticalSection mBufferLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TagForgeAudioProcessor)
};

} // namespace TagForge