/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor (RiseandfallAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    const int size = 700, margin = 10;
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(size, size);
    setLookAndFeel(&customLookAndFeel);
    
    riseTimeOffsetSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    riseTimeOffsetSlider.setRange(-120, 120, 1.0);
    riseTimeOffsetSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    riseTimeOffsetSlider.setPopupDisplayEnabled(true, false, this);
    riseTimeOffsetSlider.setTextValueSuffix(" ms");
    riseTimeOffsetSlider.setValue(0.0);

    riseTimeWarpSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    riseTimeWarpSlider.setRange(-5, 5, 1);
    riseTimeWarpSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    riseTimeWarpSlider.setPopupDisplayEnabled(true, false, this);
    riseTimeWarpSlider.setTextValueSuffix("");
    riseTimeWarpSlider.setValue(0.0);
    
    fallTimeOffsetSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    fallTimeOffsetSlider.setRange(-120, 120, 1.0);
    fallTimeOffsetSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    fallTimeOffsetSlider.setPopupDisplayEnabled(true, false, this);
    fallTimeOffsetSlider.setTextValueSuffix(" ms");
    fallTimeOffsetSlider.setValue(0.0);
    
    fallTimeWarpSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    fallTimeWarpSlider.setRange(-5, 5, 1);
    fallTimeWarpSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    fallTimeWarpSlider.setPopupDisplayEnabled(true, false, this);
    fallTimeWarpSlider.setTextValueSuffix("");
    fallTimeWarpSlider.setValue(0.0);
    
    addAndMakeVisible (&riseTimeOffsetSlider);
    addAndMakeVisible (&riseTimeWarpSlider);
    addAndMakeVisible (&fallTimeOffsetSlider);
    addAndMakeVisible (&fallTimeWarpSlider);
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor()
{
}

//==============================================================================
void RiseandfallAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid
    Image background = ImageCache::getFromMemory (BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt (background, 0, 0);

    g.setColour(customLookAndFeel.lightBlue);
    g.setFont(16.0f);
    g.drawFittedText("Time Offset", 50, 370, 80, 16, Justification::centred, 1);
    g.drawFittedText("Time Warp", 225, 370, 80, 16, Justification::centred, 1);
    g.drawFittedText("Time Offset", 395, 370, 80, 16, Justification::centred, 1);
    g.drawFittedText("Time Warp", 570, 370, 80, 16, Justification::centred, 1);
}

void RiseandfallAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    riseTimeOffsetSlider.setBounds (50, 290, 80, 80);
    riseTimeWarpSlider.setBounds (225, 290, 80, 80);
    fallTimeOffsetSlider.setBounds (395, 290, 80, 80);
    fallTimeWarpSlider.setBounds (570, 290, 80, 80);
}
