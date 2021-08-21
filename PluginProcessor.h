#pragma once

#include <JuceHeader.h>
#include "SubProcessor.h"

class RiseAndFallAudioProcessor :
  public AudioProcessor,
  public AudioProcessorListener
{
  public:
    RiseAndFallAudioProcessor();

    ~RiseAndFallAudioProcessor() override;

    void prepareToPlay(double sampleRateIn, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const String getProgramName(int index) override;

    void changeProgramName(int index, const String &newName) override;

    void getStateInformation(MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    /**
     * Get the thumbnail
     *
     * @return A reference to the thumbnail
     */
    AudioThumbnail &getThumbnail();

    AudioThumbnailCache &getThumbnailCache();

    int getPosition();

    int getNumSamples();

    /**
     * Reset the position and set the new number of samples and channels
     */
    void newSampleLoaded();

    void loadNewImpulseResponse(int id);

    /**
     * Load an audio sample from a file
     *
     * @param file
     */
    void loadSampleFromFile(File &file);

    /**
     * Cascade the multiple audio processing algorithms
     */
    void processSample();

  private:
    /**
     * Buffer containing the samples of the original audio file
     */
    AudioBuffer<float> originalSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioBuffer<float> processedSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioBuffer<float> riseSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioBuffer<float> fallSampleBuffer;

    SubProcessor riseProcessor;
    SubProcessor fallProcessor;

    /**
     * Sample rate for the current block
     */
    double sampleRate;

    double bpm;

    /**
     * Number of samples in the current block
     */
    int samplesPerBlock;

    /**
     * Current position in the processing of sample blocks
     */
    int position;

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
     * Stores all the parameters
     */
    GUIParams guiParams;

    /**
     * The loaded sample's file path
     */
    String filePath = "";

    /**
     * Block processing of the sample if it is already in process
     */
    bool processing;

    /**
     * Whether the plugin should start playback or not
     */
    bool play;

    /**
     * Array of filters (one for each channel)
     */
    OwnedArray<IIRFilter> filters;

    /**
     * Infinite Impulse Response Filter Coefficients
     */
    IIRCoefficients coeffs;

    /**
     * Clone the processed audio, reverse it and finally prepend it to the
     * processed audio buffer
     */
    void concatenate();

    /**
     * Update the thumbnail image
     */
    void updateThumbnail();

    void audioProcessorChanged (AudioProcessor* processor, const ChangeDetails& details) override;

    /**
     * Act when a parameter changes
     *
     * @param processor
     * @param parameterIndex
     * @param newValue
     */
    void audioProcessorParameterChanged(
      AudioProcessor *processor,
      int parameterIndex,
      float newValue
    ) override;

    /**
     * Act when a gesture changing a parameter ends
     *
     * @param processor
     * @param parameterIndex
     */
    void audioProcessorParameterChangeGestureEnd(
      AudioProcessor *processor,
      int parameterIndex
    ) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RiseAndFallAudioProcessor)
};
