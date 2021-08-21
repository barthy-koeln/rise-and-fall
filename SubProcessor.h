#pragma once

#include <JuceHeader.h>
#include <SoundTouch.h>
#include "GUIParams.h"

using namespace soundtouch;
using namespace dsp;

typedef enum ThreadTypeEnum
{
  RISE = 0,
  FALL
} ThreadType;

class SubProcessor
{
  public:
    SubProcessor(
      const ThreadType type,
      AudioBuffer<float> &bufferIn,
      GUIParams &parameters
    );

    ~SubProcessor();

    void process();

    void prepareToPlay(double sampleRate, double bpm);

    void prepareReverb(const void* sourceData, size_t sourceDataSize);

  private:
    AudioBuffer<float> &bufferIn;
    GUIParams &parameters;
    ThreadType type;
    double sampleRate;
    double bpm;

    const void *lastIRName;

    /**
     * SoundTouch instance for time warping
     */
    SoundTouch soundTouch;

    /**
     * Convolution engine for the reverb effect
     */
    Convolution convolution;

    /**
     * Warp audio samples to change the speed and pitch
     *
     * @param buffer
     * @param factor
     */
    void applyTimeWarp(int factor);

    /**
     *
     * @param target
     * @param base
     * @param dampen
     * @param delayTimeInSamples
     * @param iteration
     */
    void applyDelay(
      AudioBuffer<float> &base,
      float dampen,
      int delayTimeInSamples,
      int iteration
    );

    /**
     *
     * @param target
     */
    void applyReverb(float mix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubProcessor);
};
