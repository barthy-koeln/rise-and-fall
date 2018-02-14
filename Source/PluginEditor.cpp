/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define fontSize 16
#define sliderWidth 88
#define sliderHeight 96
#define comboBoxHeight 64
#define toggleButtonHeight 32
#define toggleButtonWidth 188

void RiseandfallAudioProcessorEditor::initSlider(Slider *slider, const String &label, const String &suffix,
                                                 double min, double max, double step, double start,
                                                 bool logarithmic = false, bool linear = false) {
    slider->setSliderStyle(linear ? Slider::LinearHorizontal : Slider::RotaryHorizontalVerticalDrag);
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

void RiseandfallAudioProcessorEditor::initComboBox(ComboBox *comboBox, const String &label, const StringArray *items) {
    comboBox->addItemList(*items, 1);
    comboBox->setSelectedId(1);
    comboBox->setName(label);

    addAndMakeVisible(comboBox);
}

void RiseandfallAudioProcessorEditor::initToggleButton(ToggleButton *toggleButton, bool state, String label) {
    toggleButton->setButtonText(label);
    toggleButton->setToggleState(state, NotificationType::dontSendNotification);
    toggleButton->addListener(this);
    addAndMakeVisible(toggleButton);
}

//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {

    guiParams = &processor.guiParams;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(688, 704);
    setLookAndFeel(&customLookAndFeel);

    initSlider(&timeOffsetSlider, "TIME OFFSET", customLookAndFeel.DIMENSION_MS, -120, 120, 1,
               guiParams->timeOffset, false, true);

    initSlider(&riseTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, -4, 4, 2,
               guiParams->riseTimeWarp);
    initSlider(&fallTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, -4, 4, 2,
               guiParams->fallTimeWarp);

    initSlider(&reverbMixSlider, "MIX", customLookAndFeel.DIMENSION_PERCENT, 0, 100, 1, 50);
    initSlider(&delayMixSlider, "MIX", customLookAndFeel.DIMENSION_PERCENT, 0, 100, 1, 50);
    initSlider(&delayTimeSlider, "TIME", customLookAndFeel.DIMENSION_MS, 10, 1000, 1, guiParams->delayTime);
    initSlider(&delayFeedbackSlider, "FEEDBACK", customLookAndFeel.DIMENSION_PERCENT, 0, 99, 1,
               guiParams->delayFeedback);

    initSlider(&delayFilterCutoffSlider, "CUTOFF", customLookAndFeel.DIMENSION_HERTZ, 20, 20000, 1, 3000, true);
    initSlider(&delayFilterResonanceSlider, "RESONANCE", customLookAndFeel.DIMENSION_DECIBEL, -10, 10, 1, 0);

    const ScopedPointer<StringArray> impResItems = new StringArray(
            CharPointer_UTF8("TH Köln - Entrance Hall"),
            CharPointer_UTF8("Erfthaus Köln - Stairway"),
            CharPointer_UTF8("Cologne Subway Station \"Neuser Strasse/Gürtel\"")
    );
    initComboBox(&reverbImpResComboBox, "IMPULSE RESPONSE", impResItems);

    const ScopedPointer<StringArray> filterTypes = new StringArray(
            CharPointer_UTF8("LP"),
            CharPointer_UTF8("BP"),
            CharPointer_UTF8("HP")
    );
    initComboBox(&filterTypeComboBox, "FILTER", filterTypes);

    initToggleButton(&riseReverseToggleButton, processor.guiParams.riseReverse, "REVERSE");
    initToggleButton(&riseEffectsToggleButton, processor.guiParams.riseEffects, "EFFECTS");
    initToggleButton(&fallReverseToggleButton, processor.guiParams.fallReverse, "REVERSE");
    initToggleButton(&fallEffectsToggleButton, processor.guiParams.fallEffects, "EFFECTS");

    loadFileButton.setButtonText("LOAD AUDIO FILE");
    loadFileButton.addListener(this);
    loadFileButton.setColour(TextButton::textColourOnId, customLookAndFeel.COLOUR_BLACK);
    loadFileButton.setColour(TextButton::textColourOffId, customLookAndFeel.COLOUR_BLACK);
    loadFileButton.setColour(TextButton::textColourOnId, customLookAndFeel.COLOUR_BLACK);
    addAndMakeVisible(&loadFileButton);
    processor.getThumbnail()->addChangeListener(this);
    formatManager.registerBasicFormats();
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor() {
    setLookAndFeel(nullptr);
};

//==============================================================================
void RiseandfallAudioProcessorEditor::paint(Graphics &g) {
    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);

    g.setColour(customLookAndFeel.COLOUR_WHITE);
    g.setFont(fontSize);

    const Rectangle<int> thumbnailBounds(16, 536, 656, 144);

    if (processor.getOriginalSampleBuffer()->getNumChannels() != 0) {
        g.setColour(customLookAndFeel.COLOUR_RED);
        processor.getThumbnail()->drawChannels(g, thumbnailBounds, 0.0, processor.getThumbnail()->getTotalLength(),
                                               1.0f);
    }
}

void RiseandfallAudioProcessorEditor::resized() {
    timeOffsetSlider.setBounds(368, 464, 288, 32);

    riseTimeWarpSlider.setBounds(232, 320, sliderWidth, sliderHeight);
    fallTimeWarpSlider.setBounds(568, 320, sliderWidth, sliderHeight);

    reverbMixSlider.setBounds(232, 176, sliderWidth, sliderHeight);

    delayTimeSlider.setBounds(368, 176, sliderWidth, sliderHeight);
    delayFeedbackSlider.setBounds(468, 176, sliderWidth, sliderHeight);
    delayMixSlider.setBounds(568, 176, sliderWidth, sliderHeight);

    delayFilterCutoffSlider.setBounds(468, 32, sliderWidth, sliderHeight);
    delayFilterResonanceSlider.setBounds(568, 32, sliderWidth, sliderHeight);

    reverbImpResComboBox.setBounds(32, 208, 188, comboBoxHeight);

    filterTypeComboBox.setBounds(368, 64, 88, comboBoxHeight);

    riseReverseToggleButton.setBounds(32, 336, toggleButtonWidth, toggleButtonHeight);
    riseEffectsToggleButton.setBounds(32, 384, toggleButtonWidth, toggleButtonHeight);

    fallReverseToggleButton.setBounds(368, 336, toggleButtonWidth, toggleButtonHeight);
    fallEffectsToggleButton.setBounds(368, 384, toggleButtonWidth, toggleButtonHeight);

    loadFileButton.setBounds(32, 464, 188, 32);
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
            // handle the error that the file is 20 seconds or longer..
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
    } else if (slider == &riseTimeWarpSlider) {
        guiParams->riseTimeWarp = static_cast<int>(riseTimeWarpSlider.getValue());
    } else if (slider == &fallTimeWarpSlider) {
        guiParams->fallTimeWarp = static_cast<int>(fallTimeWarpSlider.getValue());
    } else if (slider == &delayFeedbackSlider) {
        guiParams->delayFeedback = static_cast<int>(delayFeedbackSlider.getValue());
    } else if (slider == &delayTimeSlider) {
        guiParams->delayTime = static_cast<int>(delayTimeSlider.getValue());
    }

    processor.processSample();
}