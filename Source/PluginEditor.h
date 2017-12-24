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
class RiseandfallAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    RiseandfallAudioProcessorEditor (RiseandfallAudioProcessor&);
    ~RiseandfallAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RiseandfallAudioProcessor& processor;
    
    CustomLookAndFeel customLookAndFeel;
    ImageComponent headerImage;
    Slider riseTimeOffsetSlider;
    Slider riseTimeWarpSlider;
    Slider fallTimeOffsetSlider;
    Slider fallTimeWarpSlider;
    Slider reverbMixSlider;
    Slider delayMixSlider;
    Slider delayTimeSlider;
    Slider delayFeedbackSlider;
    
    void initRotarySlider(Slider *slider, const juce::String &suffix, float min, float max, float step, float start);
    void addLabelToSlider(Slider& slider, Graphics& g, const juce::String &text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessorEditor)
};
