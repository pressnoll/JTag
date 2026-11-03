#include "PluginEditor.h"

namespace TagForge {

TagForgeAudioProcessorEditor::TagForgeAudioProcessorEditor(TagForgeAudioProcessor& p)
    : AudioProcessorEditor(&p), mProcessor(p) {
    setSize(540, 400);

    // Title
    mTitleLabel.setText("JTag", juce::dontSendNotification);
    mTitleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    mTitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff3f4f6));
    addAndMakeVisible(mTitleLabel);

    // Voice Selector
    mVoiceLabel.setText("Voice", juce::dontSendNotification);
    mVoiceLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    mVoiceLabel.setColour(juce::Label::textColourId, juce::Colour(0xff9ca3af));
    addAndMakeVisible(mVoiceLabel);

    for (size_t i = 0; i < sizeof(VOICE_PROFILES) / sizeof(VOICE_PROFILES[0]); ++i)
        mVoiceSelector.addItem(VOICE_PROFILES[i].name, static_cast<int>(i) + 1);

    mVoiceSelector.setSelectedId(static_cast<int>(mProcessor.getVoicePreset()) + 1, juce::dontSendNotification);
    mVoiceSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1c1e26));
    mVoiceSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff2a2d3a));
    mVoiceSelector.setColour(juce::ComboBox::textColourId, juce::Colour(0xfff3f4f6));
    mVoiceSelector.onChange = [this]() {
        int selected = mVoiceSelector.getSelectedId() - 1;
        if (selected >= 0)
            mProcessor.setVoicePreset(static_cast<VoicePreset>(selected));
    };
    addAndMakeVisible(mVoiceSelector);

    // Text & Generate Button
    mTextInput.setText(mProcessor.getText());
    mTextInput.setFont(juce::Font(15.0f));
    mTextInput.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff1c1e26));
    mTextInput.setColour(juce::TextEditor::textColourId, juce::Colour(0xfff3f4f6));
    mTextInput.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff2a2d3a));
    mTextInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xff6366f1));
    mTextInput.onReturnKey = [this]() { mProcessor.setText(mTextInput.getText()); };
    addAndMakeVisible(mTextInput);

    mGenerateButton.onClick = [this]() { mProcessor.setText(mTextInput.getText()); };
    mGenerateButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff6366f1));
    mGenerateButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(mGenerateButton);

    // Auto Sync Toggle & Manual Slider
    mAutoSyncToggle.setToggleState(mProcessor.isAutoSync(), juce::dontSendNotification);
    mAutoSyncToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffd1d5db));
    mAutoSyncToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff6366f1));
    mAutoSyncToggle.onClick = [this]() {
        mProcessor.setAutoSync(mAutoSyncToggle.getToggleState());
        updateUIFromProcessor();
    };
    addAndMakeVisible(mAutoSyncToggle);

    mManualBpmSlider.setRange(40.0, 260.0, 1.0);
    mManualBpmSlider.setValue(mProcessor.getActiveBpm(), juce::dontSendNotification);
    mManualBpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mManualBpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 68, 24);
    mManualBpmSlider.setTextValueSuffix(" BPM");
    mManualBpmSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff6366f1));
    mManualBpmSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff818cf8));
    mManualBpmSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1c1e26));
    mManualBpmSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff1c1e26));
    mManualBpmSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff2a2d3a));
    mManualBpmSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff3f4f6));
    mManualBpmSlider.onValueChange = [this]() {
        if (!mProcessor.isAutoSync())
            mProcessor.setManualBpm(mManualBpmSlider.getValue());
    };
    addAndMakeVisible(mManualBpmSlider);

    // Waveform & Export
    addAndMakeVisible(mWaveformView);
    mWaveformView.onPlayRequested = [this]() { mProcessor.triggerPlayback(); };

    mProcessor.onBufferUpdated = [this]() {
        juce::MessageManager::callAsync([this]() {
            mWaveformView.setAudioBuffer(
                mProcessor.getProcessedBuffer(),
                mProcessor.getSampleRate(),
                mProcessor.getActiveBpm()
            );
        });
    };

    updateUIFromProcessor();
    mProcessor.onBufferUpdated();
    startTimerHz(30);
}

TagForgeAudioProcessorEditor::~TagForgeAudioProcessorEditor() {
    stopTimer();
    mProcessor.onBufferUpdated = nullptr;
}

void TagForgeAudioProcessorEditor::timerCallback() {
    mWaveformView.setPlaybackPosition(mProcessor.getPlaybackSamplePosition());
    if (mProcessor.isAutoSync())
        mManualBpmSlider.setValue(mProcessor.getActiveBpm(), juce::dontSendNotification);
}

void TagForgeAudioProcessorEditor::updateUIFromProcessor() {
    mManualBpmSlider.setEnabled(!mProcessor.isAutoSync());
}

void TagForgeAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff13141b)); // Obsidian background
}

void TagForgeAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced(24, 20);
    mTitleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(14);

    auto voiceRow = area.removeFromTop(36);
    mVoiceLabel.setBounds(voiceRow.removeFromLeft(54));
    mVoiceSelector.setBounds(voiceRow);
    area.removeFromTop(12);

    auto textRow = area.removeFromTop(38);
    mGenerateButton.setBounds(textRow.removeFromRight(110));
    textRow.removeFromRight(10);
    mTextInput.setBounds(textRow);
    area.removeFromTop(14);

    auto bpmRow = area.removeFromTop(28);
    mAutoSyncToggle.setBounds(bpmRow.removeFromLeft(135));
    mManualBpmSlider.setBounds(bpmRow);
    area.removeFromTop(16);

    mWaveformView.setBounds(area);
}

} // namespace TagForge