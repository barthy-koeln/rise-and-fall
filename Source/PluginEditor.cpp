/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define windowSize 700
#define margin 10
#define bigMargin 25
#define blockWidth 335
#define blockHeight 135
#define sliderSize 80
#define blockCenter (margin + (blockWidth/2))
#define fontSize 16

void RiseandfallAudioProcessorEditor::initRotarySlider(Slider *slider, const juce::String &suffix, float min, float max, float step, float start){
    slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slider->setRange(min, max, step);
    slider->setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix(suffix);
    slider->setValue(start);
    
    addAndMakeVisible(slider);
}

void RiseandfallAudioProcessorEditor::addLabelToSlider(Slider& slider, Graphics& g, const juce::String &text){
    Rectangle<int> bounds = slider.getBounds();
    g.drawFittedText(text, bounds.getX(), bounds.getY() + sliderSize, sliderSize, fontSize, Justification::centred, 1);
}

//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor (RiseandfallAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(windowSize, windowSize);
    setLookAndFeel(&customLookAndFeel);
    
    initRotarySlider(&riseTimeOffsetSlider, " ms", -120, 120, 1.0, 0);
    initRotarySlider(&riseTimeWarpSlider, " ms", -5, 5, 1, 0.0);
    initRotarySlider(&fallTimeOffsetSlider, "",-120, 120, 1.0, 0);
    initRotarySlider(&fallTimeWarpSlider, "", -5, 5, 1, 0.0);
    
    initRotarySlider(&reverbMixSlider, " %", 0, 100, 1.0, 50);
    initRotarySlider(&delayMixSlider, " %", 0, 100, 1.0, 50);
    initRotarySlider(&delayTimeSlider, " ms", 0, 1000, 1.0, 500);
    initRotarySlider(&delayFeedbackSlider, " dB", -10, 10, 0, 0);
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
    g.setFont(fontSize);
    
    addLabelToSlider(riseTimeOffsetSlider, g, "Time Offset");
    addLabelToSlider(riseTimeWarpSlider, g, "Time Warp");
    addLabelToSlider(fallTimeOffsetSlider, g, "Time Offset");
    addLabelToSlider(fallTimeWarpSlider, g, "Time Warp");
    addLabelToSlider(reverbMixSlider, g, "Mix");
    addLabelToSlider(delayMixSlider, g, "Mix");
    addLabelToSlider(delayTimeSlider, g, "Time");
    addLabelToSlider(delayFeedbackSlider, g, "Feedback");
}

void RiseandfallAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    riseTimeOffsetSlider.setBounds(blockCenter - (sliderSize + bigMargin), (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    riseTimeWarpSlider.setBounds(blockCenter + bigMargin, (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    fallTimeOffsetSlider.setBounds((windowSize - blockCenter) - (sliderSize + bigMargin), (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    fallTimeWarpSlider.setBounds((windowSize - blockCenter) + bigMargin, (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    reverbMixSlider.setBounds(blockWidth - sliderSize, blockHeight + margin, sliderSize, sliderSize);
    delayMixSlider.setBounds(windowSize - (2 * margin) - sliderSize, 4 * margin, sliderSize, sliderSize);
    delayTimeSlider.setBounds(windowSize - blockWidth, 4 * margin, sliderSize, sliderSize);
    delayFeedbackSlider.setBounds(windowSize - (blockWidth / 2) - (sliderSize / 2) - (margin / 2), 4 * margin, sliderSize, sliderSize);
}
