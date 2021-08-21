#define AVOID_REALLOCATING false
#define PLAY_LOOP true // FOR DEBUG MODE ONLY

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AudioBufferUtils.hpp"

RiseAndFallAudioProcessor::RiseAndFallAudioProcessor() :
  AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)),
  thumbnailCache(5),
  thumbnail(32, formatManager, thumbnailCache),
  guiParams(*this),
  position(0),
  samplesPerBlock(0),
  processing(false),
  sampleRate(-1),
  riseProcessor(
    ThreadType::RISE,
    this->riseSampleBuffer,
    this->guiParams
  ),
  fallProcessor(
    ThreadType::FALL,
    this->fallSampleBuffer,
    this->guiParams
  )
{
  this->formatManager.registerBasicFormats();

  this->addListener(this);
}

RiseAndFallAudioProcessor::~RiseAndFallAudioProcessor()
{

}

const String RiseAndFallAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool RiseAndFallAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool RiseAndFallAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool RiseAndFallAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double RiseAndFallAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int RiseAndFallAudioProcessor::getNumPrograms()
{
  return 1;
}

int RiseAndFallAudioProcessor::getCurrentProgram()
{ return 0; }

void RiseAndFallAudioProcessor::setCurrentProgram([[maybe_unused]] int index)
{}

const String RiseAndFallAudioProcessor::getProgramName([[maybe_unused]] int index)
{ return {}; }

void RiseAndFallAudioProcessor::changeProgramName(
  [[maybe_unused]] int index,
  [[maybe_unused]] const String &name
)
{}

//==============================================================================
void RiseAndFallAudioProcessor::prepareToPlay(
  double sampleRateIn,
  int samplesPerBlockIn
)
{
  this->sampleRate = sampleRateIn;
  this->samplesPerBlock = samplesPerBlockIn;

  AudioPlayHead::CurrentPositionInfo result{};
  AudioPlayHead *const head = this->getPlayHead();

  this->bpm = head && head->getCurrentPosition(result) ? result.bpm : 120;

  this->riseProcessor.prepareToPlay(this->sampleRate, this->bpm);
  this->fallProcessor.prepareToPlay(this->sampleRate, this->bpm);

  if (this->sampleRate > 0) {
    processSample();
  }
}

void RiseAndFallAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  position = 0;
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool RiseAndFallAudioProcessor::isBusesLayoutSupported(
  const BusesLayout &layouts
) const
{
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != AudioChannelSet::stereo()) {
    return false;
  }

// This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

#endif


void RiseAndFallAudioProcessor::processBlock(
  AudioBuffer<float> &buffer,
  MidiBuffer &midiMessages
)
{
  if (!midiMessages.isEmpty()) {
    for (const MidiMessageMetadata metadata : midiMessages) {
      auto message = metadata.getMessage();

      if (message.isNoteOn(false)) {
        this->position = 0;
        this->play = true;
      }

      if (message.isNoteOff(true)) {
        this->play = false;
        this->position = 0;
      }
    }
  }

#if !PLAY_LOOP
  if (play) {
#endif
  buffer.clear();

  midiMessages.clear();

  if (this->processedSampleBuffer.getNumChannels() > 0 && !this->processing) {
    auto bufferSamplesRemaining = this->processedSampleBuffer.getNumSamples() - this->position;
    int samplesThisTime = jmin(this->samplesPerBlock, bufferSamplesRemaining);

    for (int channel = 0; channel < this->processedSampleBuffer.getNumChannels(); channel++) {
      buffer.addFrom(
        channel,
        0,
        this->processedSampleBuffer,
        channel,
        this->position,
        samplesThisTime,
        0.9f
      );

      this->filters[channel]->processSamples(
        buffer.getWritePointer(channel),
        samplesThisTime
      );
    }

    this->position += samplesThisTime;
    if (this->position >= this->processedSampleBuffer.getNumSamples()) {
#if !PLAY_LOOP
      this->play = false;
#endif
      this->position = 0;
    }
  }
#if !PLAY_LOOP
  }
#endif
}

//==============================================================================
bool RiseAndFallAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *RiseAndFallAudioProcessor::createEditor()
{
  return new RiseAndFallAudioProcessorEditor(*this, this->guiParams);
}

void RiseAndFallAudioProcessor::getStateInformation(MemoryBlock &destData)
{
  auto state = this->guiParams.copyState();
  auto xml = state.createXml();
  this->copyXmlToBinary(*xml, destData);
}

void RiseAndFallAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
  auto xmlState = this->getXmlFromBinary(data, sizeInBytes);

  if (xmlState == nullptr) {
    return;
  }

  if (!xmlState->hasTagName(this->guiParams.state.getType())) {
    return;
  }

  this->guiParams.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessor *JUCE_CALLTYPE

createPluginFilter()
{
  return new RiseAndFallAudioProcessor();
}

AudioThumbnail &RiseAndFallAudioProcessor::getThumbnail()
{
  return thumbnail;
}

AudioThumbnailCache &RiseAndFallAudioProcessor::getThumbnailCache()
{
  return thumbnailCache;
}

void RiseAndFallAudioProcessor::concatenate()
{
  // TIME OFFSET
  auto timeOffset = (float) guiParams.getParameterAsValue(TIME_OFFSET_ID).getValue();
  int offsetNumSamples = (int) ceil((timeOffset / 1000) * sampleRate);
  int numSamples = jmax(1, riseSampleBuffer.getNumSamples() + fallSampleBuffer.getNumSamples() + offsetNumSamples);

  processedSampleBuffer.setSize(
    originalSampleBuffer.getNumChannels(),
    numSamples,
    false,
    true,
    AVOID_REALLOCATING
  );

  int overlapStart = riseSampleBuffer.getNumSamples() + offsetNumSamples;
  int overlapStop = overlapStart + abs(jmin(offsetNumSamples, 0));
  int overlapLength = overlapStop - overlapStart;

  for (int i = 0; i < processedSampleBuffer.getNumChannels(); i++) {
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
}

void RiseAndFallAudioProcessor::updateThumbnail()
{
  int numChannels = this->processedSampleBuffer.getNumChannels();
  int numSamples = this->processedSampleBuffer.getNumSamples();

  this->thumbnail.reset(
    numChannels,
    this->sampleRate,
    numSamples
  );

  this->thumbnail.addBlock(
    0,
    this->processedSampleBuffer,
    0,
    numSamples
  );
}

void RiseAndFallAudioProcessor::processSample()
{
  if (this->processing) {
    return;
  }

  if (this->originalSampleBuffer.getNumChannels() <= 0) {
    return;
  }

  if (this->sampleRate <= 0) {
    return;
  }

  this->processing = true;

#if DEBUG
  const clock_t start = clock();
#endif

  this->processedSampleBuffer.makeCopyOf(this->originalSampleBuffer);

  this->riseSampleBuffer.makeCopyOf(this->originalSampleBuffer);
  this->fallSampleBuffer.makeCopyOf(this->originalSampleBuffer);

  this->riseProcessor.prepareToPlay(this->sampleRate, this->bpm);
  this->fallProcessor.prepareToPlay(this->sampleRate, this->bpm);

  this->riseProcessor.process();
  this->fallProcessor.process();

  AudioBufferUtils::trim(this->riseSampleBuffer);
  AudioBufferUtils::trim(this->fallSampleBuffer);

  AudioBufferUtils::normalize(this->riseSampleBuffer);
  AudioBufferUtils::normalize(this->fallSampleBuffer);

  concatenate();

  AudioBufferUtils::normalize(this->processedSampleBuffer);

  int numSamples = this->processedSampleBuffer.getNumSamples();
  int fades = (int) (numSamples * 0.1);
  this->processedSampleBuffer.applyGainRamp(0, fades, 0, 1);
  this->processedSampleBuffer.applyGainRamp(
    numSamples - fades,
    fades,
    1,
    0
  );

  this->position = 0;

#if DEBUG
  std::cout << "Processed: " << float((clock() - start)) / CLOCKS_PER_SEC << " s, "
            << this->processedSampleBuffer.getNumChannels() << " Channels, "
            << this->processedSampleBuffer.getNumSamples() << " Samples" << std::endl;
#endif

  this->processing = false;
  updateThumbnail();
}

void RiseAndFallAudioProcessor::newSampleLoaded()
{
  this->filters.clear();
  for (int i = 0; i < this->originalSampleBuffer.getNumChannels(); i++) {
    this->filters.add(new IIRFilter());
  }

  AudioBufferUtils::normalize(this->originalSampleBuffer);
  AudioBufferUtils::trim(this->originalSampleBuffer);

  this->processSample();
}

void RiseAndFallAudioProcessor::loadSampleFromFile(File &file)
{
  filePath = file.getFullPathName();
  auto reader = formatManager.createReaderFor(file);

  if (reader == nullptr) {
    std::cout << "FILE DOES NOT EXIST" << std::endl;
    return;
  }

  auto length = static_cast<int>(reader->lengthInSamples);

  this->originalSampleBuffer.setSize(reader->numChannels, length);

  reader->read(
    &this->originalSampleBuffer,
    0,
    length,
    0,
    true,
    true
  );

  this->newSampleLoaded();
}

void RiseAndFallAudioProcessor::audioProcessorParameterChanged(
  AudioProcessor *processor,
  int parameterIndex,
  [[maybe_unused]] float newValue
)
{
  if (this->sampleRate <= 0) {
    return;
  }

  if (
    parameterIndex == FILTER_RESONANCE ||
    parameterIndex == FILTER_CUTOFF ||
      parameterIndex == FILTER_TYPE
    ) {
    auto resonanceParam = (AudioParameterFloat *) this->guiParams.getParameter(FILTER_RESONANCE_ID);
    auto cutoffParam = (AudioParameterInt *) this->guiParams.getParameter(FILTER_CUTOFF_ID);
    auto typeParam = (AudioParameterChoice *) this->guiParams.getParameter(FILTER_TYPE_ID);

    int filterType = typeParam->getIndex();
    int cutoff = cutoffParam->get();
    float resonance = resonanceParam->get();

    switch (filterType) {
      case 1:
        this->coeffs = IIRCoefficients::makeLowPass(this->sampleRate, cutoff, resonance);
        break;
      case 2:
        this->coeffs = IIRCoefficients::makeHighPass(this->sampleRate, cutoff, resonance);
        break;
      default:
        break;
    }

    for (int i = 0; i < this->processedSampleBuffer.getNumChannels(); i++) {
      this->filters[i]->setCoefficients(this->coeffs);
    }

    return;
  }

  if (parameterIndex == IMPULSE_RESPONSE) {
    auto impulseResponseParam = (AudioParameterChoice *) this->guiParams.getParameter(IMPULSE_RESPONSE_ID);
    this->loadNewImpulseResponse(impulseResponseParam->getIndex());

    return;
  }
}

void RiseAndFallAudioProcessor::audioProcessorParameterChangeGestureEnd(AudioProcessor *processor, int parameterIndex)
{
  if (
    parameterIndex == FILTER_RESONANCE ||
    parameterIndex == FILTER_CUTOFF ||
    parameterIndex == FILTER_TYPE ||
      parameterIndex == IMPULSE_RESPONSE
    ) {
    return;
  }

  if (parameterIndex == TIME_OFFSET) {
    this->processing = true;
    this->concatenate();
    this->position = 0;
    this->processing = false;
    this->updateThumbnail();
    return;
  }

  this->processSample();
}

int RiseAndFallAudioProcessor::getPosition()
{ return position; }

int RiseAndFallAudioProcessor::getNumSamples()
{
  return processedSampleBuffer.getNumSamples();
}

void RiseAndFallAudioProcessor::loadNewImpulseResponse(int id)
{
  const char *resourceName;
  int resourceSize;

  switch (id) {
    case 5:
      resourceName = BinaryData::university_of_york_stairwell48khznormtrim_wav;
      resourceSize = BinaryData::university_of_york_stairwell48khznormtrim_wavSize;
      break;
    case 4:
      resourceName = BinaryData::empty_apartment_bedroom48khznormtrim_wav;
      resourceSize = BinaryData::empty_apartment_bedroom48khznormtrim_wavSize;
      break;
    case 3:
      resourceName = BinaryData::st_georges48khznormtrim_wav;
      resourceSize = BinaryData::st_georges48khznormtrim_wavSize;
      break;
    case 2:
      resourceName = BinaryData::nuclear_reactor_hall48khznormtrim_wav;
      resourceSize = BinaryData::nuclear_reactor_hall48khznormtrim_wavSize;
      break;
    case 1:
      resourceName = BinaryData::york_minster48khznormtrim_wav;
      resourceSize = BinaryData::york_minster48khznormtrim_wavSize;
      break;
    case 0:
    default:
      resourceName = BinaryData::warehouse48khznormtrim_wav;
      resourceSize = BinaryData::warehouse48khznormtrim_wavSize;
  }

  this->riseProcessor.prepareReverb(resourceName, resourceSize);
  this->fallProcessor.prepareReverb(resourceName, resourceSize);

  this->processSample();
}

void RiseAndFallAudioProcessor::audioProcessorChanged(
  [[maybe_unused]] AudioProcessor *processor,
  [[maybe_unused]] const ChangeDetails &details
)
{

}
