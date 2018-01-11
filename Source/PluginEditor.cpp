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
#define smallMargin 5
#define bigMargin 25
#define blockWidth 335
#define blockHeight 135
#define sliderSize 80
#define comboBoxHeight 40
#define toggleButtonHeight 40
#define blockCenter (margin + (blockWidth/2))
#define fontSize 16
#define buttonHeight 40
#define buttonWidth 120

void RiseandfallAudioProcessorEditor::initRotarySlider(Slider *slider, const juce::String &suffix, float min, float max,
                                                       float step, float start, bool logarithmic = false) {
    slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slider->setRange(min, max, step);
    slider->setTextBoxStyle(Slider::NoTextBox, false, sliderSize, 0);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix(suffix);
    slider->setValue(start);

    if (logarithmic) {
        slider->setSkewFactor(0.36787944117);
    }

    addAndMakeVisible(slider);
}

void RiseandfallAudioProcessorEditor::initComboBox(juce::ComboBox *comboBox, const juce::StringArray *items) {
    comboBox->addItemList(*items, 1);
    comboBox->setSelectedId(1);

    addAndMakeVisible(comboBox);
}

void RiseandfallAudioProcessorEditor::addLabelToSlider(Slider &slider, Graphics &g, const juce::String &text) {
    Rectangle<int> bounds = slider.getBounds();
    g.drawFittedText(text, bounds.getX(), bounds.getY() + sliderSize, sliderSize, fontSize, Justification::centred, 1);
}

void RiseandfallAudioProcessorEditor::addLabelToComboBox(ComboBox &comboBox, Graphics &g, const juce::String &text) {
    Rectangle<int> bounds = comboBox.getBounds();
    g.drawFittedText(text, bounds.getX(), bounds.getY() - smallMargin - fontSize, bounds.getWidth(), fontSize,
                     Justification::left, 1);
}

//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(windowSize, windowSize);
    setLookAndFeel(&customLookAndFeel);

    initRotarySlider(&riseTimeOffsetSlider, " ms", -120, 120, 1, 0);
    initRotarySlider(&riseTimeWarpSlider, " ms", -5, 5, 1, 0);
    initRotarySlider(&fallTimeOffsetSlider, "", -120, 120, 1, 0);
    initRotarySlider(&fallTimeWarpSlider, "", -5, 5, 1, 0);

    initRotarySlider(&reverbMixSlider, " %", 0, 100, 1, 50);
    initRotarySlider(&delayMixSlider, " %", 0, 100, 1, 50);
    initRotarySlider(&delayTimeSlider, " ms", 0, 1000, 1, 500);
    initRotarySlider(&delayFeedbackSlider, " dB", -40, 10, 1, 0, true);

    initRotarySlider(&delayFilterCutoffSlider, " Hz", 20, 20000, 1, 3000, true);
    initRotarySlider(&delayFilterResonanceSlider, " dB", -10, 10, 1, 0);

    const StringArray *impResItems = new StringArray(
            CharPointer_UTF8("TH Köln - Entrance Hall"),
            CharPointer_UTF8("Erfthaus Köln - Stairway"),
            CharPointer_UTF8("Kölner Dom - Nave")
    );
    initComboBox(&reverbImpResComboBox, impResItems);

    const StringArray *filterTypes = new StringArray(
            CharPointer_UTF8("LP"),
            CharPointer_UTF8("BP"),
            CharPointer_UTF8("HP")
    );
    initComboBox(&filterTypeCombomBox, filterTypes);

    riseReverseToggleButton.setButtonText("Reverse");
    addAndMakeVisible(&riseReverseToggleButton);
    riseEffectsToggleButton.setButtonText("Effects");
    addAndMakeVisible(&riseEffectsToggleButton);
    fallReverseToggleButton.setButtonText("Reverse");
    addAndMakeVisible(&fallReverseToggleButton);
    fallEffectsToggleButton.setButtonText("Effects");
    addAndMakeVisible(&fallEffectsToggleButton);

    loadFileButton.setButtonText("Load Audio File");
    loadFileButton.addListener(this);
    addAndMakeVisible(&loadFileButton);
    processor.getThumbnail()->addChangeListener(this);
    formatManager.registerBasicFormats();
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor() = default;

