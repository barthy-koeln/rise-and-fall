/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#define AVOID_REALLOCATING false

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProcessingThreadPoolJob.h"

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
}

RiseandfallAudioProcessor::~RiseandfallAudioProcessor() {
    pool.removeAllJobs(true, 2000);
}

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

void RiseandfallAudioProcessor::concatenate() {
    time_t start, end;

    time(&start);
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

    time(&end);
    printf("concat elapsed: %.2lf seconds\n", difftime(end, start));
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
    if (numChannels > 0) {
        if (!processing) {
            processing = true;
            time_t start, end;
            time(&start);
            printf("BLOCK START\n");

            riseSampleBuffer.makeCopyOf(originalSampleBuffer);
            fallSampleBuffer.makeCopyOf(originalSampleBuffer);

            auto *risePoolJob = new ProcessingThreadPoolJob(RISE, riseSampleBuffer, guiParams, sampleRate);
            auto *fallPoolJob = new ProcessingThreadPoolJob(FALL, fallSampleBuffer, guiParams, sampleRate);

            pool.addJob(risePoolJob, true);
            pool.addJob(fallPoolJob, true);

            bool riseJobFinished = pool.waitForJobToFinish(risePoolJob, 60000);
            bool fallJobFinished = pool.waitForJobToFinish(fallPoolJob, 60000);

            if (riseJobFinished && fallJobFinished) {
                riseSampleBuffer.makeCopyOf(risePoolJob->getOutputBuffer());
                fallSampleBuffer.makeCopyOf(fallPoolJob->getOutputBuffer());
                position = 0;
                concatenate();
                time(&end);
                printf("BLOCK END: %.2f\n\n", difftime(end, start));
                updateThumbnail();
                processing = false;
            } else {
                printf("TIMEOUT THREAD POOL SHIT\n");
            }
        } else {
            pool.removeAllJobs(true, 2000);
            processing = false;
            processSample();
        }
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