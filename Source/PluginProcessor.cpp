/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#define AVOID_REALLOCATING false

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RiseandfallAudioProcessor::RiseandfallAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
), thumbnailCache(5), thumbnail(256, formatManager, thumbnailCache)
#endif
{
    position = 0;
    numChannels = 0;
    formatManager.registerBasicFormats();
    soundTouch.setChannels(1); // always iterate over single channels
}

RiseandfallAudioProcessor::~RiseandfallAudioProcessor() = default;

//==============================================================================
const String RiseandfallAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool RiseandfallAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RiseandfallAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RiseandfallAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RiseandfallAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int RiseandfallAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int RiseandfallAudioProcessor::getCurrentProgram() {
    return 0;
}

void RiseandfallAudioProcessor::setCurrentProgram(int index) {
}

const String RiseandfallAudioProcessor::getProgramName(int index) {
    return {};
}

void RiseandfallAudioProcessor::changeProgramName(int index, const String &newName) {
}

//==============================================================================
void RiseandfallAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;
    soundTouch.setSampleRate(static_cast<uint>(sampleRate));
}

void RiseandfallAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    position = 0;
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool RiseandfallAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

#endif

void RiseandfallAudioProcessor::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages) {
    ScopedNoDenormals noDenormals;

    buffer.clear();
    midiMessages.clear();

    if (numChannels > 0 && !processing) {
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (int i = numChannels; i < totalNumOutputChannels; ++i) {
            buffer.clear(i, 0, buffer.getNumSamples());
        }

        int inputNumSamples = processedSampleBuffer.getNumSamples();
        auto bufferSamplesRemaining = static_cast<int>(inputNumSamples - position);
        int samplesThisTime = jmin(samplesPerBlock, bufferSamplesRemaining);

        for (int channel = 0; channel < numChannels; ++channel) {
            buffer.addFrom(channel, 0, processedSampleBuffer, channel, static_cast<int>(position), samplesThisTime,
                           0.1);
        }

        position += samplesThisTime;
        if (position >= inputNumSamples) {
            position = 0;
        }
    }
}

//==============================================================================
bool RiseandfallAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *RiseandfallAudioProcessor::createEditor() {
    return new RiseandfallAudioProcessorEditor(*this);
}

//==============================================================================
void RiseandfallAudioProcessor::getStateInformation(MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RiseandfallAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE

createPluginFilter() {
    return new RiseandfallAudioProcessor();
}

AudioSampleBuffer *RiseandfallAudioProcessor::getOriginalSampleBuffer() {
    return &originalSampleBuffer;
}

AudioThumbnail *RiseandfallAudioProcessor::getThumbnail() {
    return &thumbnail;
}

void RiseandfallAudioProcessor::applyTimeWarp(AudioSampleBuffer *buffer, int factor) {
    float realFactor = factor;
    if (realFactor < 0) {
        realFactor = 1 / abs(realFactor);
    }

    AudioSampleBuffer copy;
    copy.makeCopyOf(*buffer);

    soundTouch.setTempo(realFactor);
    soundTouch.setPitch(1.0);

    double ratio = soundTouch.getInputOutputSampleRatio();
    buffer->setSize(buffer->getNumChannels(), static_cast<int>(ceil(buffer->getNumSamples() * ratio)), false, true,
                    AVOID_REALLOCATING);

    for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
        soundTouch.putSamples(copy.getReadPointer(channel), static_cast<uint>(copy.getNumSamples()));
        soundTouch.receiveSamples(buffer->getWritePointer(channel), static_cast<uint>(buffer->getNumSamples()));
        soundTouch.clear();
    }
}

void RiseandfallAudioProcessor::applyDelay(AudioSampleBuffer *target, AudioSampleBuffer *base, float dampen,
                                           int delayTimeInSamples, int iteration) {
    base->applyGain(dampen);
    if (base->getMagnitude(0, base->getNumSamples()) > 0.05) {
        int currentDelayPosition = delayTimeInSamples * iteration;
        int length = target->getNumSamples() + delayTimeInSamples;
        target->setSize(target->getNumChannels(), length, true, true, true);

        for (int channel = 0; channel < target->getNumChannels(); channel++) {
            for (int i = 0; i < base->getNumSamples(); i++) {
                target->addSample(channel, i + currentDelayPosition, base->getSample(channel, i));
            }
        }

        applyDelay(target, base, dampen, delayTimeInSamples, iteration + 1);
    }
}