//==============================================================================
void RiseandfallAudioProcessorEditor::paint(Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid
    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);

    g.setColour(customLookAndFeel.white);
    g.setFont(fontSize);

    addLabelToSlider(riseTimeOffsetSlider, g, "Time Offset");
    addLabelToSlider(riseTimeWarpSlider, g, "Time Warp");
    addLabelToSlider(fallTimeOffsetSlider, g, "Time Offset");
    addLabelToSlider(fallTimeWarpSlider, g, "Time Warp");

    addLabelToSlider(reverbMixSlider, g, "Mix");
    addLabelToSlider(delayMixSlider, g, "Mix");
    addLabelToSlider(delayTimeSlider, g, "Time");
    addLabelToSlider(delayFeedbackSlider, g, "Feedback");

    addLabelToSlider(delayFilterCutoffSlider, g, "Cutoff");
    addLabelToSlider(delayFilterResonanceSlider, g, "Resonance");

    addLabelToComboBox(reverbImpResComboBox, g, "Impulse Response");
    addLabelToComboBox(filterTypeCombomBox, g, "Filter");

    const Rectangle<int> thumbnailBounds(margin, windowSize - (2 * margin) - buttonHeight - 180,
                                         windowSize - (2 * margin), 180);

    if (processor.getOriginalSampleBuffer()->getNumChannels() != 0) {
        g.setColour(customLookAndFeel.red);
        processor.getThumbnail()->drawChannels(g, thumbnailBounds, 0.0, processor.getThumbnail()->getTotalLength(),
                                               1.0f);
    }
}

void RiseandfallAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    riseTimeOffsetSlider.setBounds(blockCenter - (sliderSize + bigMargin), (2 * margin + (2 * blockHeight)), sliderSize,
                                   sliderSize);
    riseTimeWarpSlider.setBounds(blockCenter + bigMargin, (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    fallTimeOffsetSlider.setBounds((windowSize - blockCenter) - (sliderSize + bigMargin),
                                   (2 * margin + (2 * blockHeight)), sliderSize, sliderSize);
    fallTimeWarpSlider.setBounds((windowSize - blockCenter) + bigMargin, (2 * margin + (2 * blockHeight)), sliderSize,
                                 sliderSize);

    reverbMixSlider.setBounds(blockWidth - sliderSize, blockHeight + margin, sliderSize, sliderSize);
    delayMixSlider.setBounds(windowSize - (2 * margin) - sliderSize, 4 * margin, sliderSize, sliderSize);
    delayTimeSlider.setBounds(windowSize - blockWidth, 4 * margin, sliderSize, sliderSize);
    delayFeedbackSlider.setBounds(windowSize - (blockWidth / 2) - (sliderSize / 2) - (margin / 2), 4 * margin,
                                  sliderSize, sliderSize);

    delayFilterCutoffSlider.setBounds(windowSize - (blockWidth / 2) - (sliderSize / 2) - (margin / 2),
                                      margin + blockHeight, sliderSize, sliderSize);
    delayFilterResonanceSlider.setBounds(windowSize - (2 * margin) - sliderSize, margin + blockHeight, sliderSize,
                                         sliderSize);

    reverbImpResComboBox.setBounds((2 * margin), blockHeight + margin + smallMargin + fontSize,
                                   blockWidth - (4 * margin) - sliderSize, comboBoxHeight);
    filterTypeCombomBox.setBounds(windowSize - blockWidth, blockHeight + margin + smallMargin + fontSize,
                                  sliderSize + bigMargin, comboBoxHeight);

    riseReverseToggleButton.setBounds(blockCenter - (sliderSize + bigMargin), (3 * blockHeight), sliderSize,
                                      toggleButtonHeight);
    riseEffectsToggleButton.setBounds(blockCenter + bigMargin, (3 * blockHeight), sliderSize, toggleButtonHeight);
    fallReverseToggleButton.setBounds((windowSize - blockCenter) - (sliderSize + bigMargin), (3 * blockHeight),
                                      sliderSize, toggleButtonHeight);
    fallEffectsToggleButton.setBounds((windowSize - blockCenter) + bigMargin, (3 * blockHeight), sliderSize,
                                      toggleButtonHeight);

    loadFileButton.setBounds(windowSize - buttonWidth - margin, windowSize - buttonHeight - margin, buttonWidth,
                             buttonHeight);
}

void RiseandfallAudioProcessorEditor::loadFileButtonCLicked() {

    FileChooser chooser("Select a Wave file",
                        File::nonexistent,
                        "*.wav");
    if (chooser.browseForFileToOpen()) {
        File file(chooser.getResult());
        ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
        const double duration = reader->lengthInSamples / reader->sampleRate;

        if (duration < 20) {
            processor.getOriginalSampleBuffer()->setSize(reader->numChannels,
                                                         static_cast<int>(reader->lengthInSamples));
            reader->read(processor.getOriginalSampleBuffer(), 0, static_cast<int>(reader->lengthInSamples), 0, true,
                         true);
            processor.newSampleLoaded();
            processor.prepare();
        } else {
            // handle the error that the file is 2 seconds or longer..
        }
    }
}

void RiseandfallAudioProcessorEditor::buttonClicked(Button *button) {
    if (button == &loadFileButton) loadFileButtonCLicked();
}

void RiseandfallAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source) {
    if (source == processor.getThumbnail()) {
        printf("THUMBNAIL CHANGED\n");
        thumbnailChanged();
    }
}

void RiseandfallAudioProcessorEditor::thumbnailChanged() {
    repaint();
}