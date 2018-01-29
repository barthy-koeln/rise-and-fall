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
#define sliderSize 80
#define halfSliderSize 40
#define thirdRowCenter 357
#define comboBoxHeight 40
#define toggleButtonHeight 40
#define halfToggleButtonHeight 20
#define blockCenter (margin + (blockWidth/2))
#define fontSize 16
#define buttonHeight 40
#define buttonWidth 120

void RiseandfallAudioProcessorEditor::initRotarySlider(Slider *slider, const juce::String &suffix, double min,
                                                       double max, double step, double start,
                                                       bool logarithmic = false) {
    slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slider->setRange(min, max, step);
    slider->setTextBoxStyle(Slider::NoTextBox, false, sliderSize, 0);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix(suffix);
    slider->setValue(start, NotificationType::dontSendNotification);

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

    guiParams = &processor.guiParams;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(windowSize, windowSize);
    setLookAndFeel(&customLookAndFeel);

    initRotarySlider(&timeOffsetSlider, " ms", -120, 120, 1, guiParams->timeOffset);
    initRotarySlider(&riseTimeWarpSlider, " x", -4, 4, 2, guiParams->riseTimeWarp);
    initRotarySlider(&fallTimeWarpSlider, " x", -4, 4, 2, guiParams->fallTimeWarp);

    initRotarySlider(&reverbMixSlider, " %", 0, 100, 1, 50);
    initRotarySlider(&delayMixSlider, " %", 0, 100, 1, 50);
    initRotarySlider(&delayTimeSlider, " ms", 10, 1000, 1, guiParams->delayTime);
    initRotarySlider(&delayFeedbackSlider, " %", 0, 99, 1, guiParams->delayFeedback);

    initRotarySlider(&delayFilterCutoffSlider, " Hz", 20, 20000, 1, 3000, true);
    initRotarySlider(&delayFilterResonanceSlider, " dB", -10, 10, 1, 0);

    const ScopedPointer<StringArray> impResItems = new StringArray(
            CharPointer_UTF8("TH Köln - Entrance Hall"),
            CharPointer_UTF8("Erfthaus Köln - Stairway"),
            CharPointer_UTF8("Kölner Dom - Nave")
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

    g.setColour(*customLookAndFeel.white);
    g.setFont(fontSize);

    addLabelToSlider(timeOffsetSlider, g, "Time Offset");
    addLabelToSlider(riseTimeWarpSlider, g, "Time Warp");
    addLabelToSlider(fallTimeWarpSlider, g, "Time Warp");

    addLabelToSlider(reverbMixSlider, g, "Mix");
    addLabelToSlider(delayMixSlider, g, "Mix");
    addLabelToSlider(delayTimeSlider, g, "Time");
    addLabelToSlider(delayFeedbackSlider, g, "Feedback");

    addLabelToSlider(delayFilterCutoffSlider, g, "Cutoff");
    addLabelToSlider(delayFilterResonanceSlider, g, "Resonance");

    addLabelToComboBox(reverbImpResComboBox, g, "Impulse Response");
    addLabelToComboBox(filterTypeComboBox, g, "Filter");

    const Rectangle<int> thumbnailBounds(margin, windowSize - (2 * margin) - buttonHeight - 175,
                                         windowSize - (2 * margin), 175);

    if (processor.getOriginalSampleBuffer()->getNumChannels() != 0) {
        g.setColour(*customLookAndFeel.red);
        processor.getThumbnail()->drawChannels(g, thumbnailBounds, 0.0, processor.getThumbnail()->getTotalLength(),
                                               1.0f);
    }
}

void RiseandfallAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    timeOffsetSlider.setBounds(blockCenter - (sliderSize + bigMargin), (2 * margin + (2 * blockHeight)), sliderSize,
                               sliderSize);


    riseTimeWarpSlider.setBounds(windowSize - blockWidth - margin, thirdRowCenter - halfSliderSize, sliderSize,
                                 sliderSize);
    fallTimeWarpSlider.setBounds(windowSize - (2 * margin) - sliderSize, thirdRowCenter - halfSliderSize, sliderSize,
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
    filterTypeComboBox.setBounds(windowSize - blockWidth, blockHeight + margin + smallMargin + fontSize,
                                 sliderSize + bigMargin, comboBoxHeight);

    riseReverseToggleButton.setBounds(thirdRowSecondBlock + margin, thirdRowCenter - halfToggleButtonHeight - bigMargin,
                                      sliderSize, toggleButtonHeight);
    riseEffectsToggleButton.setBounds(thirdRowSecondBlock + margin, thirdRowCenter - halfToggleButtonHeight + bigMargin,
                                      sliderSize, toggleButtonHeight);

    fallReverseToggleButton.setBounds(thirdRowThirdBLock + margin, thirdRowCenter - halfToggleButtonHeight - bigMargin,
                                      sliderSize, toggleButtonHeight);
    fallEffectsToggleButton.setBounds(thirdRowThirdBLock + margin, thirdRowCenter - halfToggleButtonHeight + bigMargin,
                                      sliderSize, toggleButtonHeight);

    loadFileButton.setBounds(blockCenter - (sliderSize) - (buttonWidth / 2), 3 * blockHeight - smallMargin, buttonWidth,
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