void RiseandfallAudioProcessor::applyReverb(AudioSampleBuffer *target, const char *fileName, const size_t fileSize) {
    AudioSampleBuffer copy;
    copy.makeCopyOf(*target);

    int processedSize = static_cast<int>(fileSize) + copy.getNumSamples() - 1;
    target->setSize(target->getNumChannels(), processedSize, false, true, AVOID_REALLOCATING);

    ProcessSpec processSpec = {sampleRate, static_cast<uint32>(processedSize),
                               static_cast<uint32>(target->getNumChannels())};

    convolution.prepare(processSpec);
    convolution.loadImpulseResponse(fileName, fileSize, false, true, 0);

    AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
    AudioBlock<float> audioBlockOut = AudioBlock<float>(*target);
    ProcessContextNonReplacing<float> processContext = ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);
    convolution.process(processContext);
}

void RiseandfallAudioProcessor::warp() {
    // RISE
    if (guiParams.riseTimeWarp != 0) {
        applyTimeWarp(&riseSampleBuffer, guiParams.riseTimeWarp);
    }

    // FALL
    if (guiParams.fallTimeWarp != 0) {
        applyTimeWarp(&fallSampleBuffer, guiParams.fallTimeWarp);
    }
}

void RiseandfallAudioProcessor::delayEffect() {
    AudioSampleBuffer delayBaseTempBuffer;
    float delayFeedbackNormalized = guiParams.delayFeedback / 100.0f;
    float delayTimeNormalized = guiParams.delayTime / 1000.0f;
    auto delayTimeInSamples = static_cast<int>(sampleRate * delayTimeNormalized);

    // RISE
    delayBaseTempBuffer.makeCopyOf(riseSampleBuffer);
    applyDelay(&riseSampleBuffer, &delayBaseTempBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);

    // FALL
    delayBaseTempBuffer.makeCopyOf(fallSampleBuffer);
    applyDelay(&fallSampleBuffer, &delayBaseTempBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
}

void RiseandfallAudioProcessor::reverbEffect() {
    const char *fileName = BinaryData::room_impulse_response_LBS_wav;
    const size_t fileSize = BinaryData::room_impulse_response_LBS_wavSize;

    // RISE
    applyReverb(&riseSampleBuffer, fileName, fileSize);

    // FALL
    applyReverb(&fallSampleBuffer, fileName, fileSize);
}

void RiseandfallAudioProcessor::reverse() {
    // RISE
    if (guiParams.riseReverse) {
        riseSampleBuffer.reverse(0, riseSampleBuffer.getNumSamples());
    }

    // FALL
    if (guiParams.fallReverse) {
        fallSampleBuffer.reverse(0, fallSampleBuffer.getNumSamples());
    }
}

void RiseandfallAudioProcessor::concatenate() {
    // TIME OFFSET
    auto offsetNumSamples = static_cast<int>((guiParams.timeOffset / 1000) * sampleRate);
    int processedSize = riseSampleBuffer.getNumSamples() + fallSampleBuffer.getNumSamples() + offsetNumSamples;

    processedSampleBuffer.setSize(numChannels, processedSize, false, true, AVOID_REALLOCATING);

    int overlapStart = riseSampleBuffer.getNumSamples() + offsetNumSamples;
    int overlapStop = overlapStart + abs(jmin(offsetNumSamples, 0));
    int overlapLength = overlapStop - overlapStart;

    for (int i = 0; i < numChannels; i++) {
        for (int j = 0; j < overlapStart; j++) {
            float value = riseSampleBuffer.getSample(i, j);
            processedSampleBuffer.setSample(i, j, value);
        }

        for (int j = 0; j < overlapLength; j++) {
            float value = fallSampleBuffer.getSample(i, j) + riseSampleBuffer.getSample(i, overlapStart + j);
            processedSampleBuffer.setSample(i, overlapStart + j, value);
        }

        for (int j = 0; j < fallSampleBuffer.getNumSamples() - overlapLength; j++) {
            float value = fallSampleBuffer.getSample(i, overlapLength + j);
            processedSampleBuffer.setSample(i, overlapStop + j, value);
        }
    }
}

void RiseandfallAudioProcessor::updateThumbnail() {
    thumbnail.reset(numChannels, sampleRate, processedSampleBuffer.getNumSamples());
    thumbnail.addBlock(0, processedSampleBuffer, 0, processedSampleBuffer.getNumSamples());
}


void RiseandfallAudioProcessor::normalizeSample() {
    float magnitude = originalSampleBuffer.getMagnitude(0, originalSampleBuffer.getNumSamples());
    originalSampleBuffer.applyGain(1 / magnitude);
}


void RiseandfallAudioProcessor::processSample() {
    if (!processing) {
        processing = true;
        riseSampleBuffer.makeCopyOf(originalSampleBuffer);
        fallSampleBuffer.makeCopyOf(originalSampleBuffer);
        position = 0;
        warp();
        delayEffect();
        reverbEffect();
        reverse();
        concatenate();
        updateThumbnail();
        processing = false;
    }
}

void RiseandfallAudioProcessor::newSampleLoaded() {
    position = 0;
    numChannels = originalSampleBuffer.getNumChannels();
    if (numChannels > 0) {
        normalizeSample();
        processSample();
    }
}