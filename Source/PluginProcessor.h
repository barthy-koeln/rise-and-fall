/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../modules/juce_dsp/juce_dsp.h"
#include "GUIParams.h"

//==============================================================================
/**
*/
class RiseandfallAudioProcessor : public AudioProcessor {
public:

    /**
     * Contains all extracted parameters from the GUI
     */
    GUIParams guiParams;

    //==============================================================================
    RiseandfallAudioProcessor();

    ~RiseandfallAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

#endif

    void processBlock(AudioSampleBuffer &, MidiBuffer &) override;

    //==============================================================================
    AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const String getProgramName(int index) override;

    void changeProgramName(int index, const String &newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    /**
     * Get the original sample buffer
     *
     * @return A pointer to the original audio buffer
     */
    AudioSampleBuffer *getOriginalSampleBuffer();

    /**
     * Get the processed sample buffer
     *
     * @return A pointer to the processed audio buffer
     */
    AudioSampleBuffer *getProcessedSampleBuffer();

    /**
     * Get the thumbnail
     *
     * @return A pointer to the thumbnail
     */
    AudioThumbnail *getThumbnail();

    /**
     * Reset the position and set the new number of samples and channels
     */
    void newSampleLoaded();

    /**
     * Cascade the multiple audio processing algorithms
     */
    void processSample();

private:

    /**
     * Buffer containing the samples of the original audio file
     */
    AudioSampleBuffer originalSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioSampleBuffer processedSampleBuffer;

    /**
     * Buffer containing the impulse respnse samples to use for the convolution in the reverb effect
     */
    AudioSampleBuffer reverbImpulseResponse;

    /**
     * Sample rate for the current block
     */
    double sampleRate;

    /**
     * Number of samples in the current block
     */
    int samplesPerBlock;

    /**
     * Number of channels in the original audio file
     */
    int numChannels;

    /**
     * Number of samples in the original audio file
     */
    int numSamples;

    /**
     * Current position in the processing of sample blocks
     */
    uint64 position;

    /**
     * Handles basic audio formats (wav, aiff)
     */
    AudioFormatManager formatManager;

    /**
     * Cache containing thumbnail previews
     */
    AudioThumbnailCache thumbnailCache;

    /**
     * Thumbnail of the audio waveform
     */
    AudioThumbnail thumbnail;


    /**
     * Block processing of the sample if it is already in process
     */
    bool processing;

    /**
     * Clone the processed audio, reverse it and finally prepend it to the processed audio buffer
     */
    void reverseAndPrepend();

    /**
     * Warp audio samples to change the speed and pitch
     *
     * @param buffer
     * @param factor
     */
    void applyTimeWarp(AudioSampleBuffer *buffer, int factor);

    void applyDelay(AudioSampleBuffer *target, AudioSampleBuffer *base, float dampen, int delayTimeInSamples, int iteration);

    void applyReverb(AudioSampleBuffer *target);

    /**
     * Update the thumbnail image
     */
    void updateThumbnail();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessor)
};
