#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/WaveformDragComponent.h"

namespace TagForge {

class TagForgeAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit TagForgeAudioProcessorEditor(TagForgeAudioProcessor&);
    ~TagForgeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    TagForgeAudioProcessor& mProcessor;

    juce::Label mTitleLabel;
    juce::Label mVoiceLabel;
    juce::ComboBox mVoiceSelector;
    juce::TextEditor mTextInput;
    juce::TextButton mGenerateButton { "Generate" };
    juce::ToggleButton mAutoSyncToggle { "Auto Sync BPM" };
    juce::Slider mManualBpmSlider;
    WaveformDragComponent mWaveformView;

    void updateUIFromProcessor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TagForgeAudioProcessorEditor)
};

} // namespace TagForge