/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)
#endif
{
    position = 0;
    formatManager.registerBasicFormats();
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
}

void RiseandfallAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    convolution.reset();
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

    if (this->numChannels > 0) {
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (int i = this->numChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        int inputNumSamples = processedSampleBuffer.getNumSamples();
        int bufferSamplesRemaining = inputNumSamples - position;
        int samplesThisTime = jmin(samplesPerBlock, bufferSamplesRemaining);

        // This is the place where you'd normally do the guts of your plugin's
        // audio processing...
        for (int channel = 0; channel < this->numChannels; ++channel) {
            buffer.addFrom(channel, 0, processedSampleBuffer, channel, position, samplesThisTime, 0.1);
        }
        position += samplesThisTime;
        if (position == inputNumSamples) {
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
AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new RiseandfallAudioProcessor();
}

AudioSampleBuffer *RiseandfallAudioProcessor::getOriginalSampleBuffer() {
    return &originalSampleBuffer;
}

AudioSampleBuffer *RiseandfallAudioProcessor::getProcessedSampleBuffer() {
    return &processedSampleBuffer;
}

AudioThumbnail *RiseandfallAudioProcessor::getThumbnail() {
    return &thumbnail;
}

void RiseandfallAudioProcessor::prepare() {
    position = 0;
    numChannels = originalSampleBuffer.getNumChannels();
    dsp::ProcessSpec processSpec = {sampleRate, static_cast<uint32>(samplesPerBlock), static_cast<uint32>(2)};
    convolution.prepare(processSpec);
}

void RiseandfallAudioProcessor::newSampleLoaded() {
    position = 0;
    const int numSamples = originalSampleBuffer.getNumSamples();
    const int numChannels = originalSampleBuffer.getNumChannels();

    if (numChannels > 0) {
        processedSampleBuffer.setSize(numChannels, 2 * numSamples, false, true, false);

        for (int i = 0; i < numChannels; i++) {
            processedSampleBuffer.addFrom(i, numSamples, originalSampleBuffer, i, 0, numSamples);
        }
        originalSampleBuffer.reverse(0, numSamples);
        for (int i = 0; i < numChannels; i++) {
            processedSampleBuffer.addFrom(i, 0, originalSampleBuffer, i, 0, numSamples);
        }
        originalSampleBuffer.reverse(0, numSamples);

        thumbnail.reset(numChannels, sampleRate, 2 * numSamples);
        thumbnail.addBlock(0, processedSampleBuffer, 0, processedSampleBuffer.getNumSamples());
    }
}