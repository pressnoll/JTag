#include "WaveformDragComponent.h"

namespace TagForge {

WaveformDragComponent::WaveformDragComponent() {
    addAndMakeVisible(mPlayButton);
    mPlayButton.onClick = [this]() {
        if (onPlayRequested) onPlayRequested();
    };
    mPlayButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff282a36));
    mPlayButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff8f8f2));

    addAndMakeVisible(mExportButton);
    mExportButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff10b981)); // Emerald
    mExportButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff0b1310));
    mExportButton.setInterceptsMouseClicks(false, false);
}

void WaveformDragComponent::setAudioBuffer(const juce::AudioBuffer<float>& buffer, double sampleRate, double bpm) {
    mBuffer.makeCopyOf(buffer);
    mSampleRate = sampleRate;
    mBpm = bpm;
    mPlayheadPos = -1;
    exportBufferToTempWav();
    repaint();
}

void WaveformDragComponent::setPlaybackPosition(int samplePos) {
    mPlayheadPos = samplePos;
    repaint();
}

void WaveformDragComponent::exportBufferToTempWav() {
    if (mBuffer.getNumSamples() == 0) return;
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    mTempFile = tempDir.getChildFile("JTag_Vocal_Tag.wav");
    if (mTempFile.existsAsFile()) mTempFile.deleteFile();

    std::unique_ptr<juce::FileOutputStream> stream(mTempFile.createOutputStream());
    if (stream != nullptr) {
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(
            stream.get(), mSampleRate, (unsigned int)mBuffer.getNumChannels(), 24, {}, 0
        ));
        if (writer != nullptr) {
            stream.release();
            writer->writeFromAudioSampleBuffer(mBuffer, 0, mBuffer.getNumSamples());
        }
    }
}

void WaveformDragComponent::mouseDown(const juce::MouseEvent& e) {
    if (mExportButton.getBounds().contains(e.getPosition()) && mTempFile.existsAsFile()) {
        auto desktop = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                          .getChildFile("JTag_Vocal_Tag.wav");
        mTempFile.copyFileTo(desktop);
    }
}

void WaveformDragComponent::mouseDrag(const juce::MouseEvent& e) {
    if (e.getDistanceFromDragStart() > 5 && mTempFile.existsAsFile()) {
        juce::StringArray files { mTempFile.getFullPathName() };
        if (auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this))
            dragContainer->performExternalDragDropOfFiles(files, false, this);
    }
}

void WaveformDragComponent::resized() {
    auto area = getLocalBounds().reduced(14);
    auto bottomBar = area.removeFromBottom(34);
    mPlayButton.setBounds(bottomBar.removeFromLeft(96));
    mExportButton.setBounds(bottomBar.removeFromRight(110));
}

void WaveformDragComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff1c1e26));
    g.fillRoundedRectangle(bounds, 10.0f);
    g.setColour(juce::Colour(0xff2a2d3a));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 10.0f, 1.0f);

    int numSamples = mBuffer.getNumSamples();
    if (numSamples == 0) {
        g.setColour(juce::Colour(0xff6b7280));
        g.setFont(14.0f);
        g.drawText("Choose a voice and click Generate", getLocalBounds(), juce::Justification::centred, true);
        return;
    }

    auto waveBounds = bounds.reduced(18.0f, 16.0f);
    waveBounds.removeFromBottom(44.0f);
    float midY = waveBounds.getCentreY();
    float halfH = waveBounds.getHeight() * 0.44f;
    float width = waveBounds.getWidth();

    g.setColour(juce::Colour(0xff818cf8));
    const float* data = mBuffer.getReadPointer(0);
    int step = std::max(1, numSamples / (int)width);

    for (int x = 0; x < (int)width; x += 3) {
        int start = x * step;
        int end = std::min(start + step * 3, numSamples);
        float maxVal = 0.0f;
        for (int s = start; s < end; ++s) maxVal = std::max(maxVal, std::abs(data[s]));
        float h = std::max(2.0f, maxVal * halfH);
        float drawX = waveBounds.getX() + (float)x;
        g.drawLine(drawX, midY - h, drawX, midY + h, 1.8f);
    }

    if (mPlayheadPos >= 0 && mPlayheadPos < numSamples) {
        float playX = waveBounds.getX() + ((float)mPlayheadPos / numSamples) * width;
        g.setColour(juce::Colours::white);
        g.drawLine(playX, waveBounds.getY(), playX, waveBounds.getBottom(), 2.0f);
    }

    g.setColour(juce::Colour(0xff9ca3af));
    g.setFont(12.5f);
    g.drawText(juce::String::formatted("%.0f BPM", mBpm),
               getLocalBounds().reduced(120, 14).removeFromBottom(34),
               juce::Justification::centred, true);
}

} // namespace TagForge