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
), thumbnailCache(5), thumbnail(256, formatManager, thumbnailCache), parameters(*this, nullptr)
#endif
{
    position = 0;
    numChannels = 0;
    processing = false;
    formatManager.registerBasicFormats();

    parameters.createAndAddParameter(TIME_OFFSET_ID, TIME_OFFSET_NAME, String(), NormalisableRange<float>(-120, 120, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_REVERSE_ID, RISE_REVERSE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_REVERSE_ID, FALL_REVERSE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     false, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_EFFECTS_ID, RISE_EFFECTS_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_EFFECTS_ID, FALL_EFFECTS_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_TIME_WARP_ID, RISE_TIME_WARP_NAME, String(),
                                     NormalisableRange<float>(-4, 4, 2), 0, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_TIME_WARP_ID, FALL_TIME_WARP_NAME, String(),
                                     NormalisableRange<float>(-4, 4, 2), 0, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_TIME_ID, DELAY_TIME_NAME, String(), NormalisableRange<float>(10, 1000, 1),
                                     500, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_FEEDBACK_ID, DELAY_FEEDBACK_NAME, String(),
                                     NormalisableRange<float>(0, 99, 1), 50, nullptr, nullptr);
    parameters.createAndAddParameter(IMPULSE_RESPONSE_ID, IMPULSE_RESPONSE_NAME, String(),
                                     NormalisableRange<float>(0, 1, 1), 0, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_TYPE_ID, FILTER_TYPE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_CUTOFF_ID, FILTER_CUTOFF_NAME, String(),
                                     NormalisableRange<float>(20, 20000, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_RESONANCE_ID, FILTER_RESONANCE_NAME, String(),
                                     NormalisableRange<float>(0, 10, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(REVERB_MIX_ID, REVERB_MIX_NAME, String(), NormalisableRange<float>(0, 100, 1),
                                     50, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_MIX_ID, DELAY_MIX_NAME, String(), NormalisableRange<float>(0, 100, 1),
                                     50, nullptr, nullptr);

    parameters.state = ValueTree(Identifier("RiseAndFall"));

    this->addListener(this);

    if (!filePath.isEmpty()) {
        File file(filePath);
        loadSampleFromFile(file);
    }
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
    return new RiseandfallAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void RiseandfallAudioProcessor::getStateInformation(MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ScopedPointer<XmlElement> parent = new XmlElement(Identifier("RiseAndFallAllParams"));
    ScopedPointer<XmlElement> xml = parameters.state.createXml();
    ScopedPointer<XmlElement> filePathXML = new XmlElement(Identifier(FILE_PATH_ID));
    filePathXML->setAttribute("path", filePath);

    parent->addChildElement(xml);
    parent->addChildElement(filePathXML);

    copyXmlToBinary(*parent, destData);

    parent->removeChildElement(xml, false);
    parent->removeChildElement(filePathXML, false);
}

void RiseandfallAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr) {
        ScopedPointer<XmlElement> xml = xmlState->getChildByName(parameters.state.getType());
        ScopedPointer<XmlElement> filePathXML = xmlState->getChildByName(FILE_PATH_ID);
        if (xml != nullptr) {
            parameters.state = ValueTree::fromXml(*xml);
        }
        if (filePathXML != nullptr) {
            filePath = filePathXML->getStringAttribute("path");
            File file(filePath);
            loadSampleFromFile(file);
        }

        xmlState->removeChildElement(xml, false);
        xmlState->removeChildElement(filePathXML, false);
    }
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
    const clock_t start = clock();

    // TIME OFFSET
    auto offsetNumSamples = static_cast<int>((*parameters.getRawParameterValue(TIME_OFFSET_ID) / 1000) * sampleRate);
    int processedSize = riseSampleBuffer.getNumSamples() + fallSampleBuffer.getNumSamples() + offsetNumSamples;

    processedSampleBuffer.setSize(numChannels, processedSize, false, true, AVOID_REALLOCATING);

    int overlapStart = riseSampleBuffer.getNumSamples() + offsetNumSamples;
    int overlapStop = overlapStart + abs(jmin(offsetNumSamples, 0));
    int overlapLength = overlapStop - overlapStart;

    for (int i = 0; i < numChannels; i++) {
        for (int j = 0; j < overlapStart && j < riseSampleBuffer.getNumSamples(); j++) {
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

    printf("concat elapsed: %.2lf ms\n", float(clock() - start) / CLOCKS_PER_SEC);
}

void RiseandfallAudioProcessor::updateThumbnail() {
    thumbnail.reset(numChannels, sampleRate, processedSampleBuffer.getNumSamples());
    thumbnail.addBlock(0, processedSampleBuffer, 0, processedSampleBuffer.getNumSamples());
}


void RiseandfallAudioProcessor::normalizeSample() {
    processing = true;
    float magnitude = originalSampleBuffer.getMagnitude(0, originalSampleBuffer.getNumSamples());
    originalSampleBuffer.applyGain(1 / magnitude);
}


void RiseandfallAudioProcessor::processSample() {
    if (numChannels > 0) {
        if (!processing) {
            processing = true;
            const clock_t start = clock();
            printf("BLOCK START\n");

            riseSampleBuffer.makeCopyOf(originalSampleBuffer);
            fallSampleBuffer.makeCopyOf(originalSampleBuffer);

            ProcessingThreadPoolJob risePoolJob(RISE, riseSampleBuffer, parameters, sampleRate);
            ProcessingThreadPoolJob fallPoolJob(FALL, fallSampleBuffer, parameters, sampleRate);

            pool.addJob(&risePoolJob, false);
            pool.addJob(&fallPoolJob, false);

            bool riseJobFinished = pool.waitForJobToFinish(&risePoolJob, 60000);
            bool fallJobFinished = pool.waitForJobToFinish(&fallPoolJob, 60000);

            if (riseJobFinished && fallJobFinished) {
                riseSampleBuffer.makeCopyOf(risePoolJob.getOutputBuffer());
                fallSampleBuffer.makeCopyOf(fallPoolJob.getOutputBuffer());
                position = 0;
                concatenate();
                printf("BLOCK END: %.2f ms\n\n", float(clock() - start) / CLOCKS_PER_SEC);
                updateThumbnail();
                processing = false;
            } else {
                printf("Thread pool timed out after 60 seconds.\n");
            }
        } else {
            printf("Interrupting all jobs.\n");
            while (!pool.removeAllJobs(true, 2000)) {
                // wait until all jobs are removed
                printf("Still trying to interrupting all jobs.\n");
            }
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

void RiseandfallAudioProcessor::loadSampleFromFile(File &file) {
    filePath = file.getFullPathName();
    printf("Loaded file: %s\n", filePath.toStdString().c_str());
    ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
    const double duration = reader->lengthInSamples / reader->sampleRate;

    if (duration < 20) {
        originalSampleBuffer.setSize(reader->numChannels,
                                     static_cast<int>(reader->lengthInSamples));
        reader->read(&originalSampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        newSampleLoaded();
    } else {
        // handle the error that the file is 20 seconds or longer..
    }
}

void RiseandfallAudioProcessor::audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex,
                                                               float newValue) {
    // do nothing
}

void RiseandfallAudioProcessor::audioProcessorChanged(AudioProcessor *processor) {
    //do nothing
}

void RiseandfallAudioProcessor::audioProcessorParameterChangeGestureEnd(AudioProcessor *processor, int parameterIndex) {
    AudioProcessorListener::audioProcessorParameterChangeGestureEnd(processor, parameterIndex);
    this->processSample();
}
