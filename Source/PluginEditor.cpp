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
#define thirdRowSecondBlock 210
#define thirdRowThirdBLock 455
#define sliderWidth 88
#define sliderHeight 96
#define thirdRowCenter 357
#define comboBoxHeight 40
#define toggleButtonHeight 40
#define halfToggleButtonHeight 20
#define blockCenter (margin + (blockWidth/2))
#define fontSize 16
#define buttonHeight 40
#define buttonWidth 120

void RiseandfallAudioProcessorEditor::initRotarySlider(Slider *slider, const juce::String &label, const juce::String &suffix, double min,
                                                       double max, double step, double start,
                                                       bool logarithmic = false) {
    slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slider->setRange(min, max, step);
    slider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix(suffix);
    slider->setValue(start, NotificationType::dontSendNotification);
    slider->setName(label);

    if (logarithmic) {
        slider->setSkewFactor(0.36787944117);
    }

    addAndMakeVisible(slider);
    slider->addListener(this);
}

void RiseandfallAudioProcessorEditor::initComboBox(juce::ComboBox *comboBox, const juce::StringArray *items) {
    comboBox->addItemList(*items, 1);
    comboBox->setSelectedId(1);

    addAndMakeVisible(comboBox);
}

void RiseandfallAudioProcessorEditor::initToggleButton(ToggleButton *toggleButton, bool state, String label) {
    toggleButton->setButtonText(label);
    toggleButton->setToggleState(state, NotificationType::dontSendNotification);
    toggleButton->addListener(this);
    addAndMakeVisible(toggleButton);
}

void RiseandfallAudioProcessorEditor::addLabelToComboBox(ComboBox &comboBox, Graphics &g, const juce::String &text) {
    Rectangle<int> bounds = comboBox.getBounds();
    g.drawFittedText(text, bounds.getX(), bounds.getY() - smallMargin - fontSize, bounds.getWidth(), fontSize,
                     Justification::left, 1);
}

//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {

    guiParams = &processor.guiParams;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(688, 704);
    setLookAndFeel(&customLookAndFeel);

    initRotarySlider(&timeOffsetSlider, "TIME OFFSET", customLookAndFeel.DIMENSION_MS, -120, 120, 1, guiParams->timeOffset);
    initRotarySlider(&riseTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, -4, 4, 2, guiParams->riseTimeWarp);
    initRotarySlider(&fallTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, -4, 4, 2, guiParams->fallTimeWarp);

    initRotarySlider(&reverbMixSlider, "MIX", customLookAndFeel.DIMENSION_PERCENT, 0, 100, 1, 50);
    initRotarySlider(&delayMixSlider, "MIX", customLookAndFeel.DIMENSION_PERCENT, 0, 100, 1, 50);
    initRotarySlider(&delayTimeSlider, "TIME", customLookAndFeel.DIMENSION_MS, 10, 1000, 1, guiParams->delayTime);
    initRotarySlider(&delayFeedbackSlider, "FEEDBACK", customLookAndFeel.DIMENSION_PERCENT, 0, 99, 1, guiParams->delayFeedback);

    initRotarySlider(&delayFilterCutoffSlider, "CUTOFF", customLookAndFeel.DIMENSION_HERTZ, 20, 20000, 1, 3000, true);
    initRotarySlider(&delayFilterResonanceSlider, "RESONANCE", customLookAndFeel.DIMENSION_DECIBEL, -10, 10, 1, 0);

    const ScopedPointer<StringArray> impResItems = new StringArray(
            CharPointer_UTF8("TH Köln - Entrance Hall"),
            CharPointer_UTF8("Erfthaus Köln - Stairway"),
            CharPointer_UTF8("Cologne Subway Station \"Neuser Strasse/Gürtel\"")
    );
    initComboBox(&reverbImpResComboBox, impResItems);

    const ScopedPointer<StringArray> filterTypes = new StringArray(
            CharPointer_UTF8("LP"),
            CharPointer_UTF8("BP"),
            CharPointer_UTF8("HP")
    );
    initComboBox(&filterTypeComboBox, filterTypes);

    initToggleButton(&riseReverseToggleButton, processor.guiParams.riseReverse, "Reverse");
    initToggleButton(&riseEffectsToggleButton, processor.guiParams.riseEffects, "Effects");
    initToggleButton(&fallReverseToggleButton, processor.guiParams.fallReverse, "Reverse");
    initToggleButton(&fallEffectsToggleButton, processor.guiParams.fallEffects, "Effects");

    loadFileButton.setButtonText("Load Audio File");
    loadFileButton.addListener(this);
    addAndMakeVisible(&loadFileButton);
    processor.getThumbnail()->addChangeListener(this);
    formatManager.registerBasicFormats();
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor() {
    setLookAndFeel(nullptr);
};

