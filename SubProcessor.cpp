#define AVOID_REALLOCATING false

#include "SubProcessor.h"

SubProcessor::SubProcessor(
  ThreadType type,
  AudioBuffer<float> &bufferIn,
  GUIParams &parameters
) :
  type(type),
  bufferIn(bufferIn),
  parameters(parameters),
  sampleRate(-1),
  bpm(0)
{
  this->soundTouch.setChannels(1); // always iterate over single channels
  this->soundTouch.setSampleRate(static_cast<uint>(this->sampleRate));
}

void SubProcessor::applyTimeWarp(int factor)
{
  float realFactor = factor < 0 ? (1.0f / abs(factor)) : (1.0f * factor);

  AudioBuffer<float> copy;
  copy.makeCopyOf(bufferIn);

  this->soundTouch.setTempo(realFactor);

  double ratio = this->soundTouch.getInputOutputSampleRatio();

  bufferIn.setSize(
    bufferIn.getNumChannels(),
    static_cast<int>(ceil(bufferIn.getNumSamples() * ratio)),
    false, true, AVOID_REALLOCATING
  );

  for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
    this->soundTouch.putSamples(
      copy.getReadPointer(channel),
      static_cast<uint>(copy.getNumSamples())
    );

    this->soundTouch.receiveSamples(
      bufferIn.getWritePointer(channel),
      static_cast<uint>(bufferIn.getNumSamples())
    );

    this->soundTouch.clear();
  }
}

void SubProcessor::applyDelay(
  AudioBuffer<float> &base,
  const float dampen,
  const int delayTimeInSamples,
  int iteration
)
{
  base.applyGain(dampen);
  bool continueIteration = base.getMagnitude(0, base.getNumSamples()) > 0.001f;
  int currentDelayPosition = delayTimeInSamples * iteration;
  int length = bufferIn.getNumSamples() + base.getNumSamples() + delayTimeInSamples;

  bufferIn.setSize(
    bufferIn.getNumChannels(),
    length,
    true,
    true,
    AVOID_REALLOCATING
  );

  for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
    for (int i = 0; i < base.getNumSamples(); i++) {
      bufferIn.addSample(
        channel, i + currentDelayPosition,
        base.getSample(channel, i));
    }
  }

  if (continueIteration) {
    applyDelay(base, dampen, delayTimeInSamples, iteration + 1);
  }
}

void SubProcessor::applyReverb(float mix)
{
  int irSize = this->convolution.getCurrentIRSize();

#if DEBUG
  std::cout << "Reverb Params: IR size " << irSize << ", Mix " << mix << std::endl;
#endif

  if (irSize <= 0) {
    return;
  }

  AudioBuffer<float> copy;
  copy.makeCopyOf(this->bufferIn);

  const int processedSize = irSize + copy.getNumSamples() - 1;
  this->bufferIn.setSize(
    this->bufferIn.getNumChannels(),
    processedSize,
    false,
    true,
    AVOID_REALLOCATING
  );

  AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
  AudioBlock<float> audioBlockOut = AudioBlock<float>(bufferIn);
  auto processContext = ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);

  this->convolution.process(processContext);

  bufferIn.applyGain(mix);

  for (int channel = 0; channel < copy.getNumChannels(); channel++) {
    bufferIn.addFrom(
      channel,
      0,
      copy,
      channel,
      0,
      copy.getNumSamples(),
      1 - mix
    );
  }
}

void SubProcessor::prepareToPlay(double sampleRateIn, double bpmIn)
{
  auto numChannels = this->bufferIn.getNumChannels();

  this->sampleRate = sampleRateIn;
  this->bpm = bpmIn;
  this->soundTouch.setSampleRate(static_cast<uint>(this->sampleRate));

  this->convolution.prepare({
    this->sampleRate,
    static_cast<uint32>(288000),
    static_cast<uint32>(numChannels)
  });
}

void SubProcessor::prepareReverb(const void *sourceData, size_t sourceDataSize)
{
  auto numChannels = this->bufferIn.getNumChannels();

  if (this->lastIRName == sourceData) {
    return;
  }

  this->lastIRName = sourceData;

  this->convolution.loadImpulseResponse(
    sourceData,
    sourceDataSize,
    numChannels > 1 ? Convolution::Stereo::yes : Convolution::Stereo::no,
    Convolution::Trim::yes,
    0,
    Convolution::Normalise::yes
  );
}

void SubProcessor::process()
{
  auto delayMix = (float) parameters.getParameterAsValue(DELAY_MIX_ID).getValue() / 100.0f;
  auto reverbMix = (float) parameters.getParameterAsValue(REVERB_MIX_ID).getValue() / 100.0f;

  String reverbId = (type == RISE) ? RISE_REVERB_ID : FALL_REVERB_ID;
  String delayId = (type == RISE) ? RISE_DELAY_ID : FALL_DELAY_ID;
  String timeWarpId = (type == RISE) ? RISE_TIME_WARP_ID : FALL_TIME_WARP_ID;
  String reverseId = (type == RISE) ? RISE_REVERSE_ID : FALL_REVERSE_ID;

  auto reverbEnabled = (bool) parameters.getParameterAsValue(reverbId).getValue();
  auto delayEnabled = (bool) parameters.getParameterAsValue(delayId).getValue();
  auto timeWarp = (int) parameters.getParameterAsValue(timeWarpId).getValue();
  auto reverse = (bool) parameters.getParameterAsValue(reverseId).getValue();

  if (timeWarp != 0) {
    applyTimeWarp(timeWarp);
  }

  if (reverbEnabled && reverbMix > 0) {
    applyReverb(reverbMix);
  }

  if (delayEnabled && delayMix > 0) {
    AudioBuffer<float> delayBaseBuffer;
    auto delayFeedbackNormalized = (float) parameters.getParameterAsValue(DELAY_FEEDBACK_ID).getValue() / 100.0f;
    auto delayNoteIndex = (float) parameters.getParameterAsValue(DELAY_TIME_ID).getValue();
    int samplesPerBeat = (int) ceil((60.0f / this->bpm) * sampleRate);
    int delayTimeInSamples;

    double delayNote = pow(2, abs(delayNoteIndex));

    if (delayNoteIndex > 0) {
      delayTimeInSamples = (int) (delayNote * 4 * samplesPerBeat);
    } else {
      delayTimeInSamples = (int) ceil(samplesPerBeat / (abs(delayNote) * 4));
    }

    delayBaseBuffer.makeCopyOf(bufferIn);
    delayBaseBuffer.applyGain(delayMix);
    applyDelay(delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
  }

  if (reverse) {
    bufferIn.reverse(0, bufferIn.getNumSamples());
  }
}

SubProcessor::~SubProcessor() = default;
