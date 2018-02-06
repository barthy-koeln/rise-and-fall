/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLookAndFeel.hpp"


//==============================================================================
/**
 */
class RiseandfallAudioProcessorEditor :
        public AudioProcessorEditor,
        public Button::Listener,
        public ChangeListener,
        public Slider::Listener {
public:
    RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &);

    ~RiseandfallAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics &) override;

    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RiseandfallAudioProcessor &processor;

    CustomLookAndFeel customLookAndFeel;
    Slider timeOffsetSlider;
    Slider riseTimeWarpSlider;
    Slider fallTimeWarpSlider;
    Slider reverbMixSlider;
    Slider delayMixSlider;
    Slider delayTimeSlider;
    Slider delayFeedbackSlider;
    Slider delayFilterCutoffSlider;
    Slider delayFilterResonanceSlider;
    ComboBox reverbImpResComboBox;
    ComboBox filterTypeComboBox;
    ToggleButton riseReverseToggleButton;
    ToggleButton riseEffectsToggleButton;
    ToggleButton fallReverseToggleButton;
    ToggleButton fallEffectsToggleButton;
    TextButton loadFileButton;

    GUIParams *guiParams;

    AudioFormatManager formatManager;

    void initSlider(Slider *slider, const String &label, const String &suffix, double min, double max,
                          double step, double start, bool logarithmic, bool linear);

    void initComboBox(ComboBox *comboBox, const String &label, const StringArray *items);


    void initToggleButton(ToggleButton *toggleButton, bool state, String label);

    void buttonClicked(Button *button) override;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void sliderValueChanged(Slider *slider) override;

    void loadFileButtonCLicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessorEditor)
};
