#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace TagForge {

TagForgeAudioProcessor::TagForgeAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)) {
    renderVocalTag();
}

void TagForgeAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    mSampleRate = sampleRate;
    mSynthEngine.prepare(sampleRate);
    renderVocalTag();
}

void TagForgeAudioProcessor::releaseResources() {
    stopPlayback();
}

bool TagForgeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void TagForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    if (auto* playHead = getPlayHead()) {
        if (auto posOpt = playHead->getPosition()) {
            if (posOpt->getBpm().hasValue()) {
                double hostBpm = *posOpt->getBpm();
                if (std::abs(hostBpm - mHostBpm) > 0.05) {
                    mHostBpm = hostBpm;
                    if (mAutoSyncHost) {
                        mActiveBpm = mHostBpm;
                        renderVocalTag();
                    }
                }
            }
        }
    }

    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            triggerPlayback();
            break;
        }
    }

    if (!mIsPlaying.load())
        return;

    juce::ScopedLock sl(mBufferLock);
    int numSamples = buffer.getNumSamples();
    int tagLen = mProcessedBuffer.getNumSamples();
    int currentPos = mPlaybackSamplePos.load();

    if (currentPos >= tagLen) {
        mIsPlaying.store(false);
        mPlaybackSamplePos.store(-1);
        return;
    }

    int samplesToCopy = std::min(numSamples, tagLen - currentPos);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        int srcCh = std::min(ch, mProcessedBuffer.getNumChannels() - 1);
        buffer.copyFrom(ch, 0, mProcessedBuffer, srcCh, currentPos, samplesToCopy);
    }
    mPlaybackSamplePos.store(currentPos + samplesToCopy);
}

void TagForgeAudioProcessor::setText(const juce::String& text) {
    if (mCurrentText != text) {
        mCurrentText = text;
        renderVocalTag();
    }
}

void TagForgeAudioProcessor::setVoicePreset(VoicePreset preset) {
    if (mCurrentPreset != preset) {
        mCurrentPreset = preset;
        renderVocalTag();
    }
}

void TagForgeAudioProcessor::setAutoSync(bool enabled) {
    mAutoSyncHost = enabled;
    mActiveBpm = enabled ? mHostBpm : mManualBpm;
    renderVocalTag();
}

void TagForgeAudioProcessor::setManualBpm(double bpm) {
    mManualBpm = juce::jlimit(40.0, 260.0, bpm);
    if (!mAutoSyncHost) {
        mActiveBpm = mManualBpm;
        renderVocalTag();
    }
}

void TagForgeAudioProcessor::renderVocalTag() {
    juce::ScopedLock sl(mBufferLock);
    mRawBuffer = mSynthEngine.synthesize(mCurrentText.toStdString(), mCurrentPreset);

    double secPerBeat = 60.0 / std::max(40.0, mActiveBpm);
    double targetDurationSec = 2.0 * secPerBeat; // Snap phrase cleanly across 2 beats
    int targetSamples = static_cast<int>(targetDurationSec * mSampleRate);

    mProcessedBuffer = TimeStretcher::stretchBuffer(mRawBuffer, targetSamples, mSampleRate);

    if (onBufferUpdated)
        onBufferUpdated();
}

void TagForgeAudioProcessor::triggerPlayback() {
    mPlaybackSamplePos.store(0);
    mIsPlaying.store(true);
}

void TagForgeAudioProcessor::stopPlayback() {
    mIsPlaying.store(false);
    mPlaybackSamplePos.store(-1);
}

juce::AudioProcessorEditor* TagForgeAudioProcessor::createEditor() {
    return new TagForgeAudioProcessorEditor(*this);
}

void TagForgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::XmlElement xml("JTAG_SETTINGS");
    xml.setAttribute("text", mCurrentText);
    xml.setAttribute("voice", static_cast<int>(mCurrentPreset));
    xml.setAttribute("autoSync", mAutoSyncHost);
    xml.setAttribute("manualBpm", mManualBpm);
    copyXmlToBinary(xml, destData);
}

void TagForgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName("JTAG_SETTINGS")) {
        mCurrentText = xml->getStringAttribute("text", "Murda on the beat");
        mCurrentPreset = static_cast<VoicePreset>(xml->getIntAttribute("voice", 0));
        mAutoSyncHost = xml->getBoolAttribute("autoSync", true);
        mManualBpm = xml->getDoubleAttribute("manualBpm", 140.0);
        mActiveBpm = mAutoSyncHost ? mHostBpm : mManualBpm;
        renderVocalTag();
    }
}

} // namespace TagForge

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TagForge::TagForgeAudioProcessor();
}