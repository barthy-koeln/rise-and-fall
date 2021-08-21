#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.hpp"
#include "SimpleThumbnailComponent.hpp"
#include "SimplePositionOverlay.h"
#include "NoteLengthSlider.hpp"

class RiseAndFallAudioProcessorEditor :
  public AudioProcessorEditor,
  public Button::Listener
{
  public:
    RiseAndFallAudioProcessorEditor(RiseAndFallAudioProcessor&, GUIParams&);

    ~RiseAndFallAudioProcessorEditor() override;

    void paint(Graphics &) override;

    void resized() override;

  private:
    RiseAndFallAudioProcessor &processor;
    GUIParams &parameters;

    CustomLookAndFeel customLookAndFeel;
    std::unique_ptr<Slider> timeOffsetSlider;
    std::unique_ptr<Slider> riseTimeWarpSlider;
    std::unique_ptr<Slider> fallTimeWarpSlider;
    std::unique_ptr<Slider> reverbMixSlider;
    std::unique_ptr<Slider> delayMixSlider;
    std::unique_ptr<NoteLengthSlider> delayTimeSlider;
    std::unique_ptr<Slider> delayFeedbackSlider;
    std::unique_ptr<Slider> filterCutoffSlider;
    std::unique_ptr<Slider> filterResonanceSlider;
    std::unique_ptr<ComboBox> reverbImpResComboBox;
    std::unique_ptr<ComboBox> filterTypeComboBox;
    std::unique_ptr<ToggleButton> riseReverseToggleButton;
    std::unique_ptr<ToggleButton> riseReverbToggleButton;
    std::unique_ptr<ToggleButton> riseDelayToggleButton;
    std::unique_ptr<ToggleButton> fallReverseToggleButton;
    std::unique_ptr<ToggleButton> fallReverbToggleButton;
    std::unique_ptr<ToggleButton> fallDelayToggleButton;
    TextButton loadFileButton;

    AudioFormatManager formatManager;

    const Rectangle<int> thumbnailBounds;

    SimpleThumbnailComponent thumbnailComp;
    SimplePositionOverlay positionOverlay;

    template<class T> std::unique_ptr<T> initSlider(
      String parameterId,
      const String &label,
      const String &suffix,
      Slider::SliderStyle style = Slider::RotaryHorizontalVerticalDrag
    );

    std::unique_ptr<ComboBox> initComboBox(
      String parameterId,
      const String &label
    );

    std::unique_ptr<ToggleButton> initToggleButton(String parameterId, String label);

    void buttonClicked(Button *button) override;

    void loadFileButtonCLicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RiseAndFallAudioProcessorEditor)
};
