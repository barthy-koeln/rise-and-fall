#define fontSize 16
#define sliderWidth 88
#define sliderHeight 96
#define comboBoxHeight 64
#define toggleButtonHeight 32
#define toggleButtonWidth 188

#include "PluginEditor.h"

template<class T> std::unique_ptr<T> RiseAndFallAudioProcessorEditor::initSlider(
  String parameterId,
  [[maybe_unused]] const String &label,
  const String &suffix,
  Slider::SliderStyle style
)
{
  auto slider = std::make_unique<T>(parameterId);

  slider->setSliderStyle(style);

  slider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
  slider->setPopupDisplayEnabled(true, false, this);
  slider->setTextValueSuffix(suffix);

  this->parameters.attach(*slider);
  this->addAndMakeVisible(*slider);

  return slider;
}

std::unique_ptr<ComboBox> RiseAndFallAudioProcessorEditor::initComboBox(
  String parameterId,
  [[maybe_unused]] const String &label
)
{
  auto comboBox = std::make_unique<ComboBox>(parameterId);

  this->addAndMakeVisible(*comboBox);
  this->parameters.attach(*comboBox);

  return comboBox;
}

std::unique_ptr<ToggleButton> RiseAndFallAudioProcessorEditor::initToggleButton(
  String parameterId,
  String label
)
{
  auto button = std::make_unique<ToggleButton>(parameterId);

  button->setButtonText(label);
  this->addAndMakeVisible(*button);
  this->parameters.attach(*button);

  return button;
}

RiseAndFallAudioProcessorEditor::RiseAndFallAudioProcessorEditor(
  RiseAndFallAudioProcessor &audioProcessor,
  GUIParams &parameters
) :
  AudioProcessorEditor(audioProcessor),
  processor(audioProcessor),
  parameters(parameters),
  thumbnailBounds(16, 536, 656, 144),
  thumbnailComp(
    this->processor.getThumbnail(),
    this->processor.getThumbnailCache(),
    this->customLookAndFeel
  ),
  positionOverlay(audioProcessor, customLookAndFeel)
{
  this->timeOffsetSlider = this->initSlider<Slider>(
    TIME_OFFSET_ID,
    "TIME OFFSET",
    this->customLookAndFeel.DIMENSION_MS,
    Slider::LinearHorizontal
  );

  this->riseTimeWarpSlider = this->initSlider<Slider>(
    RISE_TIME_WARP_ID,
    "TIME WARP",
    this->customLookAndFeel.DIMENSION_TIMES
  );

  this->fallTimeWarpSlider = this->initSlider<Slider>(
    FALL_TIME_WARP_ID,
    "TIME WARP",
    this->customLookAndFeel.DIMENSION_TIMES
  );

  this->reverbMixSlider = this->initSlider<Slider>(
    REVERB_MIX_ID,
    "MIX / WET",
    this->customLookAndFeel.DIMENSION_PERCENT
  );

  this->delayMixSlider = this->initSlider<Slider>(
    DELAY_MIX_ID,
    "MIX / WET",
    this->customLookAndFeel.DIMENSION_PERCENT
  );

  this->delayTimeSlider = this->initSlider<NoteLengthSlider>(
    DELAY_TIME_ID,
    "TIME",
    this->customLookAndFeel.DIMENSION_FRACTION
  );

  this->delayFeedbackSlider = this->initSlider<Slider>(
    DELAY_FEEDBACK_ID,
    "FEEDBACK",
    this->customLookAndFeel.DIMENSION_PERCENT
  );

  this->filterCutoffSlider = this->initSlider<Slider>(
    FILTER_CUTOFF_ID,
    "CUTOFF",
    this->customLookAndFeel.DIMENSION_HERTZ
  );

  this->filterResonanceSlider = this->initSlider<Slider>(
    FILTER_RESONANCE_ID,
    "RESONANCE (Q)",
    ""
  );

  this->reverbImpResComboBox = this->initComboBox(IMPULSE_RESPONSE_ID, "IMPULSE RESPONSE");

  this->filterTypeComboBox = this->initComboBox(FILTER_TYPE_ID, "FILTER");

  this->riseReverseToggleButton = this->initToggleButton(RISE_REVERSE_ID, "REVERSE");

  this->riseReverbToggleButton = this->initToggleButton(RISE_REVERB_ID, "REVERB");

  this->riseDelayToggleButton = this->initToggleButton(RISE_DELAY_ID, "DELAY");

  this->fallReverseToggleButton = this->initToggleButton(FALL_REVERSE_ID, "REVERSE");

  this->fallReverbToggleButton = this->initToggleButton(FALL_REVERB_ID, "REVERB");

  this->fallDelayToggleButton = this->initToggleButton(FALL_DELAY_ID, "DELAY");

  this->loadFileButton.setButtonText("LOAD AUDIO FILE");
  this->loadFileButton.addListener(this);
  this->loadFileButton.setColour(
    TextButton::textColourOnId,
    this->customLookAndFeel.COLOUR_BLACK
  );
  this->loadFileButton.setColour(
    TextButton::textColourOffId,
    this->customLookAndFeel.COLOUR_BLACK
  );
  this->loadFileButton.setColour(
    TextButton::textColourOnId,
    this->customLookAndFeel.COLOUR_BLACK
  );
  this->addAndMakeVisible(&loadFileButton);

  this->processor.getThumbnail().addChangeListener(&thumbnailComp);
  this->formatManager.registerBasicFormats();

  this->addAndMakeVisible(&thumbnailComp);
  this->addAndMakeVisible(&positionOverlay);

  this->setLookAndFeel(&customLookAndFeel);
  this->setSize(688, 704);
}