//==============================================================================
void RiseandfallAudioProcessorEditor::paint(Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid
    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);

    g.setColour(customLookAndFeel.COLOUR_WHITE);
    g.setFont(fontSize);

    addLabelToComboBox(reverbImpResComboBox, g, "Impulse Response");
    addLabelToComboBox(filterTypeComboBox, g, "Filter");

    const Rectangle<int> thumbnailBounds(margin, windowSize - (2 * margin) - buttonHeight - 175,
                                         windowSize - (2 * margin), 175);

    if (processor.getOriginalSampleBuffer()->getNumChannels() != 0) {
        g.setColour(customLookAndFeel.COLOUR_RED);
        processor.getThumbnail()->drawChannels(g, thumbnailBounds, 0.0, processor.getThumbnail()->getTotalLength(),
                                               1.0f);
    }
}

void RiseandfallAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    timeOffsetSlider.setBounds(blockCenter - (sliderWidth + bigMargin), (2 * margin + (2 * blockHeight)),  sliderWidth, sliderHeight);


    riseTimeWarpSlider.setBounds(232, 304, sliderWidth, sliderHeight);
    fallTimeWarpSlider.setBounds(568, 304, sliderWidth, sliderHeight);

    reverbMixSlider.setBounds(232, 128, sliderWidth, sliderHeight);
    
    delayMixSlider.setBounds(568, 32, sliderWidth, sliderHeight);
    delayTimeSlider.setBounds(368, 32, sliderWidth, sliderHeight);
    delayFeedbackSlider.setBounds(468, 32, sliderWidth, sliderHeight);

    delayFilterCutoffSlider.setBounds(468, 144, sliderWidth, sliderHeight);
    delayFilterResonanceSlider.setBounds(568, 144, sliderWidth, sliderHeight);

    reverbImpResComboBox.setBounds((2 * margin), blockHeight + margin + smallMargin + fontSize,
                                   blockWidth - (4 * margin) - sliderWidth, comboBoxHeight);
    filterTypeComboBox.setBounds(windowSize - blockWidth, blockHeight + margin + smallMargin + fontSize,
                                 sliderWidth + bigMargin, comboBoxHeight);

    riseReverseToggleButton.setBounds(thirdRowSecondBlock + margin, thirdRowCenter - halfToggleButtonHeight - bigMargin,
                                      sliderWidth, toggleButtonHeight);
    riseEffectsToggleButton.setBounds(thirdRowSecondBlock + margin, thirdRowCenter - halfToggleButtonHeight + bigMargin,
                                      sliderWidth, toggleButtonHeight);

    fallReverseToggleButton.setBounds(thirdRowThirdBLock + margin, thirdRowCenter - halfToggleButtonHeight - bigMargin,
                                      sliderWidth, toggleButtonHeight);
    fallEffectsToggleButton.setBounds(thirdRowThirdBLock + margin, thirdRowCenter - halfToggleButtonHeight + bigMargin,
                                      sliderWidth, toggleButtonHeight);

    loadFileButton.setBounds(blockCenter - (sliderWidth) - (buttonWidth / 2), 3 * blockHeight - smallMargin, buttonWidth,
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
        } else {
            // handle the error that the file is 2 seconds or longer..
        }
    }
}

void RiseandfallAudioProcessorEditor::buttonClicked(Button *button) {
    bool processSample = true;

    if (button == &loadFileButton) {
        loadFileButtonCLicked();
        processSample = false;
    } else if (button == &fallEffectsToggleButton) {
        guiParams->fallEffects = button->getToggleState();
    } else if (button == &fallReverseToggleButton) {
        guiParams->fallReverse = button->getToggleState();
    } else if (button == &riseEffectsToggleButton) {
        guiParams->riseReverse = button->getToggleState();
    } else if (button == &riseReverseToggleButton) {
        guiParams->riseReverse = button->getToggleState();
    }

    if (processSample) {
        processor.processSample();
    }
}

void RiseandfallAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source) {
    if (source == processor.getThumbnail()) {
        repaint();
    }
}

void RiseandfallAudioProcessorEditor::sliderValueChanged(Slider *slider) {
    if (slider == &timeOffsetSlider) {
        guiParams->timeOffset = timeOffsetSlider.getValue();
    } else if (slider == &riseTimeWarpSlider){
        guiParams->riseTimeWarp = static_cast<int>(riseTimeWarpSlider.getValue());
    } else if (slider == &fallTimeWarpSlider){
        guiParams->fallTimeWarp = static_cast<int>(fallTimeWarpSlider.getValue());
    } else if (slider == &delayFeedbackSlider){
        guiParams->delayFeedback = static_cast<int>(delayFeedbackSlider.getValue());
    } else if (slider == &delayTimeSlider){
        guiParams->delayTime = static_cast<int>(delayTimeSlider.getValue());
    }

    processor.processSample();
}