RiseAndFallAudioProcessorEditor::~RiseAndFallAudioProcessorEditor()
{
  this->setLookAndFeel(nullptr);
  this->parameters.detachAll();
}

void RiseAndFallAudioProcessorEditor::paint(Graphics &g)
{
  Image background = ImageCache::getFromMemory(
    BinaryData::background_png,
    BinaryData::background_pngSize
  );
  g.drawImageAt(background, 0, 0);
}

void RiseAndFallAudioProcessorEditor::resized()
{
  this->timeOffsetSlider->setBounds(238, 464, 418, 46);

  this->riseTimeWarpSlider->setBounds(232, 320, sliderWidth, sliderHeight);
  this->fallTimeWarpSlider->setBounds(568, 320, sliderWidth, sliderHeight);

  this->reverbMixSlider->setBounds(232, 176, sliderWidth, sliderHeight);

  this->delayTimeSlider->setBounds(368, 176, sliderWidth, sliderHeight);
  this->delayFeedbackSlider->setBounds(468, 176, sliderWidth, sliderHeight);
  this->delayMixSlider->setBounds(568, 176, sliderWidth, sliderHeight);

  this->filterCutoffSlider->setBounds(468, 32, sliderWidth, sliderHeight);
  this->filterResonanceSlider->setBounds(568, 32, sliderWidth, sliderHeight);

  this->reverbImpResComboBox->setBounds(32, 208, 188, comboBoxHeight);

  this->filterTypeComboBox->setBounds(368, 64, 88, comboBoxHeight);

  this->riseReverseToggleButton->setBounds(32, 320, toggleButtonWidth, toggleButtonHeight);
  this->riseReverbToggleButton->setBounds(32, 352, toggleButtonWidth, toggleButtonHeight);
  this->riseDelayToggleButton->setBounds(32, 384, toggleButtonWidth, toggleButtonHeight);

  this->fallReverseToggleButton->setBounds(368, 320, toggleButtonWidth, toggleButtonHeight);
  this->fallReverbToggleButton->setBounds(368, 352, toggleButtonWidth, toggleButtonHeight);
  this->fallDelayToggleButton->setBounds(368, 384, toggleButtonWidth, toggleButtonHeight);

  this->loadFileButton.setBounds(32, 464, 188, 32);

  this->thumbnailComp.setBounds(this->thumbnailBounds);
  this->positionOverlay.setBounds(this->thumbnailBounds.expanded(16, 16));
}

void RiseAndFallAudioProcessorEditor::loadFileButtonCLicked()
{
  FileChooser chooser("Select a Wave file", {}, "*.wav");

  if (chooser.browseForFileToOpen()) {
    File file(chooser.getResult());
    this->processor.loadSampleFromFile(file);
  }
}

void RiseAndFallAudioProcessorEditor::buttonClicked(Button *button)
{
  if (button == &this->loadFileButton) {
    this->loadFileButtonCLicked();
  }
}
